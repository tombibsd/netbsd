#	$NetBSD$

NOMAN=		1

.include <bsd.own.mk>

HOSTPROG=	mkfontscale

FREETYPE=       ${X11SRCDIR.freetype}

.PATH:	${X11SRCDIR.${HOSTPROG}}
SRCS.mkfontscale=	mkfontscale.c list.c hash.c ident.c

.PATH:	${FREETYPE}/src/base
SRCS.freetype+=	ftapi.c ftbase.c ftbbox.c ftbdf.c ftdebug.c ftfntfmt.c \
		ftglyph.c ftinit.c ftmm.c ftpfr.c ftstroke.c ftsynth.c \
		ftsystem.c fttype1.c ftwinfnt.c ftbitmap.c

.PATH: ${FREETYPE}/src/autofit
SRCS.freetype+=	autofit.c

.PATH:	${FREETYPE}/src/bdf
SRCS.freetype+=	bdf.c

.PATH:	${FREETYPE}/src/cff
SRCS.freetype+=	cff.c

.PATH:	${FREETYPE}/src/cid
SRCS.freetype+=	type1cid.c

.PATH:	${FREETYPE}/src/gzip
SRCS.freetype+=	ftgzip.c

#.PATH:	${FREETYPE}/src/bzip2
#SRCS.freetype+=	ftbzip2.c

.PATH:	${FREETYPE}/src/lzw
SRCS.freetype+=	ftlzw.c

.PATH:	${FREETYPE}/src/pcf
SRCS.freetype+=	pcf.c

.PATH:	${FREETYPE}/src/pfr
SRCS.freetype+=	pfr.c

.PATH:	${FREETYPE}/src/psaux
SRCS.freetype+=	psaux.c

.PATH:	${FREETYPE}/src/pshinter
SRCS.freetype+=	pshinter.c

.PATH:	${FREETYPE}/src/psnames
SRCS.freetype+=	psnames.c

.PATH:	${FREETYPE}/src/raster
SRCS.freetype+=	raster.c

.PATH:	${FREETYPE}/src/sfnt
SRCS.freetype+=	sfnt.c

.PATH:	${FREETYPE}/src/smooth
SRCS.freetype+=	smooth.c

.PATH:	${FREETYPE}/src/truetype
SRCS.freetype+=	truetype.c

.PATH:	${FREETYPE}/src/type1
SRCS.freetype+=	type1.c

.PATH:	${FREETYPE}/src/type42
SRCS.freetype+=	type42.c

.PATH:	${FREETYPE}/src/winfonts
SRCS.freetype+=	winfnt.c

.PATH:	${X11SRCDIR.fontenc}/src
SRCS.fontenc=	fontenc.c encparse.c

SRCS+=	${SRCS.mkfontscale} ${SRCS.freetype} ${SRCS.fontenc}

LDADD=	-lz
#LDADD+=	-lbz2

HOST_CPPFLAGS=	-DFONTENC_NO_LIBFONT -DXFREE86_FT2 -DFONTENC_NO_LIBFONT \
		-DFT2_BUILD_LIBRARY -DDARWIN_NO_CARBON \
		-DFONT_ENCODINGS_DIRECTORY=\"${X11FONTDIR}/encodings/encodings.dir\" \
		-DPACKAGE_STRING=\"NetBSD\ tools\ version\"
HOST_CPPFLAGS+=	-DFT_CONFIG_OPTION_DISABLE_BZIP2

# If you make mkfontscale multithreaded, remove this and fix the
# fontconfig atomic ops.
HOST_CPPFLAGS+=	-DFC_NO_MT=1

HOST_CPPFLAGS+=	-I${FREETYPE}/include -I${DESTDIR}${X11INCDIR} \
		-I${DESTDIR}${X11INCDIR}/freetype2

.include <bsd.x11.mk>
.include <bsd.hostprog.mk>
