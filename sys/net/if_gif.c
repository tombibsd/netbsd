/*	$NetBSD$	*/
/*	$KAME: if_gif.c,v 1.76 2001/08/20 02:01:02 kjc Exp $	*/

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

#include <sys/cdefs.h>
__KERNEL_RCSID(0, "$NetBSD$");

#ifdef _KERNEL_OPT
#include "opt_inet.h"
#endif

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/kernel.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>
#include <sys/errno.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/socketvar.h>
#include <sys/syslog.h>
#include <sys/proc.h>
#include <sys/cpu.h>
#include <sys/intr.h>
#include <sys/kmem.h>
#include <sys/sysctl.h>

#include <net/if.h>
#include <net/if_types.h>
#include <net/netisr.h>
#include <net/route.h>
#include <net/bpf.h>

#include <netinet/in.h>
#include <netinet/in_systm.h>
#include <netinet/ip.h>
#ifdef	INET
#include <netinet/in_var.h>
#endif	/* INET */
#include <netinet/in_gif.h>

#ifdef INET6
#ifndef INET
#include <netinet/in.h>
#endif
#include <netinet6/in6_var.h>
#include <netinet/ip6.h>
#include <netinet6/ip6_var.h>
#include <netinet6/in6_gif.h>
#endif /* INET6 */

#include <netinet/ip_encap.h>
#include <net/if_gif.h>

#include <net/net_osdep.h>

#include "ioconf.h"

static void	gifintr(void *);

/*
 * gif global variable definitions
 */
static LIST_HEAD(, gif_softc) gif_softc_list;

static void	gifattach0(struct gif_softc *);
static int	gif_output(struct ifnet *, struct mbuf *,
			   const struct sockaddr *, const struct rtentry *);
static int	gif_ioctl(struct ifnet *, u_long, void *);
static int	gif_set_tunnel(struct ifnet *, struct sockaddr *,
			       struct sockaddr *);
static void	gif_delete_tunnel(struct ifnet *);

static void	gif_sysctl_setup(struct sysctllog **);

static int	gif_clone_create(struct if_clone *, int);
static int	gif_clone_destroy(struct ifnet *);
static int	gif_check_nesting(struct ifnet *, struct mbuf *);

static int	gif_encap_attach(struct gif_softc *);
static int	gif_encap_detach(struct gif_softc *);

static struct if_clone gif_cloner =
    IF_CLONE_INITIALIZER("gif", gif_clone_create, gif_clone_destroy);

#ifndef MAX_GIF_NEST
/*
 * This macro controls the upper limitation on nesting of gif tunnels.
 * Since, setting a large value to this macro with a careless configuration
 * may introduce system crash, we don't allow any nestings by default.
 * If you need to configure nested gif tunnels, you can define this macro
 * in your kernel configuration file.  However, if you do so, please be
 * careful to configure the tunnels so that it won't make a loop.
 */
#define MAX_GIF_NEST 1
#endif
static int max_gif_nesting = MAX_GIF_NEST;

static void
gif_sysctl_setup(struct sysctllog **clog)
{

#ifdef INET
	sysctl_createv(clog, 0, NULL, NULL,
		       CTLFLAG_PERMANENT|CTLFLAG_READWRITE,
		       CTLTYPE_INT, "gifttl",
		       SYSCTL_DESCR("Default TTL for a gif tunnel datagram"),
		       NULL, 0, &ip_gif_ttl, 0,
		       CTL_NET, PF_INET, IPPROTO_IP,
		       IPCTL_GIF_TTL, CTL_EOL);
#endif
#ifdef INET6
	sysctl_createv(clog, 0, NULL, NULL,
		       CTLFLAG_PERMANENT|CTLFLAG_READWRITE,
		       CTLTYPE_INT, "gifhlim",
		       SYSCTL_DESCR("Default hop limit for a gif tunnel datagram"),
		       NULL, 0, &ip6_gif_hlim, 0,
		       CTL_NET, PF_INET6, IPPROTO_IPV6,
		       IPV6CTL_GIF_HLIM, CTL_EOL);
#endif
}

/* ARGSUSED */
void
gifattach(int count)
{

	LIST_INIT(&gif_softc_list);
	if_clone_attach(&gif_cloner);

	gif_sysctl_setup(NULL);
}

static int
gif_clone_create(struct if_clone *ifc, int unit)
{
	struct gif_softc *sc;

	sc = kmem_zalloc(sizeof(struct gif_softc), KM_SLEEP);
	if (sc == NULL)
		return ENOMEM;

	if_initname(&sc->gif_if, ifc->ifc_name, unit);

	gifattach0(sc);

	LIST_INSERT_HEAD(&gif_softc_list, sc, gif_list);
	return (0);
}

static void
gifattach0(struct gif_softc *sc)
{

	sc->encap_cookie4 = sc->encap_cookie6 = NULL;

	sc->gif_if.if_addrlen = 0;
	sc->gif_if.if_mtu    = GIF_MTU;
	sc->gif_if.if_flags  = IFF_POINTOPOINT | IFF_MULTICAST;
	sc->gif_if.if_ioctl  = gif_ioctl;
	sc->gif_if.if_output = gif_output;
	sc->gif_if.if_type   = IFT_GIF;
	sc->gif_if.if_dlt    = DLT_NULL;
	sc->gif_if.if_softc  = sc;
	IFQ_SET_READY(&sc->gif_if.if_snd);
	if_attach(&sc->gif_if);
	if_alloc_sadl(&sc->gif_if);
	bpf_attach(&sc->gif_if, DLT_NULL, sizeof(u_int));
}

static int
gif_clone_destroy(struct ifnet *ifp)
{
	struct gif_softc *sc = (void *) ifp;

	LIST_REMOVE(sc, gif_list);

	gif_delete_tunnel(&sc->gif_if);
	bpf_detach(ifp);
	if_detach(ifp);
	rtcache_free(&sc->gif_ro);

	kmem_free(sc, sizeof(struct gif_softc));

	return (0);
}

#ifdef GIF_ENCAPCHECK
int
gif_encapcheck(struct mbuf *m, int off, int proto, void *arg)
{
	struct ip ip;
	struct gif_softc *sc;

	sc = arg;
	if (sc == NULL)
		return 0;

	if ((sc->gif_if.if_flags & IFF_UP) == 0)
		return 0;

	/* no physical address */
	if (!sc->gif_psrc || !sc->gif_pdst)
		return 0;

	switch (proto) {
#ifdef INET
	case IPPROTO_IPV4:
		break;
#endif
#ifdef INET6
	case IPPROTO_IPV6:
		break;
#endif
	default:
		return 0;
	}

	/* Bail on short packets */
	KASSERT(m->m_flags & M_PKTHDR);
	if (m->m_pkthdr.len < sizeof(ip))
		return 0;

	m_copydata(m, 0, sizeof(ip), &ip);

	switch (ip.ip_v) {
#ifdef INET
	case 4:
		if (sc->gif_psrc->sa_family != AF_INET ||
		    sc->gif_pdst->sa_family != AF_INET)
			return 0;
		return gif_encapcheck4(m, off, proto, arg);
#endif
#ifdef INET6
	case 6:
		if (m->m_pkthdr.len < sizeof(struct ip6_hdr))
			return 0;
		if (sc->gif_psrc->sa_family != AF_INET6 ||
		    sc->gif_pdst->sa_family != AF_INET6)
			return 0;
		return gif_encapcheck6(m, off, proto, arg);
#endif
	default:
		return 0;
	}
}
#endif

/*
 * gif may cause infinite recursion calls when misconfigured.
 * We'll prevent this by introducing upper limit.
 */
static int
gif_check_nesting(struct ifnet *ifp, struct mbuf *m)
{
	struct m_tag *mtag;
	int *count;

	mtag = m_tag_find(m, PACKET_TAG_TUNNEL_INFO, NULL);
	if (mtag != NULL) {
		count = (int *)(mtag + 1);
		if (++(*count) > max_gif_nesting) {
			log(LOG_NOTICE,
			    "%s: recursively called too many times(%d)\n",
			    if_name(ifp),
			    *count);
			return EIO;
		}
	} else {
		mtag = m_tag_get(PACKET_TAG_TUNNEL_INFO, sizeof(*count),
		    M_NOWAIT);
		if (mtag != NULL) {
			m_tag_prepend(m, mtag);
			count = (int *)(mtag + 1);
			*count = 0;
		} else {
			log(LOG_DEBUG,
			    "%s: m_tag_get() failed, recursion calls are not prevented.\n",
			    if_name(ifp));
		}
	}

	return 0;
}

static int
gif_output(struct ifnet *ifp, struct mbuf *m, const struct sockaddr *dst,
    const struct rtentry *rt)
{
	struct gif_softc *sc = ifp->if_softc;
	int error = 0;
	int s;

	IFQ_CLASSIFY(&ifp->if_snd, m, dst->sa_family);

	if ((error = gif_check_nesting(ifp, m)) != 0) {
		m_free(m);
		goto end;
	}

	m->m_flags &= ~(M_BCAST|M_MCAST);
	if (!(ifp->if_flags & IFF_UP) ||
	    sc->gif_psrc == NULL || sc->gif_pdst == NULL ||
	    sc->gif_si == NULL) {
		m_freem(m);
		error = ENETDOWN;
		goto end;
	}

	/* XXX should we check if our outer source is legal? */

	/* use DLT_NULL encapsulation here to pass inner af type */
	M_PREPEND(m, sizeof(int), M_DONTWAIT);
	if (!m) {
		error = ENOBUFS;
		goto end;
	}
	*mtod(m, int *) = dst->sa_family;

	/* Clear checksum-offload flags. */
	m->m_pkthdr.csum_flags = 0;
	m->m_pkthdr.csum_data = 0;

	s = splnet();
	IFQ_ENQUEUE(&ifp->if_snd, m, error);
	if (error) {
		splx(s);
		goto end;
	}

	/* softint_schedule() must be called with kpreempt_disabled() */
	softint_schedule(sc->gif_si);
	splx(s);

	error = 0;

  end:
	if (error)
		ifp->if_oerrors++;
	return error;
}

static void
gifintr(void *arg)
{
	struct gif_softc *sc;
	struct ifnet *ifp;
	struct mbuf *m;
	int family;
	int len;
	int s;
	int error;

	sc = arg;
	ifp = &sc->gif_if;

	/*
	 * other CPUs does {set,delete}_tunnel after curcpu have done
	 * softint_schedule().
	 */
	if (sc->gif_pdst == NULL || sc->gif_psrc == NULL) {
		IFQ_PURGE(&ifp->if_snd);
		return;
	}

	/* output processing */
	while (1) {
		s = splnet();
		IFQ_DEQUEUE(&sc->gif_if.if_snd, m);
		splx(s);
		if (m == NULL)
			break;

		/* grab and chop off inner af type */
		if (sizeof(int) > m->m_len) {
			m = m_pullup(m, sizeof(int));
			if (!m) {
				ifp->if_oerrors++;
				continue;
			}
		}
		family = *mtod(m, int *);
		bpf_mtap(ifp, m);
		m_adj(m, sizeof(int));

		len = m->m_pkthdr.len;

		/* dispatch to output logic based on outer AF */
		switch (sc->gif_psrc->sa_family) {
#ifdef INET
		case AF_INET:
			mutex_enter(softnet_lock);
			error = in_gif_output(ifp, family, m);
			mutex_exit(softnet_lock);
			break;
#endif
#ifdef INET6
		case AF_INET6:
			mutex_enter(softnet_lock);
			error = in6_gif_output(ifp, family, m);
			mutex_exit(softnet_lock);
			break;
#endif
		default:
			m_freem(m);
			error = ENETDOWN;
			break;
		}

		if (error)
			ifp->if_oerrors++;
		else {
			ifp->if_opackets++;
			ifp->if_obytes += len;
		}
	}
}

void
gif_input(struct mbuf *m, int af, struct ifnet *ifp)
{
	pktqueue_t *pktq;
	size_t pktlen;
	int s;

	if (ifp == NULL) {
		/* just in case */
		m_freem(m);
		return;
	}

	m_set_rcvif(m, ifp);
	pktlen = m->m_pkthdr.len;

	bpf_mtap_af(ifp, af, m);

	/*
	 * Put the packet to the network layer input queue according to the
	 * specified address family.  Note: we avoid direct call to the
	 * input function of the network layer in order to avoid recursion.
	 * This may be revisited in the future.
	 */
	switch (af) {
#ifdef INET
	case AF_INET:
		pktq = ip_pktq;
		break;
#endif
#ifdef INET6
	case AF_INET6:
		pktq = ip6_pktq;
		break;
#endif
	default:
		m_freem(m);
		return;
	}

	s = splnet();
	if (__predict_true(pktq_enqueue(pktq, m, 0))) {
		ifp->if_ibytes += pktlen;
		ifp->if_ipackets++;
	} else {
		m_freem(m);
	}
	splx(s);
}

/* XXX how should we handle IPv6 scope on SIOC[GS]IFPHYADDR? */
static int
gif_ioctl(struct ifnet *ifp, u_long cmd, void *data)
{
	struct gif_softc *sc  = ifp->if_softc;
	struct ifreq     *ifr = (struct ifreq*)data;
	struct ifaddr    *ifa = (struct ifaddr*)data;
	int error = 0, size;
	struct sockaddr *dst, *src;

	switch (cmd) {
	case SIOCINITIFADDR:
		ifp->if_flags |= IFF_UP;
		ifa->ifa_rtrequest = p2p_rtrequest;
		break;

	case SIOCADDMULTI:
	case SIOCDELMULTI:
		switch (ifr->ifr_addr.sa_family) {
#ifdef INET
		case AF_INET:	/* IP supports Multicast */
			break;
#endif /* INET */
#ifdef INET6
		case AF_INET6:	/* IP6 supports Multicast */
			break;
#endif /* INET6 */
		default:  /* Other protocols doesn't support Multicast */
			error = EAFNOSUPPORT;
			break;
		}
		break;

	case SIOCSIFMTU:
		if (ifr->ifr_mtu < GIF_MTU_MIN || ifr->ifr_mtu > GIF_MTU_MAX)
			return EINVAL;
		else if ((error = ifioctl_common(ifp, cmd, data)) == ENETRESET)
			error = 0;
		break;

#ifdef INET
	case SIOCSIFPHYADDR:
#endif
#ifdef INET6
	case SIOCSIFPHYADDR_IN6:
#endif /* INET6 */
	case SIOCSLIFPHYADDR:
		switch (cmd) {
#ifdef INET
		case SIOCSIFPHYADDR:
			src = (struct sockaddr *)
				&(((struct in_aliasreq *)data)->ifra_addr);
			dst = (struct sockaddr *)
				&(((struct in_aliasreq *)data)->ifra_dstaddr);
			break;
#endif
#ifdef INET6
		case SIOCSIFPHYADDR_IN6:
			src = (struct sockaddr *)
				&(((struct in6_aliasreq *)data)->ifra_addr);
			dst = (struct sockaddr *)
				&(((struct in6_aliasreq *)data)->ifra_dstaddr);
			break;
#endif
		case SIOCSLIFPHYADDR:
			src = (struct sockaddr *)
				&(((struct if_laddrreq *)data)->addr);
			dst = (struct sockaddr *)
				&(((struct if_laddrreq *)data)->dstaddr);
			break;
		default:
			return EINVAL;
		}

		/* sa_family must be equal */
		if (src->sa_family != dst->sa_family)
			return EINVAL;

		/* validate sa_len */
		switch (src->sa_family) {
#ifdef INET
		case AF_INET:
			if (src->sa_len != sizeof(struct sockaddr_in))
				return EINVAL;
			break;
#endif
#ifdef INET6
		case AF_INET6:
			if (src->sa_len != sizeof(struct sockaddr_in6))
				return EINVAL;
			break;
#endif
		default:
			return EAFNOSUPPORT;
		}
		switch (dst->sa_family) {
#ifdef INET
		case AF_INET:
			if (dst->sa_len != sizeof(struct sockaddr_in))
				return EINVAL;
			break;
#endif
#ifdef INET6
		case AF_INET6:
			if (dst->sa_len != sizeof(struct sockaddr_in6))
				return EINVAL;
			break;
#endif
		default:
			return EAFNOSUPPORT;
		}

		/* check sa_family looks sane for the cmd */
		switch (cmd) {
		case SIOCSIFPHYADDR:
			if (src->sa_family == AF_INET)
				break;
			return EAFNOSUPPORT;
#ifdef INET6
		case SIOCSIFPHYADDR_IN6:
			if (src->sa_family == AF_INET6)
				break;
			return EAFNOSUPPORT;
#endif /* INET6 */
		case SIOCSLIFPHYADDR:
			/* checks done in the above */
			break;
		}

		error = gif_set_tunnel(&sc->gif_if, src, dst);
		break;

#ifdef SIOCDIFPHYADDR
	case SIOCDIFPHYADDR:
		gif_delete_tunnel(&sc->gif_if);
		break;
#endif

	case SIOCGIFPSRCADDR:
#ifdef INET6
	case SIOCGIFPSRCADDR_IN6:
#endif /* INET6 */
		if (sc->gif_psrc == NULL) {
			error = EADDRNOTAVAIL;
			goto bad;
		}
		src = sc->gif_psrc;
		switch (cmd) {
#ifdef INET
		case SIOCGIFPSRCADDR:
			dst = &ifr->ifr_addr;
			size = sizeof(ifr->ifr_addr);
			break;
#endif /* INET */
#ifdef INET6
		case SIOCGIFPSRCADDR_IN6:
			dst = (struct sockaddr *)
				&(((struct in6_ifreq *)data)->ifr_addr);
			size = sizeof(((struct in6_ifreq *)data)->ifr_addr);
			break;
#endif /* INET6 */
		default:
			error = EADDRNOTAVAIL;
			goto bad;
		}
		if (src->sa_len > size)
			return EINVAL;
		memcpy(dst, src, src->sa_len);
		break;

	case SIOCGIFPDSTADDR:
#ifdef INET6
	case SIOCGIFPDSTADDR_IN6:
#endif /* INET6 */
		if (sc->gif_pdst == NULL) {
			error = EADDRNOTAVAIL;
			goto bad;
		}
		src = sc->gif_pdst;
		switch (cmd) {
#ifdef INET
		case SIOCGIFPDSTADDR:
			dst = &ifr->ifr_addr;
			size = sizeof(ifr->ifr_addr);
			break;
#endif /* INET */
#ifdef INET6
		case SIOCGIFPDSTADDR_IN6:
			dst = (struct sockaddr *)
				&(((struct in6_ifreq *)data)->ifr_addr);
			size = sizeof(((struct in6_ifreq *)data)->ifr_addr);
			break;
#endif /* INET6 */
		default:
			error = EADDRNOTAVAIL;
			goto bad;
		}
		if (src->sa_len > size)
			return EINVAL;
		memcpy(dst, src, src->sa_len);
		break;

	case SIOCGLIFPHYADDR:
		if (sc->gif_psrc == NULL || sc->gif_pdst == NULL) {
			error = EADDRNOTAVAIL;
			goto bad;
		}

		/* copy src */
		src = sc->gif_psrc;
		dst = (struct sockaddr *)
			&(((struct if_laddrreq *)data)->addr);
		size = sizeof(((struct if_laddrreq *)data)->addr);
		if (src->sa_len > size)
			return EINVAL;
		memcpy(dst, src, src->sa_len);

		/* copy dst */
		src = sc->gif_pdst;
		dst = (struct sockaddr *)
			&(((struct if_laddrreq *)data)->dstaddr);
		size = sizeof(((struct if_laddrreq *)data)->dstaddr);
		if (src->sa_len > size)
			return EINVAL;
		memcpy(dst, src, src->sa_len);
		break;

	default:
		return ifioctl_common(ifp, cmd, data);
	}
 bad:
	return error;
}

static int
gif_encap_attach(struct gif_softc *sc)
{
	int error;

	if (sc == NULL || sc->gif_psrc == NULL)
		return EINVAL;

	switch (sc->gif_psrc->sa_family) {
#ifdef INET
	case AF_INET:
		error = in_gif_attach(sc);
		break;
#endif
#ifdef INET6
	case AF_INET6:
		error = in6_gif_attach(sc);
		break;
#endif
	default:
		error = EINVAL;
		break;
	}

	return error;
}

static int
gif_encap_detach(struct gif_softc *sc)
{
	int error;

	if (sc == NULL || sc->gif_psrc == NULL)
		return EINVAL;

	switch (sc->gif_psrc->sa_family) {
#ifdef INET
	case AF_INET:
		error = in_gif_detach(sc);
		break;
#endif
#ifdef INET6
	case AF_INET6:
		error = in6_gif_detach(sc);
		break;
#endif
	default:
		error = EINVAL;
		break;
	}

	return error;
}

static int
gif_set_tunnel(struct ifnet *ifp, struct sockaddr *src, struct sockaddr *dst)
{
	struct gif_softc *sc = ifp->if_softc;
	struct gif_softc *sc2;
	struct sockaddr *osrc, *odst;
	struct sockaddr *nsrc, *ndst;
	void *osi;
	int s;
	int error;

	s = splsoftnet();

	LIST_FOREACH(sc2, &gif_softc_list, gif_list) {
		if (sc2 == sc)
			continue;
		if (!sc2->gif_pdst || !sc2->gif_psrc)
			continue;
		/* can't configure same pair of address onto two gifs */
		if (sockaddr_cmp(sc2->gif_pdst, dst) == 0 &&
		    sockaddr_cmp(sc2->gif_psrc, src) == 0) {
			/* continue to use the old configureation. */
			splx(s);
			return EADDRNOTAVAIL;
		}

		/* XXX both end must be valid? (I mean, not 0.0.0.0) */
	}

	if ((nsrc = sockaddr_dup(src, M_WAITOK)) == NULL) {
		splx(s);
		return ENOMEM;
	}
	if ((ndst = sockaddr_dup(dst, M_WAITOK)) == NULL) {
		sockaddr_free(nsrc);
		splx(s);
		return ENOMEM;
	}

	/* Firstly, clear old configurations. */
	if (sc->gif_si) {
		osrc = sc->gif_psrc;
		odst = sc->gif_pdst;
		osi = sc->gif_si;
		sc->gif_psrc = NULL;
		sc->gif_pdst = NULL;
		sc->gif_si = NULL;
		/*
		 * At this point, gif_output() does not softint_schedule()
		 * any more. However, there are below 2 fears of other CPUs
		 * which would cause panic because of the race between
		 * softint_execute() and softint_disestablish().
		 *     (a) gif_output() has done softint_schedule(), and softint
		 *         (gifintr()) is waiting for execution
		 *         => This pattern is avoided by waiting SOFTINT_PENDING
		 *            CPUs in softint_disestablish()
		 *     (b) gifintr() is already running
		 *         => This pattern is avoided by waiting SOFTINT_ACTIVE
		 *            CPUs in softint_disestablish()
		 */

		softint_disestablish(osi);
		sc->gif_psrc = osrc;
		sc->gif_pdst = odst;
		osrc = NULL;
		odst = NULL;
	}
	/* XXX we can detach from both, but be polite just in case */
	if (sc->gif_psrc)
		(void)gif_encap_detach(sc);

	/*
	 * Secondly, try to set new configurations.
	 * If the setup failed, rollback to old configurations.
	 */
	do {
		osrc = sc->gif_psrc;
		odst = sc->gif_pdst;
		sc->gif_psrc = nsrc;
		sc->gif_pdst = ndst;

		error = gif_encap_attach(sc);
		if (error) {
			/* rollback to the last configuration. */
			nsrc = osrc;
			ndst = odst;
			osrc = sc->gif_psrc;
			odst = sc->gif_pdst;

			continue;
		}

		sc->gif_si = softint_establish(SOFTINT_NET, gifintr, sc);
		if (sc->gif_si == NULL) {
			(void)gif_encap_detach(sc);

			/* rollback to the last configuration. */
			nsrc = osrc;
			ndst = odst;
			osrc = sc->gif_psrc;
			odst = sc->gif_pdst;

			error = ENOMEM;
			continue;
		}
	} while (error != 0 && (nsrc != NULL && ndst != NULL));
	/* Thirdly, even rollback failed, clear configurations. */
	if (error) {
		osrc = sc->gif_psrc;
		odst = sc->gif_pdst;
		sc->gif_psrc = NULL;
		sc->gif_pdst = NULL;
	}

	if (osrc)
		sockaddr_free(osrc);
	if (odst)
		sockaddr_free(odst);

	if (sc->gif_psrc && sc->gif_pdst)
		ifp->if_flags |= IFF_RUNNING;
	else
		ifp->if_flags &= ~IFF_RUNNING;

	splx(s);
	return error;
}

static void
gif_delete_tunnel(struct ifnet *ifp)
{
	struct gif_softc *sc = ifp->if_softc;
	struct sockaddr *osrc, *odst;
	void *osi;
	int s;

	s = splsoftnet();

	if (sc->gif_si) {
		osrc = sc->gif_psrc;
		odst = sc->gif_pdst;
		osi = sc->gif_si;

		sc->gif_psrc = NULL;
		sc->gif_pdst = NULL;
		sc->gif_si = NULL;

		softint_disestablish(osi);
		sc->gif_psrc = osrc;
		sc->gif_pdst = odst;
	}
	if (sc->gif_psrc) {
		sockaddr_free(sc->gif_psrc);
		sc->gif_psrc = NULL;
	}
	if (sc->gif_pdst) {
		sockaddr_free(sc->gif_pdst);
		sc->gif_pdst = NULL;
	}
	/* it is safe to detach from both */
#ifdef INET
	(void)in_gif_detach(sc);
#endif
#ifdef INET6
	(void)in6_gif_detach(sc);
#endif

	if (sc->gif_psrc && sc->gif_pdst)
		ifp->if_flags |= IFF_RUNNING;
	else
		ifp->if_flags &= ~IFF_RUNNING;
	splx(s);
}
