#	$NetBSD$

NOMAN=	1

.include <bsd.own.mk>

PROG=	atobm

LDADD+=	-lXaw -lXmu -lXt -lSM -lICE -lXpm -lXext -lX11 -lm
DPADD+=	${LIBXAW} ${LIBXMU} ${LIBXT} ${LIBSM} ${LIBICE} \
	${LIBXPM} ${LIBXEXT} ${LIBX11} ${LIBM}

.PATH:	${X11SRCDIR.bitmap}

.include <bsd.x11.mk>
.include <bsd.prog.mk>
