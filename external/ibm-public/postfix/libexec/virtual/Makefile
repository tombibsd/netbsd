#	$NetBSD$

NOMAN=	# defined

.include <bsd.own.mk>

PROG=	virtual

DIST=	${NETBSDSRCDIR}/external/ibm-public/postfix/dist/src/${PROG}
.PATH:	${DIST}

SRCS=	virtual.c mailbox.c recipient.c deliver_attr.c maildir.c unknown.c

DPADD+= ${LIBPMASTER} ${LIBPGLOBAL} ${LIBPUTIL}
LDADD+= ${LIBPMASTER} ${LIBPGLOBAL} ${LIBPUTIL}

.include <bsd.prog.mk>
