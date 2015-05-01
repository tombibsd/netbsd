# $NetBSD$

.include "../Makefile.inc"

.PATH:	${S}/dev/sysmon

KMOD=	sysmon_envsys

SRCS=	sysmon_envsys.c
SRCS+=	sysmon_envsys_events.c
SRCS+=	sysmon_envsys_tables.c
SRCS+=	sysmon_envsys_util.c

.include <bsd.kmodule.mk>
