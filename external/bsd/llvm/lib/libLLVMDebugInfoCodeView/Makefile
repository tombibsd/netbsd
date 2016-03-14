#	$NetBSD$

LIB=	LLVMDebugInfoCodeView

.include <bsd.init.mk>

.PATH: ${LLVM_SRCDIR}/lib/DebugInfo/CodeView

SRCS+=	FieldListRecordBuilder.cpp \
	Line.cpp \
	MemoryTypeTableBuilder.cpp \
	MethodListRecordBuilder.cpp \
	ListRecordBuilder.cpp \
	TypeRecordBuilder.cpp \
	TypeTableBuilder.cpp

.if defined(HOSTLIB)
.include <bsd.hostlib.mk>
.else
.include <bsd.lib.mk>
.endif
