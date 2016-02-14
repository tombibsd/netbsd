#	$NetBSD$
#

.PATH:	${.CURDIR}/../../../../dev

LIB=	rumpdev_cgd
COMMENT=Cryptographic disk driver (block device crypto layer)

IOCONF=	CGD.ioconf
SRCS=   cgd.c cgd_crypto.c

SRCS+=	cgd_component.c

.include <bsd.lib.mk>
.include <bsd.klinks.mk>
