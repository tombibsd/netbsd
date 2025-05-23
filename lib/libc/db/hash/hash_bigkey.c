/*	$NetBSD$	*/

/*-
 * Copyright (c) 1990, 1993, 1994
 *	The Regents of the University of California.  All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Margo Seltzer.
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
 */

#if HAVE_NBTOOL_CONFIG_H
#include "nbtool_config.h"
#endif

#include <sys/cdefs.h>
__RCSID("$NetBSD$");

/*
 * PACKAGE: hash
 * DESCRIPTION:
 *	Big key/data handling for the hashing package.
 *
 * ROUTINES:
 * External
 *	__big_keydata
 *	__big_split
 *	__big_insert
 *	__big_return
 *	__big_delete
 *	__find_last_page
 * Internal
 *	collect_key
 *	collect_data
 */

#include <sys/param.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <db.h>
#include "hash.h"
#include "page.h"
#include "extern.h"

static int collect_key(HTAB *, BUFHEAD *, int, DBT *, int);
static int collect_data(HTAB *, BUFHEAD *, int, int);

/*
 * Big_insert
 *
 * You need to do an insert and the key/data pair is too big
 *
 * Returns:
 * 0 ==> OK
 *-1 ==> ERROR
 */
int
__big_insert(HTAB *hashp, BUFHEAD *bufp, const DBT *key, const DBT *val)
{
	uint16_t *p, n;
	size_t key_size, val_size;
	uint16_t space, move_bytes, off;
	char *cp, *key_data, *val_data;
	size_t temp;

	cp = bufp->page;		/* Character pointer of p. */
	p = (uint16_t *)(void *)cp;

	key_data = (char *)key->data;
	_DBFIT(key->size, int);
	key_size = key->size;
	val_data = (char *)val->data;
	_DBFIT(val->size, int);
	val_size = val->size;

	/* First move the Key */
	
	temp = FREESPACE(p) - BIGOVERHEAD;
	_DBFIT(temp, uint16_t);
	space = (uint16_t)temp;
	while (key_size) {
		size_t kspace = MIN(space, key_size);
		_DBFIT(kspace, uint16_t);
		move_bytes = (uint16_t)kspace;
		off = OFFSET(p) - move_bytes;
		memmove(cp + off, key_data, (size_t)move_bytes);
		key_size -= move_bytes;
		key_data += move_bytes;
		n = p[0];
		p[++n] = off;
		p[0] = ++n;
		temp = off - PAGE_META(n);
		_DBFIT(temp, uint16_t);
		FREESPACE(p) = (uint16_t)temp;
		OFFSET(p) = off;
		p[n] = PARTIAL_KEY;
		bufp = __add_ovflpage(hashp, bufp);
		if (!bufp)
			return (-1);
		n = p[0];
		if (!key_size) {
			space = FREESPACE(p);
			if (space) {
				size_t vspace = MIN(space, val_size);
				_DBFIT(vspace, uint16_t);
				move_bytes = (uint16_t)vspace;
				/*
				 * If the data would fit exactly in the
				 * remaining space, we must overflow it to the
				 * next page; otherwise the invariant that the
				 * data must end on a page with FREESPACE
				 * non-zero would fail.
				 */
				if (space == val_size && val_size == val->size)
					goto toolarge;
				off = OFFSET(p) - move_bytes;
				memmove(cp + off, val_data, (size_t)move_bytes);
				val_data += move_bytes;
				val_size -= move_bytes;
				p[n] = off;
				p[n - 2] = FULL_KEY_DATA;
				FREESPACE(p) = FREESPACE(p) - move_bytes;
				OFFSET(p) = off;
			} else {
			toolarge:
				p[n - 2] = FULL_KEY;
			}
		}
		p = (uint16_t *)(void *)bufp->page;
		cp = bufp->page;
		bufp->flags |= BUF_MOD;
		temp = FREESPACE(p) - BIGOVERHEAD;
		_DBFIT(temp, uint16_t);
		space = (uint16_t)temp;
	}

	/* Now move the data */
	temp = FREESPACE(p) - BIGOVERHEAD;
	_DBFIT(temp, uint16_t);
	space = (uint16_t)temp;
	while (val_size) {
		size_t vspace = MIN(space, val_size);
		_DBFIT(vspace, uint16_t);
		move_bytes = (uint16_t)vspace;
		/*
		 * Here's the hack to make sure that if the data ends on the
		 * same page as the key ends, FREESPACE is at least one.
		 */
		if (space == val_size && val_size == val->size)
			move_bytes--;
		off = OFFSET(p) - move_bytes;
		memmove(cp + off, val_data, (size_t)move_bytes);
		val_size -= move_bytes;
		val_data += move_bytes;
		n = p[0];
		p[++n] = off;
		p[0] = ++n;
		temp = off - PAGE_META(n);
		_DBFIT(temp, uint16_t);
		FREESPACE(p) = (uint16_t)temp;
		OFFSET(p) = off;
		if (val_size) {
			p[n] = FULL_KEY;
			bufp = __add_ovflpage(hashp, bufp);
			if (!bufp)
				return (-1);
			cp = bufp->page;
			p = (uint16_t *)(void *)cp;
		} else
			p[n] = FULL_KEY_DATA;
		bufp->flags |= BUF_MOD;
		temp = FREESPACE(p) - BIGOVERHEAD;
		_DBFIT(temp, uint16_t);
		space = (uint16_t)temp;
	}
	return (0);
}

/*
 * Called when bufp's page  contains a partial key (index should be 1)
 *
 * All pages in the big key/data pair except bufp are freed.  We cannot
 * free bufp because the page pointing to it is lost and we can't get rid
 * of its pointer.
 *
 * Returns:
 * 0 => OK
 *-1 => ERROR
 */
int
__big_delete(HTAB *hashp, BUFHEAD *bufp)
{
	BUFHEAD *last_bfp, *rbufp;
	uint16_t *bp, pageno;
	int key_done, n;
	size_t temp;

	rbufp = bufp;
	last_bfp = NULL;
	bp = (uint16_t *)(void *)bufp->page;
	pageno = 0;
	key_done = 0;

	while (!key_done || (bp[2] != FULL_KEY_DATA)) {
		if (bp[2] == FULL_KEY || bp[2] == FULL_KEY_DATA)
			key_done = 1;

		/*
		 * If there is freespace left on a FULL_KEY_DATA page, then
		 * the data is short and fits entirely on this page, and this
		 * is the last page.
		 */
		if (bp[2] == FULL_KEY_DATA && FREESPACE(bp))
			break;
		pageno = bp[bp[0] - 1];
		rbufp->flags |= BUF_MOD;
		rbufp = __get_buf(hashp, (uint32_t)pageno, rbufp, 0);
		if (last_bfp)
			__free_ovflpage(hashp, last_bfp);
		last_bfp = rbufp;
		if (!rbufp)
			return (-1);		/* Error. */
		bp = (uint16_t *)(void *)rbufp->page;
	}

	/*
	 * If we get here then rbufp points to the last page of the big
	 * key/data pair.  Bufp points to the first one -- it should now be
	 * empty pointing to the next page after this pair.  Can't free it
	 * because we don't have the page pointing to it.
	 */

	/* This is information from the last page of the pair. */
	n = bp[0];
	pageno = bp[n - 1];

	/* Now, bp is the first page of the pair. */
	bp = (uint16_t *)(void *)bufp->page;
	if (n > 2) {
		/* There is an overflow page. */
		bp[1] = pageno;
		bp[2] = OVFLPAGE;
		bufp->ovfl = rbufp->ovfl;
	} else
		/* This is the last page. */
		bufp->ovfl = NULL;
	n -= 2;
	bp[0] = n;
	temp = HASH_BSIZE(hashp) - PAGE_META(n);
	_DBFIT(temp, uint16_t);
	FREESPACE(bp) = (uint16_t)temp;
	OFFSET(bp) = HASH_BSIZE(hashp);

	bufp->flags |= BUF_MOD;
	if (rbufp)
		__free_ovflpage(hashp, rbufp);
	if (last_bfp && last_bfp != rbufp)
		__free_ovflpage(hashp, last_bfp);

	hashp->NKEYS--;
	return (0);
}
/*
 * Returns:
 *  0 = key not found
 * -1 = get next overflow page
 * -2 means key not found and this is big key/data
 * -3 error
 */
int
__find_bigpair(HTAB *hashp, BUFHEAD *bufp, int ndx, char *key, int size)
{
	uint16_t *bp;
	char *p;
	int ksize;
	uint16_t bytes;
	char *kkey;

	bp = (uint16_t *)(void *)bufp->page;
	p = bufp->page;
	ksize = size;
	kkey = key;

	for (bytes = HASH_BSIZE(hashp) - bp[ndx];
	    bytes <= size && bp[ndx + 1] == PARTIAL_KEY;
	    bytes = HASH_BSIZE(hashp) - bp[ndx]) {
		if (memcmp(p + bp[ndx], kkey, (size_t)bytes))
			return (-2);
		kkey += bytes;
		ksize -= bytes;
		bufp = __get_buf(hashp, (uint32_t)bp[ndx + 2], bufp, 0);
		if (!bufp)
			return (-3);
		p = bufp->page;
		bp = (uint16_t *)(void *)p;
		ndx = 1;
	}

	if (bytes != ksize || memcmp(p + bp[ndx], kkey, (size_t)bytes)) {
#ifdef HASH_STATISTICS
		++hash_collisions;
#endif
		return (-2);
	} else
		return (ndx);
}

/*
 * Given the buffer pointer of the first overflow page of a big pair,
 * find the end of the big pair
 *
 * This will set bpp to the buffer header of the last page of the big pair.
 * It will return the pageno of the overflow page following the last page
 * of the pair; 0 if there isn't any (i.e. big pair is the last key in the
 * bucket)
 */
uint16_t
__find_last_page(HTAB *hashp, BUFHEAD **bpp)
{
	BUFHEAD *bufp;
	uint16_t *bp, pageno;
	int n;

	bufp = *bpp;
	bp = (uint16_t *)(void *)bufp->page;
	for (;;) {
		n = bp[0];

		/*
		 * This is the last page if: the tag is FULL_KEY_DATA and
		 * either only 2 entries OVFLPAGE marker is explicit there
		 * is freespace on the page.
		 */
		if (bp[2] == FULL_KEY_DATA &&
		    ((n == 2) || (bp[n] == OVFLPAGE) || (FREESPACE(bp))))
			break;

		pageno = bp[n - 1];
		bufp = __get_buf(hashp, (uint32_t)pageno, bufp, 0);
		if (!bufp)
			return (0);	/* Need to indicate an error! */
		bp = (uint16_t *)(void *)bufp->page;
	}

	*bpp = bufp;
	if (bp[0] > 2)
		return (bp[3]);
	else
		return (0);
}

/*
 * Return the data for the key/data pair that begins on this page at this
 * index (index should always be 1).
 */
int
__big_return(HTAB *hashp, BUFHEAD *bufp, int ndx, DBT *val, int set_current)
{
	BUFHEAD *save_p;
	uint16_t *bp, len, off, save_addr;
	char *tp;

	bp = (uint16_t *)(void *)bufp->page;
	while (bp[ndx + 1] == PARTIAL_KEY) {
		bufp = __get_buf(hashp, (uint32_t)bp[bp[0] - 1], bufp, 0);
		if (!bufp)
			return (-1);
		bp = (uint16_t *)(void *)bufp->page;
		ndx = 1;
	}

	if (bp[ndx + 1] == FULL_KEY) {
		bufp = __get_buf(hashp, (uint32_t)bp[bp[0] - 1], bufp, 0);
		if (!bufp)
			return (-1);
		bp = (uint16_t *)(void *)bufp->page;
		save_p = bufp;
		save_addr = save_p->addr;
		off = bp[1];
		len = 0;
	} else
		if (!FREESPACE(bp)) {
			/*
			 * This is a hack.  We can't distinguish between
			 * FULL_KEY_DATA that contains complete data or
			 * incomplete data, so we require that if the data
			 * is complete, there is at least 1 byte of free
			 * space left.
			 */
			off = bp[bp[0]];
			len = bp[1] - off;
			save_p = bufp;
			save_addr = bufp->addr;
			bufp = __get_buf(hashp, (uint32_t)bp[bp[0] - 1], bufp,
			    0);
			if (!bufp)
				return (-1);
			bp = (uint16_t *)(void *)bufp->page;
		} else {
			/* The data is all on one page. */
			tp = (char *)(void *)bp;
			off = bp[bp[0]];
			val->data = (uint8_t *)tp + off;
			val->size = bp[1] - off;
			if (set_current) {
				if (bp[0] == 2) {	/* No more buckets in
							 * chain */
					hashp->cpage = NULL;
					hashp->cbucket++;
					hashp->cndx = 1;
				} else {
					hashp->cpage = __get_buf(hashp,
					    (uint32_t)bp[bp[0] - 1], bufp, 0);
					if (!hashp->cpage)
						return (-1);
					hashp->cndx = 1;
					if (!((uint16_t *)(void *)
					    hashp->cpage->page)[0]) {
						hashp->cbucket++;
						hashp->cpage = NULL;
					}
				}
			}
			return (0);
		}

	val->size = collect_data(hashp, bufp, (int)len, set_current);
	if (val->size == (size_t)-1)
		return (-1);
	if (save_p->addr != save_addr) {
		/* We are pretty short on buffers. */
		errno = EINVAL;			/* OUT OF BUFFERS */
		return (-1);
	}
	memmove(hashp->tmp_buf, (save_p->page) + off, (size_t)len);
	val->data = (uint8_t *)hashp->tmp_buf;
	return (0);
}
/*
 * Count how big the total datasize is by recursing through the pages.  Then
 * allocate a buffer and copy the data as you recurse up.
 */
static int
collect_data(HTAB *hashp, BUFHEAD *bufp, int len, int set)
{
	uint16_t *bp;
	char *p;
	BUFHEAD *xbp;
	uint16_t save_addr;
	int mylen, totlen;

	p = bufp->page;
	bp = (uint16_t *)(void *)p;
	mylen = HASH_BSIZE(hashp) - bp[1];
	save_addr = bufp->addr;

	if (bp[2] == FULL_KEY_DATA) {		/* End of Data */
		totlen = len + mylen;
		if (hashp->tmp_buf)
			free(hashp->tmp_buf);
		if ((hashp->tmp_buf = calloc(1, (size_t)totlen)) == NULL)
			return (-1);
		if (set) {
			hashp->cndx = 1;
			if (bp[0] == 2) {	/* No more buckets in chain */
				hashp->cpage = NULL;
				hashp->cbucket++;
			} else {
				hashp->cpage =
				    __get_buf(hashp, (uint32_t)bp[bp[0] - 1],
				    bufp, 0);
				if (!hashp->cpage)
					return (-1);
				else if (!((uint16_t *)(void *)hashp->cpage->page)[0]) {
					hashp->cbucket++;
					hashp->cpage = NULL;
				}
			}
		}
	} else {
		xbp = __get_buf(hashp, (uint32_t)bp[bp[0] - 1], bufp, 0);
		if (!xbp || ((totlen =
		    collect_data(hashp, xbp, len + mylen, set)) < 1))
			return (-1);
	}
	if (bufp->addr != save_addr) {
		errno = EINVAL;			/* Out of buffers. */
		return (-1);
	}
	memmove(&hashp->tmp_buf[len], (bufp->page) + bp[1], (size_t)mylen);
	return (totlen);
}

/*
 * Fill in the key and data for this big pair.
 */
int
__big_keydata(HTAB *hashp, BUFHEAD *bufp, DBT *key, DBT *val, int set)
{
	key->size = collect_key(hashp, bufp, 0, val, set);
	if (key->size == (size_t)-1)
		return (-1);
	key->data = (uint8_t *)hashp->tmp_key;
	return (0);
}

/*
 * Count how big the total key size is by recursing through the pages.  Then
 * collect the data, allocate a buffer and copy the key as you recurse up.
 */
static int
collect_key(HTAB *hashp, BUFHEAD *bufp, int len, DBT *val, int set)
{
	BUFHEAD *xbp;
	char *p;
	int mylen, totlen;
	uint16_t *bp, save_addr;

	p = bufp->page;
	bp = (uint16_t *)(void *)p;
	mylen = HASH_BSIZE(hashp) - bp[1];

	save_addr = bufp->addr;
	totlen = len + mylen;
	if (bp[2] == FULL_KEY || bp[2] == FULL_KEY_DATA) {    /* End of Key. */
		if (hashp->tmp_key != NULL)
			free(hashp->tmp_key);
		if ((hashp->tmp_key = calloc(1, (size_t)totlen)) == NULL)
			return (-1);
		if (__big_return(hashp, bufp, 1, val, set))
			return (-1);
	} else {
		xbp = __get_buf(hashp, (uint32_t)bp[bp[0] - 1], bufp, 0);
		if (!xbp || ((totlen =
		    collect_key(hashp, xbp, totlen, val, set)) < 1))
			return (-1);
	}
	if (bufp->addr != save_addr) {
		errno = EINVAL;		/* MIS -- OUT OF BUFFERS */
		return (-1);
	}
	memmove(&hashp->tmp_key[len], (bufp->page) + bp[1], (size_t)mylen);
	return (totlen);
}

/*
 * Returns:
 *  0 => OK
 * -1 => error
 */
int
__big_split(
	HTAB *hashp,
	BUFHEAD *op,	/* Pointer to where to put keys that go in old bucket */
	BUFHEAD *np,	/* Pointer to new bucket page */
			/* Pointer to first page containing the big key/data */
	BUFHEAD *big_keyp,
	int addr,	/* Address of big_keyp */
	uint32_t   obucket,/* Old Bucket */
	SPLIT_RETURN *ret
)
{
	BUFHEAD *tmpp;
	uint16_t *tp;
	BUFHEAD *bp;
	DBT key, val;
	uint32_t change;
	uint16_t free_space, n, off;
	size_t temp;

	bp = big_keyp;

	/* Now figure out where the big key/data goes */
	if (__big_keydata(hashp, big_keyp, &key, &val, 0))
		return (-1);
	change = (__call_hash(hashp, key.data, (int)key.size) != obucket);

	if ((ret->next_addr = __find_last_page(hashp, &big_keyp)) != 0) {
		if (!(ret->nextp =
		    __get_buf(hashp, (uint32_t)ret->next_addr, big_keyp, 0)))
			return (-1);
	} else
		ret->nextp = NULL;

	/* Now make one of np/op point to the big key/data pair */
	_DIAGASSERT(np->ovfl == NULL);
	if (change)
		tmpp = np;
	else
		tmpp = op;

	tmpp->flags |= BUF_MOD;
#ifdef DEBUG1
	(void)fprintf(stderr,
	    "BIG_SPLIT: %d->ovfl was %d is now %d\n", tmpp->addr,
	    (tmpp->ovfl ? tmpp->ovfl->addr : 0), (bp ? bp->addr : 0));
#endif
	tmpp->ovfl = bp;	/* one of op/np point to big_keyp */
	tp = (uint16_t *)(void *)tmpp->page;
	_DIAGASSERT(FREESPACE(tp) >= OVFLSIZE);
	n = tp[0];
	off = OFFSET(tp);
	free_space = FREESPACE(tp);
	tp[++n] = (uint16_t)addr;
	tp[++n] = OVFLPAGE;
	tp[0] = n;
	OFFSET(tp) = off;
	temp = free_space - OVFLSIZE;
	_DBFIT(temp, uint16_t);
	FREESPACE(tp) = (uint16_t)temp;

	/*
	 * Finally, set the new and old return values. BIG_KEYP contains a
	 * pointer to the last page of the big key_data pair. Make sure that
	 * big_keyp has no following page (2 elements) or create an empty
	 * following page.
	 */

	ret->newp = np;
	ret->oldp = op;

	tp = (uint16_t *)(void *)big_keyp->page;
	big_keyp->flags |= BUF_MOD;
	if (tp[0] > 2) {
		/*
		 * There may be either one or two offsets on this page.  If
		 * there is one, then the overflow page is linked on normally
		 * and tp[4] is OVFLPAGE.  If there are two, tp[4] contains
		 * the second offset and needs to get stuffed in after the
		 * next overflow page is added.
		 */
		n = tp[4];
		free_space = FREESPACE(tp);
		off = OFFSET(tp);
		tp[0] -= 2;
		temp = free_space + OVFLSIZE;
		_DBFIT(temp, uint16_t);
		FREESPACE(tp) = (uint16_t)temp;
		OFFSET(tp) = off;
		tmpp = __add_ovflpage(hashp, big_keyp);
		if (!tmpp)
			return (-1);
		tp[4] = n;
	} else
		tmpp = big_keyp;

	if (change)
		ret->newp = tmpp;
	else
		ret->oldp = tmpp;
	return (0);
}
