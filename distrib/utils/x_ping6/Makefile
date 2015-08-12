# $NetBSD$
# Build ping6 without IPSEC

SRCDIR=		${.CURDIR}/../../../sbin/ping6

PROG=		ping6
NOMAN=		# defined

CPPFLAGS+=	-I${SRCDIR} -DINET6 -DCRUNCHOPS

# KAME scope id hack
CPPFLAGS+=	-DKAME_SCOPEID

LDADD+=		-lm
DPADD+=		${LIBM}

.include <bsd.prog.mk>

.PATH:		${SRCDIR}
