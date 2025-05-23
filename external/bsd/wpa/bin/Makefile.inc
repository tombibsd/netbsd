# $NetBSD$

BINDIR?=	/usr/sbin

USE_FORT?= yes	# network client/server

WPA_DISTDIR?=			${.CURDIR}/../../dist
WPA_SUPPLICANT_DISTDIR?=	${WPA_DISTDIR}/wpa_supplicant
HOSTAPD_DISTDIR?=		${WPA_DISTDIR}/hostapd

.PATH.c: \
	${WPA_DISTDIR}/src/ap \
	${WPA_DISTDIR}/src/common \
	${WPA_DISTDIR}/src/crypto \
	${WPA_DISTDIR}/src/eap_common \
	${WPA_DISTDIR}/src/eapol_supp \
	${WPA_DISTDIR}/src/l2_packet \
	${WPA_DISTDIR}/src/utils

CPPFLAGS+= \
	-I${WPA_DISTDIR}/src \
	-I${WPA_DISTDIR}/src/ap \
	-I${WPA_DISTDIR}/src/common \
	-I${WPA_DISTDIR}/src/crypto \
	-I${WPA_DISTDIR}/src/l2_packet \
	-I${WPA_DISTDIR}/src/utils

CPPFLAGS+=-DCONFIG_CTRL_IFACE -DCONFIG_CTRL_IFACE_UNIX -DCONFIG_ELOOP_KQUEUE

.include <bsd.own.mk>

.if ${MKCRYPTO} == "no"
NO_OPENSSL=	# defined
.endif
