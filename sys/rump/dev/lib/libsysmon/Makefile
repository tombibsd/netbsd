#	$NetBSD$
#

.PATH:	${.CURDIR}/../../../../dev/sysmon

LIB=	rumpdev_sysmon
COMMENT=System monitoring and power management

IOCONF=	SYSMON.ioconf

SRCS=	sysmon_taskq.c sysmon_power.c sysmon_envsys.c sysmon_envsys_events.c \
	sysmon_envsys_tables.c sysmon_envsys_util.c sysmon_wdog.c sysmon.c \
	swsensor.c swwdog.c

SRCS+=	sysmon_component.c

.include <bsd.lib.mk>
.include <bsd.klinks.mk>
