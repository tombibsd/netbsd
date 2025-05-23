/*	$NetBSD$	*/
/*  from NetBSD: dirhash.h,v 1.6 2008/06/04 11:33:19 ad Exp  */

/*
 * Copyright (c) 2001 Ian Dowse.  All rights reserved.
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
 *
 * $FreeBSD: src/sys/ufs/ufs/dirhash.h,v 1.2.2.2 2004/12/08 11:54:13 dwmalone Exp $
 */

#ifndef _UFS_LFS_ULFS_DIRHASH_H_
#define _UFS_LFS_ULFS_DIRHASH_H_

/*
 * For fast operations on large directories, we maintain a hash
 * that maps the file name to the offset of the directory entry within
 * the directory file.
 *
 * The hashing uses a dumb spillover to the next free slot on
 * collisions, so we must keep the utilisation low to avoid
 * long linear searches. Deleted entries that are not the last
 * in a chain must be marked DIRHASH_DEL.
 *
 * We also maintain information about free space in each block
 * to speed up creations.
 */
#define DIRHASH_EMPTY	(-1)	/* entry unused */
#define DIRHASH_DEL	(-2)	/* deleted entry; may be part of chain */

#define DIRALIGN	4
#define DH_NFSTATS	(LFS_MAXDIRENTRYSIZE / DIRALIGN)
			 /* max DIRALIGN words in a directory entry */

/*
 * Dirhash uses a score mechanism to achieve a hybrid between a
 * least-recently-used and a least-often-used algorithm for entry
 * recycling. The score is incremented when a directory is used, and
 * decremented when the directory is a candidate for recycling. When
 * the score reaches zero, the hash is recycled. Hashes are linked
 * together on a TAILQ list, and hashes with higher scores filter
 * towards the tail (most recently used) end of the list.
 *
 * New hash entries are given an inital score of DH_SCOREINIT and are
 * placed at the most-recently-used end of the list. This helps a lot
 * in the worst-case case scenario where every directory access is
 * to a directory that is not hashed (i.e. the working set of hash
 * candidates is much larger than the configured memry limit). In this
 * case it limits the number of hash builds to 1/DH_SCOREINIT of the
 * number of accesses.
 */
#define DH_SCOREINIT	8	/* initial dh_score when dirhash built */
#define DH_SCOREMAX	64	/* max dh_score value */

/*
 * The main hash table has 2 levels. It is an array of pointers to
 * blocks of DH_NBLKOFF offsets.
 */
#define DH_BLKOFFSHIFT	8
#define DH_NBLKOFF	(1 << DH_BLKOFFSHIFT)
#define DH_BLKOFFMASK	(DH_NBLKOFF - 1)

#define DH_ENTRY(dh, slot) \
    ((dh)->dh_hash[(slot) >> DH_BLKOFFSHIFT][(slot) & DH_BLKOFFMASK])

struct dirhash {
	kmutex_t dh_lock;	/* protects all fields except dh_list */

	doff_t	**dh_hash;	/* the hash array (2-level) */
	size_t	dh_hashsz;
	int	dh_narrays;	/* number of entries in dh_hash */
	int	dh_hlen;	/* total slots in the 2-level hash array */
	int	dh_hused;	/* entries in use */

	u_int8_t *dh_blkfree;	/* free DIRALIGN words in each dir block */
	size_t	dh_blkfreesz;
	int	dh_nblk;	/* size of dh_blkfree array */
	int	dh_dirblks;	/* number of DIRBLKSIZ blocks in dir */
	int	dh_firstfree[DH_NFSTATS + 1]; /* first blk with N words free */

	int	dh_seqopt;	/* sequential access optimisation enabled */
	doff_t	dh_seqoff;	/* sequential access optimisation offset */

	int	dh_score;	/* access count for this dirhash */

	int	dh_onlist;	/* true if on the ulfsdirhash_list chain */

	/* Protected by ulfsdirhash_lock. */
	TAILQ_ENTRY(dirhash) dh_list;	/* chain of all dirhashes */
};


/*
 * Dirhash functions.
 */
int	ulfsdirhash_build(struct inode *);
doff_t	ulfsdirhash_findfree(struct inode *, int, int *);
doff_t	ulfsdirhash_enduseful(struct inode *);
int	ulfsdirhash_lookup(struct inode *, const char *, int, doff_t *,
	    struct buf **, doff_t *);
void	ulfsdirhash_newblk(struct inode *, doff_t);
void	ulfsdirhash_add(struct inode *, LFS_DIRHEADER *, doff_t);
void	ulfsdirhash_remove(struct inode *, LFS_DIRHEADER *, doff_t);
void	ulfsdirhash_move(struct inode *, LFS_DIRHEADER *, doff_t, doff_t);
void	ulfsdirhash_dirtrunc(struct inode *, doff_t);
void	ulfsdirhash_free(struct inode *);
void	ulfsdirhash_checkblock(struct inode *, char *, doff_t);
void	ulfsdirhash_init(void);
void	ulfsdirhash_done(void);

#endif /* !_UFS_LFS_ULFS_DIRHASH_H_ */
