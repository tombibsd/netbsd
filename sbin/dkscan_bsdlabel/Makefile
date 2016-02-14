#	$NetBSD$

PROG=	dkscan_bsdlabel
MAN=	dkscan_bsdlabel.8

CPPFLAGS+=	-I${NETBSDSRCDIR}/sys/dev/dkwedge -D_KERNTYPES
SRCS=	dkscan_bsdlabel.c dkscan_util.c

LDADD+=	-lutil

.include <bsd.prog.mk>
