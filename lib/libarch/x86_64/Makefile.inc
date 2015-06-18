#	$NetBSD$

.if (${MACHINE_ARCH} == "x86_64" && ${MLIBDIR:Unone} != "i386")
SRCS+=	x86_64_mtrr.c x86_64_iopl.c
.endif

MAN+=	x86_64_get_mtrr.2 x86_64_iopl.2

MLINKS+=x86_64_get_mtrr.2 x86_64_set_mtrr.2
