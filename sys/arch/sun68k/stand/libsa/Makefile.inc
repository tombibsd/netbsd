#	$NetBSD$

S!=	cd ${SA_EXTRADIR}/../../../..; pwd

.PATH.c: ${SA_EXTRADIR}

# prefered local sources: gets.c panic.c

SRCS+=	clock.c dev_disk.c devopen.c \
	idprom.c netif_sun.c \
	promboot.c promcons.c promdev.c putstr.c \
	sun2.c sun3.c sun3x.c xxboot.c

.PATH.S: ${S}/arch/sun68k/sun68k
SRCS+=	ctrlsp.S

# XXX libkern builds these only on m68000
.PATH.S: ${S}/../common/lib/libc/arch/m68k/gen
SRCS+=	mulsi3.S divsi3.S udivsi3.S modsi3.S umodsi3.S

vers.c: ${SA_EXTRADIR}/version
	${HOST_SH} ${S}/conf/newvers_stand.sh ${${MKREPRO} == "yes" :?:-D} \
	    ${SA_EXTRADIR}/version "${MACHINE}"

SRCS+=	vers.c
CLEANFILES+=	vers.c
