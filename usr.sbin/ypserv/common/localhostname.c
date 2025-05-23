/*	$NetBSD$	*/

/*-
 * Copyright (c) 1996 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#ifndef lint
__RCSID("$NetBSD$");
#endif

#include <sys/param.h>
#include <sys/socket.h>

#include <err.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "protos.h"

void
localhostname(char *buf, size_t buflen)
{
	struct addrinfo *res, hints;
	char hostname[MAXHOSTNAMELEN + 1];
	int error;

	if (gethostname(hostname, sizeof(hostname)))
		err(1, "gethostname");
	hostname[sizeof(hostname) - 1] = '\0';

	memset(buf, 0, buflen);

	/*
	 * If hostname appears to be fully-qualified,
	 * just return it.
	 */
	if (strchr(hostname, '.')) {
		strlcpy(buf, hostname, buflen);
		return;
	}

	/*
	 * Lookup the host.  Return the canonical name.
	 */
	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_CANONNAME;
	hints.ai_family = PF_UNSPEC;

	error = getaddrinfo(hostname, NULL, &hints, &res);
	if (error)
		errx(1, "getaddrinfo(%s) failed: %s", hostname,
		    gai_strerror(error));
	strlcpy(buf, res->ai_canonname, buflen);
	freeaddrinfo(res);
}
