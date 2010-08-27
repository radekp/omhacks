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
#include "resumereason.h"
#include "sysfs.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ORR_BUFSIZE 512
#define ORR_ARRSIZE 32

static struct orr_t
{
	char buf[ORR_BUFSIZE];
	int buf_size;

	char* arr[ORR_ARRSIZE];
	int arr_size;
} orr;

static void orr_append_str(const char* line)
{
	if (orr.buf_size >= ORR_BUFSIZE) return;

	for ( ; *line && *line != '\n' && orr.buf_size < ORR_BUFSIZE-1; ++line)
		orr.buf[orr.buf_size++] = *line;
	orr.buf[orr.buf_size++] = 0;
}

const char** om_resume_reason()
{
	static char line[256];
	const char* reason2 = NULL;

	orr.buf_size = 0;
	orr.arr_size = 0;

	const char* fname = om_sysfs_path("resume_reason");
	if (fname == NULL) return NULL;

	FILE* in = fopen(fname, "r");
	if (in == NULL) return NULL;

	while (fgets(line, 256, in) && orr.buf_size < ORR_BUFSIZE && orr.arr_size < ORR_ARRSIZE - 1)
	{
		// Skip empty or inactive lines
		if (line[0] != '*' || line[1] == 0) continue;

		// Append resume reason from this line
		orr.arr[orr.arr_size] = orr.buf + orr.buf_size;
		orr_append_str(line + 2);

		// Do we have extra PMU reasons?
		if (orr.buf_size < ORR_BUFSIZE && strcmp(orr.arr[orr.arr_size], "EINT09_PMU") == 0)
		{
			unsigned long int resume_reason_pmu;
			if (reason2 == NULL)
			{
				// Read them only once
				reason2 = om_sysfs_get("resume_reason2");
				if (reason2 == NULL) return NULL;
				if (strlen(reason2) < 10) return NULL;
			}

			// Overwrite the ending 0 with a semicolon
			orr.buf[orr.buf_size-1] = ':';

			resume_reason_pmu = strtoul(reason2, NULL, 16);
			
			// Append the extra PMU reason
			if (resume_reason_pmu & 0x0002000000)
			{
				orr_append_str("button");
			} else if (resume_reason_pmu & 0x0400000000) {
				orr_append_str("usb_connect");
			} else if (resume_reason_pmu & 0x4000000000) {
				orr_append_str("rtc_alarm");
			} else if (resume_reason_pmu & 0x0800000000) {
				orr_append_str("usb_disconnect");
			} else {
				orr_append_str(reason2);
			}
		}
		++orr.arr_size;
	}
	fclose(in);
	orr.arr[orr.arr_size] = NULL;
	return (const char**)orr.arr;
}

