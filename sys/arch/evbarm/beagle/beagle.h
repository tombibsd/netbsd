/*	$NetBSD$	*/
/*
 * Copyright (c) 2007 Microsoft
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
 *	This product includes software developed by Microsoft
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTERS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _EVBARM_BEAGLE_BEAGLE_H
#define _EVBARM_BEAGLE_BEAGLE_H

#include <arm/omap/omap2_reg.h>

/*
 * Memory will mapped 1:1 VA:PA starting at 0x80000000
 * Kernel VM space: 512MB at KERNEL_VM_BASE
 */
#define KERNEL_VM_BASE		0xc0000000
#define KERNEL_VM_SIZE		0x20000000

#ifdef OMAP_3430
#define OMAP_L4_CORE_BASE		OMAP3430_L4_CORE_BASE
#define OMAP_L4_CORE_SIZE		OMAP3430_L4_CORE_SIZE
#define OMAP_L4_PERIPHERAL_BASE		OMAP3430_L4_PERIPHERAL_BASE
#define OMAP_L4_PERIPHERAL_SIZE		OMAP3430_L4_PERIPHERAL_SIZE
#define OMAP_L4_WAKEUP_BASE		OMAP3430_L4_WAKEUP_BASE
#define OMAP_L4_WAKEUP_SIZE		OMAP3430_L4_WAKEUP_SIZE
#define OMAP_SDRC_BASE			OMAP3530_SDRC_BASE
#define OMAP_SDRC_SIZE			OMAP3530_SDRC_SIZE
#endif

#ifdef OMAP_3530
#define OMAP_L4_CORE_BASE		OMAP3530_L4_CORE_BASE
#define OMAP_L4_CORE_SIZE		OMAP3530_L4_CORE_SIZE
#define OMAP_L4_PERIPHERAL_BASE		OMAP3530_L4_PERIPHERAL_BASE
#define OMAP_L4_PERIPHERAL_SIZE		OMAP3530_L4_PERIPHERAL_SIZE
#define OMAP_L4_WAKEUP_BASE		OMAP3530_L4_WAKEUP_BASE
#define OMAP_L4_WAKEUP_SIZE		OMAP3530_L4_WAKEUP_SIZE
#define OMAP_SDRC_BASE			OMAP3530_SDRC_BASE
#define OMAP_SDRC_SIZE			OMAP3530_SDRC_SIZE
#endif

#ifdef OMAP_4XXX
#define OMAP_L4_CORE_BASE		OMAP4430_L4_CORE_BASE
#define OMAP_L4_CORE_SIZE		OMAP4430_L4_CORE_SIZE
#define OMAP_L4_PERIPHERAL_BASE		OMAP4430_L4_PERIPHERAL_BASE
#define OMAP_L4_PERIPHERAL_SIZE		OMAP4430_L4_PERIPHERAL_SIZE
#define OMAP_L4_WAKEUP_BASE		OMAP4430_L4_WAKEUP_BASE
#define OMAP_L4_WAKEUP_SIZE		OMAP4430_L4_WAKEUP_SIZE
#define OMAP_L4_ABE_BASE		OMAP4430_L4_ABE_BASE
#define OMAP_L4_ABE_SIZE		OMAP4430_L4_ABE_SIZE
#define OMAP_EMIF1_BASE			OMAP4430_EMIF1_BASE
#define OMAP_EMIF1_SIZE			OMAP4430_EMIF1_SIZE
#define OMAP_EMIF2_BASE			OMAP4430_EMIF2_BASE
#define OMAP_EMIF2_SIZE			OMAP4430_EMIF2_SIZE
#endif

#ifdef OMAP_5XXX
#define OMAP_L4_CORE_BASE		OMAP5430_L4_CORE_BASE
#define OMAP_L4_CORE_SIZE		OMAP5430_L4_CORE_SIZE
#define OMAP_L4_PERIPHERAL_BASE		OMAP5430_L4_PERIPHERAL_BASE
#define OMAP_L4_PERIPHERAL_SIZE		OMAP5430_L4_PERIPHERAL_SIZE
#define OMAP_L4_WAKEUP_BASE		OMAP5430_L4_WAKEUP_BASE
#define OMAP_L4_WAKEUP_SIZE		OMAP5430_L4_WAKEUP_SIZE
#define OMAP_L4_ABE_BASE		OMAP5430_L4_ABE_BASE
#define OMAP_L4_ABE_SIZE		OMAP5430_L4_ABE_SIZE
#define OMAP_EMIF1_BASE			OMAP5430_EMIF1_BASE
#define OMAP_EMIF1_SIZE			OMAP5430_EMIF1_SIZE
#define OMAP_EMIF2_BASE			OMAP5430_EMIF2_BASE
#define OMAP_EMIF2_SIZE			OMAP5430_EMIF2_SIZE
#endif

#ifdef TI_AM335X
#define OMAP_L4_CORE_BASE		TI_AM335X_L4_WAKEUP_BASE
#define OMAP_L4_CORE_SIZE		TI_AM335X_L4_WAKEUP_SIZE
#define OMAP_L4_PERIPHERAL_BASE		TI_AM335X_L4_PERIPHERAL_BASE
#define OMAP_L4_PERIPHERAL_SIZE		TI_AM335X_L4_PERIPHERAL_SIZE
#define OMAP_L4_FAST_BASE		TI_AM335X_L4_FAST_BASE
#define OMAP_L4_FAST_SIZE		TI_AM335X_L4_FAST_SIZE
#define OMAP_EMIF1_BASE			TI_AM335X_EMIF1_BASE
#define OMAP_EMIF1_SIZE			TI_AM335X_EMIF1_SIZE
#endif

#ifdef TI_DM37XX
#define OMAP_L4_CORE_BASE		TI_DM37XX_L4_CORE_BASE
#define OMAP_L4_CORE_SIZE		TI_DM37XX_L4_CORE_SIZE
#define OMAP_L4_PERIPHERAL_BASE		TI_DM37XX_L4_PERIPHERAL_BASE
#define OMAP_L4_PERIPHERAL_SIZE		TI_DM37XX_L4_PERIPHERAL_SIZE
#define OMAP_L4_WAKEUP_BASE		TI_DM37XX_L4_WAKEUP_BASE
#define OMAP_L4_WAKEUP_SIZE		TI_DM37XX_L4_WAKEUP_SIZE
#endif

/*
 * We devmap IO starting at KERNEL_VM_BASE + KERNEL_VM_SIZE
 */
#define OMAP_KERNEL_IO_VBASE	(KERNEL_VM_BASE + KERNEL_VM_SIZE)
#if defined(OMAP_3XXX)
#define OMAP_L4_CORE_VBASE	OMAP_KERNEL_IO_VBASE
#define OMAP_L4_PERIPHERAL_VBASE	(OMAP_L4_CORE_VBASE + OMAP_L4_CORE_SIZE)
#define OMAP_L4_WAKEUP_VBASE	(OMAP_L4_PERIPHERAL_VBASE + OMAP_L4_PERIPHERAL_SIZE)
#define OMAP_SDRC_VBASE		(OMAP_L4_WAKEUP_VBASE + OMAP_L4_WAKEUP_SIZE)
#define OMAP_KERNEL_IO_VEND	(OMAP_SDRC_VBASE + OMAP_SDRC_SIZE)
#define CONSADDR_VA	((CONSADDR - OMAP_L4_PERIPHERAL_BASE) + OMAP_L4_PERIPHERAL_VBASE)
#elif defined(OMAP_4XXX) || defined(OMAP_5XXX)
#define OMAP_L4_CORE_VBASE	OMAP_KERNEL_IO_VBASE
#define OMAP_L4_PERIPHERAL_VBASE (OMAP_L4_CORE_VBASE + OMAP_L4_CORE_SIZE)
#define OMAP_L4_ABE_VBASE	(OMAP_L4_PERIPHERAL_VBASE + OMAP_L4_PERIPHERAL_SIZE)
#define OMAP_EMIF1_VBASE	(OMAP_L4_ABE_VBASE + OMAP_L4_ABE_SIZE)
#define OMAP_EMIF2_VBASE	(OMAP_EMIF1_VBASE + OMAP_EMIF1_SIZE)
#define OMAP_KERNEL_IO_VEND	(OMAP_EMIF2_VBASE + OMAP_EMIF2_SIZE)
#define CONSADDR_VA	((CONSADDR - OMAP_L4_PERIPHERAL_BASE) + OMAP_L4_PERIPHERAL_VBASE)
#elif defined(TI_AM335X)
#define OMAP_L4_PERIPHERAL_VBASE	OMAP_KERNEL_IO_VBASE
#define OMAP_L4_FAST_VBASE	(OMAP_L4_PERIPHERAL_VBASE + OMAP_L4_PERIPHERAL_SIZE)
#define OMAP_EMIF1_VBASE	(OMAP_L4_FAST_VBASE + OMAP_L4_FAST_SIZE)
#define OMAP_L4_CORE_VBASE	(OMAP_EMIF1_VBASE + OMAP_EMIF1_SIZE)
#define OMAP_KERNEL_IO_VEND	(OMAP_L4_CORE_VBASE + OMAP_L4_CORE_SIZE)
#define CONSADDR_VA	((CONSADDR - OMAP_L4_CORE_BASE) + OMAP_L4_CORE_VBASE)
#elif defined(TI_DM37XX)
#define OMAP_L4_CORE_VBASE	OMAP_KERNEL_IO_VBASE
#define OMAP_L4_PERIPHERAL_VBASE	(OMAP_L4_CORE_VBASE + OMAP_L4_CORE_SIZE)
#define OMAP_KERNEL_IO_VEND	(OMAP_L4_PERIPHERAL_VBASE + OMAP_L4_PERIPHERAL_SIZE)
#define CONSADDR_VA	((CONSADDR - OMAP_L4_PERIPHERAL_BASE) + OMAP_L4_PERIPHERAL_VBASE)
#else
#error unknown OMAP variant
#endif

#endif /* _EVBARM_BEAGLE_BEAGLE_H */
