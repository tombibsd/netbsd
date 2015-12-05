#	$NetBSD$
#
#	@(#)Makefile	8.1 (Berkeley) 6/8/93

.include <bsd.own.mk>
SECTION=reference/ref1/roff
ARTICLE=memacros
SRCS=	intro.me
MACROS=	-me
.if ${MKREPRO:Uno} == "yes"
MACROS+= -rR=1
.endif

.include <bsd.doc.mk>
