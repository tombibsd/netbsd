#	$NetBSD$

PROG_CXX=	sancov
NOMAN=		yes

.include <bsd.init.mk>

.PATH: ${LLVM_SRCDIR}/tools/sancov

SRCS=	sancov.cc

LLVM_LIBS+= \
	AArch64Disassembler \
	AArch64MCTargetDesc \
	AArch64AsmPrinter \
	AArch64TargetInfo \
	AArch64Utils \
	ARMDisassembler \
	ARMMCTargetDesc \
	ARMAsmPrinter \
	ARMTargetInfo \
	MipsDisassembler \
	MipsMCTargetDesc \
	MipsAsmPrinter \
	MipsTargetInfo \
	PowerPCDisassembler \
	PowerPCMCTargetDesc \
	PowerPCAsmPrinter \
	PowerPCTargetInfo \
	SparcDisassembler \
	SparcMCTargetDesc \
	SparcAsmPrinter \
	SparcTargetInfo \
	X86Disassembler \
	X86CodeGen \
	X86MCTargetDesc \
	X86AsmPrinter \
	X86TargetInfo \
	X86Utils \
	MCDisassembler \
	DebugInfoSymbolize \
	DebugInfoDWARF \
	DebugInfoPDB \
	Object \
	BitReader \
	IR \
	MCParser \
	MC \
	Support

LDADD+=	-lz
DPADD+=	${LIBZ}

.include "${.PARSEDIR}/../../link.mk"

.include <bsd.prog.mk>
