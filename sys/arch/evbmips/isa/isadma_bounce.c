/*	$NetBSD$	*/

/*-
 * Copyright (c) 1996, 1997, 1998, 2000, 2001 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe of the Numerical Aerospace Simulation Facility,
 * NASA Ames Research Center.
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
__KERNEL_RCSID(0, "$NetBSD$");

#define _MIPS_BUS_DMA_PRIVATE

#include <sys/param.h>
#include <sys/bus.h>
#include <sys/device.h>
#include <sys/malloc.h>
#include <sys/mbuf.h>
#include <sys/proc.h>
#include <sys/systm.h>

#include <mips/cache.h>
#include <mips/locore.h>

#include <dev/isa/isareg.h>
#include <dev/isa/isavar.h>

#include <uvm/uvm_extern.h>

int	isadma_bounce_alloc_bouncebuf(bus_dma_tag_t, bus_dmamap_t,
	    bus_size_t, int);
void	isadma_bounce_free_bouncebuf(bus_dma_tag_t, bus_dmamap_t);

/*
 * Create an ISA DMA map.
 */
int
isadma_bounce_dmamap_create(bus_dma_tag_t t, bus_size_t size, int nsegments,
    bus_size_t maxsegsz, bus_size_t boundary, int flags, bus_dmamap_t *dmamp)
{
	struct mips_bus_dma_cookie *cookie;
	bus_dmamap_t map;
	int error, cookieflags;
	void *cookiestore;
	size_t cookiesize;

	/* Call common function to create the basic map. */
	error = _bus_dmamap_create(t, size, nsegments, maxsegsz, boundary,
	    flags, dmamp);
	if (error)
		return (error);

	map = *dmamp;
	map->_dm_cookie = NULL;

	cookiesize = sizeof(*cookie);

	/*
	 * ISA only has 24-bits of address space.  This means
	 * we can't DMA to pages over 16M.  In order to DMA to
	 * arbitrary buffers, we use "bounce buffers" - pages
	 * in memory below the 16M boundary.  On DMA reads,
	 * DMA happens to the bounce buffers, and is copied into
	 * the caller's buffer.  On writes, data is copied into
	 * but bounce buffer, and the DMA happens from those
	 * pages.  To software using the DMA mapping interface,
	 * this looks simply like a data cache.
	 *
	 * If we have more than 16M of RAM in the system, we may
	 * need bounce buffers.  We check and remember that here.
	 *
	 * ...or, there is an opposite case.  The most segments
	 * a transfer will require is (maxxfer / PAGE_SIZE) + 1.  If
	 * the caller can't handle that many segments (e.g. the
	 * ISA DMA controller), we may have to bounce it as well.
	 */
	cookieflags = 0;
	if (_BUS_AVAIL_END > (t->_wbase + t->_bounce_alloc_hi - t->_bounce_alloc_lo)
	    || ((map->_dm_size / PAGE_SIZE) + 1) > map->_dm_segcnt) {
		cookieflags |= _BUS_DMA_MIGHT_NEED_BOUNCE;
		cookiesize += (sizeof(bus_dma_segment_t) *
		    (map->_dm_segcnt - 1));
	}

	/*
	 * Allocate our cookie.
	 */
	if ((cookiestore = malloc(cookiesize, M_DMAMAP,
	    (flags & BUS_DMA_NOWAIT) ? M_NOWAIT : M_WAITOK)) == NULL) {
		error = ENOMEM;
		goto out;
	}
	memset(cookiestore, 0, cookiesize);
	cookie = (struct mips_bus_dma_cookie *)cookiestore;
	cookie->id_flags = cookieflags;
	map->_dm_cookie = cookie;

	if (cookieflags & _BUS_DMA_MIGHT_NEED_BOUNCE) {
		/*
		 * Allocate the bounce pages now if the caller
		 * wishes us to do so.
		 */
		if ((flags & BUS_DMA_ALLOCNOW) == 0)
			goto out;

		error = isadma_bounce_alloc_bouncebuf(t, map, size, flags);
	}

 out:
	if (error) {
		if (map->_dm_cookie != NULL)
			free(map->_dm_cookie, M_DMAMAP);
		_bus_dmamap_destroy(t, map);
	}
	return (error);
}

/*
 * Destroy an ISA DMA map.
 */
void
isadma_bounce_dmamap_destroy(bus_dma_tag_t t, bus_dmamap_t map)
{
	struct mips_bus_dma_cookie *cookie = map->_dm_cookie;

	/*
	 * Free any bounce pages this map might hold.
	 */
	if (cookie->id_flags & _BUS_DMA_HAS_BOUNCE)
		isadma_bounce_free_bouncebuf(t, map);

	free(cookie, M_DMAMAP);
	_bus_dmamap_destroy(t, map);
}

/*
 * Load an ISA DMA map with a linear buffer.
 */
int
isadma_bounce_dmamap_load(bus_dma_tag_t t, bus_dmamap_t map, void *buf,
    bus_size_t buflen, struct proc *p, int flags)
{
	struct mips_bus_dma_cookie *cookie = map->_dm_cookie;
	int error;

	/*
	 * Make sure that on error condition we return "no valid mappings."
	 */
	map->dm_mapsize = 0;
	map->dm_nsegs = 0;

	/*
	 * Try to load the map the normal way.  If this errors out,
	 * and we can bounce, we will.
	 */
	error = _bus_dmamap_load(t, map, buf, buflen, p, flags);
	if (error == 0 || (cookie->id_flags & _BUS_DMA_MIGHT_NEED_BOUNCE) == 0)
		return (error);

	/*
	 * First attempt failed; bounce it.
	 */

	/*
	 * Allocate bounce pages, if necessary.
	 */
	if ((cookie->id_flags & _BUS_DMA_HAS_BOUNCE) == 0) {
		error = isadma_bounce_alloc_bouncebuf(t, map, buflen, flags);
		if (error)
			return (error);
	}

	/*
	 * Cache a pointer to the caller's buffer and load the DMA map
	 * with the bounce buffer.
	 */
	cookie->id_origbuf = buf;
	cookie->id_origbuflen = buflen;
	cookie->id_buftype = _BUS_DMA_BUFTYPE_LINEAR;
	error = _bus_dmamap_load(t, map, cookie->id_bouncebuf, buflen,
	    p, flags);
	if (error) {
		/*
		 * Free the bounce pages, unless our resources
		 * are reserved for our exclusive use.
		 */
		if ((map->_dm_flags & BUS_DMA_ALLOCNOW) == 0)
			isadma_bounce_free_bouncebuf(t, map);
		return (error);
	}

	/* ...so isadma_bounce_dmamap_sync() knows we're bouncing */
	cookie->id_flags |= _BUS_DMA_IS_BOUNCING;
	return (0);
}

/*
 * Like isadma_bounce_dmamap_load(), but for mbufs.
 */
int
isadma_bounce_dmamap_load_mbuf(bus_dma_tag_t t, bus_dmamap_t map,
    struct mbuf *m0, int flags)  
{
	struct mips_bus_dma_cookie *cookie = map->_dm_cookie;
	int error;

	/*
	 * Make sure on error condition we return "no valid mappings."
	 */
	map->dm_mapsize = 0;
	map->dm_nsegs = 0;

#ifdef DIAGNOSTIC
	if ((m0->m_flags & M_PKTHDR) == 0)
		panic("isadma_bounce_dmamap_load_mbuf: no packet header");
#endif

	if (m0->m_pkthdr.len > map->_dm_size)
		return (EINVAL);

	/*
	 * Try to load the map the normal way.  If this errors out,
	 * and we can bounce, we will.
	 */
	error = _bus_dmamap_load_mbuf(t, map, m0, flags);
	if (error == 0 || (cookie->id_flags & _BUS_DMA_MIGHT_NEED_BOUNCE) == 0)
		return (error);

	/*
	 * First attempt failed; bounce it.
	 */

	/*
	 * Allocate bounce pages, if necessary.
	 */
	if ((cookie->id_flags & _BUS_DMA_HAS_BOUNCE) == 0) {
		error = isadma_bounce_alloc_bouncebuf(t, map, m0->m_pkthdr.len,
		    flags);
		if (error)
			return (error);
	}

	/*
	 * Cache a pointer to the caller's buffer and load the DMA map
	 * with the bounce buffer.
	 */
	cookie->id_origbuf = m0;
	cookie->id_origbuflen = m0->m_pkthdr.len;	/* not really used */
	cookie->id_buftype = _BUS_DMA_BUFTYPE_MBUF;
	error = _bus_dmamap_load(t, map, cookie->id_bouncebuf,
	    m0->m_pkthdr.len, NULL, flags);
	if (error) {
		/*
		 * Free the bounce pages, unless our resources
		 * are reserved for our exclusive use.
		 */
		if ((map->_dm_flags & BUS_DMA_ALLOCNOW) == 0)
			isadma_bounce_free_bouncebuf(t, map);
		return (error);
	}

	/* ...so isadma_bounce_dmamap_sync() knows we're bouncing */
	cookie->id_flags |= _BUS_DMA_IS_BOUNCING;
	return (0);
}

/*
 * Like isadma_bounce_dmamap_load(), but for uios.
 */
int
isadma_bounce_dmamap_load_uio(bus_dma_tag_t t, bus_dmamap_t map,
    struct uio *uio, int flags)
{

	panic("isadma_bounce_dmamap_load_uio: not implemented");
}

/*
 * Like isadma_bounce_dmamap_load(), but for raw memory allocated with
 * bus_dmamem_alloc().
 */
int
isadma_bounce_dmamap_load_raw(bus_dma_tag_t t, bus_dmamap_t map,
    bus_dma_segment_t *segs, int nsegs, bus_size_t size, int flags)
{

	panic("isadma_bounce_dmamap_load_raw: not implemented");
}

/*
 * Unload an ISA DMA map.
 */
void
isadma_bounce_dmamap_unload(bus_dma_tag_t t, bus_dmamap_t map)
{
	struct mips_bus_dma_cookie *cookie = map->_dm_cookie;

	/*
	 * If we have bounce pages, free them, unless they're
	 * reserved for our exclusive use.
	 */
	if ((cookie->id_flags & _BUS_DMA_HAS_BOUNCE) &&
	    (map->_dm_flags & BUS_DMA_ALLOCNOW) == 0)
		isadma_bounce_free_bouncebuf(t, map);

	cookie->id_flags &= ~_BUS_DMA_IS_BOUNCING;
	cookie->id_buftype = _BUS_DMA_BUFTYPE_INVALID;

	/*
	 * Do the generic bits of the unload.
	 */
	_bus_dmamap_unload(t, map);
}

/*
 * Synchronize an ISA DMA map.
 */
void
isadma_bounce_dmamap_sync(bus_dma_tag_t t, bus_dmamap_t map, bus_addr_t offset,
    bus_size_t len, int ops)
{
	struct mips_bus_dma_cookie *cookie = map->_dm_cookie;

	/*
	 * Mixing PRE and POST operations is not allowed.
	 */
	if ((ops & (BUS_DMASYNC_PREREAD|BUS_DMASYNC_PREWRITE)) != 0 &&
	    (ops & (BUS_DMASYNC_POSTREAD|BUS_DMASYNC_POSTWRITE)) != 0)
		panic("isadma_bounce_dmamap_sync: mix PRE and POST");

#ifdef DIAGNOSTIC
	if ((ops & (BUS_DMASYNC_PREWRITE|BUS_DMASYNC_POSTREAD)) != 0) {
		if (offset >= map->dm_mapsize)
			panic("isadma_bounce_dmamap_sync: bad offset");
		if (len == 0 || (offset + len) > map->dm_mapsize)
			panic("isadma_bounce_dmamap_sync: bad length");
	}
#endif

	/*
	 * If we're not bouncing, just do the normal sync operation
	 * and return.
	 */
	if ((cookie->id_flags & _BUS_DMA_IS_BOUNCING) == 0) {
		_bus_dmamap_sync(t, map, offset, len, ops);
		return;
	}

	/*
	 * Flush data cache for PREREAD.  This has the side-effect
	 * of invalidating the cache.  Done at PREREAD since it
	 * causes the cache line(s) to be written back to memory.
	 *
	 * Copy the original buffer to the bounce buffer and flush
	 * the data cache for PREWRITE, so that the contents
	 * of the data buffer in memory reflect reality.
	 *
	 * Copy the bounce buffer to the original buffer in POSTREAD.
	 */

	switch (cookie->id_buftype) {
	case _BUS_DMA_BUFTYPE_LINEAR:
		/*
		 * Nothing to do for pre-read.
		 */

		if (ops & BUS_DMASYNC_PREWRITE) {
			/*
			 * Copy the caller's buffer to the bounce buffer.
			 */
			memcpy((char *)cookie->id_bouncebuf + offset,
			    (char *)cookie->id_origbuf + offset, len);
			wbflush();
		}

		if (ops & BUS_DMASYNC_POSTREAD) {
			/*
			 * Copy the bounce buffer to the caller's buffer.
			 */
			memcpy((char *)cookie->id_origbuf + offset,
			    (char *)cookie->id_bouncebuf + offset, len);
		}

		/*
		 * Nothing to do for post-write.
		 */
		break;

	case _BUS_DMA_BUFTYPE_MBUF:
	    {
		struct mbuf *m, *m0 = cookie->id_origbuf;
		bus_size_t minlen, moff;

		/*
		 * Nothing to do for pre-read.
		 */

		if (ops & BUS_DMASYNC_PREWRITE) {
			/*
			 * Copy the caller's buffer to the bounce buffer.
			 */
			m_copydata(m0, offset, len,
			    (char *)cookie->id_bouncebuf + offset);
		}

		if (ops & BUS_DMASYNC_POSTREAD) {
			/*
			 * Copy the bounce buffer to the caller's buffer.
			 */
			for (moff = offset, m = m0; m != NULL && len != 0;
			     m = m->m_next) {
				/* Find the beginning mbuf. */
				if (moff >= m->m_len) {
					moff -= m->m_len;
					continue;
				}

				/*
				 * Now at the first mbuf to sync; nail
				 * each one until we have exhausted the
				 * length.
				 */
				minlen = len < m->m_len - moff ?
				    len : m->m_len - moff;

				memcpy(mtod(m, char *) + moff,
				    (char *)cookie->id_bouncebuf + offset,
				    minlen);

				moff = 0;
				len -= minlen;
				offset += minlen;
			}
		}

		/*
		 * Nothing to do for post-write.
		 */
		break;
	    }

	case _BUS_DMA_BUFTYPE_UIO:
		panic("isadma_bounce_dmamap_sync: _BUS_DMA_BUFTYPE_UIO");
		break;

	case _BUS_DMA_BUFTYPE_RAW:
		panic("isadma_bounce_dmamap_sync: _BUS_DMA_BUFTYPE_RAW");
		break;

	case _BUS_DMA_BUFTYPE_INVALID:
		panic("isadma_bounce_dmamap_sync: _BUS_DMA_BUFTYPE_INVALID");
		break;

	default:
		printf("unknown buffer type %d\n", cookie->id_buftype);
		panic("isadma_bounce_dmamap_sync");
	}

	/* Drain the write buffer. */
	wbflush();

	/* XXXJRT */
	if (ops & (BUS_DMASYNC_PREREAD|BUS_DMASYNC_PREWRITE))
		mips_dcache_wbinv_range((vaddr_t)cookie->id_bouncebuf + offset,
		    len);
}

/*
 * Allocate memory safe for ISA DMA.
 */
int
isadma_bounce_dmamem_alloc(bus_dma_tag_t t, bus_size_t size,
    bus_size_t alignment, bus_size_t boundary, bus_dma_segment_t *segs,
    int nsegs, int *rsegs, int flags)
{
	paddr_t high;

	if (_BUS_AVAIL_END > ISA_DMA_BOUNCE_THRESHOLD)
		high = trunc_page(ISA_DMA_BOUNCE_THRESHOLD);
	else
		high = trunc_page(_BUS_AVAIL_END);

	return (_bus_dmamem_alloc_range(t, size, alignment, boundary,
	    segs, nsegs, rsegs, flags, 0, high));
}

/**********************************************************************
 * ISA DMA utility functions
 **********************************************************************/

int
isadma_bounce_alloc_bouncebuf(bus_dma_tag_t t, bus_dmamap_t map,
    bus_size_t size, int flags)
{
	struct mips_bus_dma_cookie *cookie = map->_dm_cookie;
	int error = 0;

	cookie->id_bouncebuflen = round_page(size);
	error = isadma_bounce_dmamem_alloc(t, cookie->id_bouncebuflen,
	    PAGE_SIZE, map->_dm_boundary, cookie->id_bouncesegs,
	    map->_dm_segcnt, &cookie->id_nbouncesegs, flags);
	if (error)
		goto out;
	error = _bus_dmamem_map(t, cookie->id_bouncesegs,
	    cookie->id_nbouncesegs, cookie->id_bouncebuflen,
	    (void **)&cookie->id_bouncebuf, flags);

 out:
	if (error) {
		_bus_dmamem_free(t, cookie->id_bouncesegs,
		    cookie->id_nbouncesegs);
		cookie->id_bouncebuflen = 0;
		cookie->id_nbouncesegs = 0;
	} else
		cookie->id_flags |= _BUS_DMA_HAS_BOUNCE;

	return (error);
}

void
isadma_bounce_free_bouncebuf(bus_dma_tag_t t, bus_dmamap_t map)
{
	struct mips_bus_dma_cookie *cookie = map->_dm_cookie;

	_bus_dmamem_unmap(t, cookie->id_bouncebuf,
	    cookie->id_bouncebuflen);
	_bus_dmamem_free(t, cookie->id_bouncesegs,
	    cookie->id_nbouncesegs);
	cookie->id_bouncebuflen = 0;
	cookie->id_nbouncesegs = 0;
	cookie->id_flags &= ~_BUS_DMA_HAS_BOUNCE;
}
