/*	$NetBSD$	*/
/*	$OpenBSD: pio.h,v 1.3 1997/04/19 17:19:58 pefo Exp $	*/

/*
 * Copyright (c) 1995 Per Fogelstrom.  All rights reserved.
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
 *      This product includes software developed by Per Fogelstrom.
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

#ifndef _ARC_PIO_H_
#define _ARC_PIO_H_
/*
 * I/O macros.
 */

#define	outb(a,v)	(*(volatile unsigned char*)(a) = (v))
#define	outw(a,v)	(*(volatile unsigned short*)(a) = (v))
#define	out16(a,v)	outw(a,v)
#define	outl(a,v)	(*(volatile unsigned int*)(a) = (v))
#define	out32(a,v)	outl(a,v)
#define	inb(a)		(*(volatile unsigned char*)(a))
#define	inw(a)		(*(volatile unsigned short*)(a))
#define	in16(a)		inw(a)
#define	inl(a)		(*(volatile unsigned int*)(a))
#define	in32(a)		inl(a)

void insb(uint8_t *, uint8_t *,int);
void insw(uint16_t *, uint16_t *,int);
void insl(uint32_t *, uint32_t *,int);
void outsb(uint8_t *, const uint8_t *,int);
void outsw(uint16_t *, const uint16_t *,int);
void outsl(uint32_t *, const uint32_t *,int);

extern struct arc_bus_space arc_bus_io, arc_bus_mem;

#endif /*_ARC_PIO_H_*/
