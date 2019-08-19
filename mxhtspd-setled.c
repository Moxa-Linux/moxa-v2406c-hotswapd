#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <signal.h>
#include "mxhtsp.h"

static void usage(void)
{
	printf("Usage: mxsetled led_num on\n");
	printf("\tled_num - 1 or 2\n");
	printf("\ton - 1:on 0:of\n");
	exit(1);
}

int main(int argc, char *argv[])
{
	int led_num;
	int on;
	int fd;

	/* open device */
	fd = mxhtsp_open();
	if (fd < 0) {
		printf("can't open device\n");
		exit(1);
	}

	if (argc != 3)
		usage();

	led_num = atoi(argv[1]);
	on = atoi(argv[2]);

	if (mxhtsp_set_led(fd, led_num, on))
		printf("%s error: led_num=%d on=%d\n", argv[0], led_num, on);

	close(fd);
	return 0;
}
