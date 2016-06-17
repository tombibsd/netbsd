#	$NetBSD$

.include "../Makefile.serverlib"

.if ${MKX11LOADABLE} != "no"
XMODULEDIR=	${X11USRLIBDIR}/modules/extensions
SRCS=		afbmodule.c
.endif

.include "../Makefile.servermod"
.include "Makefile.afb"
