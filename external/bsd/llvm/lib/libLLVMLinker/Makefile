#	$NetBSD$

LIB=	LLVMLinker

.include <bsd.init.mk>

.PATH: ${LLVM_SRCDIR}/lib/Linker

SRCS+=	IRMover.cpp \
	LinkModules.cpp

.if defined(HOSTLIB)
.include <bsd.hostlib.mk>
.else
.include <bsd.lib.mk>
.endif
