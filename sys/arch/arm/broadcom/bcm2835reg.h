/*	$NetBSD$	*/

/*-
 * Copyright (c) 2012 The NetBSD Foundation, Inc.
 * All rights reserved.
 *
 * This code is derived from software contributed to The NetBSD Foundation
 * by Nick Hudson
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

/*
 * Reference: BCM2835 ARM Periperhals
 *
 * 	http://www.raspberrypi.org/wp-content/uploads/2012/02/BCM2835-ARM-Peripherals.pdf
 */

#ifndef	_BCM2835REG_H_
#define	_BCM2835REG_H_

#include "opt_bcm283x.h"

#ifdef BCM2836
#define	BCM2835_PERIPHERALS_BASE	0x3f000000
#else
#define	BCM2835_PERIPHERALS_BASE	0x20000000
#endif
#define	BCM2835_PERIPHERALS_SIZE	0x01000000	/* 16MBytes */

#define	BCM2835_PERIPHERALS_BASE_BUS	0x7e000000
#define	BCM2835_PERIPHERALS_PHYS_TO_BUS(a) \
    ((a) - BCM2835_PERIPHERALS_BASE + BCM2835_PERIPHERALS_BASE_BUS)
#define	BCM2835_PERIPHERALS_BUS_TO_PHYS(a) \
    ((a) - BCM2835_PERIPHERALS_BASE_BUS + BCM2835_PERIPHERALS_BASE)

#define	BCM2835_STIMER_BASE	(BCM2835_PERIPHERALS_BASE_BUS + 0x00003000)
#define	BCM2835_DMA0_BASE	(BCM2835_PERIPHERALS_BASE_BUS + 0x00007000)
#define	BCM2835_ARM_BASE	(BCM2835_PERIPHERALS_BASE_BUS + 0x0000B000)
#define	BCM2835_PM_BASE		(BCM2835_PERIPHERALS_BASE_BUS + 0x00100000)
#define	BCM2835_CM_BASE  	(BCM2835_PERIPHERALS_BASE_BUS + 0x00101000)
#define	BCM2835_RNG_BASE	(BCM2835_PERIPHERALS_BASE_BUS + 0x00104000)
#define	BCM2835_GPIO_BASE	(BCM2835_PERIPHERALS_BASE_BUS + 0x00200000)
#define	BCM2835_UART0_BASE	(BCM2835_PERIPHERALS_BASE_BUS + 0x00201000)
#define	BCM2835_PCM_BASE	(BCM2835_PERIPHERALS_BASE_BUS + 0x00203000)
#define	BCM2835_SPI0_BASE	(BCM2835_PERIPHERALS_BASE_BUS + 0x00204000)
#define	BCM2835_BSC0_BASE	(BCM2835_PERIPHERALS_BASE_BUS + 0x00205000)
#define	BCM2835_PWM_BASE	(BCM2835_PERIPHERALS_BASE_BUS + 0x0020C000)
#define	BCM2835_BSCSPISLV_BASE	(BCM2835_PERIPHERALS_BASE_BUS + 0x00214000)
#define	BCM2835_AUX_BASE	(BCM2835_PERIPHERALS_BASE_BUS + 0x00215000)
#define	BCM2835_EMMC_BASE	(BCM2835_PERIPHERALS_BASE_BUS + 0x00300000)
#define	BCM2835_BSC1_BASE	(BCM2835_PERIPHERALS_BASE_BUS + 0x00804000)
#define	BCM2835_BSC2_BASE	(BCM2835_PERIPHERALS_BASE_BUS + 0x00805000)
#define	BCM2835_USB_BASE	(BCM2835_PERIPHERALS_BASE_BUS + 0x00980000)
#define	BCM2835_DMA15_BASE	(BCM2835_PERIPHERALS_BASE_BUS + 0x00E05000)

#define	BCM2835_STIMER_SIZE	0x1c
#define	BCM2835_DMA0_SIZE	0x1000
#define	BCM2835_ARM_SIZE	0x1000
#define	BCM2835_PM_SIZE		0x1000
#define	BCM2835_CM_SIZE		0xa8
#define	BCM2835_RNG_SIZE	0x1000
#define	BCM2835_GPIO_SIZE	0x1000
#define	BCM2835_UART0_SIZE	0x90
#define	BCM2835_PCM_SIZE	0x1000
#define	BCM2835_SPI0_SIZE	0x1000
#define	BCM2835_BSC_SIZE	0x1000
#define	BCM2835_PWM_SIZE	0x28
#define	BCM2835_AUX_SIZE	0x1000
#define	BCM2835_EMMC_SIZE	0x1000
#define	BCM2835_USB_SIZE	0x20000
#define	BCM2835_DMA15_SIZE	0x100

#define	BCM2835_IOPHYSTOVIRT(a) \
    ((0xf0000000 | (((a) & 0xf0000000) >> 4)) + ((a) & ~0xff000000))

#define	BCM2835_BUSADDR_CACHE_MASK	0xc0000000
#define	BCM2835_BUSADDR_CACHE_COHERENT	0x40000000
#define	BCM2835_BUSADDR_CACHE_L1L2	0x00000000
#define	BCM2835_BUSADDR_CACHE_L2ONLY	0x80000000
#define	BCM2835_BUSADDR_CACHE_DIRECT	0xc0000000

#define	BCM2835_PERIPHERALS_VBASE \
	BCM2835_IOPHYSTOVIRT(BCM2835_PERIPHERALS_BASE)

#define	BCM2835_ARMICU_BASE	(BCM2835_ARM_BASE + 0x0200)
#define	BCM2835_ARMICU_SIZE	0x200

#define	BCM2835_VCHIQ_BASE	(BCM2835_ARM_BASE + 0x0800)
#define	BCM2835_VCHIQ_SIZE	0x50

#define	BCM2835_ARMMBOX_BASE	(BCM2835_ARM_BASE + 0x0880)
#define	BCM2835_ARMMBOX_SIZE	0x40

#define	BCM2835_INTC_BASE	(0x0)	/* Relative to BCM2835_ARMICU_BASE */

/* Interrupt controller */
#define	BCM2835_INTC_IRQBPENDING	(BCM2835_INTC_BASE + 0x00)	/* IRQ Basic pending */
#define	BCM2835_INTC_IRQ1PENDING	(BCM2835_INTC_BASE + 0x04)	/* IRQ pending 1 */
#define	BCM2835_INTC_IRQ2PENDING	(BCM2835_INTC_BASE + 0x08)	/* IRQ pending 2 */
#define	BCM2835_INTC_FIQCTL		(BCM2835_INTC_BASE + 0x0c)	/* FIQ control */
#define	BCM2835_INTC_IRQ1ENABLE		(BCM2835_INTC_BASE + 0x10)	/* Enable IRQs 1 */
#define	BCM2835_INTC_IRQ2ENABLE		(BCM2835_INTC_BASE + 0x14)	/* Enable IRQs 2 */
#define	BCM2835_INTC_IRQBENABLE		(BCM2835_INTC_BASE + 0x18)	/* Enable Basic IRQs */
#define	BCM2835_INTC_IRQ1DISABLE	(BCM2835_INTC_BASE + 0x1c)	/* Disable IRQ 1 */
#define	BCM2835_INTC_IRQ2DISABLE	(BCM2835_INTC_BASE + 0x20)	/* Disable IRQ 2 */
#define	BCM2835_INTC_IRQBDISABLE	(BCM2835_INTC_BASE + 0x24)	/* Disable Basic IRQs */

#define	BCM2835_INTC_ENABLEBASE		(BCM2835_INTC_BASE + 0x10)
#define	BCM2835_INTC_DISABLEBASE	(BCM2835_INTC_BASE + 0x1c)

#if defined(BCM2836)
#define	BCM2836_NCPUS			4
#define	BCM2836_NIRQPERCPU		32

#define	BCM2836_INT_LOCALBASE		0
#define	BCM2836_INT_BASECPUN(n)		(BCM2836_INT_LOCALBASE + ((n) * BCM2836_NIRQPERCPU))
#define	BCM2836_NIRQ			(BCM2836_NIRQPERCPU * BCM2836_NCPUS)

#define	BCM2835_INT_BASE		BCM2836_NIRQ

#define	BCM2836_INT_CNTPSIRQ		0
#define	BCM2836_INT_CNTPNSIRQ		1
#define	BCM2836_INT_CNTHPIRQ		2
#define	BCM2836_INT_CNTVIRQ		3
#define	BCM2836_INT_MAILBOX0		4
#define	BCM2836_INT_MAILBOX1		5
#define	BCM2836_INT_MAILBOX2		6
#define	BCM2836_INT_MAILBOX3		7
#define	BCM2836_INT_GPU_FAST		8
#define	BCM2836_INT_PMU_FAST		9
#define	BCM2836_INT_ZERO		10
#define	BCM2836_INT_TIMER		11
#define	BCM2836_INT_NLOCAL		12

#define	BCM2836_INT_CNTPSIRQ_CPUN(n)	(BCM2836_INT_BASECPUN(n) + BCM2836_INT_CNTPSIRQ)
#define	BCM2836_INT_CNTPNSIRQ_CPUN(n)	(BCM2836_INT_BASECPUN(n) + BCM2836_INT_CNTPNSIRQ)
#define	BCM2836_INT_CNTVIRQ_CPUN(n)	(BCM2836_INT_BASECPUN(n) + BCM2836_INT_CNTVIRQ)
#define	BCM2836_INT_CNTHPIRQ_CPUN(n)	(BCM2836_INT_BASECPUN(n) + BCM2836_INT_CNTHPIRQ)
#define	BCM2836_INT_MAILBOX0_CPUN(n)	(BCM2836_INT_BASECPUN(n) + BCM2836_INT_MAILBOX0)
#else
#define	BCM2835_INT_BASE		0
#endif /* !BCM2836 */

/* Periperal Interrupt sources */
#define	BCM2835_NIRQ			96

#define	BCM2835_INT_GPU0BASE		(BCM2835_INT_BASE + 0)
#define	BCM2835_INT_TIMER0		(BCM2835_INT_GPU0BASE + 0)
#define	BCM2835_INT_TIMER1		(BCM2835_INT_GPU0BASE + 1)
#define	BCM2835_INT_TIMER2		(BCM2835_INT_GPU0BASE + 2)
#define	BCM2835_INT_TIMER3		(BCM2835_INT_GPU0BASE + 3)
#define	BCM2835_INT_USB			(BCM2835_INT_GPU0BASE + 9)
#define	BCM2835_INT_DMA0		(BCM2835_INT_GPU0BASE + 16)
#define	BCM2835_INT_DMA2		(BCM2835_INT_GPU0BASE + 18)
#define	BCM2835_INT_DMA3		(BCM2835_INT_GPU0BASE + 19)
#define	BCM2835_INT_AUX			(BCM2835_INT_GPU0BASE + 29)
#define	BCM2835_INT_ARM			(BCM2835_INT_GPU0BASE + 30)

#define	BCM2835_INT_GPU1BASE		(BCM2835_INT_BASE + 32)
#define	BCM2835_INT_GPIO0		(BCM2835_INT_GPU1BASE + 17)
#define	BCM2835_INT_GPIO1		(BCM2835_INT_GPU1BASE + 18)
#define	BCM2835_INT_GPIO2		(BCM2835_INT_GPU1BASE + 19)
#define	BCM2835_INT_GPIO3		(BCM2835_INT_GPU1BASE + 20)
#define	BCM2835_INT_BSC			(BCM2835_INT_GPU1BASE + 21)
#define	BCM2835_INT_SPI0		(BCM2835_INT_GPU1BASE + 22)
#define	BCM2835_INT_PCM			(BCM2835_INT_GPU1BASE + 23)
#define	BCM2835_INT_UART0		(BCM2835_INT_GPU1BASE + 25)
#define	BCM2835_INT_EMMC		(BCM2835_INT_GPU1BASE + 30)

#define	BCM2835_INT_BASICBASE		(BCM2835_INT_BASE + 64)
#define	BCM2835_INT_ARMTIMER		(BCM2835_INT_BASICBASE + 0)
#define	BCM2835_INT_ARMMAILBOX		(BCM2835_INT_BASICBASE + 1)
#define	BCM2835_INT_ARMDOORBELL0	(BCM2835_INT_BASICBASE + 2)
#define	BCM2835_INT_ARMDOORBELL1	(BCM2835_INT_BASICBASE + 3)
#define	BCM2835_INT_GPU0HALTED		(BCM2835_INT_BASICBASE + 4)
#define	BCM2835_INT_GPU1HALTED		(BCM2835_INT_BASICBASE + 5)
#define	BCM2835_INT_ILLEGALTYPE0	(BCM2835_INT_BASICBASE + 6)
#define	BCM2835_INT_ILLEGALTYPE1	(BCM2835_INT_BASICBASE + 7)


#define	BCM2835_UART0_CLK		3000000

#define	BCM2836_ARM_LOCAL_VBASE \
	BCM2835_IOPHYSTOVIRT(BCM2836_ARM_LOCAL_BASE)
#define	BCM2836_ARM_LOCAL_BASE		0x40000000
#define	BCM2836_ARM_LOCAL_SIZE		0x00001000	/* 4KBytes */

#define	BCM2836_LOCAL_CONTROL		0x000
#define	BCM2836_LOCAL_PRESCALER		0x008
#define	BCM2836_LOCAL_GPU_INT_ROUTING	0x00c
#define	BCM2836_LOCAL_PM_ROUTING_SET	0x010
#define	BCM2836_LOCAL_PM_ROUTING_CLR	0x014
#define	BCM2836_LOCAL_TIMER_LS		0x01c
#define	BCM2836_LOCAL_TIMER_MS		0x020
#define	BCM2836_LOCAL_INT_ROUTING	0x024
#define	BCM2836_LOCAL_AXI_COUNT		0x02c
#define	BCM2836_LOCAL_AXI_IRQ		0x030
#define	BCM2836_LOCAL_TIMER_CONTROL	0x034
#define	BCM2836_LOCAL_TIMER_WRITE	0x038


#define	BCM2836_LOCAL_TIMER_IRQ_CONTROL_BASE	0x40
#define	BCM2836_LOCAL_MAILBOX_IRQ_CONTROL_BASE	0x50
#define	BCM2836_LOCAL_INTC_IRQPENDING_BASE	0x60
#define	BCM2836_LOCAL_INTC_FIQPENDING_BASE	0x70

#define	BCM2836_LOCAL_TIMER_IRQ_CONTROL_SIZE	0x10
#define	BCM2836_LOCAL_MAILBOX_IRQ_CONTROL_SIZE	0x10

#define	BCM2836_LOCAL_TIMER_IRQ_CONTROLN(n)	(BCM2836_LOCAL_TIMER_IRQ_CONTROL_BASE + 4*(n))
#define	BCM2836_LOCAL_MAILBOX_IRQ_CONTROLN(n)	(BCM2836_LOCAL_MAILBOX_IRQ_CONTROL_BASE + 4*(n))
#define	BCM2836_LOCAL_INTC_IRQPENDINGN(n)	(BCM2836_LOCAL_INTC_IRQPENDING_BASE + 4*(n))
#define	BCM2836_LOCAL_INTC_FIQPENDINGN(n)	(BCM2836_LOCAL_INTC_FIQPENDING_BASE + 4*(n))

#define	BCM2836_LOCAL_MAILBOX0_SETN(n)		(0x80 + 0x10 * (n))
#define	BCM2836_LOCAL_MAILBOX1_SETN(n)		(0x84 + 0x10 * (n))
#define	BCM2836_LOCAL_MAILBOX2_SETN(n)		(0x88 + 0x10 * (n))
#define	BCM2836_LOCAL_MAILBOX3_SETN(n)		(0x8c + 0x10 * (n))
#define	BCM2836_LOCAL_MAILBOX0_CLRN(n)		(0xc0 + 0x10 * (n))
#define	BCM2836_LOCAL_MAILBOX1_CLRN(n)		(0xc4 + 0x10 * (n))
#define	BCM2836_LOCAL_MAILBOX2_CLRN(n)		(0xc8 + 0x10 * (n))
#define	BCM2836_LOCAL_MAILBOX3_CLRN(n)		(0xcc + 0x10 * (n))

#endif /* _BCM2835REG_H_ */
