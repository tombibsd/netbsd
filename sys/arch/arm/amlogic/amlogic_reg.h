/* $NetBSD$ */

/*-
 * Copyright (c) 2015 Jared D. McNeill <jmcneill@invisible.ca>
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

#ifndef _ARM_AMLOGIC_REG_H
#define _ARM_AMLOGIC_REG_H

#define CONSADDR_VA	(CONSADDR - AMLOGIC_CORE_BASE + AMLOGIC_CORE_VBASE)

#define AMLOGIC_CORE_BASE	0xc0000000
#define AMLOGIC_CORE_SIZE	0x1b000000
#define AMLOGIC_CORE_VBASE	0xe0000000

#define AMLOGIC_REF_FREQ	24000000

#define AMLOGIC_CBUS_OFFSET	0x01100000

#define AMLOGIC_RAND_OFFSET	0x01108100
#define AMLOGIC_RAND_SIZE	0x8

#define AMLOGIC_SDIO_OFFSET	0x01108c20
#define AMLOGIC_SDIO_SIZE	0x20

#define AMLOGIC_SDHC_OFFSET	0x01108e00
#define AMLOGIC_SDHC_SIZE	0x30

#define AMLOGIC_PL310_OFFSET	0x04200000

#define AMLOGIC_DMC_OFFSET	0x08006000
#define AMLOGIC_DMC_SIZE	0x400

#define AMLOGIC_AOBUS_OFFSET	0x08100000

#define AMLOGIC_AO_RTI_PIN_MUX_OFFSET 0x08100014
#define AMLOGIC_GPIOAO_OFFSET	0x08100024

#define AMLOGIC_GPIOAO_EN_N_REG		0x00	/* [15:0] */
#define AMLOGIC_GPIOAO_OUT_REG		0x00	/* [31:16] */
#define AMLOGIC_GPIOAO_IN_REG		0x04
#define AMLOGIC_GPIOAO_PUPD_EN_REG	0x08	/* [15:0] */
#define AMLOGIC_GPIOAO_PUPD_REG		0x08	/* [31:16] */

#define AMLOGIC_UART0AO_OFFSET	0x081004c0
#define AMLOGIC_UART2AO_OFFSET	0x081004e0
#define AMLOGIC_UART_SIZE	0x20
#define AMLOGIC_UART_FREQ	AMLOGIC_REF_FREQ

#define AMLOGIC_RTC_OFFSET	0x08100740
#define AMLOGIC_RTC_SIZE	0x14

#define AMLOGIC_USB0_OFFSET	0x09040000
#define AMLOGIC_USB1_OFFSET	0x090c0000
#define AMLOGIC_USB_SIZE	0x40000

#define AMLOGIC_GMAC_OFFSET	0x09410000
#define AMLOGIC_GMAC_SIZE	0x10000

#define AMLOGIC_HDMI_OFFSET	0x10040000
#define AMLOGIC_HDMI_SIZE	0x10000	/* ? */

#define AMLOGIC_VPU_OFFSET	0x10100000
#define AMLOGIC_VPU_SIZE	0x100000 /* ? */

#define AMLOGIC_SRAM_OFFSET	0x19000000

#define AMLOGIC_BOOTINFO_OFFSET	0x1901ff00
#define AMLOGIC_CPUCONF_OFFSET	0x1901ff80

#define AMLOGIC_CBUS_CPU_CLK_CNTL_REG	0x419c

#define AMLOGIC_AOBUS_PWR_CTRL0_REG	0xe0
#define AMLOGIC_AOBUS_PWR_CTRL1_REG	0xe4
#define AMLOGIC_AOBUS_PWR_MEM_PD0_REG	0xf4

#define AMLOGIC_CPUCONF_CTRL_REG	0x00
#define AMLOGIC_CPUCONF_CPU_ADDR_REG(n)	(0x04 * (n))

#endif /* _ARM_AMLOGIC_REG_H */
