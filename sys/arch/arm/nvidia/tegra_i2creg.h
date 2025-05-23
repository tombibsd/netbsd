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

#ifndef _ARM_TEGRA_I2CREG_H
#define _ARM_TEGRA_I2CREG_H

#define I2C_CNFG_REG			0x00
#define I2C_CNFG_MSTR_CLR_BUS_ON_TIMEOUT	__BIT(15)
#define I2C_CNFG_DEBOUNCE_CNT			__BITS(14,12)
#define I2C_CNFG_NEW_MASTER_FSM			__BIT(11)
#define I2C_CNFG_PACKET_MODE_EN			__BIT(10)
#define I2C_CNFG_SEND				__BIT(9)
#define I2C_CNFG_NOACK				__BIT(8)
#define I2C_CNFG_CMD2				__BIT(7)
#define I2C_CNFG_CMD1				__BIT(6)
#define I2C_CNFG_START				__BIT(5)
#define I2C_CNFG_SLV2				__BIT(4)
#define I2C_CNFG_LENGTH				__BITS(3,1)
#define I2C_CNFG_A_MOD				__BIT(0)

#define I2C_CMD_ADDR0_REG		0x04
#define I2C_CMD_ADDR1_REG		0x08
#define I2C_CMD_DATA1_REG		0x0c
#define I2C_CMD_DATA2_REG		0x10

#define I2C_STATUS_REG			0x1c
#define I2C_STATUS_BUSY				__BIT(8)
#define I2C_STATUS_CMD2_STAT			__BITS(7,4)
#define I2C_STATUS_CMD1_STAT			__BITS(3,0)

#define I2C_SL_CNFG_REG			0x20
#define I2C_SL_CNFG_FIFO_XFER_EN		__BIT(20)
#define I2C_SL_CNFG_BUFFER_SIZE			__BITS(19,8)
#define I2C_SL_CNFG_ACK_LAST_BYTE_VALID		__BIT(7)
#define I2C_SL_CNFG_ACK_LAST_BYTE		__BIT(6)
#define I2C_SL_CNFG_ACK_WITHHOLD_EN		__BIT(5)
#define I2C_SL_CNFG_PKT_MODE_EN			__BIT(4)
#define I2C_SL_CNFG_ENABLE_SL			__BIT(3)
#define I2C_SL_CNFG_NEWSL			__BIT(2)
#define I2C_SL_CNFG_NACK			__BIT(1)
#define I2C_SL_CNFG_RESP			__BIT(0)

#define I2C_SL_RCVD_REG			0x24
#define I2C_SL_STATUS_REG		0x28
#define I2C_SL_ADDR1_REG		0x2c
#define I2C_SL_ADDR2_REG		0x30
#define I2C_TLOW_SEXT_REG		0x34
#define I2C_SL_DELAY_COUNT_REG		0x3c
#define I2C_SL_INT_MASK_REG		0x40
#define I2C_SL_INT_SOURCE_REG		0x44
#define I2C_SL_INT_SET_REG		0x48
#define I2C_TX_PACKET_FIFO_REG		0x50
#define I2C_RX_FIFO_REG			0x54

#define I2C_PACKET_TRANSFER_STATUS_REG	0x58
#define I2C_PACKET_TRANSFER_STATUS_TRANSFER_COMPLETE __BIT(24)
#define I2C_PACKET_TRANSFER_STATUS_TRANSFER_PKT_ID __BITS(23,16)
#define I2C_PACKET_TRANSFER_STATUS_TRANSFER_BYTENUM __BITS(15,4)
#define I2C_PACKET_TRANSFER_STATUS_NOACK_FOR_ADDR __BIT(3)
#define I2C_PACKET_TRANSFER_STATUS_NOACK_FOR_DATA __BIT(2)
#define I2C_PACKET_TRANSFER_STATUS_ARB_LOST	__BIT(1)
#define I2C_PACKET_TRANSFER_STATUS_CONTROLLER_BUSY __BIT(0)

#define I2C_FIFO_CONTROL_REG		0x5c
#define I2C_FIFO_CONTROL_SLV_TX_FIFO_TRIG	__BITS(15,13)
#define I2C_FIFO_CONTROL_SLV_RX_FIFO_TRIG	__BITS(12,10)
#define I2C_FIFO_CONTROL_SLV_TX_FIFO_FLUSH	__BIT(9)
#define I2C_FIFO_CONTROL_SLV_RX_FIFO_FLUSH	__BIT(8)
#define I2C_FIFO_CONTROL_TX_FIFO_TRIG		__BITS(7,5)
#define I2C_FIFO_CONTROL_RX_FIFO_TRIG		__BITS(4,2)
#define I2C_FIFO_CONTROL_TX_FIFO_FLUSH		__BIT(1)
#define I2C_FIFO_CONTROL_RX_FIFO_FLUSH		__BIT(0)

#define I2C_FIFO_STATUS_REG		0x60
#define I2C_FIFO_STATUS_SLV_XFER_ERR_REASON	__BIT(25)
#define I2C_FIFO_STATUS_SLV_TX_FIFO_EMPTY_CNT	__BITS(23,20)
#define I2C_FIFO_STATUS_SLV_RX_FIFO_FULL_CNT	__BITS(19,16)
#define I2C_FIFO_STATUS_TX_FIFO_EMPTY_CNT	__BITS(7,4)
#define I2C_FIFO_STATUS_RX_FIFO_FULL_CNT	__BITS(3,0)

#define I2C_INTERRUPT_MASK_REG		0x64
#define I2C_INTERRUPT_MASK_TIMEOUT		__BIT(8)
#define I2C_INTERRUPT_MASK_PACKET_XFER_COMPLETE	__BIT(7)
#define I2C_INTERRUPT_MASK_ALL_PACKETS_XFER_COMPLETE __BIT(6)
#define I2C_INTERRUPT_MASK_NOACK		__BIT(3)
#define I2C_INTERRUPT_MASK_ARB_LOST		__BIT(2)
#define I2C_INTERRUPT_MASK_TFIFO_DATA_REQ	__BIT(1)
#define I2C_INTERRUPT_MASK_RFIFO_DATA_REQ	__BIT(0)

#define I2C_INTERRUPT_STATUS_REG	0x68
#define I2C_INTERRUPT_STATUS_TIMEOUT		__BIT(8)
#define I2C_INTERRUPT_STATUS_PACKET_XFER_COMPLETE __BIT(7)
#define I2C_INTERRUPT_STATUS_ALL_PACKETS_XFER_COMPLETE __BIT(6)
#define I2C_INTERRUPT_STATUS_NOACK		__BIT(3)
#define I2C_INTERRUPT_STATUS_ARB_LOST		__BIT(2)
#define I2C_INTERRUPT_STATUS_TFIFO_DATA_REQ	__BIT(1)
#define I2C_INTERRUPT_STATUS_RFIFO_DATA_REQ	__BIT(0)

#define I2C_CLK_DIVISOR_REG		0x6c
#define I2C_CLK_DIVISOR_STD_FAST_MODE		__BITS(31,16)
#define I2C_CLK_DIVISOR_HSMODE			__BITS(15,0)

#define I2C_INTERRUPT_SOURCE_REG	0x70
#define I2C_INTERRUPT_SET_REG		0x74
#define I2C_SLV_TX_PACKET_FIFO_REG	0x78
#define I2C_SLV_RX_FIFO_REG		0x7c
#define I2C_SLV_PACKET_STATUS_REG	0x80
#define I2C_BUS_CLEAR_CONFIG_REG	0x84
#define I2C_BUS_CLEAR_STATUS_REG	0x88

#define I2C_BUS_CONFIG_LOAD_REG		0x8c
#define I2C_BUS_CONFIG_LOAD_TIMEOUT_CONFIG_LOAD	__BIT(2)
#define I2C_BUS_CONFIG_LOAD_SLV_CONFIG_LOAD	__BIT(1)
#define I2C_BUS_CONFIG_LOAD_MSTR_CONFIG_LOAD	__BIT(0)

#define I2C_INTERFACE_TIMING0_REG	0x94
#define I2C_INTERFACE_TIMING0_THIGH		__BITS(13,8)
#define I2C_INTERFACE_TIMING0_TLOW		__BITS(5,0)

#define I2C_INTERFACE_TIMING1_REG	0x98
#define I2C_HS_INTERFACE_TIMING0_REG	0x9c
#define I2C_HS_INTERFACE_TIMING1_REG	0xa0

/*
 * I/O Packets
 */
#define I2C_IOPACKET_WORD0_PROTHDRSZ		__BITS(29,28)
#define I2C_IOPACKET_WORD0_PROTHDRSZ_REQ	0
#define I2C_IOPACKET_WORD0_PROTHDRSZ_RES	1
#define I2C_IOPACKET_WORD0_PKTID		__BITS(23,16)
#define I2C_IOPACKET_WORD0_CONTROLLERID		__BITS(15,12)
#define I2C_IOPACKET_WORD0_PROTOCOL		__BITS(7,4)
#define I2C_IOPACKET_WORD0_PROTOCOL_I2C		1
#define I2C_IOPACKET_WORD0_PKTTYPE		__BITS(2,0)
#define I2C_IOPACKET_WORD0_PKTTYPE_REQ		0
#define I2C_IOPACKET_WORD0_PKTTYPE_RES		1
#define I2C_IOPACKET_WORD0_PKTTYPE_INT		2
#define I2C_IOPACKET_WORD0_PKTTYPE_STOP		3

#define I2C_IOPACKET_WORD1_PAYLOADSIZE		__BITS(11,0)

#define I2C_IOPACKET_XMITHDR_RESP_PKT_FREQ	__BIT(25)
#define I2C_IOPACKET_XMITHDR_RESP_PKT_ENABLE	__BIT(24)
#define I2C_IOPACKET_XMITHDR_HS_MODE		__BIT(22)
#define I2C_IOPACKET_XMITHDR_CONTINUE_ON_NACK	__BIT(21)
#define I2C_IOPACKET_XMITHDR_SEND_START_BYTE	__BIT(20)
#define I2C_IOPACKET_XMITHDR_READ		__BIT(19)
#define I2C_IOPACKET_XMITHDR_ADDRESS_MODE	__BIT(18)
#define I2C_IOPACKET_XMITHDR_IE			__BIT(17)
#define I2C_IOPACKET_XMITHDR_REPEAT_STARTSTOP	__BIT(16)
#define I2C_IOPACKET_XMITHDR_CONTINUE_XFER	__BIT(15)
#define I2C_IOPACKET_XMITHDR_HS_MASTER_ADDR	__BITS(14,12)
#define I2C_IOPACKET_XMITHDR_SLAVE_ADDR		__BITS(9,0)

#define I2C_IOPACKET_RESPHDR_RFIFO_OVF		__BIT(26)
#define I2C_IOPACKET_RESPHDR_TFIFO_OVF		__BIT(25)
#define I2C_IOPACKET_RESPHDR_TRANSFER_COMPLETE	__BIT(24)
#define I2C_IOPACKET_RESPHDR_TRANSFER_PKT_ID	__BITS(23,16)
#define I2C_IOPACKET_RESPHDR_TRANSFER_BYTENUM	__BITS(15,4)
#define I2C_IOPACKET_RESPHDR_NOACK_FOR_ADDR	__BIT(3)
#define I2C_IOPACKET_RESPHDR_NOACK_FOR_DATA	__BIT(2)
#define I2C_IOPACKET_RESPHDR_ARB_LOST		__BIT(1)
#define I2C_IOPACKET_RESPHDR_CONTROLLER_BUSY	__BIT(0)

#endif /* _ARM_TEGRA_I2CREG_H */
