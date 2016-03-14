#	$NetBSD$
#
#

.include <bsd.own.mk>

.if (defined(MKDTRACE) && ${MKDTRACE} != "no")
SUBDIR+=	libdtrace
SUBDIR+=	drti
.endif
.if ${MKCTF:Uno} != "no"
SUBDIR+=	libctf
.endif

.if !defined(MKZFS) || ${MKZFS} != "no"
SUBDIR+=	libavl
SUBDIR+=	libnvpair
SUBDIR+=	libumem
SUBDIR+=	libuutil
SUBDIR+=	.WAIT
SUBDIR+=	libzfs
SUBDIR+=	libzpool
.endif

.include <bsd.subdir.mk>
