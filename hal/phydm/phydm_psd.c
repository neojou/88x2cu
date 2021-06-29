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

/******************************************************************************
 * include files
 *****************************************************************************/
#include "mp_precomp.h"
#include "phydm_precomp.h"

#ifdef CONFIG_PSD_TOOL
u32 phydm_get_psd_data(void *dm_void, u32 psd_tone_idx, u32 igi)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct psd_info *dm_psd_table = &dm->dm_psd_table;
	u32 psd_report = 0;

	/*Get PSD Report*/
	psd_report = odm_convert_to_db((u64)psd_report) + igi;

	return psd_report;
}

u8 psd_result_cali_tone_8821[7] = {21, 28, 33, 93, 98, 105, 127};
u8 psd_result_cali_val_8821[7] = {67, 69, 71, 72, 71, 69, 67};

u8 phydm_psd(void *dm_void, u32 igi, u16 start_point, u16 stop_point)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct psd_info *dm_psd_table = &dm->dm_psd_table;
	u32 i = 0, mod_tone_idx = 0;
	u32 t = 0;
	u16 fft_max_half_bw = 0;
	u16 psd_fc_channel = dm_psd_table->psd_fc_channel;
	u8 ag_rf_mode_reg = 0;
	u8 is_5G = 0;
	u32 psd_result_tmp = 0;
	u8 psd_result = 0;
	u8 psd_result_cali_tone[7] = {0};
	u8 psd_result_cali_val[7] = {0};
	u8 noise_idx = 0;
	u8 set_result = 0;
	u32 igi_tmp = 0x6e;

	dm_psd_table->psd_in_progress = 1;

	PHYDM_DBG(dm, ODM_COMP_API, "PSD Start =>\n");

	/* @[Stop DIG]*/
	/* @IGI target at 0dBm & make it can't CCA*/
	if (phydm_pause_func(dm, F00_DIG, PHYDM_PAUSE, PHYDM_PAUSE_LEVEL_3, 1,
			     &igi_tmp) == PAUSE_FAIL) {
		return PHYDM_SET_FAIL;
	}

	ODM_delay_us(10);

	if (phydm_stop_ic_trx(dm, PHYDM_SET) == PHYDM_SET_FAIL) {
		phydm_pause_func(dm, F00_DIG, PHYDM_RESUME, PHYDM_PAUSE_LEVEL_3,
				 1, &igi_tmp);
		return PHYDM_SET_FAIL;
	}

	/* @[Set IGI]*/
	phydm_write_dig_reg(dm, (u8)igi);

	/* @[Backup RF Reg]*/
	dm_psd_table->rf_0x18_bkp = odm_get_rf_reg(dm, RF_PATH_A, RF_0x18,
						   RFREG_MASK);
	dm_psd_table->rf_0x18_bkp_b = odm_get_rf_reg(dm, RF_PATH_B, RF_0x18,
						     RFREG_MASK);

	if (psd_fc_channel > 14) {
		is_5G = 1;
	}

	/* Set RF fc*/
	odm_set_rf_reg(dm, RF_PATH_A, RF_0x18, 0xff, psd_fc_channel);
	odm_set_rf_reg(dm, RF_PATH_B, RF_0x18, 0xff, psd_fc_channel);
	odm_set_rf_reg(dm, RF_PATH_A, RF_0x18, 0x300, is_5G);
	odm_set_rf_reg(dm, RF_PATH_B, RF_0x18, 0x300, is_5G);


	PHYDM_DBG(dm, ODM_COMP_API, "RF0x18=((0x%x))\n",
		  odm_get_rf_reg(dm, RF_PATH_A, RF_0x18, RFREG_MASK));

	/* @[Stop 3-wires]*/
	phydm_stop_3_wire(dm, PHYDM_SET);

	ODM_delay_us(10);

	if (stop_point > (dm_psd_table->fft_smp_point - 1))
		stop_point = (dm_psd_table->fft_smp_point - 1);

	if (start_point > (dm_psd_table->fft_smp_point - 1))
		start_point = (dm_psd_table->fft_smp_point - 1);

	if (start_point > stop_point)
		stop_point = start_point;

	for (i = start_point; i <= stop_point; i++) {
		fft_max_half_bw = (dm_psd_table->fft_smp_point) >> 1;

		if (i < fft_max_half_bw)
			mod_tone_idx = i + fft_max_half_bw;
		else
			mod_tone_idx = i - fft_max_half_bw;

		psd_result_tmp = 0;
		for (t = 0; t < dm_psd_table->sw_avg_time; t++)
			psd_result_tmp += phydm_get_psd_data(dm, mod_tone_idx,
							     igi);
		psd_result =
			(u8)((psd_result_tmp / dm_psd_table->sw_avg_time)) -
			dm_psd_table->psd_pwr_common_offset;

		if (dm_psd_table->fft_smp_point == 128 &&
		    dm_psd_table->noise_k_en) {
			if (i > psd_result_cali_tone[noise_idx])
				noise_idx++;

			if (noise_idx > 6)
				noise_idx = 6;

			if (psd_result >= psd_result_cali_val[noise_idx])
				psd_result = psd_result -
					     psd_result_cali_val[noise_idx];
			else
				psd_result = 0;

			dm_psd_table->psd_result[i] = psd_result;
		}

		PHYDM_DBG(dm, ODM_COMP_API, "[%d] N_cali = %d, PSD = %d\n",
			  mod_tone_idx, psd_result_cali_val[noise_idx],
			  psd_result);
	}

	/*@[Start 3-wires]*/
	phydm_stop_3_wire(dm, PHYDM_REVERT);

	ODM_delay_us(10);

	/*@[Revert Reg]*/
	set_result = phydm_stop_ic_trx(dm, PHYDM_REVERT);

	odm_set_rf_reg(dm, RF_PATH_A, RF_0x18, RFREG_MASK,
		       dm_psd_table->rf_0x18_bkp);
	odm_set_rf_reg(dm, RF_PATH_B, RF_0x18, RFREG_MASK,
		       dm_psd_table->rf_0x18_bkp_b);

	PHYDM_DBG(dm, ODM_COMP_API, "PSD finished\n\n");

	phydm_pause_func(dm, F00_DIG, PHYDM_RESUME, PHYDM_PAUSE_LEVEL_3, 1,
			 &igi_tmp);
	dm_psd_table->psd_in_progress = 0;

	return PHYDM_SET_SUCCESS;
}

void phydm_psd_para_setting(void *dm_void, u8 sw_avg_time, u8 hw_avg_time,
			    u8 i_q_setting, u16 fft_smp_point, u8 ant_sel,
			    u8 psd_input, u8 channel, u8 noise_k_en)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct psd_info *dm_psd_table = &dm->dm_psd_table;
	u8 fft_smp_point_idx = 0;

	dm_psd_table->fft_smp_point = fft_smp_point;

	if (sw_avg_time == 0)
		sw_avg_time = 1;

	dm_psd_table->sw_avg_time = sw_avg_time;
	dm_psd_table->psd_fc_channel = channel;
	dm_psd_table->noise_k_en = noise_k_en;
	if (fft_smp_point == 128)
		fft_smp_point_idx = 0;
	else if (fft_smp_point == 256)
		fft_smp_point_idx = 1;
	else if (fft_smp_point == 512)
		fft_smp_point_idx = 2;
	else if (fft_smp_point == 1024)
		fft_smp_point_idx = 3;
}

void phydm_psd_init(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct psd_info *dm_psd_table = &dm->dm_psd_table;

	PHYDM_DBG(dm, ODM_COMP_API, "PSD para init\n");

	dm_psd_table->psd_in_progress = false;

	dm_psd_table->psd_pwr_common_offset = 0;

	phydm_psd_para_setting(dm, 1, 2, 3, 128, 0, 0, 7, 0);
}

u8 phydm_get_psd_result_table(void *dm_void, int index)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct psd_info *dm_psd_table = &dm->dm_psd_table;
	u8 result = 0;

	if (index < 128)
		result = dm_psd_table->psd_result[index];

	return result;
}

#endif
