#	$NetBSD$
#

.PATH:  ${.CURDIR}/../../../../fs/nilfs

LIB=	rumpfs_nilfs
COMMENT=NILFS

SRCS=	nilfs_subr.c nilfs_vfsops.c nilfs_vnops.c

CWARNFLAGS.clang+=	-Wno-self-assign

.include <bsd.lib.mk>
.include <bsd.klinks.mk>
