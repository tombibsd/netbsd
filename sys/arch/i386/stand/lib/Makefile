#	$NetBSD$

S?=	${.CURDIR}/../../../..

LIB= i386
NOPIC=# defined
NOPROFILE=# defined

I386_INCLUDE_DISK?= yes
I386_INCLUDE_DOS?= no
I386_INCLUDE_BUS?= no
I386_INCLUDE_PS2?= yes

AFLAGS.realprot.S= ${${ACTIVE_CC} == "clang":?-no-integrated-as:}

CPPFLAGS= -I$S/lib/libsa ${I386CPPFLAGS} ${I386MISCCPPFLAGS}
#CPPFLAGS+= -DDISK_DEBUG
#CPPFLAGS+= -DNO_DISKLABEL
#CPPFLAGS+= -DNO_GPT
#CPPFLAGS+= -DSAVE_MEMORY

SRCS= pcio.c conio.S comio.S comio_direct.c biosvideomode.S
SRCS+= getsecs.c biosgetrtc.S biosdelay.S biosreboot.S gatea20.c
SRCS+= biosmem.S getextmemx.c biosmemx.S printmemlist.c
SRCS+= pread.c menuutils.c parseutils.c
SRCS+= bootinfo.c bootinfo_biosgeom.c bootinfo_memmap.c
SRCS+= startprog.S multiboot.S
SRCS+= biosgetsystime.S cpufunc.S bootmenu.c
SRCS+= realprot.S message.S message32.S dump_eax.S pvcopy.S putstr.S putstr32.S
SRCS+= rasops.c vbe.c biosvbe.S
.if (${I386_INCLUDE_DISK} == "yes")
SRCS+= biosdisk.c biosdisk_ll.c bios_disk.S
.endif
.if (${I386_INCLUDE_DOS} == "yes")
SRCS+= dosfile.c dos_file.S
.endif
.if (${I386_INCLUDE_DISK} == "yes") || (${I386_INCLUDE_DOS} == "yes")
SRCS+= diskbuf.c
.endif
.if (${I386_INCLUDE_BUS} == "yes")
SRCS+= biospci.c bios_pci.S isapnp.c isadma.c
.endif
.if (${I386_INCLUDE_PS2} == "yes")
SRCS+= biosmca.S biosmemps2.S
.endif

.include <bsd.own.mk>
.undef DESTDIR
.include <bsd.lib.mk>

lib${LIB}.o:: ${OBJS:O}
	@echo building standard ${LIB} library
	@rm -f lib${LIB}.o
	@${LD} -r -o lib${LIB}.o `lorder ${OBJS} | tsort`

# XXX
.if ${ACTIVE_CC} == "gcc" && ${HAVE_GCC:U} == 48
COPTS.biosdisk_ll.c+=	-O0
.endif
