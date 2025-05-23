/*	$NetBSD$	*/

/*-
 * Copyright (c) 2004-2005 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Luke Mewburn.
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
#if defined(LIBC_SCCS) && !defined(lint)
#if 0
static char sccsid[] = "@(#)getgrouplist.c	8.2 (Berkeley) 12/8/94";
#else
__RCSID("$NetBSD$");
#endif
#endif /* LIBC_SCCS and not lint */

/*
 * calculate group access list
 */

#include "namespace.h"
#include <sys/param.h>

#include <assert.h>
#include <nsswitch.h>
#include <stdarg.h>
#include <unistd.h>

#ifdef __weak_alias
__weak_alias(getgrouplist,_getgrouplist)
#endif

int
getgrouplist(const char *uname, gid_t agroup, gid_t *groups, int *grpcnt)
{
	int	rv, groupc;

	_DIAGASSERT(uname != NULL);
	/* groups may be NULL if just sizing when invoked with *grpcnt = 0 */
	_DIAGASSERT(grpcnt != NULL);

	groupc = 0;
	rv = getgroupmembership(uname, agroup, groups, *grpcnt, &groupc);
	*grpcnt = groupc;	/* set groupc to the actual # of groups */
	return rv;
}
