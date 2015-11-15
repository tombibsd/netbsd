#	$NetBSD$
#

.PATH:	${.CURDIR}/../../../../dev/usb ${.CURDIR}/../../../../dev/bluetooth

LIB=	rumpdev_ubt
COMMENT=USB BlueTooth driver

IOCONF= UBT.ioconf

SRCS=	ubt.c bthub.c

SRCS+=	ubt_at_usb.c

.include <bsd.lib.mk>
.include <bsd.klinks.mk>
