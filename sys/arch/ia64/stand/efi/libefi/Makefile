#	$NetBSD$

S?=	${.CURDIR}/../../../../../

.PATH: ${S}/arch/${MACHINE_ARCH}/${MACHINE_ARCH}

LIB= efi
NOPROFILE=# defined
INTERNALLIB=#	defined

EFI_INCLUDE_NET?= no

CPPFLAGS= -I${EFICPPFLAGS} ${EFIMISCCPPFLAGS}
CPPFLAGS+= -I${.CURDIR}/../include
CPPFLAGS+= -I${.CURDIR}/../include/${MACHINE_ARCH}

# Pick up the bootstrap header for some interface items
CPPFLAGS+= -I${.CURDIR}/../../common

SRCS=  copy.c delay.c efi.c efi_console.c devicename.c bootinfo.c
SRCS+= time.c efifs.c efi_console.c module.c exec.c #efinet.c

.if ${MACHINE_ARCH} == "ia64"
SRCS+=	efifpswa.c pal.S
.endif

.if (${EFI_INCLUDE_NET} == "yes")
SRCS+= efinet.c #etc.
.endif

.include <bsd.own.mk>
.undef DESTDIR
.include <bsd.lib.mk>

lib${LIB}.o:: ${OBJS:O}
	@echo building standard ${LIB} $OBJS} library
	@rm -f lib${LIB}.o
	@${LD} -r -o lib${LIB}.o `lorder ${OBJS} | tsort`
