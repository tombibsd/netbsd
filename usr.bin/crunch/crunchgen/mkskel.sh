#!/bin/sh

#	$NetBSD$

# idea and sed lines taken straight from flex

cat <<!EOF
/* File created via mkskel.sh */

const char *crunched_skel[] = {
!EOF

sed 's/\\/&&/g' $* | sed 's/"/\\"/g' | sed 's/.*/  "&",/'

cat <<!EOF
  0
};
!EOF
