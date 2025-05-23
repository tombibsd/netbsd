#	$NetBSD$

.ifndef _BSD_MIPS_N32_MK_
.if ${MACHINE_ARCH} == "mips64eb"
LD+=		-m elf32btsmipn32
LDFLAGS+=	-Wl,-m,elf32btsmipn32
.else
LD+=		-m elf32ltsmipn32
LDFLAGS+=	-Wl,-m,elf32ltsmipn32
.endif

_BSD_MIPS_N32_MK_=1

KMODULEARCHDIR:=	mips-n32

CPPFLAGS+=	-mabi=n32
CPUFLAGS+=	-mabi=n32

.endif # _BSD_MIPS_N32_MK_
