#define TIME_SLEEP_MS 100  /* ms */
#define TIME_BTN_LONG_PRESSED 2 /* sec, please add mlseep time */
#define TIME_PARTITION_CHECK 60 /* sec */
#define TIME_DELAY_CHECK_USAGE 10 /* delay for os to recognize partition */
#define PID_FILE "/var/run/mxhtspd.pid"

#ifdef DEBUG
	#define d(fmt, args...) printf("%s: "fmt, __FUNCTION__, ##args)
	#define dd(fmt, args...)
#else 
	#define d(fmt, args...) 
	#define dd(fmt, args...)
#endif 


typedef struct _htsp_info {
	int disk_plugged;
	int btn_last_pressed;	/* set when the last time button is pressed */
	int btn_event_start;	/* set when button change from unpressed mode to pressed mode */
	int btn_time_accum;
	int led_on;
	int led_warn;			/* set when a partiton on disk is over usage limitation */
	int led_last_warn;		/* set when the last time led is warning */
	int first_insert;       /* Ignore btn status when disk is being insert. */
	clock_t press_time;
} htsp_info;

typedef struct _daemon_ctrl {
	int time_btn_long;
	int time_chk_part;
	int force_chk_part;		/* when disk is plugged, force to recheck */
	int log_facility;		/* set -1 if null, or 0 to 7 for local0 ~ local7 */
	int daemonize;
	int verbose;
	int exit;
} daemon_ctrl;

typedef struct _clist {
	char *mpoint;
	int percent;
	struct _clist *next;
} clist;

typedef struct _parg {
	int fd;
	htsp_info *info;
	clist *cl;
} parg;

