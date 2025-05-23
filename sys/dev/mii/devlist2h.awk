#! /usr/bin/awk -f
#	$NetBSD$
#
# Copyright (c) 1998 The NetBSD Foundation, Inc.
# All rights reserved.
#
# This code is derived from software contributed to The NetBSD Foundation
# by Christos Zoulas.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
# ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
# TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
# BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
# POSSIBILITY OF SUCH DAMAGE.
#
# Copyright (c) 1995, 1996 Christopher G. Demetriou
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
# 3. All advertising materials mentioning features or use of this software
#    must display the following acknowledgement:
#      This model includes software developed by Christopher G. Demetriou.
#      This model includes software developed by Christos Zoulas
# 4. The name of the author(s) may not be used to endorse or promote models
#    derived from this software without specific prior written permission
#
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
# IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
# OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
# INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
# NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
function collectline(f) {
	oparen = 0
	line = ""
	while (f <= NF) {
		if ($f == "#") {
			line = line "("
			oparen = 1
			f++
			continue
		}
		if (oparen) {
			line = line $f
			if (f < NF)
				line = line " "
			f++
			continue
		}
		line = line $f
		if (f < NF)
			line = line " "
		f++
	}
	if (oparen)
		line = line ")"
	return line
}
BEGIN {
	nmodels = nouis = nuios = blanklines = firstdone = 0
	hfile="miidevs.h"
	dfile="miidevs_data.h"
}
NR == 1 {
	VERSION = $0
	gsub("\\$", "", VERSION)
	gsub(/ $/, "", VERSION)

	printf("/*\t$NetBSD" "$\t*/\n\n") > hfile
	printf("/*\n") > hfile
	printf(" * THIS FILE AUTOMATICALLY GENERATED.  DO NOT EDIT.\n") \
	    > hfile
	printf(" *\n") > hfile
	printf(" * generated from:\n") > hfile
	printf(" *\t%s\n", VERSION) > hfile
	printf(" */\n") > hfile

	printf("/*\t$NetBSD" "$\t*/\n\n") > dfile
	printf("/*\n") > dfile
	printf(" * THIS FILE AUTOMATICALLY GENERATED.  DO NOT EDIT.\n") \
	    > dfile
	printf(" *\n") > dfile
	printf(" * generated from:\n") > dfile
	printf(" *\t%s\n", VERSION) > dfile
	printf(" */\n") > dfile

	next
}
NF > 0 && $1 == "oui" {
	nuios++

	ouiindex[$2] = nouis;		# record index for this name, for later.

	ouis[nouis, 1] = $2;		# name
	ouis[nouis, 2] = $3;		# id
	printf("#define\tMII_OUI_%s\t%s\t", ouis[nouis, 1],
	    ouis[nouis, 2]) > hfile
	ouis[nouis, 3] = collectline(4)
	printf("/* %s */\n", ouis[nouis, 3]) > hfile
	next
}
NF > 0 && $1 == "model" {
	nmodels++

	models[nmodels, 1] = $2;		# oui name
	models[nmodels, 2] = $3;		# model id
	models[nmodels, 3] = $4;		# id

	printf("#define\tMII_MODEL_%s_%s\t%s\n", models[nmodels, 1],
	    models[nmodels, 2], models[nmodels, 3]) > hfile

	models[nmodels, 4] = collectline(5)

	printf("#define\tMII_STR_%s_%s\t\"%s\"\n",
	    models[nmodels, 1], models[nmodels, 2],
	    models[nmodels, 4]) > hfile

	if (!firstdone) {
		printf("struct mii_knowndev mii_knowndevs[] = {\n") > dfile
		firstdone = 1
	}

	printf(" { MII_OUI_%s, MII_MODEL_%s_%s, MII_STR_%s_%s },\n",
	       $2, $2, $3, $2, $3) > dfile

	next
}
{
	if ($0 == "")
		blanklines++
	print $0 > hfile
	if (blanklines < 2)
		print $0 > dfile
}
END {
	printf(" { 0, 0, NULL }\n") > dfile
	printf("};\n") > dfile
	close(dfile)
	close(hfile)
}
