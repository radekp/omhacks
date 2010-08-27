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

                if (strcmp(line + 2, "EINT09_PMU\n") != 0)
                {
			orr.arr[orr.arr_size] = orr.buf + orr.buf_size;
			orr_append_str(line + 2);
			++orr.arr_size;
                } else {
			const char* reason2;
			unsigned long long resume_reason_pmu;
			
			reason2 = om_sysfs_get("resume_reason2");
			if (reason2 == NULL) return NULL;
			if (strlen(reason2) < 10) return NULL;
			resume_reason_pmu = strtoull(reason2, NULL, 16);
			
			while (resume_reason_pmu)
			{
				if (resume_reason_pmu & 0x0002000000ULL)
				{
					resume_reason_pmu &= ~0x0002000000ULL;
					orr.arr[orr.arr_size] = orr.buf + orr.buf_size;
					orr_append_str(line + 2);
					orr.buf[orr.buf_size-1] = ':';
					orr_append_str("button");
				} else if (resume_reason_pmu & 0x0400000000ULL) {
					resume_reason_pmu &= ~0x0400000000ULL;
					orr.arr[orr.arr_size] = orr.buf + orr.buf_size;
					orr_append_str(line + 2);
					orr.buf[orr.buf_size-1] = ':';
					orr_append_str("usb_connect");
				} else if (resume_reason_pmu & 0x4000000000ULL) {
					resume_reason_pmu &= ~0x4000000000ULL;
					orr.arr[orr.arr_size] = orr.buf + orr.buf_size;
					orr_append_str(line + 2);
					orr.buf[orr.buf_size-1] = ':';
					orr_append_str("rtc_alarm");
				} else if (resume_reason_pmu & 0x0800000000ULL) {
					resume_reason_pmu &= ~0x0800000000ULL;
					orr.arr[orr.arr_size] = orr.buf + orr.buf_size;
					orr_append_str(line + 2);
					orr.buf[orr.buf_size-1] = ':';
					orr_append_str("usb_disconnect");
				} else {
					resume_reason_pmu = 0;
					orr.arr[orr.arr_size] = orr.buf + orr.buf_size;
					orr_append_str(line + 2);
					orr.buf[orr.buf_size-1] = ':';
					orr_append_str(reason2);
				}
				++orr.arr_size;
			}
                }
	}
	fclose(in);
	orr.arr[orr.arr_size] = NULL;
	return (const char**)orr.arr;
}

