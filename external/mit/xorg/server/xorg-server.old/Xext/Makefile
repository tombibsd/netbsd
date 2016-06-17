#	$NetBSD$

LIBISPRIVATE=	yes
LIB=		Xext

.include "../Makefile.serverlib"

.include "Makefile.Xextbuiltin"
.include "Makefile.Xextmodule"

SRCS=	${SRCS.Xextbuiltin} ${SRCS.Xextmodule}

SUBDIR=	Xextbuiltin Xextmodule

.include <bsd.subdir.mk>
.include "Makefile.Xext"
