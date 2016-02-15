#	$NetBSD$

.include "../Makefile.inc"

.PATH:	${S}/fs/msdosfs

KMOD=	msdos
SRCS=	msdosfs_conv.c msdosfs_denode.c msdosfs_fat.c msdosfs_lookup.c \
	msdosfs_vfsops.c msdosfs_vnops.c msdosfs_unicode.c

.include <bsd.kmodule.mk>
