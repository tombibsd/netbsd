#	$NetBSD$
#

.PATH:	${.CURDIR}/../../../../dev/usb ${.CURDIR}/../../../../kern

LIB=	rumpdev_ucom
COMMENT=USB serial driver

IOCONF= UCOM.ioconf

SRCS=	ucom.c uplcom.c u3g.c

SRCS+=	ucom_at_usb.c

CPPFLAGS+=	-I${RUMPTOP}/librump/rumpvfs

.include <bsd.lib.mk>
.include <bsd.klinks.mk>
