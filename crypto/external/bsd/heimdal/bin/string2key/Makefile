# $NetBSD$

.include <bsd.own.mk>
.include <${.CURDIR}/../../Makefile.inc>

USE_FORT?= yes	# cryptographic software

.PATH: ${HEIMDIST}/kdc

PROG= string2key
SRCS= string2key.c
MAN=  string2key.8

CPPFLAGS+= 				\
	-I${HEIMDIST}/base		\
	-I${HEIMDIST}/lib/krb5		\
	-I${HEIMDIST}/lib/wind		\
	-I${HEIMDIST}/kdc		\
	-I${HEIMBASE}/include/krb5	\
	-I${HEIMBASE}/lib/libheimntlm	\
	-I${HEIMBASE}/lib/libwind	\

.include <${HEIMBASE}/Makefile.rules.inc>
.include <bsd.prog.mk>
