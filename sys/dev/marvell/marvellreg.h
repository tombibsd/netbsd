/*	$NetBSD$	*/
/*
 * Copyright (c) 2009 KIYOHARA Takashi
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
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef _DEV_MARVELL_MARVELLREG_H_
#define _DEV_MARVELL_MARVELLREG_H_

#include <dev/pci/pcidevs.h>

#define MARVELL_DISCOVERY		PCI_PRODUCT_MARVELL_GT64260
#define MARVELL_DISCOVERY_II		PCI_PRODUCT_MARVELL_MV64360
#define MARVELL_DISCOVERY_III		PCI_PRODUCT_MARVELL_MV64460
#if 0
#define MARVELL_DISCOVERY_LT		/* XXXX: 0x???? */
#define MARVELL_DISCOVERY_V		/* XXXX: 0x6450 ? */
#define MARVELL_DISCOVERY_VI		/* XXXX: 0x6460 ? */
#endif

#define MARVELL_ORION_1_88F1181		PCI_PRODUCT_MARVELL_88F1181
#define MARVELL_ORION_1_88F5082		PCI_PRODUCT_MARVELL_88F5082
#define MARVELL_ORION_1_88F5180N	PCI_PRODUCT_MARVELL_88F5180N
#define MARVELL_ORION_1_88F5181		PCI_PRODUCT_MARVELL_88F5181
#define MARVELL_ORION_1_88F5182		PCI_PRODUCT_MARVELL_88F5182
#define MARVELL_ORION_1_88F6082		PCI_PRODUCT_MARVELL_88F6082
#define MARVELL_ORION_1_88F6183		PCI_PRODUCT_MARVELL_88F6183
#define MARVELL_ORION_1_88W8660		PCI_PRODUCT_MARVELL_88W8660

#define MARVELL_ORION_2_88F1281		PCI_PRODUCT_MARVELL_88F1281
#define MARVELL_ORION_2_88F5281		PCI_PRODUCT_MARVELL_88F5281

#define MARVELL_KIRKWOOD_88F6180	PCI_PRODUCT_MARVELL_88F6180
#define MARVELL_KIRKWOOD_88F6192	PCI_PRODUCT_MARVELL_88F6192
#define MARVELL_KIRKWOOD_88F6281	PCI_PRODUCT_MARVELL_88F6281
#define MARVELL_KIRKWOOD_88F6282	PCI_PRODUCT_MARVELL_88F6282

#define MARVELL_MV78XX0_MV78100		PCI_PRODUCT_MARVELL_MV78100
#define MARVELL_MV78XX0_MV78200		PCI_PRODUCT_MARVELL_MV78200

#define MARVELL_ARMADAXP_MV78130	PCI_PRODUCT_MARVELL_MV78130
#define MARVELL_ARMADAXP_MV78160	PCI_PRODUCT_MARVELL_MV78160
#define MARVELL_ARMADAXP_MV78230	PCI_PRODUCT_MARVELL_MV78230
#define MARVELL_ARMADAXP_MV78260	PCI_PRODUCT_MARVELL_MV78260
#define MARVELL_ARMADAXP_MV78460	PCI_PRODUCT_MARVELL_MV78460

#define MARVELL_ARMADA370_MV6707	PCI_PRODUCT_MARVELL_MV6707
#define MARVELL_ARMADA370_MV6710	PCI_PRODUCT_MARVELL_MV6710
#define MARVELL_ARMADA370_MV6W11	PCI_PRODUCT_MARVELL_MV6W11

#define MARVELL_DISCOVERY_REVA	0x10
#define MARVELL_DISCOVERY_REVB	0x20

#define MARVELL_ATTR_MASK		0xff
#define MARVELL_ATTR_SDRAM_CFU_SHARE	0x10 /* shared and snoop enabled.*/
#define MARVELL_ATTR_SDRAM_CFU_L2_DEP	0x20 /* enable L2 deposit */
#ifdef AURORA_IO_CACHE_COHERENCY
#define MARVELL_ATTR_SDRAM_CS0		(0x0e | MARVELL_ATTR_SDRAM_CFU_SHARE)
#define MARVELL_ATTR_SDRAM_CS1		(0x0d | MARVELL_ATTR_SDRAM_CFU_SHARE)
#define MARVELL_ATTR_SDRAM_CS2		(0x0b | MARVELL_ATTR_SDRAM_CFU_SHARE)
#define MARVELL_ATTR_SDRAM_CS3		(0x07 | MARVELL_ATTR_SDRAM_CFU_SHARE)
#else
#define MARVELL_ATTR_SDRAM_CS0		0x0e
#define MARVELL_ATTR_SDRAM_CS1		0x0d
#define MARVELL_ATTR_SDRAM_CS2		0x0b
#define MARVELL_ATTR_SDRAM_CS3		0x07
#endif

#endif	/* _DEV_MARVELL_MARVELLREG_H_ */
