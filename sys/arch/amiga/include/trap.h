/*	$NetBSD$	*/

#ifndef _MACHINE_TRAP_H_
#define _MACHINE_TRAP_H_

#include <m68k/trap.h>

/*
 * make sure we don't have this one defined, it's no longer done
 * with the REI emulation.
 */
#undef T_SSIR

#endif
