#	$NetBSD$

.ifndef ABSTOP
ABSTOP!= cd ${.PARSEDIR}; pwd

.if (${.PARSEDIR} == ${.CURDIR})
RELTOP:= .
.else
RELTOP:= ${.PARSEDIR:S,^${.CURDIR},,}
.endif

# you can set BSD{SRC,OBJ}DIR in Makefile.local.inc
.if exists(${ABSTOP}/Makefile.local.inc)
.include "${ABSTOP}/Makefile.local.inc"
.endif
.endif
