#	$NetBSD$

DRIVER=		xf86-video-nouveau
DRIVER_NAME=	nouveau_drv

SRCS=		nouveau_copy.c \
		nouveau_copy85b5.c \
		nouveau_copy90b5.c \
		nouveau_copya0b5.c \
		nouveau_exa.c \
		nouveau_xv.c \
		nouveau_dri2.c \
		nouveau_glamor.c \
		nouveau_present.c \
		nouveau_sync.c \
		nouveau_wfb.c \
		nv_accel_common.c \
		nv_driver.c \
		nv_shadow.c \
		nv04_exa.c \
		nv04_xv_ovl.c \
		nv04_xv_blit.c \
		nv10_exa.c \
		nv10_xv_ovl.c \
		nv30_exa.c \
		nv30_xv_tex.c \
		nv40_exa.c \
		nv40_xv_tex.c \
		nv50_accel.c \
		nv50_exa.c \
		nv50_xv.c \
		nvc0_accel.c \
		nvc0_exa.c \
		nvc0_xv.c \
		drmmode_display.c \
		vl_hwmc.c

LIBDPLIBS+=	drm_nouveau ${.CURDIR}/../../../lib/libdrm_nouveau

MAN=		nouveau.4

CPPFLAGS+=	-DHAVE_XAA_H
CPPFLAGS+=	-I${DESTDIR}${X11INCDIR}/libdrm

CPPFLAGS+=	-DNV_MAJOR_VERSION=${PACKAGE_MAJOR}
CPPFLAGS+=	-DNV_MINOR_VERSION=${PACKAGE_MINOR}
CPPFLAGS+=	-DNV_PATCHLEVEL=${PACKAGE_PATCH}
CPPFLAGS+=	-DNV_DRIVER_DATE=${DRIVER_DATE}

.include "../Makefile.xf86-driver"

# from pkgsrc/x11/xf86-video-nouveau/patches/patch-configure
DRIVER_DATE!=awk '/^Date:/{print "\"" $0 "\"";exit}' ${X11SRCDIR.${DRIVER}}/ChangeLog
