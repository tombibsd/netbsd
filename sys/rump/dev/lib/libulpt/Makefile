#	$NetBSD$
#

.PATH:	${.CURDIR}/../../../../dev/usb

LIB=	rumpdev_ulpt
COMMENT=USB printer driver

IOCONF=	ULPT.ioconf

SRCS=	ulpt.c

SRCS+=	ulpt_at_usb.c

.include <bsd.lib.mk>
.include <bsd.klinks.mk>
