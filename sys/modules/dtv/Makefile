# $NetBSD$

.include "../Makefile.inc"

.PATH:	${S}/dev/dtv

KMOD=	dtv
IOCONF=	dtv.ioconf
SRCS=	dtv_buffer.c \
	dtv_demux.c \
	dtv_device.c \
	dtv_ioctl.c \
	dtv_scatter.c

WARNS=	4

.include <bsd.kmodule.mk>
