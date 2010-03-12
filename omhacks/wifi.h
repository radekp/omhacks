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
 * Return the power status of the Wifi chip.
 *
 * Returns 1 if on, 0 if off, a negative value in case of problems.
 */
int om_wifi_power_get();

/*
 * Turn on/off the Wifi chip
 *
 * If value is true, turn on the Wifi. Else, turn it off.
 */
int om_wifi_power_set(int value);

/*
 * Turn on/off the Wifi chip, and return the previous status.
 *
 * If value is true, turn on the Wifi. Else, turn it off.
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
 * Note that root privileges are not currently required for tuning
 * this wifi parameter so a local user can cause DoS by constantly
 * disabling maximum performance mode.
 *
 * Returns 0 on success and negative value on error.
 */
int om_wifi_maxperf_set(const char *ifname, int value);

/*
 * Return status of MCI bus during suspend. MCI bus connects wifi to
 * CPU and is required for Wake-on-Wireless to work.
 *
 * Returns 0 if MCI bus will be powered down during suspend, 1 if it
 * will be kept powered and negative value in case of problems.
 */
int om_wifi_keep_bus_on_in_suspend_get();

/*
 * Enable/disable the MCI bus during suspend.
 *
 * If value is true, MCI bus will be powered down during
 * suspend. Else, it gets switched off.
 */
int om_wifi_keep_bus_on_in_suspend_set(int value);


#endif
