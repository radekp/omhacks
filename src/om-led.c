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
#include <omhacks/omhacks.h>
#include "om-cmdline.h"

static void usage_help(FILE* out)
{
	fprintf(out, "Usage: %s help\n", argv0);
}

static void usage(FILE* out)
{
	usage_help(out);
	usage_led(out, NULL);
	usage_options(out);
}

static int do_help(int argc, char *const *argv)
{
	usage(stdout);
	return 0;
}

int main(int argc, char *const *argv)
{
	om_flags_led = OM_FLAGS_LED_STANDALONE;

	if (parse_options(argc, argv) != 0)
	{
		usage(stderr);
		return 1;
	}
	argc -= optind-1;
	argv += optind-1;

	if (opts.help)
		return do_help(argc, argv);

	if (argc == 0)
	{
		usage(stderr);
		return 1;
	}
	return do_led(argc, argv);

	return 0;
}
