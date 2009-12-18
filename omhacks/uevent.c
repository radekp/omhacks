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

#include "uevent.h"
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <linux/netlink.h>

int om_uevent_open()
{
	int sock, retval;
	struct sockaddr_nl snl;

	memset(&snl, 0x00, sizeof(struct sockaddr_nl));
	snl.nl_family = AF_NETLINK;
	snl.nl_pid = getpid();
	snl.nl_groups = 0xffffffff;

        sock = socket(PF_NETLINK, SOCK_DGRAM, NETLINK_KOBJECT_UEVENT);
        if (sock == -1)
		return sock;

        retval = bind(sock, (struct sockaddr *) &snl,
                      sizeof(struct sockaddr_nl));
        if (retval < 0)
		return retval;

	return sock;
}

int om_uevent_read(int sock, struct om_uevent* ou)
{
	ssize_t buflen = recv(sock, ou, sizeof(ou->buffer), 0);
	if (buflen < 0)
		return -1;
	if ((size_t)buflen > sizeof(ou->buffer)-1)
		buflen = sizeof(ou->buffer)-1;
	ou->buflen = buflen;
	ou->buffer[buflen] = '\0';
	return 0;
}

int om_uevent_parse(struct om_uevent* ou)
{
	// See uevent_listen.c
	int i;

	/* Save start of payload */
	size_t bufpos = strlen(ou->buffer) + 1;

	/* Split action string and sysfs path */
	char* pos = strchr(ou->buffer, '@');

	if (pos == NULL) return -1;

	pos[0] = '\0';

	/* Action string */
	ou->action = ou->buffer;

	/* Sysfs path */
	ou->devpath = &pos[1];

	/* Events have the environment attached - reconstruct envp[] */
	for (i = 0; (bufpos < ou->buflen) && (i < OM_UEVENT_ENV_MAX - 1); ++i)
	{
		int keylen;
		char *key;

		key = &ou->buffer[bufpos];
		keylen = strlen(key);
		ou->envp[i] = key;
		bufpos += keylen + 1;
	}
	ou->envp[i] = NULL;

	return 0;
}
