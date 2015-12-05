#	$NetBSD$
#	@(#)Makefile	8.1 (Berkeley) 6/6/93

WARNS=	6
PROG=	who
SRCS=	who.c utmpentry.c
CPPFLAGS+=	-DSUPPORT_UTMPX -DSUPPORT_UTMP

.include <bsd.prog.mk>
