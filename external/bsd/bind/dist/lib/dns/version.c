/*	$NetBSD$	*/

/*
 * Copyright (C) 2004, 2005, 2007, 2012, 2013  Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) 1998-2001  Internet Software Consortium.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/* Id: version.c,v 1.15.1234.1 2012/02/11 22:37:57 each Exp  */

/*! \file */

#include <dns/version.h>

const char dns_version[] = VERSION;
const char dns_major[] = MAJOR;
const char dns_mapapi[] = MAPAPI;

const unsigned int dns_libinterface = LIBINTERFACE;
const unsigned int dns_librevision = LIBREVISION;
const unsigned int dns_libage = LIBAGE;
