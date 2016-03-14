#	$NetBSD$

LIB=	LLVMPasses

.include <bsd.init.mk>

.PATH: ${LLVM_SRCDIR}/lib/Passes

SRCS+=	PassBuilder.cpp

.if defined(HOSTLIB)
.include <bsd.hostlib.mk>
.else
.include <bsd.lib.mk>
.endif
