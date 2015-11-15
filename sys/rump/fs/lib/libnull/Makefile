#	$NetBSD$
#

.PATH:  ${.CURDIR}/../../../../miscfs/nullfs

LIB=	rumpfs_null
COMMENT=Loopback file system

SRCS=	null_vfsops.c null_vnops.c

CWARNFLAGS.clang+=	-Wno-conversion

.include <bsd.lib.mk>
.include <bsd.klinks.mk>
