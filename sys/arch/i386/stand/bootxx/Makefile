# $NetBSD$

SUBDIR=	bootxx_ffsv1 .WAIT bootxx_ffsv2 bootxx_lfsv1 bootxx_lfsv2
SUBDIR+=bootxx_msdos bootxx_ustarfs

# Ext2fs doesn't have enough free space (it has only 1KB)
# to store this primary bootloader, but we can put it into
# an independent small 'boot' partition as NetBSD/hp300 does.
SUBDIR+=bootxx_ext2fs

LIBOBJ= ${.OBJDIR}
.MAKEOVERRIDES+= LIBOBJ

.include <bsd.subdir.mk>
.include <bsd.obj.mk>

cleandir distclean: .WAIT cleanlibdir

cleanlibdir:
	-rm -rf lib
