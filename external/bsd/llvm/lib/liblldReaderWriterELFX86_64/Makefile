#	$NetBSD$

LIB=	lldReaderWriterELFX86_64

.include <bsd.init.mk>

.PATH: ${LLD_SRCDIR}/lib/ReaderWriter/ELF/X86_64

CPPFLAGS+=	-I${LLD_SRCDIR}/lib/ReaderWriter/ELF

SRCS+=	X86_64LinkingContext.cpp \
	X86_64RelocationHandler.cpp \
	X86_64RelocationPass.cpp \
	X86_64TargetHandler.cpp

.if defined(HOSTLIB)
.include <bsd.hostlib.mk>
.else
.include <bsd.lib.mk>
.endif
