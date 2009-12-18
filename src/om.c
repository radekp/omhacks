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
#include <getopt.h>
#include "om-cmdline.h"

static void usage_help(FILE* out)
{
	fprintf(out, "Usage: %s help\n", argv0);
}

static void usage(FILE* out)
{
	usage_help(out);
	usage_sysfs(out);
	usage_backlight(out);
	usage_touchscreen(out);
	usage_bt(out);
	usage_gsm(out);
	usage_gps(out);
	usage_wifi(out);
	usage_battery(out);
	usage_power(out);
	usage_resume_reason(out);
	usage_led(out, NULL);
	usage_uevent(out);
	usage_usb(out);
	usage_options(out);
}

static int do_help(int argc, char *const *argv)
{
	usage(stdout);
	return 0;
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
		return do_help(argc, argv);

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
	else if (strcmp(argv[0], "touchscreen") == 0)
		return do_touchscreen(argc, argv);
	else if (strcmp(argv[0], "bt") == 0)
		return do_bt(argc, argv);
	else if (strcmp(argv[0], "gsm") == 0)
		return do_gsm(argc, argv);
	else if (strcmp(argv[0], "gps") == 0)
		return do_gps(argc, argv);
	else if (strcmp(argv[0], "wifi") == 0)
		return do_wifi(argc, argv);
	else if (strcmp(argv[0], "battery") == 0)
		return do_battery(argc, argv);
	else if (strcmp(argv[0], "power") == 0)
		return do_power(argc, argv);
	else if (strcmp(argv[0], "resume-reason") == 0)
		return do_resume_reason(argc, argv);
	else if (strcmp(argv[0], "led") == 0)
		return do_led(argc, argv);
	else if (strcmp(argv[0], "uevent") == 0)
		return do_uevent(argc, argv);
	else if (strcmp(argv[0], "usb") == 0)
		return do_usb(argc, argv);
	else {
		fprintf(stderr, "Unknown argument: %s\n", argv[0]);
		return 1;
	}

	return 0;
}
