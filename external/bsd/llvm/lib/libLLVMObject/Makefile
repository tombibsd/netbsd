#	$NetBSD$

LIB=	LLVMObject

.include <bsd.init.mk>

.PATH: ${LLVM_SRCDIR}/lib/Object

SRCS+=	Archive.cpp \
	ArchiveWriter.cpp \
	Binary.cpp \
	COFFObjectFile.cpp \
	COFFYAML.cpp \
	ELF.cpp \
	ELFObjectFile.cpp \
	ELFYAML.cpp \
	Error.cpp \
	FunctionIndexObjectFile.cpp \
	IRObjectFile.cpp \
	MachOObjectFile.cpp \
	MachOUniversal.cpp \
	Object.cpp \
	ObjectFile.cpp \
	RecordStreamer.cpp \
	SymbolSize.cpp \
	SymbolicFile.cpp

.if defined(HOSTLIB)
.include <bsd.hostlib.mk>
.else
.include <bsd.lib.mk>
.endif
