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
#include "sysfs.h"
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>

struct om_sysfs_name
{
	const char* name;
	const char* (*scanner)(void);
	const char* cached_value;
};

static int exists(const char* fname)
{
	return access(fname, F_OK) == 0;
}

static const char* scan_battery()
{
	// TODO sys_battery=`find /sys -wholename "*/power_supply/battery" -o -wholename "*/power_supply/bat" -type d`
	if (exists("/sys/class/power_supply/battery"))
		return "/sys/class/power_supply/battery";
	if (exists("/sys/class/power_supply/bat"))
		return "/sys/class/power_supply/bat";
	return NULL;
}

static const char* scan_brightness()
{
	// TODO sys_brightness=\"$(find /sys -wholename "*backlight*/brightness")\"
	if (exists("/sys/class/backlight/gta02-bl/brightness"))
		return "/sys/class/backlight/gta02-bl/brightness";
	if (exists("/sys/devices/virtual/backlight/acpi_video0/brightness"))
		return "/sys/devices/virtual/backlight/acpi_video0/brightness";
	return NULL;
}
static const char* scan_max_brightness()
{
	// TODO sys_brightness=\"$(find /sys -wholename "*backlight*/brightness")\"
	if (exists("/sys/class/backlight/gta02-bl/max_brightness"))
		return "/sys/class/backlight/gta02-bl/max_brightness";
	if (exists("/sys/devices/virtual/backlight/acpi_video0/max_brightness"))
		return "/sys/devices/virtual/backlight/acpi_video0/max_brightness";
	return NULL;
}
static const char* scan_actual_brightness()
{
	if (exists("/sys/class/backlight/gta02-bl/actual_brightness"))
		return "/sys/class/backlight/gta02-bl/actual_brightness";
	if (exists("/sys/devices/virtual/backlight/acpi_video0/actual_brightness"))
		return "/sys/devices/virtual/backlight/acpi_video0/actual_brightness";
	return NULL;
}
static const char* scan_chg_curlim()
{
	if (exists("/sys/class/i2c-adapter/i2c-0/0-0073/pcf50633-mbc/chg_curlim"))
		return "/sys/class/i2c-adapter/i2c-0/0-0073/pcf50633-mbc/chg_curlim";
	if (exists("/sys/devices/platform/s3c2440-i2c/i2c-0/0-0073/pcf50633-mbc/chg_curlim"))
		return "/sys/devices/platform/s3c2440-i2c/i2c-0/0-0073/pcf50633-mbc/chg_curlim";
	return NULL;
}
static const char* scan_resume_reason()
{
	// TODO return \"$(find /sys -wholename "*neo1973-resume.0/resume_reason")\"
	if (exists("/sys/class/i2c-adapter/i2c-0/0-0073/neo1973-resume.0/resume_reason"))
		return "/sys/class/i2c-adapter/i2c-0/0-0073/neo1973-resume.0/resume_reason";
	return NULL;
}
static const char* scan_resume_reason2()
{
	// TODO return \"$(find /sys -wholename "*/0-0073/resume_reason")\"
	if (exists("/sys/class/i2c-adapter/i2c-0/0-0073/resume_reason"))
		return "/sys/class/i2c-adapter/i2c-0/0-0073/resume_reason";
	return NULL;
}
static const char* scan_pm_bt()
{
	// TODO return \"$(find /sys -wholename "*pm-bt*/power_on" -o -wholename "*pm-bt*/pwron")\"
	if (exists("/sys/class/i2c-adapter/i2c-0/0-0073/pcf50633-regltr.6/neo1973-pm-bt.0"))
		return "/sys/class/i2c-adapter/i2c-0/0-0073/pcf50633-regltr.6/neo1973-pm-bt.0";
	return NULL;
}
static const char* scan_pm_gps()
{
	// TODO return $(find /sys -wholename "*pm-gps*/power_on" -o -wholename "*pm-gps*/pwron")
	if (exists("/sys/class/i2c-adapter/i2c-0/0-0073/pcf50633-regltr.7/neo1973-pm-gps.0"))
		return "/sys/class/i2c-adapter/i2c-0/0-0073/pcf50633-regltr.7/neo1973-pm-gps.0";
	if (exists("/sys/class/i2c-adapter/i2c-0/0-0073/pcf50633-regltr.7/gta02-pm-gps.0"))
		return "/sys/class/i2c-adapter/i2c-0/0-0073/pcf50633-regltr.7/gta02-pm-gps.0";
	return NULL;
}
static const char* scan_pm_gsm()
{
	// TODO return $(find /sys -name neo1973-pm-gsm.0 -type d)
	// TODO sys_pm_gsm_power=\"$(find /sys -wholename "*pm-gsm*/power_on" -o -wholename "*pm-gsm*/pwron")\"
	if (exists("/sys/class/i2c-adapter/i2c-0/0-0073/neo1973-pm-gsm.0"))
		return "/sys/class/i2c-adapter/i2c-0/0-0073/neo1973-pm-gsm.0";
	if (exists("/sys/class/i2c-adapter/i2c-0/0-0073/gta02-pm-gsm.0"))
		return "/sys/class/i2c-adapter/i2c-0/0-0073/gta02-pm-gsm.0";
	return NULL;
}
static const char* scan_pm_wlan()
{
	// TODO sys_pm_wlan=\"$(find /sys -wholename "*gta02-pm-wlan/gta02-pm-wlan.0")\"
	if (exists("/sys/bus/platform/drivers/gta02-pm-wlan/gta02-pm-wlan.0"))
		return "/sys/bus/platform/drivers/gta02-pm-wlan/gta02-pm-wlan.0";
	return NULL;
}

static struct om_sysfs_name om_sysfs_names[] = {
	{ "actual_brightness", scan_actual_brightness, NULL },
	{ "battery", scan_battery, NULL },
	{ "brightness", scan_brightness, NULL },
	{ "chg_curlim", scan_chg_curlim, NULL },
	{ "max_brightness", scan_max_brightness, NULL },
// TODO sys_force_usb_limit_dangerous=\"$(find /sys -name force_usb_limit_dangerous -o -name usb_curlim)\"
// TODO sys_hostmode=\"$(find /sys -name hostmode)\"
	{ "pm-bt", scan_pm_bt, NULL },
	{ "pm-gps", scan_pm_gps, NULL },
	{ "pm-gsm", scan_pm_gsm, NULL },
	{ "pm-wlan", scan_pm_wlan, NULL },
	{ "resume_reason", scan_resume_reason, NULL },
	{ "resume_reason2", scan_resume_reason2, NULL },
// TODO sys_usb_mode=\"$(find /sys -name usb_mode)\"
};
static const int om_sysfs_names_size = sizeof(om_sysfs_names) / sizeof(om_sysfs_names[0]);

static struct om_sysfs_name* om_sysfs_find_name(const char* name)
{
	int begin, end;

	/* Binary search */
	begin = -1, end = om_sysfs_names_size;
	while (end - begin > 1)
	{
		int cur = (end + begin) / 2;
		if (strcmp(om_sysfs_names[cur].name, name) > 0)
			end = cur;
		else
			begin = cur;
	}

	if (begin == -1 || strcmp(om_sysfs_names[begin].name, name) != 0)
		return NULL;
	else
		return &om_sysfs_names[begin];
}

const char* om_sysfs_path(const char* name)
{
	struct om_sysfs_name* n = om_sysfs_find_name(name);
	if (n == NULL) return NULL;
	if (n->cached_value == NULL)
		n->cached_value = n->scanner();
	return n->cached_value;
}

const char* om_sysfs_readfile(const char* pathname)
{
	static char buf[1024];
	const char* res = NULL;
	ssize_t count;
	int fd = -1;
	fd = open(pathname, O_RDONLY);
	if (fd < 0) return NULL;
	count = read(fd, buf, 1023);
	if (count < 0) goto cleanup;
	buf[count] = 0;
	res = buf;

cleanup:
	if (fd >= 0) close(fd);
	return buf;
}

int om_sysfs_writefile(const char* pathname, const char* str)
{
	int res = 0;
	size_t ssize = strlen(str);
	ssize_t count;
	int fd = -1;
	fd = open(pathname, O_WRONLY);
	if (fd < 0) return fd;
	count = write(fd, str, ssize);
	if (count != ssize)
		res = count;
//cleaup:
	if (fd >= 0) close(fd);
	return res;
}

const char* om_sysfs_get(const char* name)
{
	const char* path = om_sysfs_path(name);
	if (path == NULL) return NULL;
	return om_sysfs_readfile(path);
}

int om_sysfs_set(const char* name, const char* val)
{
	const char* path = om_sysfs_path(name);
	return om_sysfs_writefile(path, val);
}

const char* om_sysfs_swap(const char* name, const char* val)
{
	const char* path = om_sysfs_path(name);
	const char* res = NULL;
	if (path == NULL) return NULL;
	res = om_sysfs_readfile(path);
	if (om_sysfs_writefile(path, val) != 0) return NULL;
	return res;
}
