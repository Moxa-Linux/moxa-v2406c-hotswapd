#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/select.h>
#include <sys/time.h>
#include <signal.h>
#include <string.h>
#include <stdarg.h>
#include <syslog.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include "mxhtsp.h"
#include "mxhtspd.h"

/* Jared, the control should be moved the global variable for accessing safely in another sub-function */
/* default control setting */
static daemon_ctrl control= {
	.time_btn_long = TIME_BTN_LONG_PRESSED,
	.time_chk_part = TIME_PARTITION_CHECK,
	.force_chk_part = 0,
	.log_facility = -1,
	.daemonize = 0,
	.verbose = 0,
	.exit = 0
};

static daemon_ctrl *ctrl;	/* A pointer for accessing control */

static const char *dir="/usr/sbin/";

static void msleep(unsigned long msec)
{
	struct timeval	time;
	time.tv_sec = msec / 1000;
	time.tv_usec = (msec % 1000) * 1000;
	select(1, NULL, NULL, NULL, &time);
}

static void mprintf(const char *fmt, ... )
{
	char buf[BUFSIZ];
	va_list ap;

	va_start(ap, fmt);
	vsnprintf(buf, sizeof(buf), fmt, ap);
	va_end(ap);

	if (ctrl->log_facility >= 0)
		syslog(LOG_NOTICE,"%s", buf);
	else 
		printf("mxhtspd: %s", buf);
}

static void execute_script(char *cmd, int param)
{
	char name[50];

	d("%s %d\n", cmd, param);
	sprintf(name, "%s%s %d &", dir, cmd, param);
	system(name);
}

static void handle_disk_plugged(int fd, int n, htsp_info *info) 
{
	int ret;
	ret = mxhtsp_is_disk_plugged(fd, n);

	if ((ret == 1) && (info[n-1].disk_plugged == 0)) {
		mprintf("Disk %d is plugged\n", n);
		if (ctrl->verbose) mprintf("call action-disk-plugged %d\n", n);
		execute_script("action-disk-plugged", n);
		ctrl->force_chk_part = 1;
		info[n-1].first_insert = 1;
		mxhtsp_set_led(fd, n, 1);
                info[n-1].led_on = 1;
		sleep (1);
	}
	else if ((ret == 0) && (info[n-1].disk_plugged == 1)) {

		mprintf("Disk %d is unplugged\n", n);
		if (ctrl->verbose) mprintf("call action-disk-plugged %d\n", n);
		/* 
		 * Note: partition should be umounted before unplugged, and 
		 * it's advicable to umount all partition in action-disk-unplugged script.
		 * Here is just a prevention for operation mistake
		 */
		execute_script("action-disk-unplugged", n);
		ctrl->force_chk_part = 1;
		mxhtsp_set_led(fd, n, 0);
                info[n-1].led_on = 0;
	}

	info[n-1].disk_plugged = ret;
}

static int handle_button_pressed(int fd, int n, htsp_info *info)
{
	int now_press;
	double time_used;
	double time_threshold = (double)ctrl->time_btn_long * 1000.0;
	clock_t now;
	//int *ta = &info[n-1].btn_time_accum;
	int *btn_event_start = &info[n-1].btn_event_start;
	int *btn_last_pressed = &info[n-1].btn_last_pressed;

	now_press = mxhtsp_is_button_pressed(fd, n);

	if (now_press < 0) {
		mprintf("detect button pressed error!");
		return -1;
	}

	if (now_press == 1 && (*btn_last_pressed == 0)) {
		/* record pressed time */
		info[n-1].press_time = clock();
		*btn_event_start = 1;
		*btn_last_pressed = 1;
	}
	else if (now_press == 0) {
		if (*btn_last_pressed == 1) {
			now = clock();
			time_used = ((double) (now - info[n-1].press_time));
			mprintf("[btn=%d, pressed time elapsed = %f]\n", n, time_used);

			/* check elapsed time over threshold */
			if ( time_used > time_threshold ) {
				if (*btn_event_start && info[n-1].first_insert == 0) {
					mprintf("Button %d is pressed\n", n);
					if ( mxhtsp_is_disk_busy(fd, n) == 1) { /* busy */
						mprintf("Disk %d is busy.\n", n);
						if (ctrl->verbose)
							mprintf("call action-btn-pressed %d\n", n+2);
						execute_script("action-btn-pressed", n+2);
						*btn_event_start = 0;
					} else {
						mprintf("Unmount the disk %d and unplug it.\n", n);
						if (ctrl->verbose)
							mprintf("call action-btn-pressed %d\n", n);
						execute_script("action-btn-pressed", n);
						*btn_event_start = 0;
					}
				}
			}
		}

		*btn_last_pressed = 0;
		info[n-1].first_insert = 0;
	}

	return 0;
}

void sig_handler(int signo)
{
	ctrl->exit = 1;
}

static void usage() {
	printf("Usage: mxhtspd [options] \n");
//	printf("\t-i interval in seconds to check partition usage (defulat 60 secs) \n");
	printf("\t-l facility_num log daemon's message with LOCAL[facility_num]\n");
	printf("\t-v run in verbose mode \n");
	printf("\t-h print usage\n");
	exit(1);
}

static void parseopt(int argc,char **argv) {
	char opt;
	int num;
	int facility[8] = {LOG_LOCAL0, LOG_LOCAL1, LOG_LOCAL2, LOG_LOCAL3, 
		LOG_LOCAL4, LOG_LOCAL5, LOG_LOCAL6, LOG_LOCAL7};

	while ((opt = getopt(argc, argv, "t:i:l:dvh")) != -1) {
		switch(opt) {
			case 't':
				num = atoi(optarg);
				if (num <= 0) {
					mprintf("can't set -t  %d", ctrl->time_btn_long);
					exit(1);
				}
				ctrl->time_btn_long = num;
				break;

			case 'i':
				num = atoi(optarg);
				if (num <= 0) {
					mprintf("can't set -i %d", ctrl->time_chk_part);
					exit(1);
				}
				ctrl->time_chk_part = num;
				break;

			case 'l':
				num = atoi(optarg);
				if ((num > 7) && (num < 0)) {
					mprintf("set a wrong log facility %d\n", num);
					exit(1);
				} 
				openlog ("mxhtspd", LOG_CONS | LOG_PID | LOG_NDELAY, facility[num]); 
				syslog(LOG_NOTICE,"Starting mxthspd daemon..."); 
				d("start with local %d\n", num);
				ctrl->log_facility = num;
				break;

			case 'd':
				ctrl->daemonize = 1;
				break;

			case 'v':
				ctrl->verbose = 1;
				break;

			case 'h':
				usage();
				break;

			case '?':
				usage();
		}
	}
}

static int create_pid_file()
{
	FILE *fp;
	pid_t pid = getpid();

	fp = fopen(PID_FILE,"w");

	if (!fp) {
		mprintf("can't open pid file\n");
		return -1;
	}

	if (fprintf(fp, "%d\n", (int)pid) <=0 ) {
		return -1;
	}

	fclose(fp);
	return 0;
}

void remove_pid_file()
{
	if (unlink(PID_FILE))
		mprintf("can't remove pid file");
}

static void daemonize()
{
	pid_t pid, sid;

	/* already a daemon */
	if (getppid() == 1) 
		return;

	pid = fork();

	/* error */
	if (pid < 0) {
		exit(1);
	}

	/* fork off parent */
	if (pid > 0) {
		exit(0);
	}

	/* now execute as child process */
	umask(0);

	/* get a unique session id */
	sid = setsid();
	if (sid < 0) {
		exit(1);
	}

	/* change work directory to /, prevent to lock a directory */
	if ((chdir("/")) < 0) {
		exit(1);
	}

	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
}

int main(int argc, char *argv[]) {
	int fd;
	int i;
	htsp_info info[2] = {};

	ctrl = &control;
	parseopt(argc, argv);

	/* daemonzie */
	if (ctrl->daemonize)
		daemonize();

	fd = mxhtsp_open();
	if (fd < 0) {
		mprintf("can't open device\n");
		exit(1);
	}

	/* signal handler */
	signal(SIGTERM, sig_handler);
	signal(SIGINT, sig_handler);
	signal(SIGKILL, sig_handler);

	create_pid_file();

	while (1) {

		if (ctrl->exit)
			break;

		for (i = 1; i <= 2; i++) {
			handle_disk_plugged(fd, i, info);
			handle_button_pressed(fd, i, info);
		}

		msleep(TIME_SLEEP_MS);
	}

	mprintf("Stopping...\n");
	remove_pid_file();
	closelog();
	mxhtsp_set_led(fd, 1, 0);
	mxhtsp_set_led(fd, 2, 0);
	close(fd);
	return 0;
}
