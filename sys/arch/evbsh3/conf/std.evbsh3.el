# $NetBSD$
#
# standard, required NetBSD/evbsh3 'options'

machine evbsh3 sh3
include		"conf/std"	# MI standard options
include		"arch/sh3/conf/std.sh3el"	# arch standard options

makeoptions	DEFTEXTADDR="0x8c010000"
