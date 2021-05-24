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
#include "../halrf_precomp.h"

bool halrf_get_efuse_info_8822c(struct rf_info *rf, u8 *efuse_map,
		       enum rtw_efuse_info id, void *value, u32 length,
		       u8 autoload_status)
{
	struct rtw_hal_com_t *hal = rf->hal_com;

	u32 offset = 0;
	u32 default_value = 0;

	switch (id) {
		case EFUSE_INFO_RF_RFE:
			offset = EFUSE_INFO_RF_RFE_8822C_ADDR;
			default_value = EFUSE_INFO_RF_RFE_8822C_VALUE;
			break;
		case EFUSE_INFO_RF_XTAL:
			offset = EFUSE_INFO_RF_XTAL_8822C_ADDR;
			default_value = EFUSE_INFO_RF_XTAL_8822C_VALUE;
			break;
		default:
			return false;
	}

	hal_mem_cpy(hal, value, efuse_map + offset, length);

	return true;
}

