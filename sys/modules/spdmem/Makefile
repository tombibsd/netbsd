# $NetBSD$

.include "../Makefile.inc"

.PATH:	${S}/dev/i2c

KMOD=	spdmem
IOCONF=	spdmem.ioconf
SRCS=	spdmem_i2c.c

.PATH:	${S}/dev/ic

SRCS+=	spdmem.c

WARNS=	4

.include <bsd.kmodule.mk>
