#	$NetBSD$

LIB=	LLVMAArch64TargetInfo

.include <bsd.init.mk>

AARCH64_OBJDIR!=	cd ${.CURDIR}/../libLLVMAArch64CodeGen && ${PRINTOBJDIR}
CPPFLAGS+=	-I${AARCH64_OBJDIR} -I${LLVM_SRCDIR}/lib/Target/AArch64

.PATH: ${LLVM_SRCDIR}/lib/Target/AArch64/TargetInfo

SRCS+=	AArch64TargetInfo.cpp

.if defined(HOSTLIB)
.include <bsd.hostlib.mk>
.else
.include <bsd.lib.mk>
.endif
