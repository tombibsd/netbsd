#	$NetBSD$

.include <bsd.init.mk>

# We don't install mpc, just provide it for GCC.
LIBISPRIVATE?= yes

.if ${LIBISPRIVATE} != "yes"
INCS=         mpc.h mpc-log.h
INCSDIR=      /usr/include
.endif

LIB=	mpc

LIBDPLIBS+= gmp ${.CURDIR}/../../../../lgpl3/gmp/lib/libgmp
LIBDPLIBS+= mpfr ${.CURDIR}/../../../../lgpl3/mpfr/lib/libmpfr

SRCS=	\
	abs.c \
	acos.c \
	acosh.c \
	add.c \
	add_fr.c \
	add_si.c \
	add_ui.c \
	arg.c \
	asin.c \
	asinh.c \
	atan.c \
	atanh.c \
	clear.c \
	cmp.c \
	cmp_si_si.c \
	conj.c \
	cos.c \
	cosh.c \
	div_2si.c \
	div_2ui.c \
	div.c \
	div_fr.c \
	div_ui.c \
	exp.c \
	fma.c \
	fr_div.c \
	fr_sub.c \
	get_prec2.c \
	get_prec.c \
	get_version.c \
	get_x.c \
	imag.c \
	init2.c \
	init3.c \
	inp_str.c \
	log.c \
	log10.c \
	mem.c \
	mul_2si.c \
	mul_2ui.c \
	mul.c \
	mul_fr.c \
	mul_i.c \
	mul_si.c \
	mul_ui.c \
	neg.c \
	norm.c \
	out_str.c \
	pow.c \
	pow_fr.c \
	pow_ld.c \
	pow_d.c \
	pow_si.c \
	pow_ui.c \
	pow_z.c \
	proj.c \
	real.c \
	urandom.c \
	set.c \
	set_prec.c \
	set_str.c \
	set_x.c \
	set_x_x.c \
	sin.c \
	sin_cos.c \
	sinh.c \
	sqr.c \
	sqrt.c \
	strtoc.c \
	sub.c \
	sub_fr.c \
	sub_ui.c \
	swap.c \
	tan.c \
	tanh.c \
	uceil_log2.c \
	ui_div.c \
	ui_ui_sub.c

CPPFLAGS+=	-I${.CURDIR}

# Find <mpfr.h> and <gmp.h>
GMP_MACHINE_ARCH=${MACHINE_ARCH:C/armv[4-7]/arm/}
CPPFLAGS+=	-I${NETBSDSRCDIR}/external/lgpl3/gmp/lib/libgmp/arch/${GMP_MACHINE_ARCH}
CPPFLAGS+=	-I${NETBSDSRCDIR}/external/lgpl3/mpfr/dist/src

.include <bsd.lib.mk>

.PATH: ${.CURDIR}/../../dist/src
