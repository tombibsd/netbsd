#	$NetBSD$

LIB=	lldbPluginInstructionARM

.include <bsd.init.mk>

.PATH: ${LLDB_SRCDIR}/source/Plugins/Instruction/ARM

SRCS+=	EmulateInstructionARM.cpp \
	EmulationStateARM.cpp

.if defined(HOSTLIB)
.include <bsd.hostlib.mk>
.else
.include <bsd.lib.mk>
.endif
