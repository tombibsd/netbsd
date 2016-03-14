#	$NetBSD$

LIB=	LLVMOrc

.include <bsd.init.mk>

.PATH: ${LLVM_SRCDIR}/lib/ExecutionEngine/Orc

SRCS+=	ExecutionUtils.cpp \
	IndirectionUtils.cpp \
	NullResolver.cpp \
	OrcArchitectureSupport.cpp \
	OrcCBindings.cpp \
	OrcCBindingsStack.cpp \
	OrcError.cpp \
	OrcMCJITReplacement.cpp \
	OrcRemoteTargetRPCAPI.cpp

.include <bsd.lib.mk>
