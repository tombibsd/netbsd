# $NetBSD$

HELPER_NAME=	rc5test
HELPER_DIR=	rc5

.include <bsd.init.mk>

.if ${MKCRYPTO_RC5} != "no"
LDADD= -lcrypto_rc5 -lcrypto
.else
CPPFLAGS+= -DOPENSSL_NO_RC5
.endif

LDADD+=		-Wl,--no-fatal-warnings
