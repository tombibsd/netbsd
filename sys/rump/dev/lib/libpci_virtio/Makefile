#	$NetBSD$
#

RUMPTOP=${TOPRUMP}

.PATH:	${RUMPTOP}/../dev/pci

LIB=	rumpdev_pci_virtio
COMMENT=VirtIO bus support

IOCONF=	PCI_VIRTIO.ioconf

SRCS=	virtio_at_pci.c

SRCS+=	virtio.c

CPPFLAGS+= -I${RUMPTOP}/librump/rumpkern
   
.include "${RUMPTOP}/Makefile.rump"
.include <bsd.lib.mk>
.include <bsd.klinks.mk>
