/*	$NetBSD$	*/

/*
 * pid = fork();
 *
 * r12 == 0 in parent process, 1 in child process.
 * r11 == pid of child in parent, pid of parent in child.
 */

#include "SYS.h"

_SYSCALL(__fork,fork)
	l.addi	r12,r12,-1	# from 1 to 0 in child, 0 to -1 in parent
	l.and	r11,r11,r12	# return 0 in child, pid in parent
	l.jr	lr
	l.nop
END(__fork)
