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
#include "battery.h"
#include "sysfs.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>

static char battery_temperature_path[PATH_MAX];
static int battery_temperature_path_found = 0;

static char battery_energy_path[PATH_MAX];
static int battery_energy_path_found = 0;

static char battery_consumption_path[PATH_MAX];
static int battery_consumption_path_found = 0;

static const char *om_battery_temperature_path()
{
    if (!battery_temperature_path_found)
    {
        const char *root = om_sysfs_path("battery");
        if (root == NULL) return NULL;
        snprintf(battery_temperature_path, PATH_MAX, "%s/temp", root);
        if (access(battery_temperature_path, F_OK) != 0) return NULL;
        battery_temperature_path_found = 1;
    }
    return battery_temperature_path;
}

int om_battery_temperature_get(float *temperature)
{
    const char *path = om_battery_temperature_path();
    if (path == NULL) return -1;
    const char *val = om_sysfs_readfile(path);
    if (val == NULL) return -1;
    *temperature = atoi(val) / 10.0;
    return 0;
}

static const char *om_battery_energy_path()
{
    if (!battery_energy_path_found)
    {
        const char *root = om_sysfs_path("battery");
        if (root == NULL) return NULL;
        snprintf(battery_energy_path, PATH_MAX, "%s/capacity", root);
        if (access(battery_energy_path, F_OK) != 0) return NULL;
        battery_energy_path_found = 1;
    }
    return battery_energy_path;
}

int om_battery_energy_get()
{
    const char *path = om_battery_energy_path();
    if (path == NULL) return -1;
    const char *val = om_sysfs_readfile(path);
    if (val == NULL) return -1;
    return atoi(val);
}

static const char *om_battery_consumption_path()
{
    if (!battery_consumption_path_found)
    {
        const char *root = om_sysfs_path("battery");
        if (root == NULL) return NULL;
        snprintf(battery_consumption_path, PATH_MAX, "%s/current_now", root);
        if (access(battery_consumption_path, F_OK) != 0) return NULL;
        battery_consumption_path_found = 1;
    }
    return battery_consumption_path;
}

int om_battery_consumption_get(int *consumption)
{
    const char *path = om_battery_consumption_path();
    if (path == NULL) return -1;
    const char *val = om_sysfs_readfile(path);
    if (val == NULL) return -1;
    *consumption = atoi(val);
    return 0;
}


