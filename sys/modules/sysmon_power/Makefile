# $NetBSD$

.include "../Makefile.inc"

.PATH:	${S}/dev/sysmon

KMOD=	sysmon_power

SRCS=	sysmon_power.c

COPTS+=	-DCOMPAT_40

.include <bsd.kmodule.mk>
