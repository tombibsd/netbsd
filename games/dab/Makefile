#	$NetBSD$

DPADD+=${LIBCURSES} ${LIBTERMINFO} ${LIBM}
LDADD+=-lcurses -lterminfo -lm

PROG_CXX=dab
MAN=dab.6
SRCS=algor.cc board.cc main.cc human.cc box.cc player.cc gamescreen.cc \
    ttyscrn.cc random.cc

.include <bsd.prog.mk>
