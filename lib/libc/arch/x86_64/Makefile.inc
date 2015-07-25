#	$NetBSD$

.if ${RUMPRUN} != "yes"
SRCS+=	__sigaction14_sigtramp.c __sigtramp2.S
.endif
