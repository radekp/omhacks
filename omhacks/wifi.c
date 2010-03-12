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
#include "wifi.h"
#include "sysfs.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <linux/types.h>
#include <linux/if.h>
#include <linux/wireless.h>

#define WIFI_ROOT "/sys/bus/platform/drivers/s3c2440-sdi"
#define WIFI_BIND WIFI_ROOT "/bind"
#define WIFI_UNBIND WIFI_ROOT "/unbind"
#define WIFI_BOUND WIFI_ROOT "/s3c2440-sdi"
#define WIFI_MCI_PERSIST_PATH "/sys/module/s3cmci/parameters/persist"

#define AR6000_XIOCTRL_WMI_GET_POWER_MODE 34
#define AR6000_IOCTL_EXTENDED (SIOCIWFIRSTPRIV+31)
#define AR6000_IOCTL_WMI_SETPWR (SIOCIWFIRSTPRIV+11)
#define MAX_PERF_POWER 2

int om_wifi_power_get()
{
	int res = access(WIFI_BOUND, F_OK);
	if (res == 0) return 1;
	if (errno == ENOENT) return 0;
	return res;
}

int om_wifi_power_set(int value)
{
	// This feels rather silly at the moment, but it makes sense to have
	// some consistency in the various get/set/swap APIs.
	int res = om_wifi_power_swap(value);
	if (res < 0) return res;
	return 0;
}

int om_wifi_power_swap(int value)
{
	int old_val = om_wifi_power_get();

	// If we are setting it to what it already is, don't bother
	if (value == old_val) return old_val;

	if (value)
	{
		// Turn on
		if (om_sysfs_writefile(WIFI_BIND, "s3c2440-sdi\n") < 0) return -1;
	} else {
		// Turn off
		if (om_sysfs_writefile(WIFI_UNBIND, "s3c2440-sdi\n") < 0) return -1;
	}

	return old_val;
}

int om_wifi_maxperf_get(const char *ifname)
{
	int sock, ret;
	struct ifreq ifr;
	int buf[64];

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) return -1;

	memset(ifr.ifr_name, '\0', sizeof(ifr.ifr_name));
	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));

	buf[0] = AR6000_XIOCTRL_WMI_GET_POWER_MODE;
	ifr.ifr_data = buf;

	ret = ioctl(sock, AR6000_IOCTL_EXTENDED, &ifr);
	if (ret != 0) return -2;

	return ((char *)buf)[0] == MAX_PERF_POWER;
}


int om_wifi_maxperf_set(const char *ifname, int mode)
{
	int sock, ret;
	struct ifreq ifr;
	int buf[64];

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) return -1;

	memset(ifr.ifr_name, '\0', sizeof(ifr.ifr_name));
	strncpy(ifr.ifr_name, ifname, sizeof(ifr.ifr_name));

	buf[0] = mode ? MAX_PERF_POWER : 0;
	ifr.ifr_data = buf;

	ret = ioctl(sock, AR6000_IOCTL_WMI_SETPWR, &ifr);
	if (ret != 0) return -2;

	return 0;
}

int om_wifi_keep_bus_on_in_suspend_get()
{
    const char* val = om_sysfs_readfile(WIFI_MCI_PERSIST_PATH);
    if (val == NULL) return -1;
    return atoi(val);
}

int om_wifi_keep_bus_on_in_suspend_set(int value)
{
    return om_sysfs_writefile(WIFI_MCI_PERSIST_PATH, value ? "1\n" : "0\n");
}
