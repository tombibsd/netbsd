#	$NetBSD$

LIBISPRIVATE=	yes

.include <bsd.own.mk>

LIB=	grep
SRCS= \
dfa.c \
hard-locale.c \
kwset.c \
m-common.c \
m-fgrep.c \
m-regex.c \
memchr.c \
regex.c \
strdup.c 

.include "${.CURDIR}/../../Makefile.inc"

.PATH:	${DIST}/gettext-tools/libgrep 
CPPFLAGS+= -I${DIST}/gettext-tools/libgrep

COPTS.regex.c = -Wno-stack-protector

WARNS?=		0	# pointer sign issues

CWARNFLAGS.clang+=	-Wno-unused-value

.include <bsd.lib.mk>
