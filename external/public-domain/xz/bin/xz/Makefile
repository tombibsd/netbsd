# $NetBSD$

PROG=	xz
BINDIR=	/usr/bin
SRCS=	args.c coder.c file_io.c hardware.c list.c main.c \
	message.c options.c signals.c suffix.c util.c \
	tuklib_exit.c tuklib_cpucores.c tuklib_open_stdxxx.c \
	tuklib_mbstr_fw.c tuklib_mbstr_width.c \
	tuklib_physmem.c tuklib_progname.c mytime.c

USETBL=	yes

.include <bsd.init.mk>

CPPFLAGS+=	-DPACKAGE=\"xz\"
CPPFLAGS+=	-DENABLE_NLS -DLOCALEDIR=\"/usr/share/locale\"

DPADD+=	${LIBLZMA} ${LIBINTL}
LDADD+=	-llzma -lintl

LINKS+=	${BINDIR}/xz ${BINDIR}/lzcat
LINKS+=	${BINDIR}/xz ${BINDIR}/lzma
LINKS+=	${BINDIR}/xz ${BINDIR}/unlzma
LINKS+=	${BINDIR}/xz ${BINDIR}/unxz
LINKS+=	${BINDIR}/xz ${BINDIR}/xzcat

MLINKS+=	xz.1 lzcat.1
MLINKS+=	xz.1 lzma.1
MLINKS+=	xz.1 unlzma.1
MLINKS+=	xz.1 unxz.1
MLINKS+=	xz.1 xzcat.1

.PATH:	${XZSRCDIR}/src/common ${XZSRCDIR}/src/xz

.if ${MKNLS} != "no"
.for lang in cs de it
FILES+=					${XZSRCDIR}/po/${lang}.gmo
FILESDIR_${XZSRCDIR}/po/${lang}.gmo=	/usr/share/locale/${lang}/LC_MESSAGES
FILESNAME_${XZSRCDIR}/po/${lang}.gmo=	xz.mo
.endfor
.endif

LDADD+=	-lpthread
DPADD+=	${LIBPTHREAD}

.include <bsd.prog.mk>
