#	$NetBSD$

LIB=	LLVMSelectionDAG

.include <bsd.init.mk>

.PATH: ${LLVM_SRCDIR}/lib/CodeGen/SelectionDAG

SRCS+=	DAGCombiner.cpp \
	FastISel.cpp \
	FunctionLoweringInfo.cpp \
	InstrEmitter.cpp \
	LegalizeDAG.cpp \
	LegalizeFloatTypes.cpp \
	LegalizeIntegerTypes.cpp \
	LegalizeTypes.cpp \
	LegalizeTypesGeneric.cpp \
	LegalizeVectorOps.cpp \
	LegalizeVectorTypes.cpp \
	ResourcePriorityQueue.cpp \
	ScheduleDAGFast.cpp \
	ScheduleDAGRRList.cpp \
	ScheduleDAGSDNodes.cpp \
	ScheduleDAGVLIW.cpp \
	SelectionDAG.cpp \
	SelectionDAGBuilder.cpp \
	SelectionDAGDumper.cpp \
	SelectionDAGISel.cpp \
	SelectionDAGPrinter.cpp \
	StatepointLowering.cpp \
	TargetLowering.cpp \
	TargetSelectionDAGInfo.cpp

.if defined(HOSTLIB)
.include <bsd.hostlib.mk>
.else
.include <bsd.lib.mk>
.endif
