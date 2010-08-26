/*
 * omhacks - Various useful utility functions for the FreeRunner
 *
 * Copyright (C) 2009  Enrico Zini <enrico@enricozini.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#include "screen.h"
#include "sysfs.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <linux/fb.h>

#ifndef EVIOCGRAB
#define EVIOCGRAB 0x40044590
#endif

int om_screen_brightness_get()
{
	const char* res = om_sysfs_get("brightness");
	if (res == NULL) return -1;
	return atoi(res);
}

static int om_screen_actual_brightness_get()
{
	const char* res = om_sysfs_get("actual_brightness");
	if (res == NULL) return -1;
	return atoi(res);
}

int om_screen_brightness_get_max()
{
	const char* res = om_sysfs_get("max_brightness");
	if (res == NULL) return -1;
	return atoi(res);
}

int om_screen_brightness_set(int val)
{
	char sval[20];
	snprintf(sval, 20, "%d", val);
	return om_sysfs_set("brightness", sval) == 0 ? 0 : -1;
}

int om_screen_brightness_swap(int val)
{
	char sval[20];
	snprintf(sval, 20, "%d", val);
	const char* res = om_sysfs_swap("brightness", sval);
	if (res == NULL) return -1;
	return atoi(res);
}

int om_touchscreen_open()
{
	return open("/dev/input/event1", O_RDONLY);
}

int om_touchscreen_lock(int fd)
{
	return ioctl(fd, EVIOCGRAB, 1);
}

int om_touchscreen_unlock(int fd)
{
	return ioctl(fd, EVIOCGRAB, 0);
}

int om_screen_power_get()
{
	int brightness, actual_brightness;

	if ((brightness = om_screen_brightness_get()) < 0) return -1;
	if ((actual_brightness = om_screen_actual_brightness_get()) < 0) return -2;
        /* This is a hack but there is really no other way to retrieve
         * this information from userland. Xorg never reads power
         * status and fso-frameworkd uses this same trick on startup. */
	return brightness == actual_brightness;
}

int om_screen_power_set(int val)
{
	int fd, ret;

	fd = open("/dev/fb0", O_RDWR);
	if (fd < 0) return -1;

        ret = ioctl(fd, FBIOBLANK, val ? FB_BLANK_UNBLANK : FB_BLANK_POWERDOWN);
	if (ret != 0) return -2;

	return 0;
}

const char* om_screen_resolution_get()
{
	const char* res = om_sysfs_get("screen_resolution");
	if (!res) return NULL;
	if (strcmp(res, "normal\n") == 0)
		return "normal";
	if (strcmp(res, "qvga-normal\n") == 0)
		return "qvga-normal";
	return NULL;
}

int om_screen_resolution_set(const char *val)
{
	return om_sysfs_set("screen_resolution", val);
}
