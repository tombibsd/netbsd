#	$NetBSD$
#

.PATH:	${.CURDIR}/../../../../dev

LIB=	rumpdev_cgd
IOCONF=	CGD.ioconf
SRCS=   cgd.c cgd_crypto.c

SRCS+=	cgd_component.c

CPPFLAGS+=	-I${RUMPTOP}/librump/rumpvfs

.include <bsd.lib.mk>
.include <bsd.klinks.mk>
