# $NetBSD$

.include <bsd.own.mk>

TESTSDIR=	${TESTSBASE}/fs

SUBDIR+= 	common .WAIT

TESTS_SUBDIRS+=	ffs fifofs hfs kernfs lfs msdosfs nfs nullfs psshfs ptyfs puffs
TESTS_SUBDIRS+=	tmpfs umapfs union
TESTS_SUBDIRS+=	vfs cd9660

.if ${MKZFS} != "no"
TESTS_SUBDIRS+=	zfs
.endif

FILES= h_funcs.subr
FILESDIR= ${TESTSDIR}

.include <bsd.test.mk>
