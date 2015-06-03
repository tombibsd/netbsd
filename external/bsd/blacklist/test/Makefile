# $NetBSD$

MKMAN=no

PROGS=srvtest cltest 
SRCS.srvtest = srvtest.c
SRCS.cltest = cltest.c
CPPFLAGS+=-DBLDEBUG
LDADD+=-lutil
DPADD+=${LIBUTIL}

.include <bsd.prog.mk>
