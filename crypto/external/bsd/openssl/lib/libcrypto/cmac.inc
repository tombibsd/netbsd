#	$NetBSD$
#
#	SRCS extracted from src/crypto/dist/openssl/crypto/cmac/Makefile
#

.PATH:	${OPENSSLSRC}/crypto/cmac


CMAC_SRCS = cmac.c cm_ameth.c cm_pmeth.c
SRCS += ${CMAC_SRCS}

.for cryptosrc in ${CMAC_SRCS}
CPPFLAGS.${cryptosrc} = -I${OPENSSLSRC}/crypto/cmac
.endfor
