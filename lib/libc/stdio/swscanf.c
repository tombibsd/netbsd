/*	$NetBSD$	*/

/*-
 * Copyright (c) 2002 Tim J. Robbins
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#if defined(LIBC_SCCS) && !defined(lint)
#if 0
__FBSDID("$FreeBSD: src/lib/libc/stdio/swscanf.c,v 1.1 2002/09/23 12:40:06 tjr Exp $");
#else
__RCSID("$NetBSD$");
#endif
#endif /* LIBC_SCCS and not lint */

#include "namespace.h"

#include <stdarg.h>
#include <stdio.h>
#include <wchar.h>

__weak_alias(swscanf_l, _swscanf_l)

int
swscanf(const wchar_t * __restrict str, const wchar_t * __restrict fmt, ...)
{
	va_list ap;
	int r;

	va_start(ap, fmt);
	r = vswscanf(str, fmt, ap);
	va_end(ap);

	return r;
}

int
swscanf_l(const wchar_t * __restrict str, locale_t loc,
    const wchar_t * __restrict fmt, ...)
{
	va_list ap;
	int r;

	va_start(ap, fmt);
	r = vswscanf_l(str, loc, fmt, ap);
	va_end(ap);

	return r;
}
