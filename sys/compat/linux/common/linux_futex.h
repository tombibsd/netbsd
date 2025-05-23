/*	$NetBSD$ */

/*-
 * Copyright (c) 2005 Emmanuel Dreyfus, all rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. All advertising materials mentioning features or use of this software
 *    must display the following acknowledgement:
 *	This product includes software developed by Emmanuel Dreyfus
 * 4. The name of the author may not be used to endorse or promote 
 *    products derived from this software without specific prior written 
 *    permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE THE AUTHOR AND CONTRIBUTORS ``AS IS'' 
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, 
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS 
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _LINUX_FUTEX_H
#define _LINUX_FUTEX_H

#define LINUX_FUTEX_WAIT		  0 
#define LINUX_FUTEX_WAKE		  1
#define LINUX_FUTEX_FD			  2
#define LINUX_FUTEX_REQUEUE		  3
#define LINUX_FUTEX_CMP_REQUEUE		  4
#define LINUX_FUTEX_WAKE_OP		  5
#define LINUX_FUTEX_LOCK_PI		  6
#define LINUX_FUTEX_UNLOCK_PI		  7
#define LINUX_FUTEX_TRYLOCK_PI		  8
#define LINUX_FUTEX_WAIT_BITSET		  9
#define LINUX_FUTEX_WAKE_BITSET		 10
#define LINUX_FUTEX_WAIT_REQUEUE_PI	 11
#define LINUX_FUTEX_CMP_REQUEUE_PI	 12

#define LINUX_FUTEX_PRIVATE_FLAG	128
#define LINUX_FUTEX_CLOCK_REALTIME	256
#define LINUX_FUTEX_CMD_MASK		\
    (~(LINUX_FUTEX_PRIVATE_FLAG|LINUX_FUTEX_CLOCK_REALTIME))

#define FUTEX_OP_SET		0
#define FUTEX_OP_ADD		1
#define FUTEX_OP_OR		2
#define FUTEX_OP_ANDN		3
#define FUTEX_OP_XOR		4
#define FUTEX_OP_OPARG_SHIFT	8

#define FUTEX_OP_CMP_EQ		0
#define FUTEX_OP_CMP_NE		1
#define FUTEX_OP_CMP_LT		2
#define FUTEX_OP_CMP_LE		3
#define FUTEX_OP_CMP_GT		4
#define FUTEX_OP_CMP_GE		5

struct linux_robust_list {
	struct linux_robust_list	*next;
};

struct linux_robust_list_head {
	struct linux_robust_list	list;
	unsigned long			futex_offset;
	struct linux_robust_list	*pending_list;
};

#define FUTEX_WAITERS		0x80000000
#define FUTEX_OWNER_DIED	0x40000000
#define FUTEX_TID_MASK		0x3fffffff

#define FUTEX_BITSET_MATCH_ANY  0xffffffff

void	release_futexes(struct lwp *);
struct linux_sys_futex_args;
int	linux_do_futex(struct lwp *, const struct linux_sys_futex_args *,
		       register_t *, struct timespec *);
void	linux_futex_init(void);
void	linux_futex_fini(void);

#endif /* !_LINUX_FUTEX_H */
