#	$NetBSD$
#
#	Makefile	1.0	1998/6/26

.include <bsd.own.mk>

PROG=	mount_filecore
SRCS=	mount_filecore.c fattr.c
MAN=	mount_filecore.8

MOUNT=	${NETBSDSRCDIR}/sbin/mount
CPPFLAGS+= -I${MOUNT}
.PATH: ${MOUNT}

DPADD+=${LIBUTIL}
LDADD+=-lutil

.include <bsd.prog.mk>
