/*	$NetBSD$ */

/*-
 * Copyright (c) 2010 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Mihai Chelaru <kefren@NetBSD.org>
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

#ifndef _NETMPLS_MPLS_VAR_H_
#define _NETMPLS_MPLS_VAR_H_

#ifndef _KERNEL
#error This file should NOT be included anywhere else than kernel
#endif

#ifdef _KERNEL_OPT
#include "opt_mbuftrace.h"
#endif

#include "netmpls/mpls.h"

#define MPLS_GETSADDR(rt) ntohl(((struct sockaddr_mpls*)rt_gettag(rt))->smpls_addr.s_addr)

extern int mpls_defttl;
extern int mpls_mapttl_inet;
extern int mpls_mapttl_inet6;
extern int mpls_mapprec_inet;
extern int mpls_mapclass_inet6;
extern int mpls_icmp_respond;

extern struct ifqueue mplsintrq;

#ifdef MBUFTRACE
extern struct mowner mpls_owner;
#endif

void	mpls_init(void);
void	mplsintr(void);

struct mbuf *mpls_ttl_dec(struct mbuf *);

#endif	/* !_NETMPLS_MPLS_VAR_H_ */
