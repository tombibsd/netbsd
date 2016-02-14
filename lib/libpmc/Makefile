#	$NetBSD$

.include <bsd.own.mk>

MAN=	pmc.3

.if exists(${.CURDIR}/pmc_${MACHINE_CPU}.c)
LIB=	pmc
SRCS=	pmc.c
SRCS+=	pmc_${MACHINE_CPU}.c
CPPFLAGS+=	-D_KERNTYPES

INCS=	pmc.h
INCSDIR=/usr/include
.include <bsd.lib.mk>
.else
.include <bsd.man.mk>
.endif
