#	$NetBSD$

LIBISPRIVATE=	yes
LIB=		Xextmodule

COPTS+=	${PICFLAGS}

.include "../../Makefile.serverlib"
.include "../Makefile.Xextmodule"

SRCS=	${SRCS.Xextmodule} shape.c ${SRCS.Xext}

.include "../Makefile.Xext"
