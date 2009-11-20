#ifndef OMHACKS_H
#define OMHACKS_H

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

/* Get the sysfs path of a useful file */
const char* om_sysfs_path(const char* name);

/*
 * Read the contents of a sysfs file
 *
 * Note: the result is returned in a static buffer that will be overwritten by
 * following invocations.
 */
const char* om_sysfs_get(const char* name);

/*
 * Set a sysfs value
 */
int om_sysfs_set(const char* name, const char* val);

/*
 * Combination of set and set: return the old value and set a new one
 */
const char* om_sysfs_swap(const char* name, const char* val);

/* Led status information */
struct om_led
{
	char* dir;	// Cached led dir name
	int dir_len;	// Cached length of dir string
	int brightness; // 0 if off
	int delay_on;	// 0 if not blinking
	int delay_off;	// 0 if not blinking
};

/* Initialise an om_led structure */
int om_led_init(struct om_led* led, const char* name);

/* Read led status */
int om_led_get(struct om_led* led);

/* Set led status */
int om_led_set(struct om_led* led);

#endif