#	$NetBSD$
#	@(#)Makefile	8.1 (Berkeley) 6/5/93

#	dump.h			header file
#	itime.c			reads /etc/dumpdates
#	main.c			driver
#	optr.c			operator interface
#	dumprmt.c		handles remote tape via rmt(8)
#	rcache.c		read cache
#	tape.c			handles the mag tape and opening/closing
#	traverse.c		traverses the file system
#	unctime.c		undo ctime
#	ffs_inode.c		FFS-specific filestore routines
#	ffs_bswap.c		FFS byte-swapping
#
#	DEBUG			use local directory to find ddate and dumpdates
#	TDEBUG			trace out the process forking
#	FDEBUG			trace dumpdates parsing
#	WRITEDEBUG		trace slave writes
#	STATS			read cache statistics
#	DIAGNOSTICS		read cache diagnostic checks

WARNS?=	3	# XXX: sign-compare issues

.include <bsd.own.mk>

PROG=	dump
LINKS=	${BINDIR}/dump ${BINDIR}/rdump
CPPFLAGS+=-DRDUMP -I${.CURDIR}
# CPPFLAGS+= -DDEBUG -DTDEBUG -DFDEBUG -DWRITEDEBUG -DSTATS -DDIAGNOSTICS
SRCS=	itime.c main.c optr.c dumprmt.c rcache.c snapshot.c tape.c \
	traverse.c unctime.c ffs_inode.c ffs_bswap.c
MAN=	dump.8
MLINKS+=dump.8 rdump.8
DPADD+= ${LIBUTIL}
LDADD+= -lutil

.PATH:  ${NETBSDSRCDIR}/sys/ufs/ffs

COPTS.ffs_inode.c+=	-Wno-pointer-sign

.include <bsd.prog.mk>
