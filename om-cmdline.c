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

#include "libomhacks.h"
#include "om-cmdline.h"
#include <getopt.h>
#include <string.h>
#include <stdlib.h>

const char* argv0;
struct opt_t opts;

int om_flags_sysfs = 0;
int om_flags_backlight = 0;
int om_flags_resume_reason = 0;
int om_flags_led = 0;

void usage_sysfs(FILE* out)
{
	fprintf(out, "Usage: %s sysfs name [name...]\n", argv0);
}

int do_sysfs(int argc, char *const *argv)
{
	int i;
	if (argc == 1)
	{
		usage_sysfs(stderr);
		return 1;
	}
	for (i = 1; i < argc; ++i)
	{
		const char* res = om_sysfs_path(argv[i]);
		if (res == NULL)
		{
			fprintf(stderr, "%s not found\n", argv[i]);
			return 1;
		}
		puts(res);
	}
	return 0;
}

void usage_backlight(FILE* out)
{
	fprintf(out, "Usage: %s backlight\n", argv0);
	fprintf(out, "Usage: %s backlight <brightness>\n", argv0);
}

int do_backlight(int argc, char *const *argv)
{
	if (argc == 1)
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
		if (opts.swap)
		{
			const char* res = om_sysfs_swap("brightness", argv[1]);
			if (res == NULL)
			{
				perror("getting/setting brightness: ");
				return 1;
			}
			fputs(res, stdout);
		} else {
			int res = om_sysfs_set("brightness", argv[1]);
			if (res < 0)
			{
				perror("setting brightness: ");
				return 1;
			} else if (res > 0) {
				fprintf(stderr, "partial write to /sys of %d bytes", res);
				return 1;
			}
		}
	}
	return 0;
}

void usage_resume_reason(FILE* out)
{
	fprintf(out, "Usage: %s resume-reason\n", argv0);
}

int do_resume_reason(int argc, char *const *argv)
{
	const char* res = om_resume_reason();
	if (res == NULL)
	{
		perror("getting resume reason: ");
		return 1;
	}
	fputs(res, stdout);
	return 0;
}

void usage_led(FILE* out, const char* ledname)
{
	const char* ledcmd;
	if (om_flags_led & OM_FLAGS_LED_STANDALONE)
		ledcmd = "";
	else
		ledcmd = " led";

	if (ledname == NULL) ledname = "<name>";
	fprintf(out, "Usage: %s%s %s\n", argv0, ledcmd, ledname);
	fprintf(out, "Usage: %s%s %s <brightness>\n", argv0, ledcmd, ledname);
	fprintf(out, "Usage: %s%s %s <brightness> timer <ontime> <offtime>\n", argv0, ledcmd, ledname);
}

static int led_read_extra_args(struct om_led* led, int argc, char *const *argv)
{
	if (argc == 0)
	{
		strcpy(led->trigger, "none");
		led->delay_on = led->delay_off = 0;
	} else if (strcmp(argv[0], "timer") == 0 && argc == 3) {
		strcpy(led->trigger, "timer");
		led->delay_on = atoi(argv[1]);
		led->delay_off = atoi(argv[2]);
	} else if (strcmp(argv[0], "timer") != 0) {
		strncpy(led->trigger, argv[0], 254);
		led->trigger[254] = 0;
		led->delay_on = led->delay_off = 0;
	} else {
		usage_led(stderr, led->name);
		return 1;
	}
	return 0;
}

static void print_led(const struct om_led* led)
{
	if (strcmp(led->trigger, "none") == 0)
	{
		printf("%d\n", led->brightness);
	} else if (strcmp(led->trigger, "timer") == 0) {
		printf("%d timer %d %d\n", led->brightness, led->delay_on, led->delay_off);
	} else {
		printf("%d %s\n", led->brightness, led->trigger);
	}
}

int do_led(int argc, char *const *argv)
{
	struct om_led led;
	if (argc == 1)
	{
		usage_led(stderr, NULL);
		return 1;
	}

	if (om_led_init(&led, argv[1]) != 0)
	{
		perror("validating led name: ");
		return 1;
	}

	if (argc == 2)
	{
		// Get status
		if (om_led_get(&led) != 0)
		{
			perror("getting led status: ");
			return 1;
		}
		print_led(&led);
	} else {
		if (opts.swap)
		{
			if (om_led_get(&led) != 0)
			{
				perror("getting led status: ");
				return 1;
			}
			print_led(&led);
		}

		led.brightness = atoi(argv[2]);

		if (led_read_extra_args(&led, argc-3, argv+3) != 0)
			return 1;

		if (om_led_set(&led) != 0)
		{
			perror("setting led status: ");
			return 1;
		}
	}
	return 0;
}

void usage_options(FILE* out)
{
	fprintf(out, "Options:\n");
	fprintf(out, "  --help: print this help message\n");
	fprintf(out, "  --swap: set new value and print old value\n");
}

int parse_options(int argc, char *const *argv)
{
	argv0 = argv[0];

	memset(&opts, 0, sizeof(struct opt_t));
        int option_index = 0;
        
        static struct option long_options[] = {
            {"help", 0, &opts.help, 1},
	    /*
            {"version", 0, 0, 0},
            {"brightness", 1, 0, 0},
            {"on", 0, 0, 0},
            {"off", 0, 0, 0},
            {"on-time", 1, 0, 0},
            {"off-time", 1, 0, 0},
            {"blink", 0, 0, 0},
            {"no-blink", 0, 0, 0},
            {"verbose", 0, 0, 0},
	    */
            {"swap", 0, &opts.swap, 1},
            {0, 0, 0, 0}
        };
        
	while (1)
	{
		int ret = getopt_long(argc, argv, "", long_options, &option_index);
		switch (ret)
		{
			case -1: return 0;
			case 0: break;
			default: return -1;
		}
	}
}


