# $NetBSD$

PROG=id3fs
SRCS=id3fs.c virtdir.c
LDADD+= -lrefuse
CPPFLAGS+= -g -I${.CURDIR}/../virtdir
MAN=id3fs.8
WARNS=4

.PATH: ${.CURDIR}/../virtdir

.include <bsd.prog.mk>
