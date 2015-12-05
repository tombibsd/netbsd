#	$NetBSD$
#
#	@(#)Makefile	8.1 (Berkeley) 6/8/93

.include <bsd.own.mk>
SECTION=reference/ref1/roff
ARTICLE=msdiffs
SRCS=	ms.diffs
MACROS=	-ms
.if ${MKREPRO:Uno} == "yes"
MACROS+= -rR=1
.endif

EXTRAHTMLFILES=msdiffs1.png msdiffs2.png

.include <bsd.doc.mk>
