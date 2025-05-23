/*	$NetBSD$	*/

/*-
 * Copyright (c) 2010 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe and Andrew Doran.
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

#ifndef _COMPAT_OPENSOLARIS_SYS_MUTEX_IMPL_H
#define	_COMPAT_OPENSOLARIS_SYS_MUTEX_IMPL_H

#define	__MUTEX_PRIVATE
#include <sys/mutex.h>

#define	mtx_owner 		u.mtxa_owner
#define	MUTEX_THREAD		((uintptr_t)-16L)
#define	MUTEX_OWNER(mtx)	\
	((mtx)->mtx_owner & MUTEX_THREAD)
#define MUTEX_NO_OWNER		0

#define	MUTEX_BIT_SPIN		0x01
#define MUTEX_TYPE_ADAPTIVE(mtx) (((mtx)->mtx_owner & MUTEX_BIT_SPIN) == 0)
#define MUTEX_TYPE_SPIN(mtx)	 (((mtx)->mtx_owner & MUTEX_BIT_SPIN) != 0)

#endif
