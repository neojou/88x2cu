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

/*************************************************************
 * include files
 ************************************************************/
#include "mp_precomp.h"
#include "phydm_precomp.h"

/**************************************************
 * This function is for inband noise test utility only
 * To obtain the inband noise level(dbm), do the following.
 * 1. disable DIG and Power Saving
 * 2. Set initial gain = 0x1a
 * 3. Stop updating idle time pwer report (for driver read)
 *	- 0x80c[25]
 *
 *************************************************/

void phydm_set_noise_data_sum(struct noise_level *noise_data, u8 max_rf_path)
{
	u8 i = 0;

	for (i = RF_PATH_A; i < max_rf_path; i++) {
		if (noise_data->valid_cnt[i])
			noise_data->sum[i] /= noise_data->valid_cnt[i];
		else
			noise_data->sum[i] = 0;
	}
}

s16 odm_inband_noise_monitor(void *dm_void, u8 pause_dig, u8 igi,
			     u32 max_time)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	s16 val = 0;

	igi = 0x32;

	/* since HW ability is about +15~-35,
	 * we fix IGI = -60 for maximum coverage
	 */

	return val;
}

void phydm_noisy_detection(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 total_fa_cnt, total_cca_cnt;
	u32 score = 0, i, score_smooth;

	total_cca_cnt = dm->false_alm_cnt.cnt_cca_all;
	total_fa_cnt = dm->false_alm_cnt.cnt_all;

#if 0
	if (total_fa_cnt * 16 >= total_cca_cnt * 14)    /*  @87.5 */
		;
	else if (total_fa_cnt * 16 >= total_cca_cnt * 12) /*  @75 */
		;
	else if (total_fa_cnt * 16 >= total_cca_cnt * 10) /*  @56.25 */
		;
	else if (total_fa_cnt * 16 >= total_cca_cnt * 8) /*  @50 */
		;
	else if (total_fa_cnt * 16 >= total_cca_cnt * 7) /*  @43.75 */
		;
	else if (total_fa_cnt * 16 >= total_cca_cnt * 6) /*  @37.5 */
		;
	else if (total_fa_cnt * 16 >= total_cca_cnt * 5) /*  @31.25% */
		;
	else if (total_fa_cnt * 16 >= total_cca_cnt * 4) /*  @25% */
		;
	else if (total_fa_cnt * 16 >= total_cca_cnt * 3) /*  @18.75% */
		;
	else if (total_fa_cnt * 16 >= total_cca_cnt * 2) /*  @12.5% */
		;
	else if (total_fa_cnt * 16 >= total_cca_cnt * 1) /*  @6.25% */
		;
#endif
	for (i = 0; i <= 16; i++) {
		if (total_fa_cnt * 16 >= total_cca_cnt * (16 - i)) {
			score = 16 - i;
			break;
		}
	}

	/* noisy_decision_smooth = noisy_decision_smooth>>1 + (score<<3)>>1; */
	dm->noisy_decision_smooth = (dm->noisy_decision_smooth >> 1) +
				    (score << 2);

	/* Round the noisy_decision_smooth: +"3" comes from (2^3)/2-1 */
	if (total_cca_cnt >= 300)
		score_smooth = (dm->noisy_decision_smooth + 3) >> 3;
	else
		score_smooth = 0;

	dm->noisy_decision = (score_smooth >= 3) ? 1 : 0;

	PHYDM_DBG(dm, DBG_ENV_MNTR,
		  "[NoisyDetection] CCA_cnt=%d,FA_cnt=%d, noisy_dec_smooth=%d, score=%d, score_smooth=%d, noisy_dec=%d\n",
		  total_cca_cnt, total_fa_cnt, dm->noisy_decision_smooth, score,
		  score_smooth, dm->noisy_decision);
}
