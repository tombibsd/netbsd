#	$NetBSD$

SRCS=	midirecord decode.c
PROG=	midirecord

.include <bsd.own.mk>

#LIBAUDIO != cd ${.CURDIR}/../audio/common && ${PRINTOBJDIR}
CPPFLAGS+=-I${.CURDIR}/../audio/common
#DPADD+=	${LIBAUDIO}/libaudio.a
#LDADD+=	-L${LIBAUDIO} -laudio

DPADD+=	${LIBUTIL}
LDADD+=	-lutil

.PATH: ${.CURDIR}/../audio/common

.include <bsd.prog.mk>
