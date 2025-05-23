#	$NetBSD$
# Copyright (c) 1992,1991 Carnegie Mellon University
# All Rights Reserved.
# 
# Permission to use, copy, modify and distribute this software and its
# documentation is hereby granted, provided that both the copyright
# notice and this permission notice appear in all copies of the
# software, derivative works or modified versions, and any portions
# thereof, and that both notices appear in supporting documentation.
#
# CARNEGIE MELLON ALLOWS FREE USE OF THIS SOFTWARE IN ITS "AS IS"
# CONDITION.  CARNEGIE MELLON DISCLAIMS ANY LIABILITY OF ANY KIND FOR
# ANY DAMAGES WHATSOEVER RESULTING FROM THE USE OF THIS SOFTWARE.
#
# Carnegie Mellon requests users of this software to return to
#
#  Software Distribution Coordinator   or   Software.Distribution@CS.CMU.EDU
#  School of Computer Science
#  Carnegie Mellon University
#  Pittsburgh PA 15213-3890
#
# any improvements or extensions that they make and grant Carnegie the rights
# to redistribute these changes.
######################################################################
# Makefile to build sup (the client side), supfilesrv (the repository
# side, and supscan (used by the repository. If you only want to sup
# files from CMU, just build sup.
# The header files: c.h and libc.h are only
# necessary if you are compiling on a non-Mach system. Likewise the
# files in libextra.a are normally found in libcs.a on a Mach system.
# DOPRINT_VA is used by vprintf.c and should be defined if your version
# of libc/doprnt.c defines the routine _doprnt_va. If it defines _doprnt
# instead, leave DORPINT_VA undefined.
######################################################################
#
# If you need to build a sup for export outside of North America use
# 	"make EXPORTABLE_SYSTEM=true"
# this will remove (not use) any vestiges of crypt code that is present
# on the system.
#
# If you have crypt/crypt.c and /usr/lib/libcrypt.a, you will be building
# a system that uses the SUP crypt mechanism by default.
#
# If building on non 4.4BSD systems, you'll need the vis(3) and the daemon(3)
# functions from libc and libutil

#SITE			= SUNOS
#SITE			= SOLARIS
#SITE			= NETBSD
#SITE			= FREEBSD
#SITE			= CMUCS
SITE1			!= uname -s | tr '[a-z]' '[A-Z]' \
			    | sed -e s/_.*//g
SITE2			= $(shell uname -s | tr '[a-z]' '[A-Z]' \
			    | sed -e s/_.*//g)
SITE			= ${SITE1}${SITE2}
TARDIR			= sup-0.3

LINUX_DEFINES		= -UMACH -DVAR_TMP -DHAS_DAEMON -DHAS_POSIX_DIR \
			  -DNEED_SETPROCTITLE
NETBSD_DEFINES		= -UMACH -DVAR_TMP -DHAS_DAEMON -DHAS_POSIX_DIR \
			    -DHAS_FPARSELN -DHAS_FGETLN -DHAS_VIS
FREEBSD_DEFINES		= -UMACH -DVAR_TMP -DHAS_DAEMON -DHAS_POSIX_DIR \
			    -DHAS_FPARSELN -DHAS_FGETLN -DHAS_VIS
SOLARIS_DEFINES		= -UMACH -DVAR_TMP -DHAS_POSIX_DIR -DNEED_VSNPRINTF
AFS_DEFINES		= -DAFS -I/usr/afsws/include
OSF_DEFINES		= -UMACH -DOSF -D_BSD -noshrlib -g -DNEED_VSNPRINTF \
			    -DVAR_TMP
SUNOS_DEFINES		= -UMACH -D_BSD -DNEED_VSNPRINTF -DVAR_TMP \
			    -DHAS_POSIX_DIR -DHAS_DAEMON -DNEED_DAEMON
CYGWIN_DEFINES	= -UMACH -DVAR_TMP -DHAS_POSIX_DIR
CMUCS_DEFINES		= -DMACH -DDOPRINT_VA -DNEED_VPRINTF
NON_MACH_DEFINES	= -UMACH


#INSTALLATION PARAMETERS
PREFIX		       ?= /usr/pkg/
NETBSD_BINDIR		= ${PREFIX}/sbin
NETBSD_MAN1		= ${PREFIX}/man/man1
NETBSD_MAN8		= ${PREFIX}/man/man8

CFLAGS			= ${DEFS} -O -I. -g

SUPCL			= supcmain.o supcvers.o supcparse.o supcname.o \
			  supcmisc.o supcmeat.o
SUPS			= scm.o scmio.o stree.o log.o supmsg.o netcrypt.o
EXTRA			= atoo.o expand.o ffilecopy.o filecopy.o \
			  nxtarg.o path.o quit.o read_line.o run.o estrdup.o \
			  skipto.o vprintf.o setproctitle.o


PROGRAMS		= sup supscan supfilesrv
MAN1			= sup.1
MAN8			= supservers.8

AFS_LIBPATH		= /usr/afs/lib
AFS_LIBS		= -L${AFS_LIBPATH}/afs -lkauth -lprot -L${AFS_LIBPATH} -lubik -lauth -lrxkad -lsys -ldes -lrx -llwp -lcmd -lcom_err -lc ${AFS_LIBPATH}/afs/util.a

NETBSD_LIBS		= -lcrypt
FREEBSD_LIBS		= -lcrypt
DRAGONFLY_LIBS		= -lcrypt
LINUX_LIBS		= -lcrypt
CYGWIN_LIBS		= -lcrypt
CMUCS_LIBS		= -lsys
OSF_LIBS		= -lbsd
EXTRALIBS		= libextra.a
sup_OFILES		= ${SUPCL} ${SUPS}
supfilesrv_OFILES	= supfilesrv.o scan.o ${SUPS}
supfilesrv_LIBS		= libextra.a 
supscan_OFILES		= supscan.o stree.o scan.o
SOLARIS_LIBS		= -lsocket -lnsl
SUNOS_LIBS		= 

DEFS			= -UCMUCS -UCMU  ${${SITE}_DEFINES}
#DEFS			+= -DDEFDIR=\"${PREFIX}/libdata/sup\"
#DEFS			+= -DEE_XXX -DDEFSCAN=\"/var/\" -DDEFUSER=\"nobody\"
LIBS			= ${EXTRALIBS}

all: ${PROGRAMS}

sup: ${sup_OFILES} ${LIBS}
	${CC} ${CFLAGS} -o sup ${sup_OFILES} ${LIBS} ${${SITE}_LIBS}

supfilesrv: ${supfilesrv_OFILES} ${supfilesrv_LIBS}
	${CC} ${CFLAGS} -o supfilesrv ${supfilesrv_OFILES} ${supfilesrv_LIBS} ${LIBS} ${${SITE}_LIBS}

supscan:  ${supscan_OFILES} ${LIBS}
	${CC} ${CFLAGS} -o supscan ${supscan_OFILES} ${LIBS} ${${SITE}_LIBS}

libextra.a: ${EXTRA}
	ar r libextra.a $?
	ranlib libextra.a

clean cleandir distclean:
	rm -f ${PROGRAMS} libextra.a netcrypt.c *.o core a.out

install: ${PROGRAMS}
	install -cs -m 555 -o bin -g bin ${PROGRAMS} ${NETBSD_BINDIR}
	install -c -m 444 -o bin -g bin ${MAN1} ${NETBSD_MAN1}
	install -c -m 444 -o bin -g bin ${MAN8} ${NETBSD_MAN8}

netcrypt.c:
	@echo "[ Using netcryptvoid.c ]"
	cp netcryptvoid.c netcrypt.c

tar:
	mkdir ${TARDIR} ${TARDIR}/sys
	cp -p *.[ch18] Makefile ${TARDIR}
	cp -p sys/*.h ${TARDIR}/sys
	tar -czf ${TARDIR}.tar.gz ${TARDIR}
	rm -fr ${TARDIR}

read_line.o: supextern.h
scan.o:	sup.h
scm.o:	sup.h
scmio.o: sup.h supmsg.h
stree.o: sup.h
supcmain.o: sup.h supmsg.h supcdefs.h
supcmeat.o: sup.h supmsg.h supcdefs.h
supcmisc.o: sup.h supmsg.h supcdefs.h
supcname.o: sup.h supmsg.h supcdefs.h
supcparse.o: sup.h supmsg.h supcdefs.h
supfilesrv.o: sup.h supmsg.h
supmsg.o: sup.h supmsg.h
supscan.o: sup.h
netcryptvoid.o: sup.h supmsg.h
netcrypt.o: sup.h supmsg.h
