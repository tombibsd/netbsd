#	$NetBSD$

.include <bsd.own.mk>

CFDIR=		commands/x11r6
CFFILES=	@List LoadMonitor Terminal
CFSED=		-e 's,ENVPREFIX,RSTART,g'

.include "../../Makefile.rstart"

.include <bsd.x11.mk>
.include <bsd.prog.mk>
