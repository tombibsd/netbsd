/*	$NetBSD$	*/

/*-
 * Copyright (c) 2010 Department of Software Engineering,
 *		      University of Szeged, Hungary
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by the Department of Software Engineering, University of Szeged, Hungary
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef EBH_MISC_H_
#define EBH_MISC_H_

/* EBH specific functions */

#define CHFS_GET_MEMBER_POS(type, member)				      \
	((unsigned long)(&((type *)0)->member))

#define CHFS_GET_LID(lid) (le32toh(lid) & CHFS_LID_DIRTY_BIT_MASK)

/*
 * EBH_TREE_DESTROY - destroys an RB-tree and frees the memory of its elements.
 */
#define EBH_TREE_DESTROY(name, head, type)				      \
	{								      \
		type *var, *nxt;					      \
		for (var = RB_MIN(name, head); var != NULL; var = nxt) {      \
			nxt = RB_NEXT(name, head, var);			      \
			RB_REMOVE(name, head, var);			      \
			kmem_free(var, sizeof(type));			      \
		}							      \
	}

/* XXX HACK! we need a clean solution for destroying mutexes in trees */
#define EBH_TREE_DESTROY_MUTEX(name, head, type)			      \
	{								      \
		type *var, *nxt;					      \
		for (var = RB_MIN(name, head); var != NULL; var = nxt) {      \
			nxt = RB_NEXT(name, head, var);			      \
			RB_REMOVE(name, head, var);			      \
			rw_destroy(&var->mutex);			      \
			kmem_free(var, sizeof(type));			      \
		}							      \
	}

/*
 * EBH_QUEUE_DESTROY - destroys a TAILQ and frees the memory of its elements.
 */
#define EBH_QUEUE_DESTROY(head, type, entry)				      \
	{								      \
		type *var;						      \
		while ((var = TAILQ_FIRST(head))) {			      \
			TAILQ_REMOVE(head, var, entry);			      \
			kmem_free(var, sizeof(type));			      \
		}							      \
	}

#endif /* EBH_MISC_H_ */
