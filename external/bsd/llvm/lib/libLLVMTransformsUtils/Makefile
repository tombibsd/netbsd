#	$NetBSD$

LIB=	LLVMTransformsUtils

.include <bsd.init.mk>

.PATH: ${LLVM_SRCDIR}/lib/Transforms/Utils

SRCS+=	ASanStackFrameLayout.cpp \
	AddDiscriminators.cpp \
	BasicBlockUtils.cpp \
	BreakCriticalEdges.cpp \
	BuildLibCalls.cpp \
	BypassSlowDivision.cpp \
	CloneFunction.cpp \
	CloneModule.cpp \
	CmpInstAnalysis.cpp \
	CodeExtractor.cpp \
	CtorUtils.cpp \
	DemoteRegToStack.cpp \
	FlattenCFG.cpp \
	GlobalStatus.cpp \
	InlineFunction.cpp \
	InstructionNamer.cpp \
	IntegerDivision.cpp \
	LCSSA.cpp \
	Local.cpp \
	LoopSimplify.cpp \
	LoopUnroll.cpp \
	LoopUnrollRuntime.cpp \
	LoopUtils.cpp \
	LoopVersioning.cpp \
	LowerInvoke.cpp \
	LowerSwitch.cpp \
	Mem2Reg.cpp \
	MetaRenamer.cpp \
	ModuleUtils.cpp \
	PromoteMemoryToRegister.cpp \
	SSAUpdater.cpp \
	SimplifyCFG.cpp \
	SimplifyIndVar.cpp \
	SimplifyInstructions.cpp \
	SimplifyLibCalls.cpp \
	SplitModule.cpp \
	SymbolRewriter.cpp \
	UnifyFunctionExitNodes.cpp \
	Utils.cpp \
	ValueMapper.cpp

.if defined(HOSTLIB)
.include <bsd.hostlib.mk>
.else
.include <bsd.lib.mk>
.endif
