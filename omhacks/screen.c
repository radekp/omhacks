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
#include <stdint.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
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

#define TIMINGS_444 0x1bc0
#define TIMINGS_242 0x1380

const char* om_screen_glamo_bus_timings_get()
{
	int fd;
	uint8_t* glamo;
	uint32_t timings;

	fd = open("/dev/mem", O_RDONLY);
	if (fd < 0)
		return NULL;
	
	glamo = mmap(NULL,
		     4096,
		     PROT_READ,
		     MAP_SHARED,
		     fd,
		     0x48000000);
	if (glamo == MAP_FAILED)
	{
		close(fd);
		return NULL;
	}
	timings = *(uint32_t*)(glamo + 8);
	munmap(glamo, 4096);
	close(fd);
	if (timings == TIMINGS_444)
		return "4-4-4";
	if (timings == TIMINGS_242)
		return "2-4-2";
	return NULL;
}

int om_screen_glamo_bus_timings_set(const char* val)
{
	int fd;
	uint8_t* glamo;
	uint32_t timings;

	if (strcmp(val, "4-4-4") == 0)
		timings = TIMINGS_444;
	else if (strcmp(val, "2-4-2") == 0)
		timings = TIMINGS_242;
	else
		return -1;

	fd = open("/dev/mem", O_RDWR);
	if (fd < 0)
		return -2;
	
	glamo = mmap(NULL,
		     4096,
		     PROT_READ | PROT_WRITE,
		     MAP_SHARED,
		     fd,
		     0x48000000);
	if (glamo == MAP_FAILED)
	{
		close(fd);
		return -3;
	}
	*(uint32_t*)(glamo + 8) = timings;
	munmap(glamo, 4096);
	close(fd);
	return 0;
}

