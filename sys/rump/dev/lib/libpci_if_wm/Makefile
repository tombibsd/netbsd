#	$NetBSD$
#

RUMPTOP=${TOPRUMP}

.PATH:	${RUMPTOP}/../dev/pci

LIB=	rumpdev_pci_if_wm
COMMENT=Intel GigE device driver

IOCONF=	PCI_IF_WM.ioconf
RUMP_COMPONENT=ioconf

SRCS+=	if_wm.c

CPPFLAGS+= -I${RUMPTOP}/librump/rumpkern
   
.include "${RUMPTOP}/Makefile.rump"
.include <bsd.lib.mk>
.include <bsd.klinks.mk>
