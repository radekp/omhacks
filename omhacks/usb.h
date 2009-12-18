#ifndef OMHACKS_USB_H
#define OMHACKS_USB_H

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
 * Return the mode in which the USB controller is.
 *
 * Returns 0 for device mode, 1 for host mode and negative for
 * failure.
 */
int om_usb_mode_get();

/*
 * Set mode of the USB controller.
 *
 * If mode is 0 controller will be put to device mode and if it is 1
 * it will be put to host mode.
 *
 * Returns 0 on success and negative value on failure.
 */
int om_usb_mode_set(int mode);

/*
 * Get status of battery charger.
 *
 * Returns 1 if charger charges the battery from USB power.
 * Returns 0 if charger is disabled and power from battery is provided to USB bus.
 * Returns negative value on failure.
 */
int om_usb_charger_mode_get();

/*
 * Set status of battery charger.
 *
 * See above for description of the two possible modes.
 *
 * Returns 0 on success and negative value on failure.
 */
int om_usb_charger_mode_set(int value);
 
#endif
