#ifndef OMHACKS_GPS_H
#define OMHACKS_GPS_H

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
 * Return the power status of the GPS chip.
 *
 * Returns 1 if on, 0 if off, a negative value in case of problems.
 */
int om_gps_power_get();

/*
 * Turn on/off the GPS chip
 *
 * If value is true, turn on the GPS. Else, turn it off.
 */
int om_gps_power_set(int value);

/*
 * Turn on/off the GPS chip, and return the previous status.
 *
 * If value is true, turn on the GPS. Else, turn it off.
 *
 * Returns 1 if on, 0 if off, a negative value in case of problems.
 */
int om_gps_power_swap(int value);


/*
 * Return the keep_on_in_suspend status of the GPS chip.
 *
 * Returns 1 if on, 0 if off, a negative value in case of problems.
 */
int om_gps_keep_on_in_suspend_get();

/*
 * Turn on/off the keep_on_in_suspend bit on the GPS chip
 *
 * If value is true, GPS stays on during suspend. Else, it gets switched off.
 */
int om_gps_keep_on_in_suspend_set(int value);

/*
 * Turn on/off the keep_on_in_suspend bit on the GPS chip, and return the
 * previous value.
 *
 * If value is true, GPS stays on during suspend. Else, it gets switched off.
 *
 * Returns 1 if on, 0 if off, a negative value in case of problems.
 */
int om_gps_keep_on_in_suspend_swap(int value);

/*
 * Open a connection to the GPS chip.
 *
 * Returns a connection handle (file descriptor) or negative value on
 * error.
 */
int om_gps_open(void);

/*
 * Close a connection to the GPS chip.
 *
 * The parameter should be a value returned by om_gps_open.
 */
void om_gps_close(int fd);

/*
 * Send an arbitrary UBX packet to the GPS chip.
 *
 * @param fd handle returned by om_gps_open
 * @param klass UBX packet class
 * @param type UBX packet type
 * @param payload actual payload of the packet
 * @param payloadlen length of payload.
 * @return 0 on success and a negative value in case of problems.
 *
 * Please read "ANTARIS_Protocol_Specification(GPS.G3-X-03002).chm" to
 * understand the protocol. Here are examples of commands that are
 * tested to work:
 *
 * type class payload # description
 * 06   01    f0 01 00 # disable GPGLL messages
 * 06   01    f0 02 00 # disable GPGSA messages
 * 06   01    f0 03 00 # disable GPGSV messages
 * 06   01    f0 05 00 # disable GPGTG messages
 * 06   01    f0 08 00 # disable GPZDA messages
 * 06   08    fa 00 01 00 00 00 # report position 4 times/s
 * 06   08    f4 01 01 00 00 00 # report position 2 times/s
 *
 */
int om_gps_ubx_send(int fd, int klass, int type, const char *payload, int payloadlen);

#endif
