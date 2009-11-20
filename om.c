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

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "libomhacks.h"

static const char* argv0;

static void led_usage(FILE* out)
{
	fprintf(out, "Usage: %s led name\n", argv0);
	fprintf(out, "       %s led name brightness\n", argv0);
	fprintf(out, "       %s led name brightness ontime offtime\n", argv0);
}

int main(int argc, const char* argv[])
{
	argv0 = argv[0];

	if (argc == 1)
	{
		fprintf(stderr, "Usage: %s action [params]\n", argv[0]);
		return 1;
	}
	if (strcmp(argv[1], "sysfs") == 0)
	{
		int i;
		if (argc == 2)
		{
			fprintf(stderr, "Usage: %s sysfs name [name...]\n", argv[0]);
			return 1;
		}
		for (i = 2; i < argc; ++i)
		{
			const char* res = om_sysfs_path(argv[i]);
			if (res == NULL)
			{
				fprintf(stderr, "%s not found\n", argv[i]);
				return 1;
			}
			puts(res);
		}
	} else if (strcmp(argv[1], "backlight") == 0) {
		if (argc == 2)
		{
			const char* res = om_sysfs_get("brightness");
			if (res == NULL)
			{
				perror("getting brightness: ");
				return 1;
			}
			fputs(res, stdout);
		}
		else
		{
			const char* res = om_sysfs_swap("brightness", argv[2]);
			if (res == NULL)
			{
				perror("getting/setting brightness: ");
				return 1;
			}
			fputs(res, stdout);
			/*
			int res = om_sysfs_set("brightness", argv[2]);
			if (res < 0)
			{
				perror("setting brightness: ");
				return 1;
			} else if (res > 0) {
				fprintf(stderr, "partial write to /sys of %d bytes", res);
				return 1;
			}
			*/
		}
	} else if (strcmp(argv[1], "led") == 0) {
		struct om_led led;
		if (argc == 2)
		{
			led_usage(stderr);
			return 1;
		}

		if (om_led_init(&led, argv[2]) != 0)
		{
			perror("validating led name: ");
			return 1;
		}

		if (argc == 3)
		{
			// Get status
			if (om_led_get(&led) != 0)
			{
				perror("getting led status: ");
				return 1;
			}
			if (strcmp(led.trigger, "none") == 0)
			{
				printf("%d\n", led.brightness);
			} else if (strcmp(led.trigger, "timer") == 0) {
				printf("%d timer %d %d\n", led.brightness, led.delay_on, led.delay_off);
			} else {
				printf("%d %s\n", led.brightness, led.trigger);
			}
		} else {
			led.brightness = atoi(argv[3]);

			if (argc > 4)
			{
				if (strcmp(argv[4], "timer") == 0 && argc == 6)
				{
					strcpy(led.trigger, "timer");
					led.delay_on = atoi(argv[4]);
					led.delay_off = atoi(argv[5]);
				} else if (strcmp(argv[4], "timer") != 0) {
					strncpy(led.trigger, argv[4], 19);
					led.trigger[19] = 0;
				} else {
					fprintf(stderr, "Usage: %s led %s timer <delay_on> <delay_off>\n", argv[0], argv[2]);
					fprintf(stderr, "Usage: %s led %s <trigger>\n", argv[0], argv[2]);
					return 1;
				}
			}

			if (om_led_set(&led) != 0)
			{
				perror("setting led status: ");
				return 1;
			}
		}
	} else {
		fprintf(stderr, "Unknown argument: %s\n", argv[1]);
		return 1;
	}

	return 0;
}
