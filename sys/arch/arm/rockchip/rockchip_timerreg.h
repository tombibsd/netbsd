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

#ifndef _ROCKCHIP_TIMERREG_H
#define _ROCKCHIP_TIMERREG_H

#define TIMER0_LOAD_COUNT0_REG		0x0000
#define TIMER0_LOAD_COUNT1_REG		0x0004
#define TIMER0_CURRENT_VALUE0_REG	0x0008
#define TIMER0_CURRENT_VALUE1_REG	0x000c
#define TIMER0_CONTROL_REG		0x0010
#define TIMER0_INTSTATUS_REG		0x0018

#define TIMER1_LOAD_COUNT0_REG		0x0020
#define TIMER1_LOAD_COUNT1_REG		0x0024
#define TIMER1_CURRENT_VALUE0_REG	0x0028
#define TIMER1_CURRENT_VALUE1_REG	0x002c
#define TIMER1_CONTROL_REG		0x0030
#define TIMER1_INTSTATUS_REG		0x0038

#define TIMER2_LOAD_COUNT0_REG		0x0040
#define TIMER2_LOAD_COUNT1_REG		0x0044
#define TIMER2_CURRENT_VALUE0_REG	0x0048
#define TIMER2_CURRENT_VALUE1_REG	0x004c
#define TIMER2_CONTROL_REG		0x0050
#define TIMER2_INTSTATUS_REG		0x0058

#define TIMER3_LOAD_COUNT0_REG		0x0060
#define TIMER3_LOAD_COUNT1_REG		0x0064
#define TIMER3_CURRENT_VALUE0_REG	0x0068
#define TIMER3_CURRENT_VALUE1_REG	0x006c
#define TIMER3_CONTROL_REG		0x0070
#define TIMER3_INTSTATUS_REG		0x0078

#define TIMER4_LOAD_COUNT0_REG		0x0080
#define TIMER4_LOAD_COUNT1_REG		0x0084
#define TIMER4_CURRENT_VALUE0_REG	0x0088
#define TIMER4_CURRENT_VALUE1_REG	0x008c
#define TIMER4_CONTROL_REG		0x0090
#define TIMER4_INTSTATUS_REG		0x0098

#define TIMER5_LOAD_COUNT0_REG		0x00a0
#define TIMER5_LOAD_COUNT1_REG		0x00a4
#define TIMER5_CURRENT_VALUE0_REG	0x00a8
#define TIMER5_CURRENT_VALUE1_REG	0x00ac
#define TIMER5_CONTROL_REG		0x00b0
#define TIMER5_INTSTATUS_REG		0x00b8

#define TIMER_CONTROL_INTERRUPT_MASK	__BIT(2)
#define TIMER_CONTROL_MODE		__BIT(1)
#define TIMER_CONTROL_ENABLE		__BIT(0)

#endif /* !_ROCKCHIP_TIMERREG_H */
