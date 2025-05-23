#	$NetBSD$


.if empty(LD:M-m)
LD+=			-m elf32lriscv
.endif
.if !defined(MBLIBDIR)
MLIBDIR=		rv32
LIBC_MACHINE_ARCH=	riscv32
COMMON_MACHINE_ARCH=	riscv32
KVM_MACHINE_ARCH=	riscv32
LDELFSO_MACHINE_ARCH=	riscv32
PTHREAD_MACHINE_ARCH=	riscv32
BFD_MACHINE_ARCH=	riscv32
CSU_MACHINE_ARCH=	riscv32
CRYPTO_MACHINE_CPU=	riscv32
LDELFSO_MACHINE_CPU=	riscv32
GOMP_MACHINE_ARCH=	riscv32
XORG_MACHINE_ARCH=	riscv32

.include "${.PARSEDIR}/../../m32.mk"
.endif
