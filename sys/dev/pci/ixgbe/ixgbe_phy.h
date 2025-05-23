/******************************************************************************

  Copyright (c) 2001-2013, Intel Corporation 
  All rights reserved.
  
  Redistribution and use in source and binary forms, with or without 
  modification, are permitted provided that the following conditions are met:
  
   1. Redistributions of source code must retain the above copyright notice, 
      this list of conditions and the following disclaimer.
  
   2. Redistributions in binary form must reproduce the above copyright 
      notice, this list of conditions and the following disclaimer in the 
      documentation and/or other materials provided with the distribution.
  
   3. Neither the name of the Intel Corporation nor the names of its 
      contributors may be used to endorse or promote products derived from 
      this software without specific prior written permission.
  
  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
  ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR 
  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF 
  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN 
  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) 
  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
  POSSIBILITY OF SUCH DAMAGE.

******************************************************************************/
/*$FreeBSD: head/sys/dev/ixgbe/ixgbe_phy.h 251964 2013-06-18 21:28:19Z jfv $*/
/*$NetBSD$*/

#ifndef _IXGBE_PHY_H_
#define _IXGBE_PHY_H_

#include "ixgbe_type.h"
#define IXGBE_I2C_EEPROM_DEV_ADDR	0xA0
#define IXGBE_I2C_EEPROM_DEV_ADDR2	0xA2
#define IXGBE_I2C_EEPROM_BANK_LEN	0xFF

/* EEPROM byte offsets */
#define IXGBE_SFF_IDENTIFIER		0x0
#define IXGBE_SFF_IDENTIFIER_SFP	0x3
#define IXGBE_SFF_VENDOR_OUI_BYTE0	0x25
#define IXGBE_SFF_VENDOR_OUI_BYTE1	0x26
#define IXGBE_SFF_VENDOR_OUI_BYTE2	0x27
#define IXGBE_SFF_1GBE_COMP_CODES	0x6
#define IXGBE_SFF_10GBE_COMP_CODES	0x3
#define IXGBE_SFF_CABLE_TECHNOLOGY	0x8
#define IXGBE_SFF_CABLE_SPEC_COMP	0x3C
#define IXGBE_SFF_SFF_8472_SWAP		0x5C
#define IXGBE_SFF_SFF_8472_COMP		0x5E
#define IXGBE_SFF_SFF_8472_OSCB		0x6E
#define IXGBE_SFF_SFF_8472_ESCB		0x76

/* Bitmasks */
#define IXGBE_SFF_DA_PASSIVE_CABLE	0x4
#define IXGBE_SFF_DA_ACTIVE_CABLE	0x8
#define IXGBE_SFF_DA_SPEC_ACTIVE_LIMITING	0x4
#define IXGBE_SFF_1GBASESX_CAPABLE	0x1
#define IXGBE_SFF_1GBASELX_CAPABLE	0x2
#define IXGBE_SFF_1GBASET_CAPABLE	0x8
#define IXGBE_SFF_10GBASESR_CAPABLE	0x10
#define IXGBE_SFF_10GBASELR_CAPABLE	0x20
#define IXGBE_SFF_SOFT_RS_SELECT_MASK	0x8
#define IXGBE_SFF_SOFT_RS_SELECT_10G	0x8
#define IXGBE_SFF_SOFT_RS_SELECT_1G	0x0
#define IXGBE_I2C_EEPROM_READ_MASK	0x100
#define IXGBE_I2C_EEPROM_STATUS_MASK	0x3
#define IXGBE_I2C_EEPROM_STATUS_NO_OPERATION	0x0
#define IXGBE_I2C_EEPROM_STATUS_PASS	0x1
#define IXGBE_I2C_EEPROM_STATUS_FAIL	0x2
#define IXGBE_I2C_EEPROM_STATUS_IN_PROGRESS	0x3

/* Flow control defines */
#define IXGBE_TAF_SYM_PAUSE		0x400
#define IXGBE_TAF_ASM_PAUSE		0x800

/* Bit-shift macros */
#define IXGBE_SFF_VENDOR_OUI_BYTE0_SHIFT	24
#define IXGBE_SFF_VENDOR_OUI_BYTE1_SHIFT	16
#define IXGBE_SFF_VENDOR_OUI_BYTE2_SHIFT	8

/* Vendor OUIs: format of OUI is 0x[byte0][byte1][byte2][00] */
#define IXGBE_SFF_VENDOR_OUI_TYCO	0x00407600
#define IXGBE_SFF_VENDOR_OUI_FTL	0x00906500
#define IXGBE_SFF_VENDOR_OUI_AVAGO	0x00176A00
#define IXGBE_SFF_VENDOR_OUI_INTEL	0x001B2100

/* I2C SDA and SCL timing parameters for standard mode */
#define IXGBE_I2C_T_HD_STA	4
#define IXGBE_I2C_T_LOW		5
#define IXGBE_I2C_T_HIGH	4
#define IXGBE_I2C_T_SU_STA	5
#define IXGBE_I2C_T_HD_DATA	5
#define IXGBE_I2C_T_SU_DATA	1
#define IXGBE_I2C_T_RISE	1
#define IXGBE_I2C_T_FALL	1
#define IXGBE_I2C_T_SU_STO	4
#define IXGBE_I2C_T_BUF		5

#define IXGBE_TN_LASI_STATUS_REG	0x9005
#define IXGBE_TN_LASI_STATUS_TEMP_ALARM	0x0008

/* SFP+ SFF-8472 Compliance */
#define IXGBE_SFF_SFF_8472_UNSUP	0x00
#define IXGBE_SFF_SFF_8472_REV_9_3	0x01
#define IXGBE_SFF_SFF_8472_REV_9_5	0x02
#define IXGBE_SFF_SFF_8472_REV_10_2	0x03
#define IXGBE_SFF_SFF_8472_REV_10_4	0x04
#define IXGBE_SFF_SFF_8472_REV_11_0	0x05

s32 ixgbe_init_phy_ops_generic(struct ixgbe_hw *hw);
bool ixgbe_validate_phy_addr(struct ixgbe_hw *hw, u32 phy_addr);
enum ixgbe_phy_type ixgbe_get_phy_type_from_id(u32 phy_id);
s32 ixgbe_get_phy_id(struct ixgbe_hw *hw);
s32 ixgbe_identify_phy_generic(struct ixgbe_hw *hw);
s32 ixgbe_reset_phy_generic(struct ixgbe_hw *hw);
s32 ixgbe_read_phy_reg_mdi(struct ixgbe_hw *hw, u32 reg_addr, u32 device_type,
			   u16 *phy_data);
s32 ixgbe_write_phy_reg_mdi(struct ixgbe_hw *hw, u32 reg_addr, u32 device_type,
			    u16 phy_data);
s32 ixgbe_read_phy_reg_generic(struct ixgbe_hw *hw, u32 reg_addr,
			       u32 device_type, u16 *phy_data);
s32 ixgbe_write_phy_reg_generic(struct ixgbe_hw *hw, u32 reg_addr,
				u32 device_type, u16 phy_data);
s32 ixgbe_setup_phy_link_generic(struct ixgbe_hw *hw);
s32 ixgbe_setup_phy_link_speed_generic(struct ixgbe_hw *hw,
				       ixgbe_link_speed speed,
				       bool autoneg_wait_to_complete);
s32 ixgbe_get_copper_link_capabilities_generic(struct ixgbe_hw *hw,
					       ixgbe_link_speed *speed,
					       bool *autoneg);

/* PHY specific */
s32 ixgbe_check_phy_link_tnx(struct ixgbe_hw *hw,
			     ixgbe_link_speed *speed,
			     bool *link_up);
s32 ixgbe_setup_phy_link_tnx(struct ixgbe_hw *hw);
s32 ixgbe_get_phy_firmware_version_tnx(struct ixgbe_hw *hw,
				       u16 *firmware_version);
s32 ixgbe_get_phy_firmware_version_generic(struct ixgbe_hw *hw,
					   u16 *firmware_version);

s32 ixgbe_reset_phy_nl(struct ixgbe_hw *hw);
s32 ixgbe_identify_module_generic(struct ixgbe_hw *hw);
s32 ixgbe_identify_sfp_module_generic(struct ixgbe_hw *hw);
s32 ixgbe_get_sfp_init_sequence_offsets(struct ixgbe_hw *hw,
					u16 *list_offset,
					u16 *data_offset);
s32 ixgbe_tn_check_overtemp(struct ixgbe_hw *hw);
s32 ixgbe_read_i2c_byte_generic(struct ixgbe_hw *hw, u8 byte_offset,
				u8 dev_addr, u8 *data);
s32 ixgbe_write_i2c_byte_generic(struct ixgbe_hw *hw, u8 byte_offset,
				 u8 dev_addr, u8 data);
s32 ixgbe_read_i2c_eeprom_generic(struct ixgbe_hw *hw, u8 byte_offset,
				  u8 *eeprom_data);
s32 ixgbe_write_i2c_eeprom_generic(struct ixgbe_hw *hw, u8 byte_offset,
				   u8 eeprom_data);
void ixgbe_i2c_bus_clear(struct ixgbe_hw *hw);
#endif /* _IXGBE_PHY_H_ */
