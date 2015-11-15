#	$NetBSD$
#

RUMPTOP=${TOPRUMP}

.include <bsd.own.mk>
.include "${NETBSDSRCDIR}/sys/modules/hdaudio/Makefile.inc"
.include "${NETBSDSRCDIR}/sys/modules/hdaudio_pci/Makefile.inc"

LIB=	rumpdev_pci_hdaudio
COMMENT=HDaudio PCI attachment
   
.include "${RUMPTOP}/Makefile.rump"
.include <bsd.lib.mk>
.include <bsd.klinks.mk>
