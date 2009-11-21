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
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

#ifndef EVIOCGRAB
#define EVIOCGRAB 0x40044590
#endif

int om_screen_brightness_get()
{
	const char* res = om_sysfs_get("brightness");
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