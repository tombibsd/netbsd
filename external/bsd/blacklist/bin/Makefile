# $NetBSD$

BINDIR=/sbin

PROGS=blacklistd blacklistctl
MAN.blacklistd=blacklistd.8 blacklistd.conf.5
MAN.blacklistctl=blacklistctl.8
SRCS.blacklistd = blacklistd.c conf.c run.c state.c support.c internal.c
SRCS.blacklistctl = blacklistctl.c conf.c state.c support.c internal.c
DBG=-g

LDADD+=-lutil
DPADD+=${LIBUTIL}

.include <bsd.prog.mk>
