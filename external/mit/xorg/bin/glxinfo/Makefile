#	$NetBSD$

NOMAN=	# defined

.include <bsd.own.mk>

.if ${MKPIC} == "no" || ${LDSTATIC:U} != ""
PROG_CXX=	glxinfo
.else
PROG=		glxinfo
.endif

SRCS+=	glinfo_common.c glxinfo.c

CPPFLAGS+=${X11FLAGS.THREADS} -DDO_GLU

LDADD+=	-lGL -lXext -lX11 -lpthread -lm
DPADD+=	${LIBGL} ${LIBXEXT} ${LIBX11} ${LIBPTHREAD} ${LIBM}

.PATH:	${X11SRCDIR.mesa-demos}/src/xdemos

.include <bsd.x11.mk>
.include <bsd.prog.mk>
