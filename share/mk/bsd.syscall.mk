# $NetBSD$
#
.include <bsd.own.mk>

SYSCALL_OBJS?=	${SYSCALL_PREFIX}_sysent.c ${SYSCALL_PREFIX}_syscalls.c \
	${SYSCALL_PREFIX}_syscall.h ${SYSCALL_PREFIX}_syscallargs.h

SYSCALL_DEPS?=	${NETBSDSRCDIR}/sys/kern/makesyscalls.sh \
	syscalls.conf syscalls.master

all: ${SYSCALL_OBJS}

${SYSCALL_OBJS}: ${SYSCALL_DEPS}
	${HOST_SH} ${.ALLSRC}

.include <bsd.kinc.mk>
