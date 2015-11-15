#	$NetBSD$
#

.PATH:  ${.CURDIR}/../../../../miscfs/umapfs

LIB=	rumpfs_umap
COMMENT=uid/gid mapping layer

SRCS=	umap_subr.c umap_vfsops.c umap_vnops.c

CWARNFLAGS.clang+=	-Wno-conversion

.include <bsd.lib.mk>
.include <bsd.klinks.mk>
