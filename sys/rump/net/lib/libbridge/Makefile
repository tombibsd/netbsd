#	$NetBSD$
#

.PATH:	${.CURDIR}/../../../../net

LIB=	rumpnet_bridge
IOCONF=	BRIDGE.ioconf
SRCS=	if_bridge.c bridgestp.c

SRCS+=	bridge_component.c

CPPFLAGS+=	-I${.CURDIR}/../libnet/opt

.include <bsd.lib.mk>
.include <bsd.klinks.mk>
