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
#include "usb.h"
#include "sysfs.h"
#include <stdio.h>
#include <limits.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int om_usb_mode_get()
{
    const char* usb_mode_path = om_sysfs_path("usb_mode");
    if (usb_mode_path == NULL) return -3;
    const char *mode = om_sysfs_readfile(usb_mode_path);
    if (mode == NULL) return -1;
    if (!strcmp(mode, "device\n")) {
        return 0;
    } else if (!strcmp(mode, "host\n")) {
        return 1;
    } else {
        return -2;
    }
}

int om_usb_mode_set(int mode)
{
    const char* usb_mode_path = om_sysfs_path("usb_mode");
    if (usb_mode_path == NULL) return -3;
    if (mode == 0) {
        if (om_sysfs_writefile(usb_mode_path, "device\n") < 0) return -1;
    } else if (mode == 1) {
        if (om_sysfs_writefile(usb_mode_path, "host\n") < 0) return -1;
    } else {
        return -2;
    }
    return 0;
}

int om_usb_charger_mode_get()
{
    const char* usb_charger_mode_path = om_sysfs_path("usb_charger_mode");
    if (usb_charger_mode_path == NULL) return -2;
    const char *mode = om_sysfs_readfile(usb_charger_mode_path);
    if (mode == NULL) return -1;
    return atoi(mode);
}

int om_usb_charger_mode_set(int mode)
{
    const char* usb_charger_mode_path = om_sysfs_path("usb_charger_mode");
    if (usb_charger_mode_path == NULL) return -2;
    if (om_sysfs_writefile(usb_charger_mode_path, mode ? "1\n" : "0\n") < 0) return -1;
    return 0;
}

static const char *usb_charger_limit_path = "/sys/class/i2c-adapter/i2c-0/0-0073/pcf50633-mbc/usb_curlim";

int om_usb_charger_limit_get()
{
    const char *limit = om_sysfs_readfile(usb_charger_limit_path);
    if (limit == NULL) return -1;
    return atoi(limit);
}

int om_usb_charger_limit_set(int limit)
{
    char buf[128];
    snprintf(buf, sizeof(buf), "%d\n", limit);
    if (om_sysfs_writefile(usb_charger_limit_path, buf) < 0) return -1;
    return 0;
}
