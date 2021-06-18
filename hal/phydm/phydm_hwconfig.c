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

/*@************************************************************
 * include files
 ************************************************************/

#include "mp_precomp.h"
#include "phydm_precomp.h"

#define READ_AND_CONFIG_MP(ic, txt) (odm_read_and_config_mp_##ic##txt(dm))
#define READ_AND_CONFIG_TC(ic, txt) (odm_read_and_config_tc_##ic##txt(dm))

#if (PHYDM_TESTCHIP_SUPPORT == 1)
#define READ_AND_CONFIG(ic, txt)                     \
	do {                                         \
		if (dm->is_mp_chip)                  \
			READ_AND_CONFIG_MP(ic, txt); \
		else                                 \
			READ_AND_CONFIG_TC(ic, txt); \
	} while (0)
#else
#define READ_AND_CONFIG READ_AND_CONFIG_MP
#endif

#define GET_VERSION_MP(ic, txt) (odm_get_version_mp_##ic##txt())
#define GET_VERSION_TC(ic, txt) (odm_get_version_tc_##ic##txt())

#if (PHYDM_TESTCHIP_SUPPORT == 1)
#define GET_VERSION(ic, txt) (dm->is_mp_chip ? GET_VERSION_MP(ic, txt) : GET_VERSION_TC(ic, txt))
#else
#define GET_VERSION(ic, txt) GET_VERSION_MP(ic, txt)
#endif

enum hal_status
odm_config_rf_with_header_file(struct dm_struct *dm,
			       enum odm_rf_config_type config_type,
			       u8 e_rf_path)
{
	enum hal_status result = HAL_STATUS_SUCCESS;

	PHYDM_DBG(dm, ODM_COMP_INIT, "===>%s (%s)\n", __func__,
		  (dm->is_mp_chip) ? "MPChip" : "TestChip");
	PHYDM_DBG(dm, ODM_COMP_INIT,
		  "support_platform: 0x%X, support_interface: 0x%X, board_type: 0x%X\n",
		  dm->support_platform, dm->support_interface, dm->board_type);

	if (config_type == CONFIG_RF_RADIO) {
		if (e_rf_path == RF_PATH_A)
			READ_AND_CONFIG_MP(8822c, _radioa);
		else if (e_rf_path == RF_PATH_B)
			READ_AND_CONFIG_MP(8822c, _radiob);
	} else if (config_type == CONFIG_RF_TXPWR_LMT) {
		if (dm->rfe_type == 5)
			READ_AND_CONFIG_MP(8822c, _txpwr_lmt_type5);
		else
			READ_AND_CONFIG_MP(8822c, _txpwr_lmt);
	}

	return result;
}

enum hal_status
odm_config_rf_with_tx_pwr_track_header_file(struct dm_struct *dm)
{
	PHYDM_DBG(dm, ODM_COMP_INIT, "===>%s (%s)\n", __func__,
		  (dm->is_mp_chip) ? "MPChip" : "TestChip");
	PHYDM_DBG(dm, ODM_COMP_INIT,
		  "support_platform: 0x%X, support_interface: 0x%X, board_type: 0x%X\n",
		  dm->support_platform, dm->support_interface, dm->board_type);

	if (dm->en_tssi_mode)
		READ_AND_CONFIG_MP(8822c, _txpowertracktssi);
	else
		READ_AND_CONFIG_MP(8822c, _txpowertrack);

	return HAL_STATUS_SUCCESS;
}

enum hal_status
odm_config_bb_with_header_file(struct dm_struct *dm,
			       enum odm_bb_config_type config_type)
{
	enum hal_status result = HAL_STATUS_SUCCESS;

	if (config_type == CONFIG_BB_PHY_REG)
		odm_read_and_config_mp_8822c_phy_reg(dm);
	else if (config_type == CONFIG_BB_AGC_TAB)
		READ_AND_CONFIG_MP(8822c, _agc_tab);
	else if (config_type == CONFIG_BB_PHY_REG_PG)
		READ_AND_CONFIG(8822c, _phy_reg_pg);

	return result;
}

u32 odm_get_hw_img_version(struct dm_struct *dm)
{
	u32 version = 0;

	version = odm_get_version_mp_8822c_phy_reg();

	return version;
}

u32 query_phydm_trx_capability(struct dm_struct *dm)
{
	u32 value32 = 0xFFFFFFFF;

	return value32;
}

u32 query_phydm_stbc_capability(struct dm_struct *dm)
{
	u32 value32 = 0xFFFFFFFF;


	return value32;
}

u32 query_phydm_ldpc_capability(struct dm_struct *dm)
{
	u32 value32 = 0xFFFFFFFF;

	return value32;
}

u32 query_phydm_txbf_parameters(struct dm_struct *dm)
{
	u32 value32 = 0xFFFFFFFF;

	return value32;
}

u32 query_phydm_txbf_capability(struct dm_struct *dm)
{
	u32 value32 = 0xFFFFFFFF;

	return value32;
}
