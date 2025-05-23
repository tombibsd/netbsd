#	$NetBSD$

SRCS+=		ppc_reloc.c
LDFLAGS+=	-Wl,-e,_rtld_start

CPPFLAGS+=	-fPIC

.if ${LDELFSO_MACHINE_ARCH} == "powerpc64"
SRCS+=		rtld_start64.S
CPPFLAGS+=	-DELFSIZE=64
.else
SRCS+=		rtld_start.S
CPPFLAGS+=	-DELFSIZE=32
LDFLAGS+=	-Wl,--script,${.CURDIR}/arch/powerpc/ld.so.script
.endif
