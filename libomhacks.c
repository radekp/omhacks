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

#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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

static const char* scan_brightness()
{
	// TODO sys_brightness=\"$(find /sys -wholename "*backlight*/brightness")\"
	if (exists("/sys/class/backlight/gta02-bl/brightness"))
		return "/sys/class/backlight/gta02-bl/brightness";
	if (exists("/sys/devices/virtual/backlight/acpi_video0/brightness"))
		return "/sys/devices/virtual/backlight/acpi_video0/brightness";
	return NULL;
}

static struct om_sysfs_name om_sysfs_names[] = {
// TODO sys_auxled=\"$(find /sys -name "gta02-aux:red")\"
// TODO sys_battery=\"$(find /sys -wholename "*/power_supply/battery" -o -wholename "*/power_supply/bat" -type d)\"
	{ "brightness", scan_brightness, NULL },
// TODO sys_force_usb_limit_dangerous=\"$(find /sys -name force_usb_limit_dangerous -o -name usb_curlim)\"
// TODO sys_hostmode=\"$(find /sys -name hostmode)\"
// TODO sys_pm_bt_power=\"$(find /sys -wholename "*pm-bt*/power_on" -o -wholename "*pm-bt*/pwron")\"
// TODO sys_pm_gps_power=\"$(find /sys -wholename "*pm-gps*/power_on" -o -wholename "*pm-gps*/pwron")\"
// TODO sys_pm_gsm=\"$(find /sys -name neo1973-pm-gsm.0 -type d)\"
// TODO sys_pm_gsm_power=\"$(find /sys -wholename "*pm-gsm*/power_on" -o -wholename "*pm-gsm*/pwron")\"
// TODO sys_pm_wlan=\"$(find /sys -wholename "*gta02-pm-wlan/gta02-pm-wlan.0")\"
// TODO sys_resume_reason2=\"$(find /sys -wholename "*/0-0073/resume_reason")\"
// TODO sys_resume_reason=\"$(find /sys -wholename "*neo1973-resume.0/resume_reason")\"
// TODO sys_usb_mode=\"$(find /sys -name usb_mode)\"
// TODO sys_vibrator=\"$(find /sys -name neo1973:vibrator)\"
// TODO sys_wlan_driver=\"/sys/bus/platform/drivers/s3c2440-sdi\"
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
	if (n->cached_value == NULL)
		n->cached_value = n->scanner();
	return n->cached_value;
}

static const char* readsmallfile(const char* pathname)
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

static int writetofile(const char* pathname, const char* str)
{
	int res = 0;
	size_t ssize = strlen(str);
	ssize_t count;
	int fd = -1;
	fd = open(pathname, O_WRONLY | O_CREAT);
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
	return readsmallfile(path);
}

int om_sysfs_set(const char* name, const char* val)
{
	const char* path = om_sysfs_path(name);
	return writetofile(path, val);
}
