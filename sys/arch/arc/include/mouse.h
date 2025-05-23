/*	$NetBSD$	*/
/*	$OpenBSD: mouse.h,v 1.2 1996/06/06 23:06:52 deraadt Exp $	*/
/*	NetBSD: mouse.h,v 1.4 1994/10/27 04:16:10 cgd Exp 	*/

/*-
 * Copyright (c) 1992, 1993 Erik Forsberg.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * THIS SOFTWARE IS PROVIDED BY ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.  IN
 * NO EVENT SHALL I BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _MOUSE_H_
#define _MOUSE_H_

struct mouseinfo {
	unsigned char status;
	char xmotion, ymotion;
};

#define BUTSTATMASK	0x07	/* Any mouse button down if any bit set */
#define BUTCHNGMASK	0x38	/* Any mouse button changed if any bit set */

#define BUT3STAT	0x01	/* Button 3 down if set */
#define BUT2STAT	0x02	/* Button 2 down if set */
#define BUT1STAT	0x04	/* Button 1 down if set */
#define BUT3CHNG	0x08	/* Button 3 changed if set */
#define BUT2CHNG	0x10	/* Button 2 changed if set */
#define BUT1CHNG	0x20	/* Button 1 changed if set */
#define MOVEMENT	0x40	/* Mouse movement detected */

/* Ioctl definitions */

#define MOUSEIOC        ('M'<<8)
#define MOUSEIOCREAD    (MOUSEIOC|60)

#endif /* !_MOUSE_H_ */
