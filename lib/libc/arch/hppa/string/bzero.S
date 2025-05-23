/*	$NetBSD$	*/

/*	$OpenBSD: bzero.S,v 1.3 2001/06/04 23:14:02 mickey Exp $	*/

/*
 *  (c) Copyright 1988 HEWLETT-PACKARD COMPANY
 *
 *  To anyone who acknowledges that this file is provided "AS IS"
 *  without any express or implied warranty:
 *      permission to use, copy, modify, and distribute this file
 *  for any purpose is hereby granted without fee, provided that
 *  the above copyright notice and this notice appears in all
 *  copies, and that the name of Hewlett-Packard Company not be
 *  used in advertising or publicity pertaining to distribution
 *  of the software without specific, written prior permission.
 *  Hewlett-Packard Company makes no representations about the
 *  suitability of this software for any purpose.
 */
/*
 * Copyright (c) 1990,1994 The University of Utah and
 * the Computer Systems Laboratory (CSL).  All rights reserved.
 *
 * THE UNIVERSITY OF UTAH AND CSL PROVIDE THIS SOFTWARE IN ITS "AS IS"
 * CONDITION, AND DISCLAIM ANY LIABILITY OF ANY KIND FOR ANY DAMAGES
 * WHATSOEVER RESULTING FROM ITS USE.
 *
 * CSL requests users of this software to return to csl-dist@cs.utah.edu any
 * improvements that they make and grant CSL redistribution rights.
 *
 * 	Utah $Hdr: bzero.s 1.9 94/12/14$
 *	Author: Bob Wheeler, University of Utah CSL
 */

#include <machine/asm.h>

#ifdef SYSLIBC_SCCS
	.text
	.asciz "$OpenBSD: bzero.S,v 1.3 2001/06/04 23:14:02 mickey Exp $"
	.align	4
#endif

/*
 * void 
 * bzero(dst, count)
 *	void *dst;
 *	size_t count;
 */
LEAF_ENTRY(bzero)
        comb,>=,n %r0,%arg1,$bzero_exit

	/*
	 * If we need to clear less than a word do it a byte at a time
	 */

	comib,>>,n 4,%arg1,$bzero_bytes

	/*
	 * Since we are only clearing memory the alignment restrictions 
	 * are simplified. Figure out how many "extra" bytes we need to
	 * store with stbys.
	 */

        extru   %arg0,31,2,%t1
        add     %arg1,%t1,%arg1

	/*
	 * We will zero the destination in blocks of 16 bytes as long as we 
	 * can and then we'll go to the 4 byte moves.
	 */

	comib,>>=	15, %arg1, $bzero_word
	addi		-16, %arg1, %arg1

$bzero_loop_16:
        stbys,b,m %r0,4(%arg0)
        stwm    %r0,4(%arg0)
        stwm    %r0,4(%arg0)
        stwm    %r0,4(%arg0)
	comib,<<	15, %arg1, $bzero_loop_16
	addi		-16, %arg1, %arg1

	/*
	 * see if there is anything left that needs to be zeroed in a word 
	 * move. Since the count was decremented by 16, add 12 to test if 
	 * there are any full word moves left to do.
	 */

$bzero_word:
        addib,<,n 12,%arg1,$bzero_cleanup

$bzero_loop_4:
        addib,>= -4,%arg1,$bzero_loop_4
        stbys,b,m %r0,4(%arg0)

	/*
	 * zero the last bytes that may be unaligned on a word boundary
	 */

$bzero_cleanup:
        addib,=,n 4,%arg1,$bzero_exit
        add	%arg0,%arg1,%arg0
        b       $bzero_exit
        stbys,e %r0,0(%arg0)
	b,n	$bzero_exit


	/*
	 * zero by bytes
	 */

$bzero_bytes:
        addib,> -1,%arg1,$bzero_bytes
        stbs,ma %r0,1(%arg0) 

$bzero_exit:
	bv,n	%r0(%rp)
EXIT(bzero)

	.end
