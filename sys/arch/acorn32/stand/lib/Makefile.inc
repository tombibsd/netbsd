#	$NetBSD$
#
#	Configuration variables (default values are below):
#
#	S	must be set to the top of the 'sys' tree.
#	RISCOSDST	may be set to the location of the directory where library
#		objects are to be built.  Defaults to ${.OBJDIR}/lib/riscos.
#	RISCOSMISCCPPFLAGS
#		Miscellaneous cpp flags to be passed to the library's Makefile
#		when building.
#	RISCOSMISCMAKEFLAGS
#		Miscellaneous flags to be passed to the library's Makefile when
#		building.  See library's Makefile for more details about
#		supported flags and their default values.

# Default values:
RISCOSDST?=		${.OBJDIR}/lib/riscos

RISCOSDIR=		${S}/arch/acorn32/stand/lib
RISCOSLIB=		${RISCOSDST}/libriscos.a

RISCOSMAKE= \
	cd ${RISCOSDIR} && MAKEOBJDIRPREFIX= && unset MAKEOBJDIRPREFIX && \
	  MAKEOBJDIR=${RISCOSDST} ${MAKE} \
	    CC=${CC:Q} CFLAGS=${CFLAGS:Q} \
	    AS=${AS:Q} AFLAGS=${AFLAGS:Q} \
	    LD=${LD:Q} STRIP=${STRIP:Q} \
	    MACHINE=${MACHINE} MACHINE_ARCH=${MACHINE_ARCH:Q} \
	    RISCOSCPPFLAGS=${CPPFLAGS:S@^-I.@-I../../.@g:Q} \
	    RISCOSMISCCPPFLAGS=${RISCOSMISCCPPFLAGS:Q} \
	    ${RISCOSMISCMAKEFLAGS}

${RISCOSLIB}:		.NOTMAIN .MAKE __always_make_riscoslib
	@echo making sure the riscos library is up to date...
	@${RISCOSMAKE} libriscos.a

clean:			.NOTMAIN cleanriscoslib
cleanriscoslib:		.NOTMAIN
	@echo cleaning the riscos library objects
	@if [ -d "${RISCOSDST}" ]; then ${RISCOSMAKE} clean; fi

cleandir distclean:	.NOTMAIN cleandirriscoslib
cleandirriscoslib:	.NOTMAIN
	@echo cleandiring the riscos library objects
	@if [ -d "${RISCOSDST}" ]; then ${RISCOSMAKE} cleandir; fi

depend:			.NOTMAIN dependriscoslib
dependriscoslib:	.NOTMAIN .MAKE __always_make_riscoslib
	@echo depending the riscos library objects
	@${RISCOSMAKE} depend

__always_make_riscoslib:	.NOTMAIN
	@mkdir -p ${RISCOSDST}

.PHONY: __always_make_riscoslib
.PHONY: cleanriscoslib cleandirriscoslib dependriscoslib
