/******************************************************************************
 *
 * Copyright(c) 2007 - 2020  Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
 * Realtek Corporation, No. 2, Innovation Road II, Hsinchu Science Park,
 * Hsinchu 300, Taiwan.
 *
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 *****************************************************************************/
#include "halrf_precomp.h"

u32 odm_read_4byte_g6(struct rf_info *rf, u32 addr)
{
	return hal_read32(rf->hal_com, addr);
}

void odm_write_1byte_g6(struct rf_info *rf, u32 addr, u8 data)
{
	hal_write8(rf->hal_com, addr, data);
}

void odm_write_4byte_g6(struct rf_info *rf, u32 addr, u32 data)
{
	hal_write32(rf->hal_com, addr, data);
}

u32 odm_get_rf_reg_g6(struct rf_info *rf, u8 path, u32 addr)
{
	u32 offset[2] = {0x3c00, 0x4c00};

	if (path >= 2)
		return 0xFFFFFFFF;

	addr &= 0xFF;
	addr = offset[path] + (addr << 2);

	return hal_read32(rf->hal_com, addr);
}

static void config_phydm_direct_write_rf_reg_8822c(struct rf_info *rf, u8 path, u32 addr, u32 mask, u32 data)
{
	u32 offset[2] = {0x3c00, 0x4c00};

	addr &= 0xFF;
	addr = offset[path] + (addr << 2);

	halrf_wreg(rf, addr, mask, data);

	udelay(1);
}

static u32 config_phydm_read_rf_reg_8822c(struct rf_info *rf, u8 path, u32 addr, u32 mask)
{
	u32 offset[2] = {0x3c00, 0x4c00};

	if (path >= 2)
		return 0xFFFFFFFF;

	addr &= 0xFF;
	addr = offset[path] + (addr << 2);
	mask &= 0xFFFFF;

	return halrf_rreg(rf, addr, mask);
}

static u32 phydm_check_bit_mask_8822c(u32 mask, u32 data_orig, u32 data)
{
	u8 shift;

	if (mask != 0xFFFFF) {
		for (shift = 0; shift <= 19; shift++) {
			if (mask & BIT(shift))
				break;
		}
		return (data_orig & (~mask)) | (data << shift);
	}

	return data;
}

void odm_set_rf_reg_g6(struct rf_info *rf, u8 path, u32 addr, u32 mask, u32 data)
{
	u32 offset[2] = {0x1808, 0x4108};
	u32 data_and_addr;

	if (path >= 2)
		return;

	if (!addr) {
		config_phydm_direct_write_rf_reg_8822c(rf, path, addr, mask, data);
		return;
	}

	addr &= 0xFF;
	mask &= 0xFFFFF;

	if (mask != 0xFFFFF) {
		u32 value32;
		value32 = config_phydm_read_rf_reg_8822c(rf, path, addr, RFREG_MASK);
		if (value32 == 0xFFFFFFFF)
			return;

		data = phydm_check_bit_mask_8822c(mask, value32, data);
	}

	data_and_addr = ((addr << 20) | (data & 0x000FFFFF)) & 0x0FFFFFFF;
	halrf_wreg(rf, offset[path], 0xFFFFFFFF, data_and_addr);
}

u32 halrf_get_sys_time(struct rf_info *rf)
{
	return 0;
}

u32 halrf_cal_bit_shift(u32 bit_mask)
{
	u32 i;

	for (i = 0; i <= 31; i++) {
		if ((bit_mask >> i) & BIT0)
			break;
	}
	return i;
}

void halrf_wreg(struct rf_info *rf, u32 addr, u32 mask, u32 val)
{
	u32 ori_val, bit_shift;

	if (mask != MASKDWORD) {
		ori_val = halrf_r32(rf, addr);
		bit_shift = halrf_cal_bit_shift(mask);
		val = ((ori_val) & (~mask)) |( ((val << bit_shift)) & mask);
	}
	halrf_w32(rf, addr, val);
}

u32 halrf_rreg(struct rf_info *rf, u32 addr, u32 mask)
{
	u32 reg_val = 0, ori_val, bit_shift;

	ori_val = halrf_r32(rf, addr);
	bit_shift = halrf_cal_bit_shift(mask);
	reg_val = (ori_val & mask) >> bit_shift;

	return reg_val;
}

#if 0 //NEO
void halrf_delay_10us(struct rf_info *rf, u32 count)
{
	u32 i;

	for (i = 0; i < count; i++)
		halrf_delay_us(rf, 10);
}

void halrf_fill_h2c_cmd(struct rf_info *rf, u16 cmdlen, u8 cmdid,
			u8 classid, u32 cmdtype, u32 *pval)
{
	u32 rt_val = 0;
	struct rtw_g6_h2c_hdr hdr = {0};
	struct rtw_hal_com_t *hal_com = NULL;

	hdr.h2c_class = classid;
	hdr.h2c_func = cmdid;
	hdr.type = cmdtype;
	hdr.content_len = cmdlen;
	hal_com = rf->hal_com;
	RF_DBG(rf, DBG_RF_FW, "======>%s   H2C: %x %x %x\n",
		__func__, classid, cmdid, cmdlen);
	rt_val =  rtw_hal_mac_send_h2c(hal_com, &hdr, pval);
	if (rt_val != 0)
		RF_WARNING("Error H2C CLASS=%d, ID=%d\n", classid, cmdid);
}
#endif //NEO

