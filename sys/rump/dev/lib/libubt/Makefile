#	$NetBSD$
#

.PATH:	${.CURDIR}/../../../../dev/usb ${.CURDIR}/../../../../dev/bluetooth

LIB=	rumpdev_ubt
COMMENT=USB BlueTooth driver

IOCONF= UBT.ioconf
RUMP_COMPONENT=ioconf

SRCS=	ubt.c bthub.c

.include <bsd.lib.mk>
.include <bsd.klinks.mk>
