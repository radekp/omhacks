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
#include <string.h>

const char* om_resume_reason()
{
	static char buf[1024];
	const char* fname = om_sysfs_path("resume_reason");
	FILE* in;
	if (fname == NULL) return NULL;
	in = fopen(fname, "r");
	if (in == NULL) return NULL;
	while (fgets(buf, 1024, in))
	{
		if (buf[0] == '*' && buf[1] != 0)
		{
			int dst = 0, src = 2;
			for ( ; buf[src] && buf[src] != '\n'; ++src, ++dst)
				buf[dst] = buf[src];
			buf[dst] = 0;
			break;
		}
	}
	fclose(in);
	if (strcmp(buf, "EINT09_PMU") == 0)
	{
		const char* reason2 = om_sysfs_get("resume_reason2");
		if (reason2 == NULL) return NULL;
		if (strlen(reason2) < 10) return NULL;
		if (reason2[3] == '2')
		{
			strcat(buf, ":button");
		} else if (reason2[1] == '4') {
			strcat(buf, ":usb_connect");
		} else if (reason2[0] == '4') {
			strcat(buf, ":rtc_alarm");
		} else if (reason2[1] == '8') {
			strcat(buf, ":usb_disconnect");
		} else {
			strcat(buf, ":");
			strcat(buf, reason2);
			buf[strlen(buf)-1] = 0;
		}
	}
	return buf;
}
