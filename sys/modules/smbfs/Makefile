#	$NetBSD$

.include "../Makefile.inc"

.PATH:	${S}/fs/smbfs

KMOD=	smbfs
SRCS=	smbfs_io.c smbfs_kq.c smbfs_node.c smbfs_smb.c smbfs_subr.c \
	smbfs_vfsops.c smbfs_vnops.c

.include <bsd.kmodule.mk>
