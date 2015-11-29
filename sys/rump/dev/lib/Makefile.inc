#	$NetBSD$
#

RUMPTOP=	${.CURDIR}/../../..
CPPFLAGS+=	-I${RUMPTOP}/librump/rumpkern -I${RUMPTOP}/librump/rumpdev

RUMPDEVLIBDIR:= ${.PARSEDIR}

.if defined(RUMP_COMPONENT) && !empty(RUMP_COMPONENT:Mioconf)
.PATH:	${RUMPDEVLIBDIR}
SRCS+=   component_simple.c
.endif

.include "${RUMPTOP}/Makefile.rump"
