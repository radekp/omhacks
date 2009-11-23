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

#endif
