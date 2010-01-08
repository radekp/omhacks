#ifndef OMHACKS_WIFI_H
#define OMHACKS_WIFI_H

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
 * Return the power status of the BlueTooth chip.
 *
 * Returns 1 if on, 0 if off, a negative value in case of problems.
 */
int om_wifi_power_get();

/*
 * Turn on/off the BlueTooth chip
 *
 * If value is true, turn on the BlueTooth. Else, turn it off.
 */
int om_wifi_power_set(int value);

/*
 * Turn on/off the BlueTooth chip, and return the previous status.
 *
 * If value is true, turn on the BlueTooth. Else, turn it off.
 *
 * Returns 1 if on, 0 if off, a negative value in case of problems.
 */
int om_wifi_power_swap(int value);


/*
 * Return status maximum performance mode of the wifi chip.
 *
 * Returns 0 if wifi if maximum performance mode is disable, 1 if it
 * is enabled and negative value on error.
 */
int om_wifi_maxperf_get(const char *ifname);

/*
 * Enable/disable wifi maximum performance mode
 *
 * If value is true, enable maximum performance mode. Else, turn it
 * off. Performance mode reduces latency but consumes more energy.
 *
 * Returns 0 on success and negative value on error.
 */
int om_wifi_maxperf_set(const char *ifname, int value);
#endif
