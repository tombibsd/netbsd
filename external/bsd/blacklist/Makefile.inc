#	$NetBSD$

WARNS=6
.if !defined(LIB)
LDADD+=	-lblacklist
DPADD+= ${LIBBLACKLIST}
.endif
CPPFLAGS+= -I${.CURDIR}/../include
CPPFLAGS+=-DHAVE_STRUCT_SOCKADDR_SA_LEN -DHAVE_UTIL_H -DHAVE_DB_H

