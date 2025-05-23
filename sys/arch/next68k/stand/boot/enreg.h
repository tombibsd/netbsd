/*	$NetBSD$	*/
/*
 * Copyright (c) 1997 Rolf Grossmann
 * All rights reserved.
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
 *      This product includes software developed by Rolf Grossmann.
 * 4. The name of the author may not be used to endorse or promote products
 *    derived from this software without specific prior written permission
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* from next/cpu.h */
#define	SLOT_ID		0x0
#ifdef	MC68030
#define	SLOT_ID_BMAP	0x0
#endif
#ifdef	MC68040
#define	SLOT_ID_BMAP	0x00100000
#endif

#define P_ENETX_CSR     ((void *)(SLOT_ID+0x02000110))
#define P_ENETR_CSR     ((void *)(SLOT_ID+0x02000150))
#define P_ENET          ((void *)(SLOT_ID_BMAP+0x02006000))
#define	P_BMAP		((void *)(SLOT_ID+0x020c0000))

#define ENETX_DMA_INTR	(1<<28)	/* I_BIT(I_ENETX_DMA) */
#define ENETR_DMA_INTR	(1<<27)	/* I_BIT(I_ENETR_DMA) */
#define ENETX_INTR	(1<<10)	/* I_BIT(I_ENETX) */
#define ENETR_INTR	(1<<9)	/* I_BIT(I_ENETR) */


