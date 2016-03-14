#	$NetBSD$

PROG_CXX=	llvm-link
NOMAN=		yes

.include <bsd.init.mk>

.PATH: ${LLVM_SRCDIR}/tools/llvm-link

SRCS=	llvm-link.cpp

LLVM_LIBS+= \
	Object \
	IRReader \
	AsmParser \
	BitReader \
	BitWriter \
	Linker \
	TransformsUtils \
	Analysis \
	Target \
	IR \
	Support

.include "${.PARSEDIR}/../../link.mk"

.include <bsd.prog.mk>
