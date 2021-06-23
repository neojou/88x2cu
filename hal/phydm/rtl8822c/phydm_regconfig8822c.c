/******************************************************************************
 *
 * Copyright(c) 2007 - 2017  Realtek Corporation.
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

#include "mp_precomp.h"
#include "../phydm_precomp.h"

#if (RTL8822C_SUPPORT)

void phydm_agc_lower_bound_8822c(struct dm_struct *dm, u32 addr, u32 data)
{
	u8 rxbb_gain = (u8)(data & 0x0000001f);
	u8 mp_gain = (u8)((data & 0x003f0000) >> 16);
	u8 tab = (u8)((data & 0x03c00000) >> 22);

	if (addr != R_0x1d90)
		return;

	PHYDM_DBG(dm, ODM_COMP_INIT,
		  "data = 0x%x, mp_gain = 0x%x, tab = 0x%x, rxbb_gain = 0x%x\n",
		  data, mp_gain, tab, rxbb_gain);

	if (!dm->l_bnd_detect[tab] && rxbb_gain == RXBB_MAX_GAIN_8822C) {
		dm->ofdm_rxagc_l_bnd[tab] = mp_gain;
		dm->l_bnd_detect[tab] = true;
	}
}

void odm_config_bb_phy_reg_pg_8822c(struct dm_struct *dm, u32 band, u32 rf_path,
				    u32 tx_num, u32 addr, u32 bitmask, u32 data)
{
	if (addr == 0xfe || addr == 0xffe) {
		ODM_sleep_ms(50);
	} else {
		phy_store_tx_power_by_rate(dm->adapter, band, rf_path, tx_num,
					   addr, bitmask, data);
	}
}

void odm_config_bb_txpwr_lmt_8822c_ex(struct dm_struct *dm, u8 regulation,
				      u8 band, u8 bandwidth, u8 rate_section,
				      u8 rf_path, u8 channel, s8 power_limit)
{
	phy_set_tx_power_limit_ex(dm, regulation, band, bandwidth, rate_section,
				  rf_path, channel, power_limit);
}

void odm_config_bb_txpwr_lmt_8822c(struct dm_struct *dm, u8 *regulation,
				   u8 *band, u8 *bandwidth, u8 *rate_section,
				   u8 *rf_path, u8 *channel, u8 *power_limit)
{
#if (DM_ODM_SUPPORT_TYPE & ODM_CE)
	phy_set_tx_power_limit(dm, regulation, band, bandwidth, rate_section,
			       rf_path, channel, power_limit);
#elif (DM_ODM_SUPPORT_TYPE & ODM_WIN)
	PHY_SetTxPowerLimit(dm, regulation, band, bandwidth, rate_section,
			    rf_path, channel, power_limit);
#endif
}

#endif
