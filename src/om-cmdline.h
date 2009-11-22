#ifndef OM_CMDLINE_H
#define OM_CMDLINE_H

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

struct opt_t
{
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
};

extern const char* argv0;
extern struct opt_t opts;

extern int om_flags_sysfs;
void usage_sysfs(FILE* out);
int do_sysfs(int argc, char *const *argv);

extern int om_flags_backlight;
void usage_backlight(FILE* out);
int do_backlight(int argc, char *const *argv);

extern int om_flags_touchscreen;
void usage_touchscreen(FILE* out);
int do_touchscreen(int argc, char *const *argv);

extern int om_flags_gsm;
void usage_gsm(FILE* out);
int do_gsm(int argc, char *const *argv);

extern int om_flags_resume_reason;
void usage_resume_reason(FILE* out);
int do_resume_reason(int argc, char *const *argv);

#define OM_FLAGS_LED_STANDALONE 1
extern int om_flags_led;
void usage_led(FILE* out, const char* ledname);
int do_led(int argc, char *const *argv);

void usage_options(FILE* out);

int parse_options(int argc, char *const *argv);

#endif
