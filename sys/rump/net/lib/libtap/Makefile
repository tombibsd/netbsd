#	$NetBSD$
#

.PATH:	${.CURDIR}/../../../../net

LIB=	rumpnet_tap

SRCS=	if_tap.c

SRCS+=	tap_component.c

CPPFLAGS+=	-I${.CURDIR}/../libnet/opt -I${.CURDIR}/../../../librump/rumpvfs

.include <bsd.lib.mk>
.include <bsd.klinks.mk>
