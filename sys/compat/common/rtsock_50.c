/*	$NetBSD$	*/

/*
 * Copyright (C) 1995, 1996, 1997, and 1998 WIDE Project.
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
 * 3. Neither the name of the project nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PROJECT AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE PROJECT OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Copyright (c) 1988, 1991, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *	@(#)rtsock.c	8.7 (Berkeley) 10/12/95
 */

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD$");

#ifdef _KERNEL_OPT
#include "opt_compat_netbsd.h"
#endif

#if defined(COMPAT_14) && !defined(COMPAT_50)
#define	COMPAT_50	1	/* 1.4 needs 5.0 */
#endif

#if defined(COMPAT_50)
#define	COMPAT_RTSOCK

#include <net/rtsock.c>

void
compat_50_rt_oifmsg(struct ifnet *ifp)
{
	struct if_msghdr50 oifm;
	struct mbuf *m;
	struct rt_addrinfo info;

	if (COMPATNAME(route_info).ri_cb.any_count == 0)
		return;
	(void)memset(&info, 0, sizeof(info));
	(void)memset(&oifm, 0, sizeof(oifm));
	oifm.ifm_index = ifp->if_index;
	oifm.ifm_flags = ifp->if_flags;
	oifm.ifm_data.ifi_type = ifp->if_data.ifi_type;
	oifm.ifm_data.ifi_addrlen = ifp->if_data.ifi_addrlen;
	oifm.ifm_data.ifi_hdrlen = ifp->if_data.ifi_hdrlen;
	oifm.ifm_data.ifi_mtu = ifp->if_data.ifi_mtu;
	oifm.ifm_data.ifi_metric = ifp->if_data.ifi_metric;
	oifm.ifm_data.ifi_baudrate = ifp->if_data.ifi_baudrate;
	oifm.ifm_data.ifi_ipackets = ifp->if_data.ifi_ipackets;
	oifm.ifm_data.ifi_ierrors = ifp->if_data.ifi_ierrors;
	oifm.ifm_data.ifi_opackets = ifp->if_data.ifi_opackets;
	oifm.ifm_data.ifi_oerrors = ifp->if_data.ifi_oerrors;
	oifm.ifm_data.ifi_collisions = ifp->if_data.ifi_collisions;
	oifm.ifm_data.ifi_ibytes = ifp->if_data.ifi_ibytes;
	oifm.ifm_data.ifi_obytes = ifp->if_data.ifi_obytes;
	oifm.ifm_data.ifi_imcasts = ifp->if_data.ifi_imcasts;
	oifm.ifm_data.ifi_omcasts = ifp->if_data.ifi_omcasts;
	oifm.ifm_data.ifi_iqdrops = ifp->if_data.ifi_iqdrops;
	oifm.ifm_data.ifi_noproto = ifp->if_data.ifi_noproto;
	TIMESPEC_TO_TIMEVAL(&oifm.ifm_data.ifi_lastchange,
	    &ifp->if_data.ifi_lastchange);
	oifm.ifm_addrs = 0;
	m = COMPATNAME(rt_msg1)(RTM_OIFINFO, &info, (void *)&oifm, sizeof(oifm));
	if (m == NULL)
		return;
	COMPATNAME(route_enqueue)(m, 0);
}

int
compat_50_iflist(struct ifnet *ifp, struct rt_walkarg *w,
    struct rt_addrinfo *info, size_t len)
{
	struct if_msghdr50 *ifm;
	int error;

	ifm = (struct if_msghdr50 *)w->w_tmem;
	ifm->ifm_index = ifp->if_index;
	ifm->ifm_flags = ifp->if_flags;
	ifm->ifm_data.ifi_type = ifp->if_data.ifi_type;
	ifm->ifm_data.ifi_addrlen = ifp->if_data.ifi_addrlen;
	ifm->ifm_data.ifi_hdrlen = ifp->if_data.ifi_hdrlen;
	ifm->ifm_data.ifi_mtu = ifp->if_data.ifi_mtu;
	ifm->ifm_data.ifi_metric = ifp->if_data.ifi_metric;
	ifm->ifm_data.ifi_baudrate = ifp->if_data.ifi_baudrate;
	ifm->ifm_data.ifi_ipackets = ifp->if_data.ifi_ipackets;
	ifm->ifm_data.ifi_ierrors = ifp->if_data.ifi_ierrors;
	ifm->ifm_data.ifi_opackets = ifp->if_data.ifi_opackets;
	ifm->ifm_data.ifi_oerrors = ifp->if_data.ifi_oerrors;
	ifm->ifm_data.ifi_collisions = ifp->if_data.ifi_collisions;
	ifm->ifm_data.ifi_ibytes = ifp->if_data.ifi_ibytes;
	ifm->ifm_data.ifi_obytes = ifp->if_data.ifi_obytes;
	ifm->ifm_data.ifi_imcasts = ifp->if_data.ifi_imcasts;
	ifm->ifm_data.ifi_omcasts = ifp->if_data.ifi_omcasts;
	ifm->ifm_data.ifi_iqdrops = ifp->if_data.ifi_iqdrops;
	ifm->ifm_data.ifi_noproto = ifp->if_data.ifi_noproto;
	TIMESPEC_TO_TIMEVAL(&ifm->ifm_data.ifi_lastchange,
	    &ifp->if_data.ifi_lastchange);
	ifm->ifm_addrs = info->rti_addrs;
	error = copyout(ifm, w->w_where, len);
	if (error)
		return error;
	w->w_where = (char *)w->w_where + len;
	return 0;
}

#endif /* COMPAT_50 */
