#	$NetBSD$
#

RUMPTOP=${TOPRUMP}

.PATH:	${RUMPTOP}/../dev/pci

LIB=	rumpdev_pci_if_iwn
COMMENT=Intel wireless device driver

IOCONF=	PCI_IF_IWN.ioconf
RUMP_COMPONENT=ioconf

SRCS+=	if_iwn.c

CPPFLAGS+= -I${RUMPTOP}/librump/rumpkern
   
.include "${RUMPTOP}/Makefile.rump"
.include <bsd.lib.mk>
.include <bsd.klinks.mk>
