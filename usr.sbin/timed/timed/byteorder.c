/*-
 * Copyright (c) 1985, 1993
 *	The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include <sys/cdefs.h>
#ifndef lint
#if 0
static char sccsid[] = "@(#)byteorder.c	8.1 (Berkeley) 6/6/93";
#else
__RCSID("$NetBSD$");
#endif
#endif /* not lint */

#include "globals.h"

/*
 * Two routines to do the necessary byte swapping for timed protocol
 * messages. Protocol is defined in /usr/include/protocols/timed.h
 */
void
bytenetorder(struct tsp *ptr)
{
	ptr->tsp_seq = htons((uint16_t)ptr->tsp_seq);
	switch (ptr->tsp_type) {

	case TSP_SETTIME:
	case TSP_ADJTIME:
	case TSP_SETDATE:
	case TSP_SETDATEREQ:
		ptr->tsp_time.tv_sec = htonl((uint32_t)ptr->tsp_time.tv_sec);
		ptr->tsp_time.tv_usec = htonl((uint32_t)ptr->tsp_time.tv_usec);
		break;
	
	default:
		break;		/* nothing more needed */
	}
}

void
bytehostorder(struct tsp *ptr)
{
	ptr->tsp_seq = ntohs((uint16_t)ptr->tsp_seq);
	switch (ptr->tsp_type) {

	case TSP_SETTIME:
	case TSP_ADJTIME:
	case TSP_SETDATE:
	case TSP_SETDATEREQ:
		ptr->tsp_time.tv_sec = ntohl((uint32_t)ptr->tsp_time.tv_sec);
		ptr->tsp_time.tv_usec = ntohl((uint32_t)ptr->tsp_time.tv_usec);
		break;
	
	default:
		break;		/* nothing more needed */
	}
}
