#	$NetBSD$
#

RUMPTOP=${TOPRUMP}

.PATH:	${RUMPTOP}/../dev/pci

LIB=	rumpdev_virtio_if_vioif
COMMENT=VirtIO network interface driver

IOCONF=	VIRTIO_IF_VIOIF.ioconf

SRCS=	vioif_at_virtio.c

SRCS+=	if_vioif.c

CPPFLAGS+= -I${RUMPTOP}/librump/rumpkern
   
.include "${RUMPTOP}/Makefile.rump"
.include <bsd.lib.mk>
.include <bsd.klinks.mk>
