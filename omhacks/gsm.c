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
#include "gsm.h"
#include "sysfs.h"
#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>

static char gsm_power_path[PATH_MAX];
static int gsm_power_path_found = 0;
static char gsm_reset_path[PATH_MAX];
static int gsm_reset_path_found = 0;

static const char* om_gsm_power_path()
{
	if (!gsm_power_path_found)
	{
		const char* root = om_sysfs_path("pm-gsm");
		if (root == NULL) return NULL;
		snprintf(gsm_power_path, PATH_MAX, "%s/power_on", root);
		if (access(gsm_power_path, F_OK) != 0) return NULL;
		gsm_power_path_found = 1;
	}
	return gsm_power_path;
}

static const char* om_gsm_reset_path()
{
	if (!gsm_reset_path_found)
	{
		const char* root = om_sysfs_path("pm-gsm");
		if (root == NULL) return NULL;
		snprintf(gsm_reset_path, PATH_MAX, "%s/reset", root);
		if (access(gsm_reset_path, F_OK) != 0) return NULL;
		gsm_reset_path_found = 1;
	}
	return gsm_reset_path;
}

int om_gsm_power_get()
{
	const char* path = om_gsm_power_path();
	if (path == NULL) return -1;
	const char* val = om_sysfs_readfile(path);
	if (val == NULL) return -1;
	return atoi(val);
}

int om_gsm_power_set(int value)
{
	// This feels rather silly at the moment, but it makes sense to have
	// some consistency in the various get/set/swap APIs.
	int res = om_gsm_power_swap(value);
	if (res < 0) return res;
	return 0;
}

int om_gsm_power_swap(int value)
{
	const char* power_path = om_gsm_power_path();
	if (power_path == NULL) return -1;

	const char* val = om_sysfs_readfile(power_path);
	if (val == NULL) return -1;
	int old_val = atoi(val);

	// If we are setting it to what it already is, don't bother
	if (value == old_val) return old_val;

	if (value)
	{
		// Note: the logic here is Enrico cargo-culting Lindi

		// Turn on
		const char* reset_path = om_gsm_reset_path();
		if (reset_path == NULL) return -1;

		if (om_sysfs_writefile(power_path, "0\n") < 0) return -1;
		if (om_sysfs_writefile(power_path, "1\n") < 0) return -1;
		if (om_sysfs_writefile(reset_path, "1\n") < 0) return -1;
	} else {
		// Turn off
		if (om_sysfs_writefile(power_path, "0\n") < 0) return -1;
	}
	return old_val;
}
