# $NetBSD$
#
# Unit tests for make(1)
# The main targets are:
# 
# all:	run all the tests
# test:	run 'all', and compare to expected results
# accept: move generated output to expected results
#
# Adding a test case.  
# Each feature should get its own set of tests in its own suitably
# named makefile (*.mk), with its own set of expected results (*.exp),
# and it should be added to the TESTNAMES list.
# 

.MAIN: all

UNIT_TESTS:= ${.PARSEDIR}
.PATH: ${UNIT_TESTS}

# Each test is in a sub-makefile.
# Keep the list sorted.
TESTNAMES= \
	comment \
	cond1 \
	cond2 \
	error \
	export \
	export-all \
	export-env \
	doterror \
	dotwait \
	forloop \
	forsubst \
	hash \
	misc \
	moderrs \
	modmatch \
	modmisc \
	modorder \
	modts \
	modword \
	order \
	posix \
	qequals \
	sunshcmd \
	sysv \
	ternary \
	unexport \
	unexport-env \
	varcmd \
	varmisc \
	varshell

# these tests were broken by referting POSIX chanegs
STRICT_POSIX_TESTS = \
	escape \
	impsrc \
	phony-end \
	posix1 \
	suffixes

# Override make flags for certain tests
flags.doterror=
flags.order=-j1

OUTFILES= ${TESTNAMES:S/$/.out/}

all: ${OUTFILES}

CLEANFILES += *.rawout *.out *.status *.tmp *.core *.tmp
CLEANFILES += obj*.[och] lib*.a		# posix1.mk
CLEANFILES += issue* .[ab]*		# suffixes.mk
CLEANRECURSIVE += dir dummy		# posix1.mk

clean:
	rm -f ${CLEANFILES}
.if !empty(CLEANRECURSIVE)
	rm -rf ${CLEANRECURSIVE}
.endif

TEST_MAKE?= ${.MAKE}
TOOL_SED?= sed

# ensure consistent results from sort(1)
LC_ALL= C
LANG= C
.export LANG LC_ALL

# the tests are actually done with sub-makes.
.SUFFIXES: .mk .rawout .out
.mk.rawout:
	@echo ${TEST_MAKE} ${flags.${.TARGET:R}:U-k} -f ${.IMPSRC}
	-@cd ${.OBJDIR} && \
	{ ${TEST_MAKE} ${flags.${.TARGET:R}:U-k} -f ${.IMPSRC} \
	  2>&1 ; echo $$? >${.TARGET:R}.status ; } > ${.TARGET}.tmp
	@mv ${.TARGET}.tmp ${.TARGET}

# We always pretend .MAKE was called 'make' 
# and strip ${.CURDIR}/ from the output
# and replace anything after 'stopped in' with unit-tests
# so the results can be compared.
.rawout.out:
	@echo postprocess ${.TARGET}
	@${TOOL_SED} -e 's,^${TEST_MAKE:T:C/\./\\\./g}[][0-9]*:,make:,' \
	  -e 's,${TEST_MAKE:C/\./\\\./g},make,' \
	  -e '/stopped/s, /.*, unit-tests,' \
	  -e 's,${.CURDIR:C/\./\\\./g}/,,g' \
	  -e 's,${UNIT_TESTS:C/\./\\\./g}/,,g' \
	  < ${.IMPSRC} > ${.TARGET}.tmp
	@echo "exit status `cat ${.TARGET:R}.status`" >> ${.TARGET}.tmp
	@mv ${.TARGET}.tmp ${.TARGET}

# Compare all output files
test:	${OUTFILES} .PHONY
	@failed= ; \
	for test in ${TESTNAMES}; do \
	  diff -u ${UNIT_TESTS}/$${test}.exp $${test}.out \
	  || failed="$${failed}$${failed:+ }$${test}" ; \
	done ; \
	if [ -n "$${failed}" ]; then \
	  echo "Failed tests: $${failed}" ; false ; \
	else \
	  echo "All tests passed" ; \
	fi

accept:
	@for test in ${TESTNAMES}; do \
	  cmp -s ${UNIT_TESTS}/$${test}.exp $${test}.out \
	  || { echo "Replacing $${test}.exp" ; \
	       cp $${test}.out ${UNIT_TESTS}/$${test}.exp ; } \
	done

.if exists(${TEST_MAKE})
${TESTNAMES:S/$/.rawout/}: ${TEST_MAKE}
.endif

.-include <bsd.obj.mk>
