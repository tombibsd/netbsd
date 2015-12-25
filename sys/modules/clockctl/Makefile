# $NetBSD$

.include "../Makefile.inc"

.PATH: ${S}/dev

KMOD=		clockctl
IOCONF=		clockctl.ioconf
SRCS=		clockctl.c

CPPFLAGS+=	-DNTP -DCOMPAT_50

.include <bsd.kmodule.mk>
