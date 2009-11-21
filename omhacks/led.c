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
#include "led.h"
#include "sysfs.h"
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

int om_led_init(struct om_led* led, const char* name)
{
	if (strchr(name, '/') != NULL)
	{
		errno = EINVAL;
		return -1;
	}
	strncpy(led->name, name, 254); led->name[254] = 0;
	led->dir = (char*)malloc(PATH_MAX);
	if (led->dir == NULL) return -1;
	led->dir_len = snprintf(led->dir, PATH_MAX, "/sys/class/leds/%s/", name);
	strcpy(led->trigger, "none");
	led->brightness = led->delay_on = led->delay_off = 0;
	return 0;
}

int om_led_init_copy(struct om_led* dstled, const struct om_led* srcled)
{
	dstled->dir = strdup(srcled->dir);
	if (dstled->dir == NULL) return -1;
	strncpy(dstled->name, srcled->name, 254); dstled->name[254] = 0;
	dstled->dir_len = srcled->dir_len;
	strcpy(dstled->trigger, srcled->trigger);
	dstled->brightness = srcled->brightness;
	dstled->delay_on = srcled->delay_on;
	dstled->delay_off = srcled->delay_off;
	return 0;
}

static const char* led_get(struct om_led* led, const char* param)
{
	strncpy(led->dir + led->dir_len, param, PATH_MAX - led->dir_len);
	led->dir[PATH_MAX-1] = 0;
	return om_sysfs_readfile(led->dir);
}

static int led_set(struct om_led* led, const char* param, const char* val)
{
	strncpy(led->dir + led->dir_len, param, PATH_MAX - led->dir_len);
	led->dir[PATH_MAX-1] = 0;
	return om_sysfs_writefile(led->dir, val);
}

static int led_read_current_trigger(struct om_led* led)
{
	const char* trigger = led_get(led, "trigger");
	int s, t, copy = 0;
	if (trigger == NULL) return -1;
	for (s = t = 0; trigger[s] != 0 && trigger[s] != '\n' && t < 254; ++s)
	{
		switch (trigger[s])
		{
			case '[': copy = 1; break;
			case ']': copy = 0; break;
			default: if (copy) led->trigger[t++] = trigger[s]; break;
		}
	}
	if (t == 0)
		strcpy(led->trigger, "none");
	else
		led->trigger[t] = 0;
	return 0;
}

int om_led_get(struct om_led* led)
{
	const char* res;

	if ((res = led_get(led, "brightness")) == NULL) return -1;
	led->brightness = atoi(res);

	if (led_read_current_trigger(led) != 0) return -1;

	if (strcmp(led->trigger, "timer") == 0)
	{
		if ((res = led_get(led, "delay_on")) == NULL) return -1;
		led->delay_on = atoi(res);

		if ((res = led_get(led, "delay_off")) == NULL) return -1;
		led->delay_off = atoi(res);
	} else {
		led->delay_on = led->delay_off = 0;
	}

	if (strcmp(led->trigger, "none") != 0 && led->brightness == 0)
		led->brightness = 255;

	return 0;
}

int om_led_set(struct om_led* led)
{
	char val[30];

	snprintf(val, 30, "%d\n", led->brightness);
	if (led_set(led, "brightness", val) != 0) return -1;

	snprintf(val, 30, "%s\n", led->trigger);
	if (led_set(led, "trigger", val) != 0) return -1;

	if (strcmp(led->trigger, "timer") == 0)
	{
		snprintf(val, 30, "%d\n", led->delay_on);
		if (led_set(led, "delay_on", val) != 0) return -1;

		snprintf(val, 30, "%d\n", led->delay_off);
		if (led_set(led, "delay_off", val) != 0) return -1;
	}
	return 0;
}
