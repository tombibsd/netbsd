#	$NetBSD$
#

LIB=	rumpnet_sockin
COMMENT=PF_INET/PF_INET6 via hypercalls

SRCS=	sockin.c
SRCS+=	sockin_component.c

RUMPCOMP_USER_SRCS=	sockin_user.c

.include <bsd.lib.mk>
.include <bsd.klinks.mk>
