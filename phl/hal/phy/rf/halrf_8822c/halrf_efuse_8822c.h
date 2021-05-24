/******************************************************************************
 *
 * Copyright(c) 2021  Realtek Corporation.
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
#ifndef _HALRF_EFUSE_8822C_H_
#define _HALRF_EFUSE_8822C_H_

enum halrf_efsue_info_8822c_offset {
	EFUSE_INFO_RF_BOARD_OPTION_8822C_ADDR	= 0x2c1,
	EFUSE_INFO_RF_RFE_8822C_ADDR		= 0xca,
	EFUSE_INFO_RF_COUNTRY_8822C_ADDR	= 0x2cb,
	EFUSE_INFO_RF_CHAN_PLAN_8822C_ADDR	= 0x2b8,
	EFUSE_INFO_RF_XTAL_8822C_ADDR		= 0x2b9,
	EFUSE_INFO_RF_THERMAL_A_8822C_ADDR	= 0x2d0,
	EFUSE_INFO_RF_THERMAL_B_8822C_ADDR	= 0x2d1,
	EFUSE_INFO_RF_2G_CCK_A_TSSI_DE_1_8822C_ADDR	= 0x210,
	EFUSE_INFO_RF_2G_CCK_A_TSSI_DE_2_8822C_ADDR	= 0x211,
	EFUSE_INFO_RF_2G_CCK_A_TSSI_DE_3_8822C_ADDR	= 0x212,
	EFUSE_INFO_RF_2G_CCK_A_TSSI_DE_4_8822C_ADDR	= 0x213,
	EFUSE_INFO_RF_2G_CCK_A_TSSI_DE_5_8822C_ADDR	= 0x214,
	EFUSE_INFO_RF_2G_CCK_A_TSSI_DE_6_8822C_ADDR	= 0x215,
	EFUSE_INFO_RF_2G_BW40M_A_TSSI_DE_1_8822C_ADDR	= 0x216,
	EFUSE_INFO_RF_2G_BW40M_A_TSSI_DE_2_8822C_ADDR	= 0x217,
	EFUSE_INFO_RF_2G_BW40M_A_TSSI_DE_3_8822C_ADDR	= 0x218,
	EFUSE_INFO_RF_2G_BW40M_A_TSSI_DE_4_8822C_ADDR	= 0x219,
	EFUSE_INFO_RF_2G_BW40M_A_TSSI_DE_5_8822C_ADDR	= 0x21a,
	EFUSE_INFO_RF_5G_BW40M_A_TSSI_DE_1_8822C_ADDR	= 0x222,
	EFUSE_INFO_RF_5G_BW40M_A_TSSI_DE_2_8822C_ADDR	= 0x223,
	EFUSE_INFO_RF_5G_BW40M_A_TSSI_DE_3_8822C_ADDR	= 0x224,
	EFUSE_INFO_RF_5G_BW40M_A_TSSI_DE_4_8822C_ADDR	= 0x225,
	EFUSE_INFO_RF_5G_BW40M_A_TSSI_DE_5_8822C_ADDR	= 0x226,
	EFUSE_INFO_RF_5G_BW40M_A_TSSI_DE_6_8822C_ADDR	= 0x227,
	EFUSE_INFO_RF_5G_BW40M_A_TSSI_DE_7_8822C_ADDR	= 0x228,
	EFUSE_INFO_RF_5G_BW40M_A_TSSI_DE_8_8822C_ADDR	= 0x229,
	EFUSE_INFO_RF_5G_BW40M_A_TSSI_DE_9_8822C_ADDR	= 0x22a,
	EFUSE_INFO_RF_5G_BW40M_A_TSSI_DE_10_8822C_ADDR	= 0x22b,
	EFUSE_INFO_RF_5G_BW40M_A_TSSI_DE_11_8822C_ADDR	= 0x22c,
	EFUSE_INFO_RF_5G_BW40M_A_TSSI_DE_12_8822C_ADDR	= 0x22d,
	EFUSE_INFO_RF_5G_BW40M_A_TSSI_DE_13_8822C_ADDR	= 0x22e,
	EFUSE_INFO_RF_5G_BW40M_A_TSSI_DE_14_8822C_ADDR	= 0x22f,
	EFUSE_INFO_RF_2G_CCK_B_TSSI_DE_1_8822C_ADDR	= 0x23a,
	EFUSE_INFO_RF_2G_CCK_B_TSSI_DE_2_8822C_ADDR	= 0x23b,
	EFUSE_INFO_RF_2G_CCK_B_TSSI_DE_3_8822C_ADDR	= 0x23c,
	EFUSE_INFO_RF_2G_CCK_B_TSSI_DE_4_8822C_ADDR	= 0x23d,
	EFUSE_INFO_RF_2G_CCK_B_TSSI_DE_5_8822C_ADDR	= 0x23e,
	EFUSE_INFO_RF_2G_CCK_B_TSSI_DE_6_8822C_ADDR	= 0x23f,
	EFUSE_INFO_RF_2G_BW40M_B_TSSI_DE_1_8822C_ADDR	= 0x240,
	EFUSE_INFO_RF_2G_BW40M_B_TSSI_DE_2_8822C_ADDR	= 0x241,
	EFUSE_INFO_RF_2G_BW40M_B_TSSI_DE_3_8822C_ADDR	= 0x242,
	EFUSE_INFO_RF_2G_BW40M_B_TSSI_DE_4_8822C_ADDR	= 0x243,
	EFUSE_INFO_RF_2G_BW40M_B_TSSI_DE_5_8822C_ADDR	= 0x244,
	EFUSE_INFO_RF_5G_BW40M_B_TSSI_DE_1_8822C_ADDR	= 0x24c,
	EFUSE_INFO_RF_5G_BW40M_B_TSSI_DE_2_8822C_ADDR	= 0x24d,
	EFUSE_INFO_RF_5G_BW40M_B_TSSI_DE_3_8822C_ADDR	= 0x24e,
	EFUSE_INFO_RF_5G_BW40M_B_TSSI_DE_4_8822C_ADDR	= 0x24f,
	EFUSE_INFO_RF_5G_BW40M_B_TSSI_DE_5_8822C_ADDR	= 0x250,
	EFUSE_INFO_RF_5G_BW40M_B_TSSI_DE_6_8822C_ADDR	= 0x251,
	EFUSE_INFO_RF_5G_BW40M_B_TSSI_DE_7_8822C_ADDR	= 0x252,
	EFUSE_INFO_RF_5G_BW40M_B_TSSI_DE_8_8822C_ADDR	= 0x253,
	EFUSE_INFO_RF_5G_BW40M_B_TSSI_DE_9_8822C_ADDR	= 0x254,
	EFUSE_INFO_RF_5G_BW40M_B_TSSI_DE_10_8822C_ADDR	= 0x255,
	EFUSE_INFO_RF_5G_BW40M_B_TSSI_DE_11_8822C_ADDR	= 0x256,
	EFUSE_INFO_RF_5G_BW40M_B_TSSI_DE_12_8822C_ADDR	= 0x257,
	EFUSE_INFO_RF_5G_BW40M_B_TSSI_DE_13_8822C_ADDR	= 0x258,
	EFUSE_INFO_RF_5G_BW40M_B_TSSI_DE_14_8822C_ADDR	= 0x259,
	EFUSE_INFO_RF_RX_GAIN_K_A_2G_CCK_8822C_ADDR	= 0x2d6,
	EFUSE_INFO_RF_RX_GAIN_K_A_2G_OFMD_8822C_ADDR	= 0x2d4,
	EFUSE_INFO_RF_RX_GAIN_K_A_5GL_8822C_ADDR	= 0x2d8,
	EFUSE_INFO_RF_RX_GAIN_K_A_5GM_8822C_ADDR	= 0x2da,
	EFUSE_INFO_RF_RX_GAIN_K_A_5GH_8822C_ADDR	= 0x2dc
};

enum halrf_efsue_default_value_8822c {
	EFUSE_INFO_RF_RFE_8822C_VALUE		= 0x1,
	EFUSE_INFO_RF_CHAN_PLAN_8822C_VALUE	= 0x7f,
	EFUSE_INFO_RF_CHAN_PLAN_FORCE_HW_8822C_VALUE	= 0x0,
	EFUSE_INFO_RF_XTAL_8822C_VALUE	= 0x3f,
	EFUSE_INFO_RF_THERMAL_A_8822C_VALUE	= 0x22,
	EFUSE_INFO_RF_THERMAL_B_8822C_VALUE	= 0x22,
	EFUSE_INFO_RF_TSSI_DE_8822C_VALUE	= 0x0,
	EFUSE_INFO_RF_RX_GAIN_K_8822C_VALUE	= 0xf,
	EFUSE_INFO_RF_BOARD_OPTION_8822C_VALUE	= 0x1,
	EFUSE_INFO_RF_COUNTRY_8822C_VALUE	= 0xffff
};

bool halrf_get_efuse_info_8822c(struct rf_info *rf, u8 *efuse_map,
		       enum rtw_efuse_info id, void *value, u32 length,
		       u8 autoload_status);

#endif	/*_HALRF_EFUSE_8822C_H_*/

