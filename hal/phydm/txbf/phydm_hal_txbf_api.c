/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 *****************************************************************************/

#include "mp_precomp.h"
#include "phydm_precomp.h"

#if (defined(CONFIG_BB_TXBF_API))
#if (RTL8822B_SUPPORT == 1 || RTL8192F_SUPPORT == 1 || RTL8812F_SUPPORT == 1 ||\
	RTL8822C_SUPPORT == 1 || RTL8198F_SUPPORT == 1 || RTL8814B_SUPPORT == 1)
/*@Add by YuChen for 8822B MU-MIMO API*/

/*this function is only used for BFer*/
u8 phydm_get_ndpa_rate(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u8 ndpa_rate = ODM_RATE6M;

	if (dm->rssi_min >= 30) /*@link RSSI > 30%*/
		ndpa_rate = ODM_RATE24M;
	else if (dm->rssi_min <= 25)
		ndpa_rate = ODM_RATE6M;

	PHYDM_DBG(dm, DBG_TXBF, "[%s] ndpa_rate = 0x%x\n", __func__, ndpa_rate);

	return ndpa_rate;
}

/*this function is only used for BFer*/
u8 phydm_get_beamforming_sounding_info(void *dm_void, u16 *throughput,
				       u8 total_bfee_num, u8 *tx_rate)
{
	u8 idx = 0;
	u8 snddecision = 0xff;
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	for (idx = 0; idx < total_bfee_num; idx++) {
		if ((tx_rate[idx] >= ODM_RATEVHTSS2MCS7 &&
		     tx_rate[idx] <= ODM_RATEVHTSS2MCS9))
			snddecision = snddecision & ~(1 << idx);
	}

	for (idx = 0; idx < total_bfee_num; idx++) {
		if (throughput[idx] <= 10)
			snddecision = snddecision & ~(1 << idx);
	}

	PHYDM_DBG(dm, DBG_TXBF, "[%s] soundingdecision = 0x%x\n", __func__,
		  snddecision);

	return snddecision;
}

/*this function is only used for BFer*/
u8 phydm_get_mu_bfee_snding_decision(void *dm_void, u16 throughput)
{
	u8 snding_score = 0;
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	/*throughput unit is Mbps*/
	if (throughput >= 500)
		snding_score = 100;
	else if (throughput >= 450)
		snding_score = 90;
	else if (throughput >= 400)
		snding_score = 80;
	else if (throughput >= 350)
		snding_score = 70;
	else if (throughput >= 300)
		snding_score = 60;
	else if (throughput >= 250)
		snding_score = 50;
	else if (throughput >= 200)
		snding_score = 40;
	else if (throughput >= 150)
		snding_score = 30;
	else if (throughput >= 100)
		snding_score = 20;
	else if (throughput >= 50)
		snding_score = 10;
	else
		snding_score = 0;

	PHYDM_DBG(dm, DBG_TXBF, "[%s] snding_score = 0x%x\n", __func__,
		  snding_score);

	return snding_score;
}

#endif
#if (DM_ODM_SUPPORT_TYPE != ODM_AP)
u8 beamforming_get_htndp_tx_rate(void *dm_void, u8 bfer_str_num)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u8 nr_index = 0;
	u8 ndp_tx_rate;
/*@Find nr*/
	ndp_tx_rate = ODM_MGN_MCS8;

	return ndp_tx_rate;
}

u8 beamforming_get_vht_ndp_tx_rate(void *dm_void, u8 bfer_str_num)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u8 nr_index = 0;
	u8 ndp_tx_rate;
/*@Find nr*/
	ndp_tx_rate = ODM_MGN_VHT2SS_MCS0;

	return ndp_tx_rate;
}
#endif

#ifdef PHYDM_IC_JGR3_SERIES_SUPPORT
/*this function is only used for BFer*/
void phydm_txbf_rfmode(void *dm_void, u8 su_bfee_cnt, u8 mu_bfee_cnt)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u8 i;

	if (dm->rf_type == RF_1T1R)
		return;
	if (su_bfee_cnt > 0 || mu_bfee_cnt > 0) {
		/*Path A ==================*/
		/*RF mode table write enable*/
		odm_set_rf_reg(dm, RF_PATH_A, RF_0xef, BIT(19), 0x1);
		/*Select RX mode*/
		odm_set_rf_reg(dm, RF_PATH_A, RF_0x33, 0xF, 3);
		/*Set Table data*/
		odm_set_rf_reg(dm, RF_PATH_A, RF_0x3e, 0x3, 0x2);
		/*Set Table data*/
		odm_set_rf_reg(dm, RF_PATH_A, RF_0x3f, 0xfffff,
			       0x65AFF);
		/*RF mode table write disable*/
		odm_set_rf_reg(dm, RF_PATH_A, RF_0xef, BIT(19), 0x0);

		/*Path B ==================*/
		/*RF mode table write enable*/
		odm_set_rf_reg(dm, RF_PATH_B, RF_0xef, BIT(19), 0x1);
		/*Select RX mode*/
		odm_set_rf_reg(dm, RF_PATH_B, RF_0x33, 0xF, 3);
		/*Set Table data*/
		odm_set_rf_reg(dm, RF_PATH_B, RF_0x3f, 0xfffff,
			       0x996BF);
		/*Select Standby mode*/
		odm_set_rf_reg(dm, RF_PATH_B, RF_0x33, 0xF, 1);
		/*Set Table data*/
		odm_set_rf_reg(dm, RF_PATH_B, RF_0x3f, 0xfffff,
			       0x99230);
		/*RF mode table write disable*/
		odm_set_rf_reg(dm, RF_PATH_B, RF_0xef, BIT(19), 0x0);
	}

	/*@if Nsts > Nc, don't apply V matrix*/
	odm_set_bb_reg(dm, R_0x1e24, BIT(11), 1);

	if (su_bfee_cnt > 0 || mu_bfee_cnt > 0) {
		/*@enable BB TxBF ant mapping register*/
		odm_set_bb_reg(dm, R_0x1e24, BIT(28) | BIT29, 0x2);
		odm_set_bb_reg(dm, R_0x1e24, BIT(30), 1);

		/* logic mapping */
		/* TX BF logic map and TX path en for Nsts = 1~2 */
		odm_set_bb_reg(dm, R_0x820, 0xff, 0x33);
		odm_set_bb_reg(dm, R_0x1e2c, 0xffff, 0x404);
		odm_set_bb_reg(dm, R_0x820, 0xffff0000, 0x33);
		odm_set_bb_reg(dm, R_0x1e30, 0xffff, 0x404);
	} else {
		/*@Disable BB TxBF ant mapping register*/
		odm_set_bb_reg(dm, R_0x1e24, BIT(28) | BIT29, 0x0);
		odm_set_bb_reg(dm, R_0x1e24, BIT(31), 0);
		/*@1SS~2ss A, AB*/
		odm_set_bb_reg(dm, R_0x820, 0xff, 0x31);
		odm_set_bb_reg(dm, R_0x1e2c, 0xffff, 0x400);
	}
}

void phydm_mu_rsoml_reset(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_bf_rate_info_jgr3 *rateinfo = &dm->bf_rate_info_jgr3;

	PHYDM_DBG(dm, DBG_TXBF, "[MU RSOML] %s cnt reset\n", __func__);

	odm_memory_set(dm, &rateinfo->num_mu_vht_pkt[0], 0, VHT_RATE_NUM * 2);
	odm_memory_set(dm, &rateinfo->num_qry_vht_pkt[0], 0, VHT_RATE_NUM * 2);
}
#endif /*PHYSTS_3RD_TYPE_IC*/

void phydm_txbf_avoid_hang(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	
	/* avoid CCK CCA hang when the BF mode */
#ifdef PHYDM_IC_JGR3_SERIES_SUPPORT	
	odm_set_bb_reg(dm, R_0x1e6c, 0x100000, 0x1);
#endif

	/* avoid CCK CCA hang when the BFee mode for 92F */
#if (RTL8192F_SUPPORT == 1)
	odm_set_bb_reg(dm, R_0xa70, 0xffff0000, 0x80ff);
#endif
}

void phydm_bf_debug(void *dm_void, char input[][16], u32 *_used, char *output,
		    u32 *_out_len)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	char help[] = "-h";
	u32 var1[3] = {0};
	u32 i;

	if ((strcmp(input[1], help) == 0)) {
		PDM_SNPF(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "{BF ver1 :0}, {NO applyV:0; applyV:1; default:2}\n");
		PDM_SNPF(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "{MU RSOML:1}, {MU enable:1/0}, {MU Ratio:40}\n");
		PDM_SNPF(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "{MU TRxPath:2}, {TRxPath enable:1/0}\n");
		return;
	}
	for (i = 0; i < 3; i++) {
		PHYDM_SSCANF(input[i + 1], DCMD_DECIMAL, &var1[i]);
	}
	if (var1[0] == 0) {
		#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN | ODM_CE))
		#ifdef PHYDM_BEAMFORMING_SUPPORT
		struct _RT_BEAMFORMING_INFO *beamforming_info = NULL;

		beamforming_info = &dm->beamforming_info;

		if (var1[1] == 0) {
			beamforming_info->apply_v_matrix = false;
			beamforming_info->snding3ss = true;
			PDM_SNPF(*_out_len, *_used, output + *_used,
				 *_out_len - *_used,
				"\r\n dont apply V matrix and 3SS 789 snding\n");
		} else if (var1[1] == 1) {
			beamforming_info->apply_v_matrix = true;
			beamforming_info->snding3ss = true;
			PDM_SNPF(*_out_len, *_used, output + *_used,
				 *_out_len - *_used,
				 "\r\n apply V matrix and 3SS 789 snding\n");
		} else if (var1[1] == 2) {
			beamforming_info->apply_v_matrix = true;
			beamforming_info->snding3ss = false;
			PDM_SNPF(*_out_len, *_used, output + *_used,
				 *_out_len - *_used,
				 "\r\n default txbf setting\n");
		} else {
			PDM_SNPF(*_out_len, *_used, output + *_used,
				 *_out_len - *_used,
				 "\r\n unknown cmd!!\n");
		}
		#endif
		#endif
	} else if (var1[0] == 1) {
		#ifdef PHYDM_IC_JGR3_SERIES_SUPPORT
		struct dm_struct *dm = (struct dm_struct *)dm_void;
		struct phydm_bf_rate_info_jgr3 *bfinfo = &dm->bf_rate_info_jgr3;

		bfinfo->enable = (u8)var1[1];
		bfinfo->mu_ratio_th = (u8)var1[2];
		PDM_SNPF(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "[MU RSOML] enable= %d, MU ratio TH= %d\n",
			 bfinfo->enable, bfinfo->mu_ratio_th);
		#endif
	} else if (var1[0] == 2) {
		#ifdef PHYDM_IC_JGR3_SERIES_SUPPORT
		struct dm_struct *dm = (struct dm_struct *)dm_void;
		struct phydm_bf_rate_info_jgr3 *bfinfo = &dm->bf_rate_info_jgr3;

		bfinfo->mu_set_trxpath = (u8)var1[1];
		PDM_SNPF(*_out_len, *_used, output + *_used, *_out_len - *_used,
			 "[MU TRxPath] mu_set_trxpath = %d\n",
			 bfinfo->mu_set_trxpath);
		#endif
	}
}

#endif /*CONFIG_BB_TXBF_API*/
