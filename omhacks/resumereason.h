#ifndef OMHACKS_RESUME_REASON_H
#define OMHACKS_RESUME_REASON_H

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
 * Return a NULL-terminated string array with the reasons for the last
 * resume or NULL if there was an error while trying to determine
 * resume reason.
 *
 * Note that the strings are in a statically allocated buffer that is rewritten
 * at every invocation.
 */
const char** om_resume_reason();

#endif
