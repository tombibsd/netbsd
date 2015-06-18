#	$NetBSD$

.include <bsd.own.mk>

SUBDIR=	libgrep libnlsut libnlspr info .WAIT \
	msgattrib msgcat msgcmp msgcomm msgconv msgen msgfilter \
	msgfmt msginit msgmerge msgunfmt msguniq xgettext

.if ${MKNLS} != "no"
SUBDIR+=locale
.endif

.include <bsd.subdir.mk>
