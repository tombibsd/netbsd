#	$NetBSD$
#

LIB=	rumpkern_sysproxy
COMMENT=Remote system call support (rump kernel as a server)

SRCS=	sysproxy.c

CPPFLAGS+= -I${RUMPTOP}/librump/rumpkern

.include <bsd.lib.mk>
.include <bsd.klinks.mk>
