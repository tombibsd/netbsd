#	$NetBSD$

WARNS=	0

IDIST=	${NETBSDSRCDIR}/external/gpl2/diffutils/dist

CPPFLAGS+=	-DLOCALEDIR=\"/usr/share/locale\" -DHAVE_CONFIG_H \
		-I${IDIST}/../include -I${IDIST}/lib
