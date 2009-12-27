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
#include "gps.h"
#include "sysfs.h"
#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

static char gps_power_path[PATH_MAX];
static int gps_power_path_found = 0;

static char gps_keep_on_in_suspend_path[PATH_MAX];
static int gps_keep_on_in_suspend_path_found = 0;

static const char* om_gps_power_path()
{
	if (!gps_power_path_found)
	{
		const char* root = om_sysfs_path("pm-gps");
		if (root == NULL) return NULL;
		snprintf(gps_power_path, PATH_MAX, "%s/power_on", root);
		if (access(gps_power_path, F_OK) != 0) return NULL;
		gps_power_path_found = 1;
	}
	return gps_power_path;
}

static const char* om_gps_keep_on_in_suspend_path()
{
	if (!gps_keep_on_in_suspend_path_found)
	{
		const char* root = om_sysfs_path("pm-gps");
		if (root == NULL) return NULL;
		snprintf(gps_keep_on_in_suspend_path, PATH_MAX, "%s/keep_on_in_suspend", root);
		if (access(gps_keep_on_in_suspend_path, F_OK) != 0) return NULL;
		gps_keep_on_in_suspend_path_found = 1;
	}
	return gps_keep_on_in_suspend_path;
}

int om_gps_power_get()
{
	const char* path = om_gps_power_path();
	if (path == NULL) return -1;
	const char* val = om_sysfs_readfile(path);
	if (val == NULL) return -1;
	return atoi(val);
}

int om_gps_power_set(int value)
{
	// This feels rather silly at the moment, but it makes sense to have
	// some consistency in the various get/set/swap APIs.
	int res = om_gps_power_swap(value);
	if (res < 0) return res;
	return 0;
}

int om_gps_power_swap(int value)
{
	const char* power_path = om_gps_power_path();
	if (power_path == NULL) return -1;

	const char* val = om_sysfs_readfile(power_path);
	if (val == NULL) return -1;
	int old_val = atoi(val);

	// If we are setting it to what it already is, don't bother
	if (value == old_val) return old_val;

	if (value)
	{
		// Turn on
		if (om_sysfs_writefile(power_path, "1\n") < 0) return -1;
	} else {
		// Note: the logic here is Enrico cargo-culting Lindi

		// Turn off
		if (om_sysfs_writefile(power_path, "0\n") < 0) return -1;

		// Is it off?
		const char* val = om_sysfs_readfile(power_path);
		if (val == NULL) return -1;
		if (val[0] == '1')
		{
			// No, try again
			if (om_sysfs_writefile(power_path, "1\n") < 0) return -1;
			if (om_sysfs_writefile(power_path, "0\n") < 0) return -1;
		}
	}
	return old_val;
}


int om_gps_keep_on_in_suspend_get()
{
	const char* path = om_gps_keep_on_in_suspend_path();
	if (path == NULL) return -1;
	const char* val = om_sysfs_readfile(path);
	if (val == NULL) return -1;
	return atoi(val);
}

int om_gps_keep_on_in_suspend_set(int value)
{
	const char* path = om_gps_keep_on_in_suspend_path();
	if (path == NULL) return -1;

	return om_sysfs_writefile(path, value ? "1\n" : "0\n");
}

int om_gps_keep_on_in_suspend_swap(int value)
{
	const char* keep_on_in_suspend_path = om_gps_keep_on_in_suspend_path();
	if (keep_on_in_suspend_path == NULL) return -1;

	const char* val = om_sysfs_readfile(keep_on_in_suspend_path);
	if (val == NULL) return -1;
	int old_val = atoi(val);

	// If we are setting it to what it already is, don't bother
	if (value == old_val) return old_val;

	if (om_sysfs_writefile(keep_on_in_suspend_path, value ? "1\n" : "0\n") < 0)
		return -1;

	return old_val;
}

int om_gps_open(void)
{
    return open("/dev/ttySAC1", O_RDWR);
}

void om_gps_close(int fd)
{
    close(fd);
}

int om_gps_ubx_send(int fd, int klass, int type, const char *payload, int payloadlen)
{
    char *packet;
    int i, res = 0, packetlen = 2 + 1 + 1 + 2 + payloadlen + 2;
    ssize_t count;

    packet = malloc(packetlen);
    if (packet == NULL) return -1;

    packet[0] = 0xb5;
    packet[1] = 0x62;
    packet[2] = klass;
    packet[3] = type;
    packet[4] = payloadlen & 0xff;
    packet[5] = (payloadlen >> 8) & 0xff;

    for (i = 0; i < payloadlen; i++) {
        packet[6 + i] = payload[i];
    }

    {
        unsigned char sum[2] = { 0, 0 };

        for (i = 0; i < packetlen - 4; i++) {
            sum[0] += packet[2 + i];
            sum[1] += sum[0];
        }
        packet[packetlen - 2] = sum[0];
        packet[packetlen - 1] = sum[1];
    }

    count = write(fd, packet, packetlen);
    if (count != packetlen) res = -3;
    free(packet);

    return res;
}
