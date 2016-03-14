#	$NetBSD$

LIB=	clangToolingCore

.include <bsd.init.mk>

.PATH: ${CLANG_SRCDIR}/lib/Tooling/Core

SRCS+=	Lookup.cpp \
	Replacement.cpp

.if defined(HOSTLIB)
.include <bsd.hostlib.mk>
.else
.include <bsd.lib.mk>
.endif
