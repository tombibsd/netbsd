/*	$NetBSD$	*/

/*
 * Copyright (c) 2005, 2006, 2007 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Julio M. Merino Vidal, developed as part of Google's Summer of Code
 * 2005 program.
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

#ifndef _FS_TMPFS_TMPFS_H_
#define _FS_TMPFS_TMPFS_H_

#if !defined(_KERNEL) && !defined(_KMEMUSER)
#error "not supposed to be exposed to userland"
#endif

#include <sys/dirent.h>
#include <sys/mount.h>
#include <sys/pool.h>
#include <sys/queue.h>
#include <sys/vnode.h>

/*
 * Internal representation of a tmpfs directory entry.
 *
 * All fields are protected by vnode lock.
 */
typedef struct tmpfs_dirent {
	TAILQ_ENTRY(tmpfs_dirent)	td_entries;

	/* Pointer to the inode this entry refers to. */
	struct tmpfs_node *		td_node;

	/* Sequence number, see tmpfs_dir_getseq(). */
	uint32_t			td_seq;

	/* Name and its length. */
	char *				td_name;
	uint16_t			td_namelen;
} tmpfs_dirent_t;

TAILQ_HEAD(tmpfs_dir, tmpfs_dirent);

/*
 * Internal representation of a tmpfs file system node -- inode.
 *
 * This structure is split in two parts: one holds attributes common
 * to all file types and the other holds data that is only applicable to
 * a particular type.
 *
 * All fields are protected by vnode lock.  The vnode association itself
 * is protected by vcache.
 */
typedef struct tmpfs_node {
	LIST_ENTRY(tmpfs_node)	tn_entries;

	/*
	 * Each inode has a corresponding vnode.  It is a bi-directional
	 * association.  Whenever vnode is allocated, its v_data field is
	 * set to the inode it reference, and tmpfs_node_t::tn_vnode is
	 * set to point to the said vnode.
	 *
	 * Further attempts to allocate a vnode for this same node will
	 * result in returning a new reference to the value stored in
	 * tn_vnode.  It may be NULL when the node is unused (that is,
	 * no vnode has been allocated or it has been reclaimed).
	 */
	vnode_t *		tn_vnode;

	/* Prevent node from being reclaimed. */
	uint32_t		tn_holdcount;

	/* Directory entry.  Only a hint, since hard link can have multiple. */
	tmpfs_dirent_t *	tn_dirent_hint;

	/* The inode type: VBLK, VCHR, VDIR, VFIFO, VLNK, VREG or VSOCK. */
	enum vtype		tn_type;

	/* Inode identifier and generation number. */
	ino_t			tn_id;
	uint32_t		tn_gen;

	/* The inode size. */
	off_t			tn_size;

	/* Generic node attributes. */
	uid_t			tn_uid;
	gid_t			tn_gid;
	mode_t			tn_mode;
	int			tn_flags;
	nlink_t			tn_links;
	struct timespec		tn_atime;
	struct timespec		tn_mtime;
	struct timespec		tn_ctime;
	struct timespec		tn_birthtime;

	/* Head of byte-level lock list (used by tmpfs_advlock). */
	struct lockf *		tn_lockf;

	union {
		/* Type case: VBLK or VCHR. */
		struct {
			dev_t			tn_rdev;
		} tn_dev;

		/* Type case: VDIR. */
		struct {
			/* Parent directory (root inode points to itself). */
			struct tmpfs_node *	tn_parent;

			/* List of directory entries. */
			struct tmpfs_dir	tn_dir;

			/* Last given sequence number and their arena. */
			uint32_t		tn_next_seq;
			void *			tn_seq_arena;

			/*
			 * Pointer of the last directory entry returned
			 * by the readdir(3) operation.
			 */
			struct tmpfs_dirent *	tn_readdir_lastp;
		} tn_dir;

		/* Type case: VLNK. */
		struct tn_lnk {
			/* The link's target. */
			char *			tn_link;
		} tn_lnk;

		/* Type case: VREG. */
		struct tn_reg {
			/* Underlying UVM object to store contents. */
			struct uvm_object *	tn_aobj;
			size_t			tn_aobj_pages;
		} tn_reg;
	} tn_spec;
} tmpfs_node_t;

#if defined(_KERNEL)

VFS_PROTOS(tmpfs);

LIST_HEAD(tmpfs_node_list, tmpfs_node);

#define	TMPFS_MAXNAMLEN		255
/* Validate maximum td_namelen length. */
CTASSERT(TMPFS_MAXNAMLEN < UINT16_MAX);

/*
 * Reserved values for the virtual entries (the first must be 0) and EOF.
 * The start/end of the incremental range, see tmpfs_dir_getseq().
 */
#define	TMPFS_DIRSEQ_DOT	0
#define	TMPFS_DIRSEQ_DOTDOT	1
#define	TMPFS_DIRSEQ_EOF	2

#define	TMPFS_DIRSEQ_START	3		/* inclusive */
#define	TMPFS_DIRSEQ_END	(1U << 30)	/* exclusive */

/* Mark to indicate that the number is not set. */
#define	TMPFS_DIRSEQ_NONE	(1U << 31)

/* Flags: time update requests. */
#define	TMPFS_UPDATE_ATIME	0x01
#define	TMPFS_UPDATE_MTIME	0x02
#define	TMPFS_UPDATE_CTIME	0x04

/*
 * Bits indicating whiteout use for the directory.
 * We abuse tmpfs_node_t::tn_gen for that.
 */
#define	TMPFS_WHITEOUT_BIT	(1U << 31)
#define	TMPFS_NODE_GEN_MASK	(TMPFS_WHITEOUT_BIT - 1)

#define	TMPFS_NODE_GEN(node) \
    ((node)->tn_gen & TMPFS_NODE_GEN_MASK)

/* White-out inode indicator. */
#define	TMPFS_NODE_WHITEOUT	((tmpfs_node_t *)-1)

/*
 * Bit indicating this node must be reclaimed when holdcount reaches zero.
 * Ored into tmpfs_node_t::tn_holdcount.
 */
#define TMPFS_NODE_RECLAIMED		(1U << 30)

/*
 * Internal representation of a tmpfs mount point.
 */
typedef struct tmpfs_mount {
	/* Limit and number of bytes in use by the file system. */
	uint64_t		tm_mem_limit;
	uint64_t		tm_bytes_used;
	kmutex_t		tm_acc_lock;

	/* Pointer to the root inode. */
	tmpfs_node_t *		tm_root;

	/* Maximum number of possible nodes for this file system. */
	unsigned int		tm_nodes_max;

	/* Number of nodes currently allocated. */
	unsigned int		tm_nodes_cnt;

	/* List of inodes and the lock protecting it. */
	kmutex_t		tm_lock;
	struct tmpfs_node_list	tm_nodes;
} tmpfs_mount_t;

/*
 * This structure maps a file identifier to a tmpfs node.  Used by the
 * NFS code.
 */
typedef struct tmpfs_fid {
	uint16_t		tf_len;
	uint16_t		tf_pad;
	uint32_t		tf_gen;
	ino_t			tf_id;
} tmpfs_fid_t;

/*
 * Prototypes for tmpfs_subr.c.
 */

void		tmpfs_free_node(tmpfs_mount_t *, tmpfs_node_t *);

int		tmpfs_construct_node(vnode_t *, vnode_t **, struct vattr *,
		    struct componentname *, char *);

int		tmpfs_alloc_dirent(tmpfs_mount_t *, const char *, uint16_t,
		    tmpfs_dirent_t **);
void		tmpfs_free_dirent(tmpfs_mount_t *, tmpfs_dirent_t *);
void		tmpfs_dir_attach(tmpfs_node_t *, tmpfs_dirent_t *, tmpfs_node_t *);
void		tmpfs_dir_detach(tmpfs_node_t *, tmpfs_dirent_t *);

tmpfs_dirent_t *tmpfs_dir_lookup(tmpfs_node_t *, struct componentname *);
tmpfs_dirent_t *tmpfs_dir_cached(tmpfs_node_t *);

uint32_t	tmpfs_dir_getseq(tmpfs_node_t *, tmpfs_dirent_t *);
tmpfs_dirent_t *tmpfs_dir_lookupbyseq(tmpfs_node_t *, off_t);
int		tmpfs_dir_getdents(tmpfs_node_t *, struct uio *, off_t *);

int		tmpfs_reg_resize(vnode_t *, off_t);

int		tmpfs_chflags(vnode_t *, int, kauth_cred_t, lwp_t *);
int		tmpfs_chmod(vnode_t *, mode_t, kauth_cred_t, lwp_t *);
int		tmpfs_chown(vnode_t *, uid_t, gid_t, kauth_cred_t, lwp_t *);
int		tmpfs_chsize(vnode_t *, u_quad_t, kauth_cred_t, lwp_t *);
int		tmpfs_chtimes(vnode_t *, const struct timespec *,
		    const struct timespec *, const struct timespec *, int,
		    kauth_cred_t, lwp_t *);
void		tmpfs_update(vnode_t *, unsigned);

/*
 * Prototypes for tmpfs_mem.c.
 */

void		tmpfs_mntmem_init(tmpfs_mount_t *, uint64_t);
void		tmpfs_mntmem_destroy(tmpfs_mount_t *);
int		tmpfs_mntmem_set(tmpfs_mount_t *, uint64_t);

size_t		tmpfs_mem_info(bool);
uint64_t	tmpfs_bytes_max(tmpfs_mount_t *);
size_t		tmpfs_pages_avail(tmpfs_mount_t *);
bool		tmpfs_mem_incr(tmpfs_mount_t *, size_t);
void		tmpfs_mem_decr(tmpfs_mount_t *, size_t);

tmpfs_dirent_t *tmpfs_dirent_get(tmpfs_mount_t *);
void		tmpfs_dirent_put(tmpfs_mount_t *, tmpfs_dirent_t *);

tmpfs_node_t *	tmpfs_node_get(tmpfs_mount_t *);
void		tmpfs_node_put(tmpfs_mount_t *, tmpfs_node_t *);

char *		tmpfs_strname_alloc(tmpfs_mount_t *, size_t);
void		tmpfs_strname_free(tmpfs_mount_t *, char *, size_t);
bool		tmpfs_strname_neqlen(struct componentname *, struct componentname *);

/*
 * Ensures that the node pointed by 'node' is a directory and that its
 * contents are consistent with respect to directories.
 */
#define	TMPFS_VALIDATE_DIR(node) \
    KASSERT((node)->tn_vnode == NULL || VOP_ISLOCKED((node)->tn_vnode)); \
    KASSERT((node)->tn_type == VDIR); \
    KASSERT((node)->tn_size % sizeof(tmpfs_dirent_t) == 0);

/*
 * Routines to convert VFS structures to tmpfs internal ones.
 */

static inline tmpfs_mount_t *
VFS_TO_TMPFS(struct mount *mp)
{
	tmpfs_mount_t *tmp = mp->mnt_data;

	KASSERT(tmp != NULL);
	return tmp;
}

static inline tmpfs_node_t *
VP_TO_TMPFS_DIR(vnode_t *vp)
{
	tmpfs_node_t *node = vp->v_data;

	KASSERT(node != NULL);
	TMPFS_VALIDATE_DIR(node);
	return node;
}

#endif /* defined(_KERNEL) */

static __inline tmpfs_node_t *
VP_TO_TMPFS_NODE(vnode_t *vp)
{
	tmpfs_node_t *node = vp->v_data;
#ifdef KASSERT
	KASSERT(node != NULL);
#endif
	return node;
}

#endif /* _FS_TMPFS_TMPFS_H_ */
