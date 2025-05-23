/*	$NetBSD$	*/

/*
 * Copyright (c) 1997-2014 Erez Zadok
 * Copyright (c) 1990 Jan-Simon Pendry
 * Copyright (c) 1990 Imperial College of Science, Technology & Medicine
 * Copyright (c) 1990 The Regents of the University of California.
 * All rights reserved.
 *
 * This code is derived from software contributed to Berkeley by
 * Jan-Simon Pendry at Imperial College, London.
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
 *
 * File: am-utils/conf/nfs_prot/nfs_prot_aix5_1.h
 *
 */

#ifndef _AMU_NFS_PROT_H
#define _AMU_NFS_PROT_H


/*
 * AIX 5.1 has a different aix5_nfs_args structure, hence the separate header.
 */

#ifdef HAVE_RPCSVC_NFS_PROT_H
# include <rpcsvc/nfs_prot.h>
#endif /* HAVE_RPCSVC_NFS_PROT_H */
#ifdef HAVE_NFS_NFSV2_H
# include <nfs/nfsv2.h>
#endif /* HAVE_NFS_NFSV2_H */
#ifdef HAVE_NFS_RPCV2_H
# include <nfs/rpcv2.h>
#endif /* HAVE_NFS_RPCV2_H */
#ifdef HAVE_SYS_FS_NFS_H
# include <sys/fs/nfs.h>
#endif /* HAVE_SYS_FS_NFS_H */
#ifdef HAVE_RPCSVC_MOUNT_H
# include <rpcsvc/mount.h>
#endif /* HAVE_RPCSVC_MOUNT_H */


/*
 * MACROS
 */

#ifndef MNTPATHLEN
# define MNTPATHLEN 1024
#endif /* not MNTPATHLEN */
#ifndef MNTNAMLEN
# define MNTNAMLEN 255
#endif /* not MNTNAMLEN */

/* map field names */
#define ex_dir ex_name
#define gr_name g_name
#define gr_next g_next
#define ml_directory ml_path
#define ml_hostname ml_name
#define ml_next ml_nxt

#define	dr_drok_u	diropres
#define ca_attributes	attributes
#define ca_where	where
#define da_fhandle	dir
#define da_name		name
#define dl_entries	entries
#define dl_eof		eof
#define dr_status	status
#define dr_u		diropres_u
#define drok_attributes	attributes
#define drok_fhandle	file
#define fh_data		data
#define la_fhandle	from
#define la_to		to
#define na_atime	atime
#define na_blocks	blocks
#define na_blocksize	blocksize
#define na_ctime	ctime
#define na_fileid	fileid
#define na_fsid		fsid
#define na_gid		gid
#define na_mode		mode
#define na_mtime	mtime
#define na_nlink	nlink
#define na_rdev		rdev
#define na_size		size
#define na_type		type
#define na_uid		uid
#define ne_cookie	cookie
#define ne_fileid	fileid
#define ne_name		name
#define ne_nextentry	nextentry
#define ns_attr_u	attributes
#define ns_status	status
#define ns_u		attrstat_u
#define nt_seconds	seconds
#define nt_useconds	useconds
#define ra_count	count
#define ra_fhandle	file
#define ra_offset	offset
#define ra_totalcount	totalcount
#define raok_attributes	attributes
#define raok_len_u	data_len
#define raok_u		data
#define raok_val_u	data_val
#define rda_cookie	cookie
#define rda_count	count
#define rda_fhandle	dir
#define rdr_reply_u	reply
#define rdr_status	status
#define rdr_u		readdirres_u
#define rlr_data_u	data
#define rlr_status	status
#define rlr_u		readlinkres_u
#define rna_from	from
#define rna_to		to
#define rr_reply_u	reply
#define rr_status	status
#define rr_u		readres_u
#define sa_atime	atime
#define sa_gid		gid
#define sa_mode		mode
#define sa_mtime	mtime
#define sa_size		size
#define sa_uid		uid
#define sag_attributes	attributes
#define sag_fhandle	file
#define sfr_reply_u	reply
#define sfr_status	status
#define sfr_u		statfsres_u
#define sfrok_bavail	bavail
#define sfrok_bfree	bfree
#define sfrok_blocks	blocks
#define sfrok_bsize	bsize
#define sfrok_tsize	tsize
#define sla_attributes	attributes
#define sla_from	from
#define sla_to		to
#define wra_beginoffset	beginoffset
#define wra_fhandle	file
#define wra_len_u	data_len
#define wra_offset	offset
#define wra_totalcount	totalcount
#define wra_u		data
#define wra_val_u	data_val


/*
 * TYPEDEFS:
 */
typedef char *dirpath;
typedef char *name;
typedef struct exports *exports;
typedef struct exports exportnode;
typedef struct groups *groups;
typedef struct groups groupnode;
typedef struct mountlist *mountlist;

typedef attrstat	nfsattrstat;
typedef createargs	nfscreateargs;
typedef dirlist		nfsdirlist;
typedef diropargs	nfsdiropargs;
typedef diropokres	nfsdiropokres;
typedef diropres	nfsdiropres;
typedef entry		nfsentry;
typedef fattr		nfsfattr;
typedef ftype		nfsftype;
typedef linkargs	nfslinkargs;
typedef readargs	nfsreadargs;
typedef readdirargs	nfsreaddirargs;
typedef readdirres	nfsreaddirres;
typedef readlinkres	nfsreadlinkres;
typedef readokres	nfsreadokres;
typedef readres		nfsreadres;
typedef renameargs	nfsrenameargs;
typedef sattr		nfssattr;
typedef sattrargs	nfssattrargs;
typedef statfsokres	nfsstatfsokres;
typedef statfsres	nfsstatfsres;
typedef symlinkargs	nfssymlinkargs;
typedef writeargs	nfswriteargs;


/*
 * EXTERNALS:
 */


/*
 * STRUCTURES:
 */

/*
 * AIX 5.1 has NFS V3, but it is undefined in the header files.
 * so I define everything that's needed for NFS V3 here.
 */
#ifdef MNT_NFS3

struct aix5_nfs_args {
  struct sockaddr_in addr;	/* server address and port */
  caddr_t u0;			/* ??? UNKNOWN ??? */
  unsigned long proto;		/* IPPROTO_TCP/IPPROTO_UDP */
  char *hostname;		/* pointer to hostname? */
  char *netname;		/* pointer to netname? */
  caddr_t fh;			/* pointer to NFS v3 fh? */
  unsigned long u5;		/* IBM sux, IBM sux, IBM sux... */
  unsigned long flags;		/* flags */
  unsigned long wsize;		/* wsize */
  unsigned long rsize;		/* rsize */
  unsigned long timeo;		/* timeo */
  unsigned long retrans;	/* retrans */
  unsigned long acregmin;	/* acregmin */
  unsigned long acregmax;	/* acregmax */
  unsigned long acdirmin;	/* acdirmin */
  unsigned long acdirmax;	/* acdirmax */
  unsigned long u15;		/* ??? UNKNOWN ??? */
  struct pathcnf *pathconf;	/* pathconf */
};

#endif /* MNT_NFS3 */

#ifdef MNT_AUTOFS
#define A_MAXNAME 255
#define A_MAXOPTS 255
#define A_MAXPATH 1024

struct mntrequest {
  char *name;
  char *map;
  char *opts;
  char *path;
};
typedef struct mntrequest mntrequest;

struct mntres {
  int status;
};
typedef struct mntres mntres;

struct umntrequest {
  int isdirect;
  u_int devid;
  u_long rdevid;
  struct umntrequest *next;
};
typedef struct umntrequest umntrequest;

struct umntres {
  int status;
};
typedef struct umntres umntres;

#define AUTOFS_PROG ((unsigned long)(100099))
#define AUTOFS_VERS ((unsigned long)(1))

#endif /* MNT_AUTOFS */

/*
 **************************************************************************
 * AIX 5.1's autofs is not ported or tested yet...
 * For now, undefine it or define dummy entries.
 **************************************************************************
 */
#ifdef MNT_AUTOFS
# undef MNT_AUTOFS
#endif /* MNT_AUTOFS */
#ifdef HAVE_FS_AUTOFS
# undef HAVE_FS_AUTOFS
#endif /* HAVE_FS_AUTOFS */

/*
 * EXTERNALS:
 */
extern bool_t xdr_groups(XDR *xdrs, groups *objp);
extern char *yperr_string (int incode);

#endif /* not _AMU_NFS_PROT_H */
