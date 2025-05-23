#	$NetBSD$
#
#	etc.news68k/Makefile.inc -- news68k-specific etc Makefile targets
#

# If you change the list of distributed kernels, don't forget
# to update the release documentation in distrib/notes/common/contents

KERNEL_SETS=		GENERIC GENERIC_TINY

BUILD_KERNELS=		INSTALL

INSTALLATION_DIRS+=	installation/floppy

snap_md_post:
	${MAKESUMS} -t ${RELEASEDIR}/${RELEASEMACHINEDIR}/installation/floppy '*.fs'
