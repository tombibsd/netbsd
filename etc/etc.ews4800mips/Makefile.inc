#	$NetBSD$
#
#	etc.ews4800mips/Makefile.inc --
#		ews4800mips-specific etc Makefile targets
#

# If you change the list of distributed kernels, don't forget
# to update the release documentation in distrib/notes/common/contents

KERNEL_SETS=		GENERIC

BUILD_KERNELS=	 	RAMDISK

MD_INSTALLATION_DIRS=	installation/floppy installation/boot

INSTALLATION_DIRS+=	${MD_INSTALLATION_DIRS}

snap_md_post:
	# install bootloader binaries in boot directory
	${HOST_INSTALL_FILE} -m ${NONBINMODE} ${DESTDIR}/usr/mdec/boot \
	    ${RELEASEDIR}/${RELEASEMACHINEDIR}/installation/boot
	${HOST_INSTALL_FILE} -m ${NONBINMODE} ${DESTDIR}/usr/mdec/boot.coff \
	    ${RELEASEDIR}/${RELEASEMACHINEDIR}/installation/boot

	# compute checksums
.for dir in ${MD_INSTALLATION_DIRS}
	${MAKESUMS} -t ${RELEASEDIR}/${RELEASEMACHINEDIR}/${dir} '*'
.endfor
