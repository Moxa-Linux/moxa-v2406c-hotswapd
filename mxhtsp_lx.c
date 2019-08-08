/*  Copyright (C) MOXA Inc. All rights reserved.

    This software is distributed under the terms of the
    MOXA License.  See the file COPYING-MOXA for details.

    2010-12-10 Wade Liang Wade.Liang@moxa.com
		new release
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include "mxhtsp.h" /* prototype */

int mxhtsp_set_led(int fd, int led_num, int on) 
{
	mxhtsp_param p;

	p.disk_num = led_num;
	p.val = on;
	return ioctl(fd, IOCTL_SET_DISK_LED, &p);
}

int mxhtsp_is_button_pressed(int fd, int btn_num)
{
	mxhtsp_param p;

	p.disk_num = btn_num;
	p.val = 0;
	if (ioctl(fd, IOCTL_GET_DISK_BTN, &p)) {
		return -1;
	} else {
		return p.val;
	}

}

int mxhtsp_is_disk_busy(int fd, int disk_num) 
{
	mxhtsp_param p;

	p.disk_num = disk_num;
	p.val = 0;
	if (ioctl(fd, IOCTL_GET_DISK_STATUS, &p)) 
		return -1;
	else  
		return p.val;
}

int mxhtsp_is_disk_plugged(int fd, int disk_num) 
{
	mxhtsp_param p;

	p.disk_num = disk_num;
	p.val = 0;
	if (ioctl(fd, IOCTL_CHECK_DISK_PLUGGED, &p)) 
		return -1;
	else 
		return p.val;
}

int mxhtsp_open(void)
{
	int fd;
	fd = open("/dev/hotswap", O_RDWR);
	return fd;
}

int mxhtsp_check_partition_usage(const char *mount_point) {
   	FILE *fp = NULL;
	char buf[BUFSIZ];
	char *delim=" \n";
	char cmd[50];
	char *token_percent;
	char *token_path;
	int percent;

	sprintf(cmd, "df -h %s 2>&1", mount_point);
	if (!(fp = popen(cmd, "r"))) {
		perror("can't execute popen");
		return -1;
	}

	/* skip the first time  */
	if (!fgets(buf, BUFSIZ, fp)) 
		goto err;

	while (fgets(buf, BUFSIZ, fp)) {
		if (!strtok(buf, delim)) break;
		if (!strtok(NULL, delim)) break;
		if (!strtok(NULL, delim)) break;
		if (!strtok(NULL, delim)) break;
		if (!(token_percent = strtok(NULL, delim))) break;

		if (!(token_path = strtok(NULL, delim))) {
			break;
		}
		else if (strcmp(token_path, mount_point) == 0) {
			sscanf(token_percent, "%d%%", &percent);
			fclose(fp);
			return percent;
		}
	}
err:
	pclose(fp);
	return -1;
}


