#ifndef OMHACKS_BATTERY_H
#define OMHACKS_BATTERY_H

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

/*
 * Read temperature of battery (in celsius degrees)
 *
 * Writes result to supplied pointer. Returns zero on success and
 * negative on error.
 */ 
int om_battery_temperature_get(float *temperature);

/*
 * Read battery energy percentage.
 *
 * Returns 0-100 on success and negative on error.
 */
int om_battery_energy_get();

/*
 * Read battery consumption.
 *
 * Writes battery current in microamperes is to supplied
 * pointer. Negative current means that battery is being
 * charged. Return value is zero on success and negative on error.
 */
int om_battery_consumption_get(int *consumption);

/*
 * Read battery charger current limit.
 *
 * Returns battery charger current limit in milliamperes or negative value on error.
 *
 */
int om_battery_charger_limit_get(void);

/*
 * Set battery charger current limit.
 *
 * Normally usb charger limit and battery charger limit have the same
 * value. However, sometimes it is useful to charger battery very
 * slowly or not at all and still power rest of the system from
 * USB. This allows one for example to keep battery at its recommended
 * storage capacity of 40% without having to physically remove the
 * battery.
 *
 * Note that kernel will round the limit to nearest suitable value
 * which is usually a few milliamperes lower than the supplied limit.
 *
 * Also note that changing usb charger limit will reset also this
 * battery charger limit to the same value so you must first set usb
 * charger limit and only then battery charger limit.
 *
 * Returns 0 on success and negative value on failure.
 */
int om_battery_charger_limit_set(int limit);

#endif
