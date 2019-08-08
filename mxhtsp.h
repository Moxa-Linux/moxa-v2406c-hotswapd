/*
  Copyright (C) MOXA Inc. All rights reserved.
  This software is distributed under the terms of the
  MOXA License.  See the file COPYING-MOXA for details.
*/
#ifndef _MXHTSP_H_
#define _MXHTSP_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mxhtsp_ioctl.h"

#ifdef WIN32
#include <windows.h>
#endif



/**
 * Set the led to on /off
 * @param fd the open port
 * @param led_num the led number
 * @param on 1:on 0:off
 * @return  0 on success, otherwise return -1;
 */
int mxhtsp_set_led(int fd, int led_num, int on);

/**
 * Check if a button is pressed
 * @param fd the open port
 * @param btn_num the button number 
 * @return  1:pressed 0:not pressed -1:fail
 */
int mxhtsp_is_button_pressed(int fd, int btn_num);

/**
 * Check if a disk is busy
 * @param fd the open port
 * @param disk_num the disk number 
 * @return  1:busy 0:idle -1:fail
 */
int mxhtsp_is_disk_busy(int fd, int disk_num);

/**
 * Check if a disk is plugged
 * @param fd the open port
 * @param disk_num the disk number 
 * @return  1:plugged 0:unplugged -1:fail
 */
int mxhtsp_is_disk_plugged(int fd, int disk_num);

/**
 * Open the hotswap devices
 * @return	fd on success, otherwise returns -1
 */
int mxhtsp_open(void);

/**
 * Get the used percentage of a partition
 * @param partition_name the name of partition being checked. In linux, it should be /media/diskxpx
 * @return	percentage the used percentage, otherwise return -1
 */
int mxhtsp_check_partition_usage(const char *partition_name);

/**
 * Close the hotswap devies
 * @param fd the open port
 * @return  None
 */
#ifdef WIN32
void mxhtsp_close(int fd);
#else
#define mxhtsp_close(fd) close(fd)
#endif


#ifdef __cplusplus
}
#endif

#endif /* _MXHTSP_H_ */
