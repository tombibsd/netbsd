#	$NetBSD$

.include "../Makefile.inc"

.PATH:	${S}/dev/usb

KMOD=	emdtv
IOCONF=	emdtv.ioconf
SRCS=	emdtv.c
SRCS+=	emdtv_board.c
SRCS+=	emdtv_dtv.c
SRCS+=	emdtv_i2c.c
SRCS+=	emdtv_ir.c

WARNS=	4

.include <bsd.kmodule.mk>
