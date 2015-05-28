# $NetBSD$

.include "../Makefile.inc"

.PATH:	${S}/kern

KMOD=	sysv_ipc

CPPFLAGS+=	-DSYSVSEM -DSYSVSHM -DSYSVMSG

SRCS=	sysv_ipc.c sysv_msg.c sysv_sem.c sysv_shm.c

.include <bsd.kmodule.mk>
