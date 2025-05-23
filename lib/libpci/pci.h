/*	$NetBSD$	*/

/*
 * Copyright 2001 Wasabi Systems, Inc.
 * All rights reserved.
 *
 * Written by Jason R. Thorpe for Wasabi Systems, Inc.
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
 *	This product includes software developed for the NetBSD Project by
 *	Wasabi Systems, Inc.
 * 4. The name of Wasabi Systems, Inc. may not be used to endorse
 *    or promote products derived from this software without specific prior
 *    written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY WASABI SYSTEMS, INC. ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL WASABI SYSTEMS, INC
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _PCI_H_
#define	_PCI_H_

#include <stddef.h>
#include <stdint.h> /* XXX */

/*
 * Interface to the PCI bus for user programs.
 */

typedef uint32_t pcireg_t;	/* XXX */

/* pci_bus.c */
int	pcibus_conf_read(int, unsigned int, unsigned int, unsigned int,
		unsigned int, pcireg_t *);
int	pcibus_conf_write(int, unsigned int, unsigned int, unsigned int,
		unsigned int, pcireg_t);

/* pci_device.c */
int	pcidev_conf_read(int, unsigned int, pcireg_t *);
int	pcidev_conf_write(int, unsigned int, pcireg_t);

/* pci_drvname.c */
int	pci_drvname(int, unsigned int, unsigned int, char *, size_t);

/* pci_subr.c */
void	pci_devinfo(pcireg_t, pcireg_t, int, char *, size_t);
void	pci_conf_print(int, unsigned int, unsigned int, unsigned int);

#endif /* _PCI_H_ */
