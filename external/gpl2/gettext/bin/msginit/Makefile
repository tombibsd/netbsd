#	$NetBSD$

PROG=	msginit
SRCS+=	msginit.c \
localealias.c \
localename.c

.include "${.CURDIR}/../Makefile.inc.prog"

CPPFLAGS+=-I${DIST}/gettext-runtime/intl
CPPFLAGS+=-DLIBDIR=\"/usr/share\" \
    -DPROJECTSDIR=\"/usr/share\" \
    -DLOCALE_ALIAS_PATH=\"/usr/share/locale\"

COPTS.msginit.c = -Wno-stack-protector
COPTS.localealias.c = -Wno-stack-protector

.PATH: ${DIST}/gettext-runtime/intl
