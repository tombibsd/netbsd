#	$NetBSD$

.include "../Makefile.inc"

.PATH:		${S}/dev/gpio

KMOD=   	gpiosim
IOCONF=		gpiosim.conf
SRCS=		gpiosim.c

CPPFLAGS+=	-I${S}/gpio

.include <bsd.kmodule.mk>
