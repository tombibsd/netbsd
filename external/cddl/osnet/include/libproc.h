/*	$NetBSD$	*/

/*
 * Copyright (C) 2008 John Birrell <jb@freebsd.org>
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD: src/cddl/compat/opensolaris/include/libproc.h,v 1.1.4.1 2009/08/03 08:13:06 kensmith Exp $
 *
 */

#ifndef _COMPAT_OPENSOLARIS_LIBPROC_H_
#define _COMPAT_OPENSOLARIS_LIBPROC_H_

#include <fcntl.h>

#define ps_prochandle	proc_handle
#define Lmid_t		int

#define PR_RLC		0x0001
#define PR_KLC		0x0002

#define	PGRAB_RDONLY	O_RDONLY
#define	PGRAB_FORCE	0

struct proc_handle;
typedef void (*proc_child_func)(void *);

/* Values returned by proc_state(). */
#define PS_IDLE         1
#define PS_STOP         2
#define PS_RUN          3
#define PS_UNDEAD       4
#define PS_DEAD         5
#define PS_LOST         6

#include_next <libproc.h>

#endif
