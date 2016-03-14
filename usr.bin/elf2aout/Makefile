#	$NetBSD$
#	from: @(#)Makefile	5.4 (Berkeley) 5/11/90

.include <bsd.own.mk>	# for MACHINE_CPU
WARNS=6

# Build ELF to {ecoff, aout} tools on m68k/powerpc, for kernels with old amigappc bootblocks.
.if (${MACHINE_CPU} == "m68k" || \
     ${MACHINE_ARCH} == "powerpc" || \
     ${MACHINE_ARCH} == "powerpc64")
PROG=	elf2aout
.endif

MAN=	elf2aout.1

.include <bsd.prog.mk>
