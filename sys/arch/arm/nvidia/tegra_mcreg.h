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

#ifndef _ARM_TEGRA_MCREG_H
#define _ARM_TEGRA_MCREG_H

#define MC_INTSTATUS_REG	0x00
#define MC_INTMASK_REG		0x04

#define MC_INT_DECERR_MTS		__BIT(16)
#define MC_INT_SECERR_SEC		__BIT(13)
#define MC_INT_DECERR_VPR		__BIT(12)
#define MC_INT_INVALID_APB_ASID_UPDATE	__BIT(11)
#define MC_INT_INVALID_SMMU_PAGE	__BIT(10)
#define MC_INT_SECURITY_VIOLATION	__BIT(8)
#define MC_INT_ARBITRATION_EMEM		__BIT(9)
#define MC_INT_DECERR_EMEM		__BIT(6)
#define MC_INT__ALL			(MC_INT_DECERR_MTS | \
    MC_INT_SECERR_SEC | MC_INT_DECERR_VPR | \
    MC_INT_INVALID_APB_ASID_UPDATE | MC_INT_INVALID_SMMU_PAGE | \
    MC_INT_SECURITY_VIOLATION | MC_INT_ARBITRATION_EMEM | \
    MC_INT_DECERR_EMEM)

#define MC_ERR_STATUS_REG	0x08

#define MC_ERR_TYPE			__BITS(30,28)
#define MC_ERR_INVALID_SMMU_PAGE_READABLE	__BIT(27)
#define MC_ERR_INVALID_SMMU_PAGE_WRITABLE	__BIT(26)
#define MC_ERR_INVALID_SMMU_PAGE_NONSECURE	__BIT(25)
#define MC_ERR_ADDR_HI			__BITS(21,20)
#define MC_ERR_SWAP			__BIT(18)
#define MC_ERR_SECURITY			__BIT(17)
#define MC_ERR_RW			__BIT(16)
#define MC_ERR_RW_READ			0
#define MC_ERR_RW_WRITE			1
#define MC_ERR_ADR1			__BITS(14,12)
#define MC_ERR_ID			__BITS(6,0)

#define MC_ERR_ADR_REG		0x0c

#define MC_SMMU_CONFIG_REG	0x10
#define MC_SMMU_ENABLE			__BIT(0)

#define MC_SMMU_TLB_CONFIG_REG	0x14
#define MC_SMMU_PTC_CONFIG_REG	0x18
#define MC_SMMU_PTB_ASID_REG	0x1c
#define MC_SMMU_PTB_DATA_REG	0x20
#define MC_SMMU_TLB_FLUSH_REG	0x30
#define MC_SMMU_PTC_FLUSH_REG	0x34

#define MC_EMEM_CFG_0_REG	0x50

#define MC_EMEM_CFG_0_EMEM_BOM		__BIT(31)
#define MC_EMEM_CFG_0_EMEM_SIZE_MB	__BITS(13,0)

#endif /* _ARM_TEGRA_MCREG_H */
