#	$NetBSD$
#

.PATH:	${.CURDIR}/../../../../fs/tmpfs

LIB=	rumpfs_tmpfs
COMMENT=tmpfs (efficient in-memory file system)

SRCS=	tmpfs_fifoops.c tmpfs_specops.c tmpfs_vfsops.c tmpfs_mem.c \
	tmpfs_rename.c tmpfs_subr.c tmpfs_vnops.c

.include <bsd.lib.mk>
.include <bsd.klinks.mk>
