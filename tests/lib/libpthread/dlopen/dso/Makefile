# $NetBSD$

NOMAN=		# defined
NOLINT=		# defined

.include <bsd.own.mk>

LIB=			h_pthread_dlopen
SRCS=                   h_pthread_dlopen.c

LDADD+=-latf-c -lpthread

LIBDIR=         ${TESTSBASE}/lib/libpthread/dlopen
SHLIBDIR=       ${TESTSBASE}/lib/libpthread/dlopen
SHLIB_MAJOR=    1

LIBISCXX=       yes
LIBISMODULE=    yes

.include <bsd.lib.mk>
