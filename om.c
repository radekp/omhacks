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
#include <getopt.h>
#include "libomhacks.h"

static const char* argv0;
static struct opt_t {
	int help;
	/*
	int version;
	int verbose;
	int brightness;
	int ontime;
	int offtime;
	const char *led;
	int on;
	int off;
	int blink;
	int no_blink;
	*/
	int swap;
} opts;

static void usage_sysfs(FILE* out)
{
	fprintf(out, "Usage: %s sysfs name [name...]\n", argv0);
}

static int do_sysfs(int argc, char *const *argv)
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

static void usage_backlight(FILE* out)
{
	fprintf(out, "Usage: %s backlight\n", argv0);
	fprintf(out, "Usage: %s backlight <brightness>\n", argv0);
}

static int do_backlight(int argc, char *const *argv)
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

static void usage_led(FILE* out, const char* ledname)
{
	if (ledname == NULL) ledname = "<name>";
	fprintf(out, "Usage: %s led %s\n", argv0, ledname);
	fprintf(out, "Usage: %s led %s <brightness>\n", argv0, ledname);
	fprintf(out, "Usage: %s led %s <brightness> timer <ontime> <offtime>\n", argv0, ledname);
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
		strncpy(led->trigger, argv[0], 19);
		led->trigger[19] = 0;
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

static int do_led(int argc, char *const *argv)
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

static void usage_help(FILE* out)
{
	fprintf(out, "Usage: %s help\n", argv0);
}

static void usage(FILE* out)
{
	usage_help(out);
	usage_sysfs(out);
	usage_backlight(out);
	usage_led(out, NULL);

	fprintf(stderr, "Options:\n");
	fprintf(stderr, "  --help: print this help message\n");
	fprintf(stderr, "  --swap: set new value and print old value\n");
}

static int do_help(int argc, char *const *argv)
{
	usage(stdout);
	return 0;
}

static int parse_options(int argc, char *const *argv)
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

int main(int argc, char *const *argv)
{
	if (parse_options(argc, argv) != 0)
	{
		usage(stderr);
		return 1;
	}
	argc -= optind;
	argv += optind;

	if (opts.help)
	{
		usage(stdout);
		return 0;
	}

	if (argc == 0)
	{
		usage(stderr);
		return 1;
	}
	if (strcmp(argv[0], "help") == 0)
		return do_help(argc, argv);
	else if (strcmp(argv[0], "sysfs") == 0)
		return do_sysfs(argc, argv);
	else if (strcmp(argv[0], "backlight") == 0)
		return do_backlight(argc, argv);
	else if (strcmp(argv[0], "led") == 0)
		return do_led(argc, argv);
	else {
		fprintf(stderr, "Unknown argument: %s\n", argv[1]);
		return 1;
	}

	return 0;
}
