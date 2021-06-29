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
#include "phydm_precomp.h"

#define CALCULATE_SWINGTALBE_OFFSET(_offset, _direction, _size, _delta_thermal)\
	do {                                                                   \
		u32 __offset = (u32)_offset;                                   \
		u32 __size = (u32)_size;                                       \
		for (__offset = 0; __offset < __size; __offset++) {            \
			if (_delta_thermal <                                   \
				thermal_threshold[_direction][__offset]) {     \
				if (__offset != 0)                             \
					__offset--;                            \
				break;                                         \
			}                                                      \
		}                                                              \
		if (__offset >= __size)                                        \
			__offset = __size - 1;                                 \
	} while (0)

void configure_txpower_track(void *dm_void, struct txpwrtrack_cfg *config)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	configure_txpower_track_8822c(config);
}

/*@ **********************************************************************
 * <20121113, Kordan> This function should be called when tx_agc changed.
 * Otherwise the previous compensation is gone, because we record the
 * delta of temperature between two TxPowerTracking watch dogs.
 *
 * NOTE: If Tx BB swing or Tx scaling is varified during run-time, still
 * need to call this function.
 * **********************************************************************
 */
void odm_clear_txpowertracking_state(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _hal_rf_ *rf = &dm->rf_table;
	u8 p = 0;
	struct dm_rf_calibration_struct *cali_info = &dm->rf_calibrate_info;

	cali_info->bb_swing_idx_cck_base = cali_info->default_cck_index;
	cali_info->bb_swing_idx_cck = cali_info->default_cck_index;
	dm->rf_calibrate_info.CCK_index = 0;

	for (p = RF_PATH_A; p < MAX_RF_PATH; ++p) {
		cali_info->bb_swing_idx_ofdm_base[p]
						= cali_info->default_ofdm_index;
		cali_info->bb_swing_idx_ofdm[p] = cali_info->default_ofdm_index;
		cali_info->OFDM_index[p] = cali_info->default_ofdm_index;

		cali_info->power_index_offset[p] = 0;
		cali_info->delta_power_index[p] = 0;
		cali_info->delta_power_index_last[p] = 0;

		/* Initial Mix mode power tracking*/
		cali_info->absolute_ofdm_swing_idx[p] = 0;
		cali_info->remnant_ofdm_swing_idx[p] = 0;
		cali_info->kfree_offset[p] = 0;
	}
	/* Initial Mix mode power tracking*/
	cali_info->modify_tx_agc_flag_path_a = false;
	cali_info->modify_tx_agc_flag_path_b = false;
	cali_info->modify_tx_agc_flag_path_c = false;
	cali_info->modify_tx_agc_flag_path_d = false;
	cali_info->remnant_cck_swing_idx = 0;
	cali_info->thermal_value = rf->eeprom_thermal;
	cali_info->modify_tx_agc_value_cck = 0;
	cali_info->modify_tx_agc_value_ofdm = 0;
}

void odm_pwrtrk_method(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u8 p, idxforchnl = 0;

	struct txpwrtrack_cfg c = {0};

	configure_txpower_track(dm, &c);

	RF_DBG(dm, DBG_RF_TX_PWR_TRACK,
	       "***Enter PwrTrk BBSWING_MODE***\n");
	for (p = RF_PATH_A; p < c.rf_path_count; p++)
		(*c.odm_tx_pwr_track_set_pwr)
			(dm, BBSWING, p, idxforchnl);
}

/*@3============================================================
 * 3 IQ Calibration
 * 3============================================================
 */

void odm_reset_iqk_result(void *dm_void)
{
}

#if !(DM_ODM_SUPPORT_TYPE & ODM_AP)
u8 odm_get_right_chnl_place_for_iqk(u8 chnl)
{
	u8 channel_all[ODM_TARGET_CHNL_NUM_2G_5G] = {
		1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14,
		36, 38, 40, 42, 44, 46, 48, 50, 52, 54, 56, 58, 60, 62, 64,
		100, 102, 104, 106, 108, 110, 112, 114, 116, 118, 120, 122,
		124, 126, 128, 130, 132, 134, 136, 138, 140,
		149, 151, 153, 155, 157, 159, 161, 163, 165};
	u8 place = chnl;

	if (chnl > 14) {
		for (place = 14; place < sizeof(channel_all); place++) {
			if (channel_all[place] == chnl)
				return place - 13;
		}
	}
	return 0;
}
#endif

void phydm_rf_init(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	odm_txpowertracking_init(dm);

#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN | ODM_CE))
	odm_clear_txpowertracking_state(dm);
#endif
}

