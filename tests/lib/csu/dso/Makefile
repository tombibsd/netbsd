# $NetBSD$

NOMAN=		# defined

.include <bsd.own.mk>

.include "${.PARSEDIR}/../Makefile.check_stack"

LIB=		h_initfini3_dso
SRCS=		h_initfini3_dso.cxx ${SRCS_CHECK_STACK}
CPPFLAGS+=	${CPPFLAGS_CHECK_STACK}

LIBDIR=		${TESTSBASE}/lib/csu
SHLIBDIR=	${TESTSBASE}/lib/csu
SHLIB_MAJOR=	1

LIBISMODULE=	yes
LIBISCXX=	yes


.include <bsd.lib.mk>
