#	$NetBSD$
#
#	@(#) Copyright (c) 1995 Simon J. Gerraty
#
#	SRCS extracted from src/crypto/dist/openssl/crypto/rc4/Makefile
#

.PATH:	${OPENSSLSRC}/crypto/rc4

.if !defined(RC4_SRCS)
RC4_SRCS = rc4_skey.c rc4_enc.c
.endif
SRCS += ${RC4_SRCS} rc4_utl.c

.for cryptosrc in ${RC4_SRCS}
CPPFLAGS.${cryptosrc} = -I${OPENSSLSRC}/crypto/rc4
.endfor
