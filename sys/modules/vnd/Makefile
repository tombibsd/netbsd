#	$NetBSD$

.include "../Makefile.inc"

CPPFLAGS+= -DVND_COMPRESSION -DCOMPAT_30 -DCOMPAT_50

.PATH:	${S}/dev


KMOD=	vnd
IOCONF=	vnd.ioconf
SRCS=	vnd.c

.include <bsd.kmodule.mk>
