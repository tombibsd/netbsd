/*	$NetBSD$	*/

/*
 * Copyright (c) 2007 Internet Initiative Japan, Inc.
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
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * IPD Registers
 */

#ifndef _OCTEON_IPDREG_H_
#define _OCTEON_IPDREG_H_

#define	IPD_1ST_MBUFF_SKIP		0x00014f0000000000ULL
#define	IPD_NOT_1ST_MBUFF_SKIP		0x00014f0000000008ULL
#define	IPD_PACKET_MBUFF_SIZE		0x00014f0000000010ULL
#define	IPD_CTL_STATUS			0x00014f0000000018ULL
#define	IPD_WQE_FPA_QUEUE		0x00014f0000000020ULL
#define	IPD_PORT0_BP_PAGE_CNT		0x00014f0000000028ULL
#define	IPD_PORT1_BP_PAGE_CNT		0x00014f0000000030ULL
#define	IPD_PORT2_BP_PAGE_CNT		0x00014f0000000038ULL
#define	IPD_PORT32_BP_PAGE_CNT		0x00014f0000000128ULL
#define	IPD_SUB_PORT_BP_PAGE_CNT	0x00014f0000000148ULL
#define	IPD_1ST_NEXT_PTR_BACK		0x00014f0000000150ULL
#define	IPD_2ND_NEXT_PTR_BACK		0x00014f0000000158ULL
#define	IPD_INT_ENB			0x00014f0000000160ULL
#define	IPD_INT_SUM			0x00014f0000000168ULL
#define	IPD_SUB_PORT_FCS		0x00014f0000000170ULL
#define	IPD_QOS0_RED_MARKS		0x00014f0000000178ULL
#define	IPD_QOS1_RED_MARKS		0x00014f0000000180ULL
#define	IPD_QOS2_RED_MARKS		0x00014f0000000188ULL
#define	IPD_QOS3_RED_MARKS		0x00014f0000000190ULL
#define	IPD_QOS4_RED_MARKS		0x00014f0000000198ULL
#define	IPD_QOS5_RED_MARKS		0x00014f00000001a0ULL
#define	IPD_QOS6_RED_MARKS		0x00014f00000001a8ULL
#define	IPD_QOS7_RED_MARKS		0x00014f00000001b0ULL
#define	IPD_PORT_BP_COUNTERS_PAIR0	0x00014f00000001b8ULL
#define	IPD_PORT_BP_COUNTERS_PAIR1	0x00014f00000001c0ULL
#define	IPD_PORT_BP_COUNTERS_PAIR2	0x00014f00000001c8ULL
#define	IPD_PORT_BP_COUNTERS_PAIR32	0x00014f00000002b8ULL
#define	IPD_RED_PORT_ENABLE		0x00014f00000002d8ULL
#define	IPD_RED_QUE0_PARAM		0x00014f00000002e0ULL
#define	IPD_RED_QUE1_PARAM		0x00014f00000002e8ULL
#define	IPD_RED_QUE2_PARAM		0x00014f00000002f0ULL
#define	IPD_RED_QUE3_PARAM		0x00014f00000002f8ULL
#define	IPD_RED_QUE4_PARAM		0x00014f0000000300ULL
#define	IPD_RED_QUE5_PARAM		0x00014f0000000308ULL
#define	IPD_RED_QUE6_PARAM		0x00014f0000000310ULL
#define	IPD_RED_QUE7_PARAM		0x00014f0000000318ULL
#define	IPD_PTR_COUNT			0x00014f0000000320ULL
#define	IPD_BP_PRT_RED_END		0x00014f0000000328ULL
#define	IPD_QUE0_FREE_PAGE_CNT		0x00014f0000000330ULL
#define	IPD_CLK_COUNT			0x00014f0000000338ULL
#define	IPD_PWP_PTR_FIFO_CTL		0x00014f0000000340ULL
#define	IPD_PRC_HOLD_PTR_FIFO_CTL	0x00014f0000000348ULL
#define	IPD_PRC_PORT_PTR_FIFO_CTL	0x00014f0000000350ULL
#define	IPD_PKT_PTR_VALID		0x00014f0000000358ULL
#define	IPD_WQE_PTR_VALID		0x00014f0000000360ULL
#define	IPD_BIST_STATUS			0x00014f00000007f8ULL

#define	IPD_BASE			0x00014f0000000000ULL
#define	IPD_SIZE			0x800ULL

#define	IPD_1ST_MBUFF_SKIP_OFFSET		0x0ULL
#define	IPD_NOT_1ST_MBUFF_SKIP_OFFSET		0x8ULL
#define	IPD_PACKET_MBUFF_SIZE_OFFSET		0x10ULL
#define	IPD_CTL_STATUS_OFFSET			0x18ULL
#define	IPD_WQE_FPA_QUEUE_OFFSET		0x20ULL
#define	IPD_PORT0_BP_PAGE_CNT_OFFSET		0x28ULL
#define	IPD_PORT1_BP_PAGE_CNT_OFFSET		0x30ULL
#define	IPD_PORT2_BP_PAGE_CNT_OFFSET		0x38ULL
#define	IPD_PORT32_BP_PAGE_CNT_OFFSET		0x128ULL
#define	IPD_SUB_PORT_BP_PAGE_CNT_OFFSET		0x148ULL
#define	IPD_1ST_NEXT_PTR_BACK_OFFSET		0x150ULL
#define	IPD_2ND_NEXT_PTR_BACK_OFFSET		0x158ULL
#define	IPD_INT_ENB_OFFSET			0x160ULL
#define	IPD_INT_SUM_OFFSET			0x168ULL
#define	IPD_SUB_PORT_FCS_OFFSET			0x170ULL
#define	IPD_QOS0_RED_MARKS_OFFSET		0x178ULL
#define	IPD_QOS1_RED_MARKS_OFFSET		0x180ULL
#define	IPD_QOS2_RED_MARKS_OFFSET		0x188ULL
#define	IPD_QOS3_RED_MARKS_OFFSET		0x190ULL
#define	IPD_QOS4_RED_MARKS_OFFSET		0x198ULL
#define	IPD_QOS5_RED_MARKS_OFFSET		0x1a0ULL
#define	IPD_QOS6_RED_MARKS_OFFSET		0x1a8ULL
#define	IPD_QOS7_RED_MARKS_OFFSET		0x1b0ULL
#define	IPD_PORT_BP_COUNTERS_PAIR0_OFFSET	0x1b8ULL
#define	IPD_PORT_BP_COUNTERS_PAIR1_OFFSET	0x1c0ULL
#define	IPD_PORT_BP_COUNTERS_PAIR2_OFFSET	0x1c8ULL
#define	IPD_PORT_BP_COUNTERS_PAIR32_OFFSET	0x2b8ULL
#define	IPD_RED_PORT_ENABLE_OFFSET		0x2d8ULL
#define	IPD_RED_QUE0_PARAM_OFFSET		0x2e0ULL
#define	IPD_RED_QUE1_PARAM_OFFSET		0x2e8ULL
#define	IPD_RED_QUE2_PARAM_OFFSET		0x2f0ULL
#define	IPD_RED_QUE3_PARAM_OFFSET		0x2f8ULL
#define	IPD_RED_QUE4_PARAM_OFFSET		0x300ULL
#define	IPD_RED_QUE5_PARAM_OFFSET		0x308ULL
#define	IPD_RED_QUE6_PARAM_OFFSET		0x310ULL
#define	IPD_RED_QUE7_PARAM_OFFSET		0x318ULL
#define	IPD_PTR_COUNT_OFFSET			0x320ULL
#define	IPD_BP_PRT_RED_END_OFFSET		0x328ULL
#define	IPD_QUE0_FREE_PAGE_CNT_OFFSET		0x330ULL
#define	IPD_CLK_COUNT_OFFSET			0x338ULL
#define	IPD_PWP_PTR_FIFO_CTL_OFFSET		0x340ULL
#define	IPD_PRC_HOLD_PTR_FIFO_CTL_OFFSET	0x348ULL
#define	IPD_PRC_PORT_PTR_FIFO_CTL_OFFSET	0x350ULL
#define	IPD_PKT_PTR_VALID_OFFSET		0x358ULL
#define	IPD_WQE_PTR_VALID_OFFSET		0x360ULL
#define	IPD_BIST_STATUS_OFFSET			0x7f8ULL

/* ----- */
/*
 * Work Queue Entry Format (for input packet)
 */

/* 
 * word 2
 * Work-Queue Entry format; Word2 Cases
 */
/* RAWFULL */
#define IPD_WQE_WORD2_RAW_BUFS		UINT64_C(0xff00000000000000)
#define IPD_WQE_WORD2_RAW_WORD		UINT64_C(0x00ffffffffffffff)

/* is IP */
#define IPD_WQE_WORD2_IP_BUFS		UINT64_C(0xff00000000000000)
#define IPD_WQE_WORD2_IP_IPOFF		UINT64_C(0x00ff000000000000)
#define IPD_WQE_WORD2_IP_VV		UINT64_C(0x0000800000000000)
#define IPD_WQE_WORD2_IP_VS		UINT64_C(0x0000400000000000)
#define IPD_WQE_WORD2_IP_45		UINT64_C(0x0000200000000000)
#define IPD_WQE_WORD2_IP_VC		UINT64_C(0x0000100000000000)
#define IPD_WQE_WORD2_IP_VLANID		UINT64_C(0x00000fff00000000)
#define IPD_WQE_WORD2_IP_31_20		UINT64_C(0x00000000fff00000)
#define IPD_WQE_WORD2_IP_CO		UINT64_C(0x0000000000080000)
#define IPD_WQE_WORD2_IP_TU		UINT64_C(0x0000000000040000)
#define IPD_WQE_WORD2_IP_SE		UINT64_C(0x0000000000020000)
#define IPD_WQE_WORD2_IP_V6		UINT64_C(0x0000000000010000)
#define IPD_WQE_WORD2_IP_15		UINT64_C(0x0000000000008000)
#define IPD_WQE_WORD2_IP_LE		UINT64_C(0x0000000000004000)
#define IPD_WQE_WORD2_IP_FR		UINT64_C(0x0000000000002000)
#define IPD_WQE_WORD2_IP_IE		UINT64_C(0x0000000000001000)
#define IPD_WQE_WORD2_IP_B		UINT64_C(0x0000000000000800)
#define IPD_WQE_WORD2_IP_M		UINT64_C(0x0000000000000400)
#define IPD_WQE_WORD2_IP_NI		UINT64_C(0x0000000000000200)
#define IPD_WQE_WORD2_IP_RE		UINT64_C(0x0000000000000100)
#define IPD_WQE_WORD2_IP_OPCODE		UINT64_C(0x00000000000000ff)

/* All other */
#define IPD_WQE_WORD2_OTH_BUFS		UINT64_C(0xff00000000000000)
#define IPD_WQE_WORD2_OTH_55_48		UINT64_C(0x00ff000000000000)
#define IPD_WQE_WORD2_OTH_VV		UINT64_C(0x0000800000000000)
#define IPD_WQE_WORD2_OTH_VS		UINT64_C(0x0000400000000000)
#define IPD_WQE_WORD2_OTH_45		UINT64_C(0x0000200000000000)
#define IPD_WQE_WORD2_OTH_VC		UINT64_C(0x0000100000000000)
#define IPD_WQE_WORD2_OTH_VLANID	UINT64_C(0x00000fff00000000)
#define IPD_WQE_WORD2_OTH_31_14		UINT64_C(0x00000000ffffc000)
#define IPD_WQE_WORD2_OTH_IR		UINT64_C(0x0000000000002000)
#define IPD_WQE_WORD2_OTH_IA		UINT64_C(0x0000000000001000)
#define IPD_WQE_WORD2_OTH_B		UINT64_C(0x0000000000000800)
#define IPD_WQE_WORD2_OTH_M		UINT64_C(0x0000000000000400)
#define IPD_WQE_WORD2_OTH_NI		UINT64_C(0x0000000000000200)
#define IPD_WQE_WORD2_OTH_RE		UINT64_C(0x0000000000000100)
#define IPD_WQE_WORD2_OTH_OPCODE	UINT64_C(0x00000000000000ff)

/*
 * word 3
 */
#define IPD_WQE_WORD3_63		UINT64_C(0x8000000000000000)
#define IPD_WQE_WORD3_BACK		UINT64_C(0x7800000000000000)
#define IPD_WQE_WORD3_58_56		UINT64_C(0x0700000000000000)
#define IPD_WQE_WORD3_SIZE		UINT64_C(0x00ffff0000000000)
#define IPD_WQE_WORD3_ADDR		UINT64_C(0x000000ffffffffff)

/*
 * IPD_1ST_MBUFF_SKIP 
 */
#define IPD_1ST_MBUFF_SKIP_63_6		UINT64_C(0xffffffffffffffc0)
#define IPD_1ST_MBUFF_SKIP_SZ		UINT64_C(0x000000000000003f)

/*
 * IPD_NOT_1ST_MBUFF_SKIP 
 */
#define IPD_NOT_1ST_MBUFF_SKIP_63_6	UINT64_C(0xffffffffffffffc0)
#define IPD_NOT_1ST_MBUFF_SKIP_SZ	UINT64_C(0x000000000000003f)

/*
 * IPD_PACKET_MBUFF_SIZE 
 */
#define IPD_PACKET_MBUFF_SIZE_63_12	UINT64_C(0xfffffffffffff000)
#define IPD_PACKET_MBUFF_SIZE_MB_SIZE	UINT64_C(0x0000000000000fff)

/*
 * IPD_CTL_STATUS 
 */
#define IPD_CTL_STATUS_63_10		UINT64_C(0xfffffffffffffc00)
#define IPD_CTL_STATUS_LEN_M8		UINT64_C(0x0000000000000200)
#define IPD_CTL_STATUS_RESET		UINT64_C(0x0000000000000100)
#define IPD_CTL_STATUS_ADDPKT		UINT64_C(0x0000000000000080)
#define IPD_CTL_STATUS_NADDBUF		UINT64_C(0x0000000000000040)
#define IPD_CTL_STATUS_PKT_LEND		UINT64_C(0x0000000000000020)
#define IPD_CTL_STATUS_WQE_LEND		UINT64_C(0x0000000000000010)
#define IPD_CTL_STATUS_PBP_EN		UINT64_C(0x0000000000000008)
#define IPD_CTL_STATUS_OPC_MODE		UINT64_C(0x0000000000000006)
#define  IPD_CTL_STATUS_OPC_MODE_SHIFT	1
#define   IPD_CTL_STATUS_OPC_MODE_NONE	(0ULL << IPD_CTL_STATUS_OPC_MODE_SHIFT)
#define   IPD_CTL_STATUS_OPC_MODE_ALL	(1ULL << IPD_CTL_STATUS_OPC_MODE_SHIFT)
#define   IPD_CTL_STATUS_OPC_MODE_ONE	(2ULL << IPD_CTL_STATUS_OPC_MODE_SHIFT)
#define   IPD_CTL_STATUS_OPC_MODE_TWO	(3ULL << IPD_CTL_STATUS_OPC_MODE_SHIFT)
#define IPD_CTL_STATUS_IPD_EN		UINT64_C(0x0000000000000001)

/*
 * IPD_WQE_FPA_QUEUE
 */
#define IPD_WQE_FPA_QUEUE_63_3		UINT64_C(0xfffffffffffffff8)
#define IPD_WQE_FPA_QUEUE_WQE_QUE	UINT64_C(0x0000000000000007)

/*
 * IPD_PORTN_BP_PAGE_CNT
 */
#define IPD_PORTN_BP_PAGE_CNT_63_18	UINT64_C(0xfffffffffffc0000)
#define IPD_PORTN_BP_PAGE_CNT_BP_ENB	UINT64_C(0x0000000000020000)
#define IPD_PORTN_BP_PAGE_CNT_PAGE_CNT	UINT64_C(0x000000000001ffff)

/*
 * IPD_SUB_PORT_BP_PAGE_CNT
 */
#define IPD_SUB_PORT_BP_PAGE_CNT_63_18		UINT64_C(0xffffffff80000000)
#define IPD_SUB_PORT_BP_PAGE_CNT_PORT		UINT64_C(0x000000007e000000)
#define IPD_SUB_PORT_BP_PAGE_CNT_PAGE_CNT	UINT64_C(0x0000000001ffffff)

/*
 * IPD_1ST_NEXT_PTR_BACK
 */
#define IPD_1ST_NEXT_PTR_BACK_63_4		UINT64_C(0xfffffffffffffff0)
#define IPD_1ST_NEXT_PTR_BACK_BACK		UINT64_C(0x000000000000000f)

/*
 * IPD_2ND_NEXT_PTR_BACK
 */
#define IPD_2ND_NEXT_PTR_BACK_63_4		UINT64_C(0xfffffffffffffff0)
#define IPD_2ND_NEXT_PTR_BACK_BACK		UINT64_C(0x000000000000000f)

/*
 * IPD_INT_ENB
 */
#define IPD_INT_ENB_63_4		UINT64_C(0xffffffffffffffe0)
#define IPD_INT_ENB_BP_SUB		UINT64_C(0x0000000000000010)
#define IPD_INT_ENB_PRC_PAR3		UINT64_C(0x0000000000000008)
#define IPD_INT_ENB_PRC_PAR2		UINT64_C(0x0000000000000004)
#define IPD_INT_ENB_PRC_PAR1		UINT64_C(0x0000000000000002)
#define IPD_INT_ENB_PRC_PAR0		UINT64_C(0x0000000000000001)

/*
 * IPD_INT_SUM
 */
#define IPD_INT_SUM_63_4		UINT64_C(0xffffffffffffffe0)
#define IPD_INT_SUM_BP_SUB		UINT64_C(0x0000000000000010)
#define IPD_INT_SUM_PRC_PAR3		UINT64_C(0x0000000000000008)
#define IPD_INT_SUM_PRC_PAR2		UINT64_C(0x0000000000000004)
#define IPD_INT_SUM_PRC_PAR1		UINT64_C(0x0000000000000002)
#define IPD_INT_SUM_PRC_PAR0		UINT64_C(0x0000000000000001)

/*
 * IPD_SUB_PORT_FCS
 */
#define IPD_SUB_PORT_FCS_63_3		UINT64_C(0xfffffffffffffff8)
#define IPD_SUB_PORT_FCS_PORT_BIT	UINT64_C(0x0000000000000007)

/*
 * IPD_QOSN_RED_MARKS
 */
#define IPD_QOSN_READ_MARKS_DROP	UINT64_C(0xffffffff00000000)
#define IPD_QOSN_READ_MARKS_PASS	UINT64_C(0x00000000ffffffff)

/*
 * IPD_PORT_BP_COUNTERS_PAIRN
 */
#define IPD_PORT_BP_COUNTERS_PAIRN_63_25	UINT64_C(0xfffffffffe000000)
#define IPD_PORT_BP_COUNTERS_PAIRN_CNT_VAL	UINT64_C(0x0000000001ffffff)

/*
 * IPD_RED_PORT_ENABLE
 */
#define IPD_RED_PORT_ENABLE_PRB_DLY	UINT64_C(0xfffc000000000000)
#define IPD_RED_PORT_ENABLE_AVG_DLY	UINT64_C(0x0003fff000000000)
#define IPD_RED_PORT_ENABLE_PRT_ENB	UINT64_C(0x0000000fffffffff)

/*
 * IPD_RED_QUEN_PARAM
 */
#define IPD_RED_QUEN_PARAM_63_49	UINT64_C(0xfffe000000000000)
#define IPD_RED_QUEN_PARAM_USE_PCNT	UINT64_C(0x0001000000000000)
#define IPD_RED_QUEN_PARAM_NEW_CON	UINT64_C(0x0000ff0000000000)
#define IPD_RED_QUEN_PARAM_AVG_CON	UINT64_C(0x000000ff00000000)
#define IPD_RED_QUEN_PARAM_PRB_CON	UINT64_C(0x00000000ffffffff)

/*
 * IPD_PTR_COUNT
 */
#define IPD_PTR_COUNT_63_19		UINT64_C(0xfffffffffff80000)
#define IPD_PTR_COUNT_PKTV_CNT		UINT64_C(0x0000000000040000)
#define IPD_PTR_COUNT_WQEV_CNT		UINT64_C(0x0000000000020000)
#define IPD_PTR_COUNT_PFIF_CNT		UINT64_C(0x000000000001c000)
#define IPD_PTR_COUNT_PKT_PCNT		UINT64_C(0x0000000000003f80)
#define IPD_PTR_COUNT_WQE_PCNT		UINT64_C(0x000000000000007f)

/*
 * IPD_BP_PRT_RED_END
 */
#define IPD_BP_PRT_RED_END_63_36	UINT64_C(0xfffffff000000000)
#define IPD_BP_PRT_RED_END_PRT_ENB	UINT64_C(0x0000000fffffffff)

/*
 * IPD_QUE0_FREE_PAGE_CNT
 */
#define IPD_QUE0_FREE_PAGE_CNT_63_32	UINT64_C(0xffffffff00000000)
#define IPD_QUE0_FREE_PAGE_CNT_Q0_PCNT	UINT64_C(0x00000000ffffffff)

/*
 * IPD_CLK_COUNT
 */
#define IPD_CLK_COUNT_CLK_CNT		UINT64_C(0xffffffffffffffff)

/*
 * IPD_PWP_PTR_FIFO_CTL
 */
#define IPD_PWP_PTR_FIFO_CTL_63_61	UINT64_C(0xe000000000000000)
#define IPD_PWP_PTR_FIFO_CTL_MAX_CNTS	UINT64_C(0x1fc0000000000000)
#define IPD_PWP_PTR_FIFO_CTL_WRADDR	UINT64_C(0x003fc00000000000)
#define IPD_PWP_PTR_FIFO_CTL_PRADDR	UINT64_C(0x00003fc000000000)
#define IPD_PWP_PTR_FIFO_CTL_PTR	UINT64_C(0x0000003ffffffe00)
#define IPD_PWP_PTR_FIFO_CTL_CENA	UINT64_C(0x0000000000000100)
#define IPD_PWP_PTR_FIFO_CTL_RADDR	UINT64_C(0x00000000000000ff)

/*
 * IPD_PRC_HOLD_PTR_FIFO_CTL
 */
#define IPD_PRC_HOLD_PTR_FIFO_CTL_63_39		UINT64_C(0xffffff8000000000)
#define IPD_PRC_HOLD_PTR_FIFO_CTL_MAX_PTR	UINT64_C(0x0000007000000000)
#define IPD_PRC_HOLD_PTR_FIFO_CTL_PRADDR	UINT64_C(0x0000000e00000000)
#define IPD_PRC_HOLD_PTR_FIFO_CTL_PTR		UINT64_C(0x00000001fffffff0)
#define IPD_PRC_HOLD_PTR_FIFO_CTL_CENA		UINT64_C(0x0000000000000008)
#define IPD_PRC_HOLD_PTR_FIFO_CTL_RADDR		UINT64_C(0x0000000000000007)

/*
 * IPD_PRC_PORT_PTR_FIFO_CTL
 */
#define IPD_PRC_PORT_PTR_FIFO_CTL_63_44		UINT64_C(0xfffff00000000000)
#define IPD_PRC_PORT_PTR_FIFO_CTL_MAX_PTR	UINT64_C(0x00000fe000000000)
#define IPD_PRC_PORT_PTR_FIFO_CTL_PTR		UINT64_C(0x0000001fffffff00)
#define IPD_PRC_PORT_PTR_FIFO_CTL_CENA		UINT64_C(0x0000000000000080)
#define IPD_PRC_PORT_PTR_FIFO_CTL_RADDR		UINT64_C(0x000000000000007f)

/*
 * IPD_PKT_PTR_VALID
 */
#define IPD_PKT_PTR_VALID_63_29	UINT64_C(0xffffffffe0000000)
#define IPD_PKT_PTR_VALID_PTR	UINT64_C(0x000000001fffffff)

/*
 * IPD_WQE_PTR_VALID
 */
#define IPD_WQE_PTR_VALID_63_29	UINT64_C(0xffffffffe0000000)
#define IPD_WQE_PTR_VALID_PTR	UINT64_C(0x000000001fffffff)

/*
 * IPD_BIST_STATUS
 */
#define IPD_BIST_STATUS_63_29		UINT64_C(0xffffffffffff0000)
#define IPD_BIST_STATUS_PWQ_WQED	UINT64_C(0x0000000000008000)
#define IPD_BIST_STATUS_PWQ_WP1		UINT64_C(0x0000000000004000)
#define IPD_BIST_STATUS_PWQ_POW		UINT64_C(0x0000000000002000)
#define IPD_BIST_STATUS_IPQ_PBE1	UINT64_C(0x0000000000001000)
#define IPD_BIST_STATUS_IPQ_PBE0	UINT64_C(0x0000000000000800)
#define IPD_BIST_STATUS_PBM3		UINT64_C(0x0000000000000400)
#define IPD_BIST_STATUS_PBM2		UINT64_C(0x0000000000000200)
#define IPD_BIST_STATUS_PBM1		UINT64_C(0x0000000000000100)
#define IPD_BIST_STATUS_PBM0		UINT64_C(0x0000000000000080)
#define IPD_BIST_STATUS_PBM_WORD	UINT64_C(0x0000000000000040)
#define IPD_BIST_STATUS_PWQ1		UINT64_C(0x0000000000000020)
#define IPD_BIST_STATUS_PWQ0		UINT64_C(0x0000000000000010)
#define IPD_BIST_STATUS_PRC_OFF		UINT64_C(0x0000000000000008)
#define IPD_BIST_STATUS_IPD_OLD		UINT64_C(0x0000000000000004)
#define IPD_BIST_STATUS_IPD_NEW		UINT64_C(0x0000000000000002)
#define IPD_BIST_STATUS_PWP		UINT64_C(0x0000000000000001)

/*
 * word2[Opcode]
 */
/* L3 (IP) error */
#define IPD_WQE_L3_NOT_IP		1
#define IPD_WQE_L3_V4_CSUM_ERR		2
#define IPD_WQE_L3_HEADER_MALFORMED	3
#define IPD_WQE_L3_MELFORMED		4
#define IPD_WQE_L3_TTL_HOP		5
#define IPD_WQE_L3_IP_OPT		6

/* L4 (UDP/TCP) error */
#define IPD_WQE_L4_MALFORMED		1
#define IPD_WQE_L4_CSUM_ERR		2
#define IPD_WQE_L4_UDP_LEN_ERR		3
#define IPD_WQE_L4_BAD_PORT		4
#define IPD_WQE_L4_FIN_ONLY		8
#define IPD_WQE_L4_NO_FLAGS		9
#define IPD_WQE_L4_FIN_RST		10
#define IPD_WQE_L4_SYN_URG		11
#define IPD_WQE_L4_SYN_RST		12
#define IPD_WQE_L4_SYN_FIN		13

#define	IPD_1ST_MBUFF_SKIP_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \
	"f\x06\x3a"	"63_6\0" \
	"f\x00\x06"	"SZ\0"
#define	IPD_NOT_1ST_MBUFF_SKIP_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \
	"f\x06\x3a"	"63_6\0" \
	"f\x00\x06"	"SZ\0"
#define	IPD_PACKET_MBUFF_SIZE_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \
	"f\x0c\x34"	"63_12\0" \
	"f\x00\x0c"	"MB_SIZE\0"
#define	IPD_CTL_STATUS_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \
	"f\x0a\x36"	"63_10\0" \
	"b\x09"		"LEN_M8\0" \
	"b\x08"		"RESET\0" \
	"b\x07"		"ADDPKT\0" \
	"b\x06"		"NADDBUF\0" \
	"b\x05"		"PKT_LEND\0" \
	"b\x04"		"WQE_LEND\0" \
	"b\x03"		"PBP_EN\0" \
	"f\x01\x02"	"OPC_MODE\0" \
	"b\x00"		"IPD_EN\0"
#define	IPD_WQE_FPA_QUEUE_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \
	"f\x03\x3d"	"63_3\0" \
	"f\x00\x03"	"WQE_QUE\0"
#define	IPD_PORT0_BP_PAGE_CNT_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \

#define	IPD_PORT1_BP_PAGE_CNT_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \

#define	IPD_PORT2_BP_PAGE_CNT_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \

#define	IPD_PORT32_BP_PAGE_CNT_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \

#define	IPD_SUB_PORT_BP_PAGE_CNT_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \
	"f\x1f\x21"	"63_18\0" \
	"f\x19\x06"	"PORT\0" \
	"f\x00\x19"	"PAGE_CNT\0"
#define	IPD_1ST_NEXT_PTR_BACK_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \
	"f\x04\x3c"	"63_4\0" \
	"f\x00\x04"	"BACK\0"
#define	IPD_2ND_NEXT_PTR_BACK_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \
	"f\x04\x3c"	"63_4\0" \
	"f\x00\x04"	"BACK\0"
#define	IPD_INT_ENB_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \
	"f\x05\x3b"	"63_4\0" \
	"b\x04"		"BP_SUB\0" \
	"b\x03"		"PRC_PAR3\0" \
	"b\x02"		"PRC_PAR2\0" \
	"b\x01"		"PRC_PAR1\0" \
	"b\x00"		"PRC_PAR0\0"
#define	IPD_INT_SUM_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \
	"f\x05\x3b"	"63_4\0" \
	"b\x04"		"BP_SUB\0" \
	"b\x03"		"PRC_PAR3\0" \
	"b\x02"		"PRC_PAR2\0" \
	"b\x01"		"PRC_PAR1\0" \
	"b\x00"		"PRC_PAR0\0"
#define	IPD_SUB_PORT_FCS_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \
	"f\x03\x3d"	"63_3\0" \
	"f\x00\x03"	"PORT_BIT\0"
#define	IPD_QOS0_RED_MARKS_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \

#define	IPD_QOS1_RED_MARKS_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \

#define	IPD_QOS2_RED_MARKS_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \

#define	IPD_QOS3_RED_MARKS_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \

#define	IPD_QOS4_RED_MARKS_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \

#define	IPD_QOS5_RED_MARKS_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \

#define	IPD_QOS6_RED_MARKS_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \

#define	IPD_QOS7_RED_MARKS_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \

#define	IPD_PORT_BP_COUNTERS_PAIR0_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \

#define	IPD_PORT_BP_COUNTERS_PAIR1_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \

#define	IPD_PORT_BP_COUNTERS_PAIR2_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \

#define	IPD_PORT_BP_COUNTERS_PAIR32_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \

#define	IPD_RED_PORT_ENABLE_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \
	"f\x32\x0e"	"PRB_DLY\0" \
	"f\x24\x0e"	"AVG_DLY\0" \
	"f\x00\x24"	"PRT_ENB\0"
#define	IPD_RED_QUE0_PARAM_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \

#define	IPD_RED_QUE1_PARAM_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \

#define	IPD_RED_QUE2_PARAM_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \

#define	IPD_RED_QUE3_PARAM_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \

#define	IPD_RED_QUE4_PARAM_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \

#define	IPD_RED_QUE5_PARAM_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \

#define	IPD_RED_QUE6_PARAM_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \

#define	IPD_RED_QUE7_PARAM_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \

#define	IPD_PTR_COUNT_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \
	"f\x13\x2d"	"63_19\0" \
	"b\x12"		"PKTV_CNT\0" \
	"b\x11"		"WQEV_CNT\0" \
	"f\x0e\x03"	"PFIF_CNT\0" \
	"f\x07\x07"	"PKT_PCNT\0" \
	"f\x00\x07"	"WQE_PCNT\0"
#define	IPD_BP_PRT_RED_END_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \
	"f\x24\x1c"	"63_36\0" \
	"f\x00\x24"	"PRT_ENB\0"
#define	IPD_QUE0_FREE_PAGE_CNT_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \
	"f\x20\x20"	"63_32\0" \
	"f\x00\x20"	"Q0_PCNT\0"
#define	IPD_CLK_COUNT_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \
	"f\x00\x40"	"CLK_CNT\0"
#define	IPD_PWP_PTR_FIFO_CTL_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \
	"f\x3d\x03"	"63_61\0" \
	"f\x36\x07"	"MAX_CNTS\0" \
	"f\x2e\x08"	"WRADDR\0" \
	"f\x26\x08"	"PRADDR\0" \
	"f\x09\x1d"	"PTR\0" \
	"b\x08"		"CENA\0" \
	"f\x00\x08"	"RADDR\0"
#define	IPD_PRC_HOLD_PTR_FIFO_CTL_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \
	"f\x27\x19"	"63_39\0" \
	"f\x24\x03"	"MAX_PTR\0" \
	"f\x21\x03"	"PRADDR\0" \
	"f\x04\x1d"	"PTR\0" \
	"b\x03"		"CENA\0" \
	"f\x00\x03"	"RADDR\0"
#define	IPD_PRC_PORT_PTR_FIFO_CTL_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \
	"f\x2c\x14"	"63_44\0" \
	"f\x25\x07"	"MAX_PTR\0" \
	"f\x08\x1d"	"PTR\0" \
	"b\x07"		"CENA\0" \
	"f\x00\x07"	"RADDR\0"
#define	IPD_PKT_PTR_VALID_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \
	"f\x1d\x23"	"63_29\0" \
	"f\x00\x1d"	"PTR\0"
#define	IPD_WQE_PTR_VALID_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \
	"f\x1d\x23"	"63_29\0" \
	"f\x00\x1d"	"PTR\0"
#define	IPD_BIST_STATUS_BITS \
	"\177"		/* new format */ \
	"\020"		/* hex display */ \
	"\020"		/* %016x format */ \
	"f\x10\x30"	"63_29\0" \
	"b\x0f"		"PWQ_WQED\0" \
	"b\x0e"		"PWQ_WP1\0" \
	"b\x0d"		"PWQ_POW\0" \
	"b\x0c"		"IPQ_PBE1\0" \
	"b\x0b"		"IPQ_PBE0\0" \
	"b\x0a"		"PBM3\0" \
	"b\x09"		"PBM2\0" \
	"b\x08"		"PBM1\0" \
	"b\x07"		"PBM0\0" \
	"b\x06"		"PBM_WORD\0" \
	"b\x05"		"PWQ1\0" \
	"b\x04"		"PWQ0\0" \
	"b\x03"		"PRC_OFF\0" \
	"b\x02"		"IPD_OLD\0" \
	"b\x01"		"IPD_NEW\0" \
	"b\x00"		"PWP\0"

#endif /* _OCTEON_IPDREG_H_ */
