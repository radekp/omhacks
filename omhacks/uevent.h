#ifndef OMHACKS_UEVENT_H
#define OMHACKS_UEVENT_H

/*
 * uevent - Listen to uevent events
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

#include <stddef.h>

#define OM_UEVENT_ENV_MAX 32

/* UEvent information */
struct om_uevent
{
	char buffer[1024 + 512]; // Buffer with the raw uevent data
				 // At the beginning of the buffer, is the
				 // null-terminated action@devpath string
	size_t buflen;		 // Buffer length
	const char *action;	 // Parsed action
	const char *devpath;	 // Parsed device path
	const char *envp[OM_UEVENT_ENV_MAX];	 // Parsed environment
};

/**
 * Open a uevent socket.
 *
 * The socket is a normal unix file descriptor, to be closed using normal
 * close()
 *
 * Returns the socket, or a negative value in case of problems.
 */
int om_uevent_open();

/**
 * Read a uevent record
 */
int om_uevent_read(int sock, struct om_uevent* ou);

/**
 * Parse the raw uevent buffer in its components
 *
 * Returns 0 if all parsed correctly, -1 if this record does not parse properly
 * and should be skipped.
 */
int om_uevent_parse(struct om_uevent* ou);

#endif
