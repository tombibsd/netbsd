# $NetBSD$

.include "../Makefile.inc"

.PATH:	${S}/dev/iscsi

KMOD=	iscsi
SRCS=	base64.c iscsi_ioctl.c iscsi_main.c iscsi_rcv.c
SRCS+=	iscsi_send.c iscsi_text.c iscsi_utils.c

.include <bsd.kmodule.mk>
