#	$NetBSD$

LIB=	lldbPluginDisassemblerLLVM

.include <bsd.init.mk>

.PATH: ${LLDB_SRCDIR}/source/Plugins/Disassembler/llvm

SRCS+=	DisassemblerLLVMC.cpp

.if defined(HOSTLIB)
.include <bsd.hostlib.mk>
.else
.include <bsd.lib.mk>
.endif
