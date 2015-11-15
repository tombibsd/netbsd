#	$NetBSD$
#

.PATH:  ${.CURDIR}/../../../../nfs

LIB=	rumpfs_nfsserver
COMMENT=NFS server

SRCS=	nfs_export.c nfs_serv.c nfs_srvcache.c nfs_srvsubs.c		\
	nfs_srvsocket.c nfs_syscalls.c

.include <bsd.lib.mk>
.include <bsd.klinks.mk>
