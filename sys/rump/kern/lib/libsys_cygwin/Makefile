#	$NetBSD$
#

LIB=	rumpkern_sys_cygwin
COMMENT=Cygwin system call translation

SRCS=	rump_cygwin_compat.c
SRCS+=	sys_cygwin_component.c

# XXX
CPPFLAGS+= -I${RUMPTOP}/librump/rumpkern

SYS_P=rump_cygwin_sys
.include "../Makefile.sys"

.include <bsd.lib.mk>
.include <bsd.klinks.mk>
