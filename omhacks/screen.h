#ifndef OMHACKS_SCREEN_H
#define OMHACKS_SCREEN_H

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
 * Read screen brightness.
 *
 * If the result is negative, then an error happened.
 */ 
int om_screen_brightness_get();

/*
 * Read the maximum allowed brightness value
 *
 * If the result is negative, then an error happened.
 */ 
int om_screen_brightness_get_max();

/* Set screen brightness */
int om_screen_brightness_set(int val);

/* 
 * Set screen brightness and read the old value
 *
 * If the result is negative, then an error happened.
 */ 
int om_screen_brightness_swap(int val);

/*
 * Open the touchscreen input device.
 *
 * The result is a file descriptor that can be closed with a normal close()
 */
int om_touchscreen_open();

/*
 * Lock the touchscreen input device
 */
int om_touchscreen_lock(int fd);

/*
 * Unlock the touchscreen input device
 */
int om_touchscreen_unlock(int fd);

/*
 * Return the power status of the display
 *
 * Returns 1 if on, 0 if off and a negative value in case of problems.
 */
int om_screen_power_get();

/*
 * Turn on/off the display.
 *
 * If value is true, turn on the screen. Else, turn it off.
 *
 * Note that Xorg and fso-frameworkd do not know how to read the power
 * status of the screen (frameworkd reads it on startup only). If Xorg
 * turns the screen and after that you turn the screen off with
 * omhacks then touching the screen won't turn the screen on (Xorg
 * thinks the screen is still on and does not bother to try to power
 * it on).
 */
int om_screen_power_set(int val);

#endif
