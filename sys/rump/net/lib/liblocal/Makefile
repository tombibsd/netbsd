#	$NetBSD$
#

.PATH:	${.CURDIR}/../../../../kern

LIB=	rumpnet_local
COMMENT=Local domain sockets (PF_LOCAL/PF_UNIX)

SRCS=	uipc_proto.c uipc_usrreq.c
SRCS+=	local_component.c

.include <bsd.lib.mk>
.include <bsd.klinks.mk>
