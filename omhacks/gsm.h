#ifndef OMHACKS_GSM_H
#define OMHACKS_GSM_H

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
 * Return the power status of the GSM chip.
 *
 * Returns 1 if on, 0 if off, a negative value in case of problems.
 */
int om_gsm_power_get();

/*
 * Turn on/off the GSM chip
 *
 * If value is true, turn on the GSM. Else, turn it off.
 */
int om_gsm_power_set(int value);

/*
 * Turn on/off the GSM chip, and return the previous status.
 *
 * If value is true, turn on the GSM. Else, turn it off.
 *
 * Returns 1 if on, 0 if off, a negative value in case of problems.
 */
int om_gsm_power_swap(int value);

/*
 * Return flow control status of GSM serial port.
 *
 * Returns 1 if port is flow contolled, 0 if it is not and negative if an error occured.
 */
int om_gsm_flowcontrol_get();

/*
 * Set GSM serial port flow control status
 *
 * If value is true, enable flow contorl. Else, disable it.
 *
 */
int om_gsm_flowcontrol_set(int value);


#endif
