/*	$NetBSD$	*/

/*-
 * Copyright (c) 2006, 2007 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Jason R. Thorpe, and by Andrew Doran.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *      
 * THIS SOFTWARE IS PROVIDED BY THE NETBSD FOUNDATION, INC. AND CONTRIBUTORS
 * ``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE FOUNDATION OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "atomic_op_asm.h"

// If we are on building for MIPS III or later, put in SYNCs
#if __mips >= 3 || !defined(__mips_o32)
# define SYNC		sync
#else
# define SYNC		nop
#endif

	.text
	.set noreorder

LEAF(_membar_sync)
	j	ra
	 SYNC
END(_membar_sync)

#ifdef __OCTEON__
LEAF(_membar_producer)
	j	ra
	 syncw
END(_membar_producer)
#endif

#ifdef _KERNEL
STRONG_ALIAS(mb_read, _membar_sync)
#ifdef __OCTEON__
STRONG_ALIAS(mb_write, _membar_producer)
#else
STRONG_ALIAS(mb_write, _membar_sync)
#endif
STRONG_ALIAS(mb_memory, _membar_sync)
#endif

ATOMIC_OP_ALIAS(membar_sync,_membar_sync)
ATOMIC_OP_ALIAS(membar_enter,_membar_sync)
STRONG_ALIAS(_membar_enter,_membar_sync)
#ifdef __OCTEON__
ATOMIC_OP_ALIAS(membar_exit,_membar_producer)
STRONG_ALIAS(_membar_exit,_membar_producer)
STRONG_ALIAS(membar_producer,_membar_producer)
#else
ATOMIC_OP_ALIAS(membar_exit,_membar_sync)
STRONG_ALIAS(_membar_exit,_membar_sync)
ATOMIC_OP_ALIAS(membar_producer,_membar_sync)
STRONG_ALIAS(_membar_producer,_membar_sync)
#endif
ATOMIC_OP_ALIAS(membar_consumer,_membar_sync)
STRONG_ALIAS(_membar_consumer,_membar_sync)
