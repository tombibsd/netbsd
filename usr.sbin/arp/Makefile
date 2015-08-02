#	$NetBSD$
#	from: @(#)Makefile	8.2 (Berkeley) 4/18/94

.include <bsd.own.mk>

RUMPPRG=	arp
MAN=		arp.4 arp.8

.PATH:		${.CURDIR}/../../lib/libc/net
RUMPSRCS=	getifaddrs.c getnameinfo.c
.if (${MKRUMP} != "no")
CPPFLAGS+= 	-DRUMP_ACTION
.endif

.include <bsd.prog.mk>
