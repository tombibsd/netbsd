#	$NetBSD$
#

.PATH:	${.CURDIR}/../../../../net

LIB=	rumpnet_bridge
COMMENT=Bridging for IEEE 802

IOCONF=	BRIDGE.ioconf
SRCS=	if_bridge.c bridgestp.c

SRCS+=	bridge_component.c

.include <bsd.lib.mk>
.include <bsd.klinks.mk>
