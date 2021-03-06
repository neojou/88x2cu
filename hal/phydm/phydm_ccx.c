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

u8 phydm_env_mntr_get_802_11_k_rsni(void *dm_void, s8 rcpi, s8 anpi)
{
	u8 rsni = 0;
	u8 signal = 0;
	u8 sig_to_rsni[13] = {0, 8, 15, 20, 24, 27, 30, 32, 35, 37, 39, 41, 43};

	/*rcpi = signal + noise + interference = rssi*/
	/*anpi = noise + interferecne = nhm*/
	/*signal = rcpi - anpi*/

	/*rsni = 2*(10*log10((rcpi_lin/anpi_lin)-1)+10), unit = 0.5dB*/
	/*rcpi_lin/anpi_lin=10^((rcpi_dB-anpi_db)/10)*/
	/*rsni is approximated as 2*((rcpi_db-anpi_db)+10) when signal >= 13*/

	if (rcpi <= anpi)
		signal = 0;
	else if (rcpi - anpi >= 117)
		signal = 117;
	else
		signal = rcpi - anpi;

	if (signal < 13)
		rsni = sig_to_rsni[signal];
	else
		rsni = 2 * (signal + 10);

	return rsni;
}

u8 phydm_ccx_get_rpt_ratio(void *dm_void, u16 rpt, u16 denom)
{
	u32 numer = 0;

	numer = rpt * 100 + (denom >> 1);

	return (u8)PHYDM_DIV(numer, denom);
}

#ifdef NHM_SUPPORT

void phydm_nhm_trigger(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct ccx_info *ccx = &dm->dm_ccx_info;
	u32 nhm_reg1 = 0;

	nhm_reg1 = R_0x1e60;
	PHYDM_DBG(dm, DBG_ENV_MNTR, "[%s]===>\n", __func__);

	/* @Trigger NHM*/
	pdm_set_reg(dm, nhm_reg1, BIT(1), 0);
	pdm_set_reg(dm, nhm_reg1, BIT(1), 1);
	ccx->nhm_trigger_time = dm->phydm_sys_up_time;
	ccx->nhm_rpt_stamp++;
	ccx->nhm_ongoing = true;
}

boolean
phydm_nhm_check_rdy(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	boolean is_ready = false;
	u32 reg1 = 0, reg1_bit = 0;

	reg1 = R_0x2d4c;
	reg1_bit = 16;
	if (odm_get_bb_reg(dm, reg1, BIT(reg1_bit)))
		is_ready = true;

	PHYDM_DBG(dm, DBG_ENV_MNTR, "NHM rdy=%d\n", is_ready);

	return is_ready;
}

u8 phydm_nhm_cal_wgt_avg(void *dm_void, u8 start_i, u8 end_i, u8 n_sum)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct ccx_info *ccx = &dm->dm_ccx_info;
	u8 i = 0;
	u32 noise_tmp = 0;
	u8 noise = 0;
	u32 nhm_valid = 0;

	if (n_sum == 0) {
		PHYDM_DBG(dm, DBG_ENV_MNTR,
			  "n_sum = 0, don't need to update noise\n");
		return 0x0;
	} else if (end_i > NHM_RPT_NUM - 1) {
		PHYDM_DBG(dm, DBG_ENV_MNTR,
			  "[WARNING]end_i is larger than 11!!\n");
		return 0x0;
	}

	for (i = start_i; i <= end_i; i++)
		noise_tmp += ccx->nhm_result[i] * ccx->nhm_wgt[i];

	/* protection for the case of minus noise(RSSI)*/
	noise = (u8)(NTH_TH_2_RSSI(MAX_2(PHYDM_DIV(noise_tmp, n_sum), 20)));
	nhm_valid = (n_sum * 100) >> 8;
	PHYDM_DBG(dm, DBG_ENV_MNTR,
		  "cal wgt_avg : valid: ((%d)) percent, noise(RSSI)=((%d))\n",
		  nhm_valid, noise);

	return noise;
}

u8 phydm_nhm_cal_nhm_env(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct ccx_info *ccx = &dm->dm_ccx_info;
	u8 first_idx = 0;
	u8 nhm_env = 0;
	u8 i = 0;

	nhm_env = ccx->nhm_rpt_sum;

	/*search first cluster*/
	for (i = 0; i < NHM_RPT_NUM; i++) {
		if (ccx->nhm_result[i]) {
			first_idx = i;
			break;
		}
	}

	/*exclude first cluster under -80dBm*/
	for (i = 0; i < 4; i++) {
		if (((first_idx + i) < NHM_RPT_NUM) &&
		    (ccx->nhm_wgt[first_idx + i] <= NHM_IC_NOISE_TH))
			nhm_env -= ccx->nhm_result[first_idx + i];
	}

	/*exclude nhm_rpt[0] above -80dBm*/
	if (ccx->nhm_wgt[0] > NHM_IC_NOISE_TH)
		nhm_env -= ccx->nhm_result[0];

	PHYDM_DBG(dm, DBG_ENV_MNTR, "cal nhm_env: first_idx=%d, nhm_env=%d\n",
		  first_idx, nhm_env);

	return nhm_env;
}

#ifdef NHM_DYM_PW_TH_SUPPORT
void
phydm_nhm_restore_pw_th(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct ccx_info *ccx = &dm->dm_ccx_info;

	odm_set_bb_reg(dm, R_0x82c, 0x3f, ccx->pw_th_rf20_ori);
}

void
phydm_nhm_set_pw_th(void *dm_void, u8 noise, boolean chk_succ)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct ccx_info *ccx = &dm->dm_ccx_info;
	boolean not_update = false;
	u8 pw_th_rf20_new = 0;
	u8 pw_th_u_bnd = 0;
	s8 noise_diff = 0;
	u8 point_mean = 15;

	PHYDM_DBG(dm, DBG_ENV_MNTR, "[%s]===>\n", __func__);

	if (*dm->band_width != CHANNEL_WIDTH_20 ||
	    *dm->band_type == ODM_BAND_5G) {
		PHYDM_DBG(dm, DBG_ENV_MNTR,  "bandwidth=((%d)), band=((%d))\n",
			  *dm->band_width, *dm->band_type);
		phydm_nhm_restore_pw_th(dm);
		return;
	}

	if (chk_succ) {
		noise_diff = noise - (ccx->nhm_igi - 10);
		pw_th_u_bnd = (u8)(noise_diff + 32 + point_mean);

		pw_th_u_bnd = MIN_2(pw_th_u_bnd, ccx->nhm_pw_th_max);

		PHYDM_DBG(dm, DBG_ENV_MNTR,
			  "noise_diff=((%d)), max=((%d)), pw_th_u_bnd=((%d))\n",
			  noise_diff, ccx->nhm_pw_th_max, pw_th_u_bnd);

		if (pw_th_u_bnd > ccx->pw_th_rf20_cur) {
			pw_th_rf20_new = ccx->pw_th_rf20_cur + 1;
		} else if (pw_th_u_bnd < ccx->pw_th_rf20_cur) {
			if (ccx->pw_th_rf20_cur > ccx->pw_th_rf20_ori)
				pw_th_rf20_new = ccx->pw_th_rf20_cur - 1;
			else /*ccx->pw_th_rf20_cur == ccx->pw_th_ori*/
				not_update = true;
		} else {/*pw_th_u_bnd == ccx->pw_th_rf20_cur*/
			not_update = true;
		}
	} else {
		if (ccx->pw_th_rf20_cur > ccx->pw_th_rf20_ori)
			pw_th_rf20_new = ccx->pw_th_rf20_cur - 1;
		else /*ccx->pw_th_rf20_cur == ccx->pw_th_ori*/
			not_update = true;
	}

	PHYDM_DBG(dm, DBG_ENV_MNTR, "pw_th_cur=((%d)), pw_th_new=((%d))\n",
		  ccx->pw_th_rf20_cur, pw_th_rf20_new);

	if (!not_update) {
		odm_set_bb_reg(dm, R_0x82c, 0x3f, pw_th_rf20_new);
		ccx->pw_th_rf20_cur = pw_th_rf20_new;
	}
}

void
phydm_nhm_dym_pw_th(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct ccx_info *ccx = &dm->dm_ccx_info;
	u8 i = 0;
	u8 n_sum = 0;
	u8 noise = 0;
	boolean chk_succ = false;

	PHYDM_DBG(dm, DBG_ENV_MNTR, "[%s]===>\n", __func__);

	for (i = 0; i < NHM_RPT_NUM - 3; i++) {
		n_sum = ccx->nhm_result[i] + ccx->nhm_result[i + 1] +
			ccx->nhm_result[i + 2] + ccx->nhm_result[i + 3];
		if (n_sum >= ccx->nhm_sl_pw_th) {
			PHYDM_DBG(dm, DBG_ENV_MNTR, "Do sl[%d:%d]\n", i, i + 3);
			chk_succ = true;
			noise = phydm_nhm_cal_wgt_avg(dm, i, i + 3, n_sum);
			break;
		}
	}

	if (!chk_succ)
		PHYDM_DBG(dm, DBG_ENV_MNTR, "SL method failed!\n");

	phydm_nhm_set_pw_th(dm, noise, chk_succ);
}

#endif

/*Environment Monitor*/
boolean
phydm_nhm_mntr_racing_chk(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct ccx_info *ccx = &dm->dm_ccx_info;
	u32 sys_return_time = 0;

	if (ccx->nhm_manual_ctrl) {
		PHYDM_DBG(dm, DBG_ENV_MNTR, "NHM in manual ctrl\n");
		return true;
	}

	sys_return_time = ccx->nhm_trigger_time + MAX_ENV_MNTR_TIME;

	if (ccx->nhm_app != NHM_BACKGROUND &&
	    (sys_return_time > dm->phydm_sys_up_time)) {
		PHYDM_DBG(dm, DBG_ENV_MNTR,
			  "nhm_app=%d, trigger_time %d, sys_time=%d\n",
			  ccx->nhm_app, ccx->nhm_trigger_time,
			  dm->phydm_sys_up_time);

		return true;
	}

	return false;
}

void phydm_nhm_init(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct ccx_info *ccx = &dm->dm_ccx_info;
	u8 igi_curr = phydm_get_igi(dm, BB_PATH_A);

	ccx->nhm_app = NHM_BACKGROUND;
	ccx->nhm_igi = 0xff;

	/*Set NHM threshold*/
	ccx->nhm_ongoing = false;
	ccx->nhm_set_lv = NHM_RELEASE;

	ccx->nhm_igi = (u32)igi_curr;
	ccx->nhm_period = 0;

	ccx->nhm_include_cca = NHM_CCA_INIT;
	ccx->nhm_include_txon = NHM_TXON_INIT;
	ccx->nhm_divider_opt = NHM_CNT_INIT;

	ccx->nhm_manual_ctrl = 0;
	ccx->nhm_rpt_stamp = 0;

	#ifdef NHM_DYM_PW_TH_SUPPORT
	ccx->nhm_dym_pw_th_en = false;
	ccx->pw_th_rf20_ori = (u8)odm_get_bb_reg(dm, R_0x82c, 0x3f);
	ccx->pw_th_rf20_cur = ccx->pw_th_rf20_ori;
	ccx->nhm_pw_th_max = 63;
	ccx->nhm_sl_pw_th = 100; /*39%*/
	ccx->nhm_period_decre = 1;
	ccx->dym_pwth_manual_ctrl = false;
	#endif
}

#endif /*@#ifdef NHM_SUPPORT*/

#ifdef CLM_SUPPORT

void phydm_clm_racing_release(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct ccx_info *ccx = &dm->dm_ccx_info;

	PHYDM_DBG(dm, DBG_ENV_MNTR, "[%s]===>\n", __func__);
	PHYDM_DBG(dm, DBG_ENV_MNTR, "lv:(%d)->(0)\n", ccx->clm_set_lv);

	ccx->clm_ongoing = false;
	ccx->clm_set_lv = CLM_RELEASE;
	ccx->clm_app = CLM_BACKGROUND;
}

void phydm_clm_c2h_report_handler(void *dm_void, u8 *cmd_buf, u8 cmd_len)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct ccx_info *ccx_info = &dm->dm_ccx_info;
	u8 clm_report = cmd_buf[0];
	/*@u8 clm_report_idx = cmd_buf[1];*/

	if (cmd_len >= 12)
		return;

	ccx_info->clm_fw_result_acc += clm_report;
	ccx_info->clm_fw_result_cnt++;

	PHYDM_DBG(dm, DBG_ENV_MNTR, "[%d] clm_report= %d\n",
		  ccx_info->clm_fw_result_cnt, clm_report);
}

void phydm_clm_h2c(void *dm_void, u16 obs_time, u8 fw_clm_en)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u8 h2c_val[H2C_MAX_LENGTH] = {0};
	u8 i = 0;
	u8 obs_time_idx = 0;

	PHYDM_DBG(dm, DBG_ENV_MNTR, "[%s] ======>\n", __func__);
	PHYDM_DBG(dm, DBG_ENV_MNTR, "obs_time_index=%d *4 us\n", obs_time);

	for (i = 1; i <= 16; i++) {
		if (obs_time & BIT(16 - i)) {
			obs_time_idx = 16 - i;
			break;
		}
	}
#if 0
	obs_time = (2 ^ 16 - 1)~(2 ^ 15)  => obs_time_idx = 15  (65535 ~32768)
	obs_time = (2 ^ 15 - 1)~(2 ^ 14)  => obs_time_idx = 14
	...
	...
	...
	obs_time = (2 ^ 1 - 1)~(2 ^ 0)  => obs_time_idx = 0

#endif

	h2c_val[0] = obs_time_idx | (((fw_clm_en) ? 1 : 0) << 7);
	h2c_val[1] = CLM_MAX_REPORT_TIME;

	PHYDM_DBG(dm, DBG_ENV_MNTR, "PHYDM h2c[0x4d]=0x%x %x %x %x %x %x %x\n",
		  h2c_val[6], h2c_val[5], h2c_val[4], h2c_val[3], h2c_val[2],
		  h2c_val[1], h2c_val[0]);

	odm_fill_h2c_cmd(dm, PHYDM_H2C_FW_CLM_MNTR, H2C_MAX_LENGTH, h2c_val);
}

boolean
phydm_clm_check_rdy(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	boolean is_ready = false;
	u32 reg1 = 0, reg1_bit = 0;

	reg1 = R_0x2d88;
	reg1_bit = 16;
	if (odm_get_bb_reg(dm, reg1, BIT(reg1_bit)))
		is_ready = true;

	PHYDM_DBG(dm, DBG_ENV_MNTR, "CLM rdy=%d\n", is_ready);

	return is_ready;
}

void phydm_clm_get_utility(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct ccx_info *ccx = &dm->dm_ccx_info;

	if (ccx->clm_period == 0) {
		PHYDM_DBG(dm, DBG_ENV_MNTR, "[warning] clm_period = 0\n");
		ccx->clm_ratio = 0;
	} else {
		ccx->clm_ratio = phydm_ccx_get_rpt_ratio(dm, ccx->clm_result,
							 ccx->clm_period);
	}
}

boolean
phydm_clm_get_result(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct ccx_info *ccx_info = &dm->dm_ccx_info;
	u32 reg1 = 0;
	u32 val = 0;

	reg1 = R_0x1e60;
	if (!(phydm_clm_check_rdy(dm))) {
		PHYDM_DBG(dm, DBG_ENV_MNTR, "Get CLM report Fail\n");
		phydm_clm_racing_release(dm);
		return false;
	}

	val = odm_get_bb_reg(dm, R_0x2d88, MASKLWORD);
	ccx_info->clm_result = (u16)val;

	PHYDM_DBG(dm, DBG_ENV_MNTR, "CLM result = %d *4 us\n",
		  ccx_info->clm_result);
	phydm_clm_racing_release(dm);
	return true;
}

boolean
phydm_clm_mntr_racing_chk(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct ccx_info *ccx = &dm->dm_ccx_info;
	u32 sys_return_time = 0;

	if (ccx->clm_manual_ctrl) {
		PHYDM_DBG(dm, DBG_ENV_MNTR, "CLM in manual ctrl\n");
		return true;
	}

	sys_return_time = ccx->clm_trigger_time + MAX_ENV_MNTR_TIME;

	if (ccx->clm_app != CLM_BACKGROUND &&
	    (sys_return_time > dm->phydm_sys_up_time)) {
		PHYDM_DBG(dm, DBG_ENV_MNTR,
			  "clm_app=%d, trigger_time %d, sys_time=%d\n",
			  ccx->clm_app, ccx->clm_trigger_time,
			  dm->phydm_sys_up_time);

		return true;
	}

	return false;
}

boolean
phydm_clm_mntr_result(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct ccx_info *ccx = &dm->dm_ccx_info;
	boolean clm_chk_result = false;
	u32 val = 0;

	PHYDM_DBG(dm, DBG_ENV_MNTR, "[%s] ======>\n", __func__);

	if (phydm_clm_mntr_racing_chk(dm))
		return clm_chk_result;

	if (ccx->clm_mntr_mode == CLM_DRIVER_MNTR) {
		if (phydm_clm_get_result(dm)) {
			PHYDM_DBG(dm, DBG_ENV_MNTR, "Get CLM_rpt success\n");
			phydm_clm_get_utility(dm);
			clm_chk_result = true;
		}
	} else {
		if (ccx->clm_fw_result_cnt != 0) {
			val = ccx->clm_fw_result_acc / ccx->clm_fw_result_cnt;
			ccx->clm_ratio = (u8)val;
			clm_chk_result = true;
		} else {
			ccx->clm_ratio = 0;
		}

		PHYDM_DBG(dm, DBG_ENV_MNTR,
			  "clm_fw_result_acc=%d, clm_fw_result_cnt=%d\n",
			  ccx->clm_fw_result_acc, ccx->clm_fw_result_cnt);

		ccx->clm_fw_result_acc = 0;
		ccx->clm_fw_result_cnt = 0;
	}

	PHYDM_DBG(dm, DBG_ENV_MNTR, "clm_ratio=%d\n", ccx->clm_ratio);

	return clm_chk_result;
}

void phydm_clm_init(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct ccx_info *ccx = &dm->dm_ccx_info;

	PHYDM_DBG(dm, DBG_ENV_MNTR, "[%s]===>\n", __func__);
	ccx->clm_ongoing = false;
	ccx->clm_manual_ctrl = 0;
	ccx->clm_mntr_mode = CLM_DRIVER_MNTR;
	ccx->clm_period = 0;
	ccx->clm_rpt_stamp = 0;

	ccx->clm_period = 65535;
}

#endif /*@#ifdef CLM_SUPPORT*/

#ifdef FAHM_SUPPORT

void phydm_fahm_trigger(void *dm_void)
{ /*@unit (4us)*/
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct ccx_info *ccx = &dm->dm_ccx_info;
	u32 reg = 0;

	PHYDM_DBG(dm, DBG_ENV_MNTR, "[%s]===>\n", __func__);

	switch (dm->ic_ip_series) {
	case PHYDM_IC_JGR3:
		reg = R_0x1e60;
		break;
	case PHYDM_IC_AC:
		reg = R_0x994;
		break;
	case PHYDM_IC_N:
		reg = R_0x890;
		break;
	default:
		break;
	}

	odm_set_bb_reg(dm, reg, BIT(2), 0);
	odm_set_bb_reg(dm, reg, BIT(2), 1);

	ccx->fahm_trigger_time = dm->phydm_sys_up_time;
	ccx->fahm_rpt_stamp++;
	ccx->fahm_ongoing = true;
}

boolean
phydm_fahm_check_rdy(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	boolean is_ready = false;
	u32 reg = 0, reg_bit = 0;

	switch (dm->ic_ip_series) {
	case PHYDM_IC_JGR3:
		reg = R_0x2d84;
		reg_bit = 31;
		break;
	case PHYDM_IC_AC:
		reg = R_0x1f98;
		reg_bit = 31;
		break;
	case PHYDM_IC_N:
		reg = R_0x9f0;
		reg_bit = 31;
		break;
	default:
		break;
	}

	if (odm_get_bb_reg(dm, reg, BIT(reg_bit)))
		is_ready = true;

	PHYDM_DBG(dm, DBG_ENV_MNTR, "FAHM rdy=%d\n", is_ready);

	return is_ready;
}

boolean
phydm_fahm_mntr_racing_chk(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct ccx_info *ccx = &dm->dm_ccx_info;
	u32 sys_return_time = 0;

	if (ccx->fahm_manual_ctrl) {
		PHYDM_DBG(dm, DBG_ENV_MNTR, "FAHM in manual ctrl\n");
		return true;
	}

	sys_return_time = ccx->fahm_trigger_time + MAX_ENV_MNTR_TIME;

	if (ccx->fahm_app != FAHM_BACKGROUND &&
	    (sys_return_time > dm->phydm_sys_up_time)) {
		PHYDM_DBG(dm, DBG_ENV_MNTR,
			  "fahm_app=%d, trigger_time %d, sys_time=%d\n",
			  ccx->fahm_app, ccx->fahm_trigger_time,
			  dm->phydm_sys_up_time);

		return true;
	}

	return false;
}

void phydm_fahm_init(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct ccx_info *ccx = &dm->dm_ccx_info;
	boolean is_update = false;
	u32 reg = 0;


	ccx->fahm_app = FAHM_BACKGROUND;
	ccx->fahm_igi = 0xff;

	/*Set FAHM threshold*/
	ccx->fahm_ongoing = false;
	ccx->fahm_set_lv = FAHM_RELEASE;
	ccx->fahm_igi = phydm_get_igi(dm, BB_PATH_A);
	ccx->fahm_period = 0;
	ccx->fahm_numer_opt = 0;
	ccx->fahm_denom_opt = 0;
	ccx->fahm_manual_ctrl = 0;
	ccx->fahm_rpt_stamp = 0;
	ccx->fahm_inclu_cck = false;
}

#endif /*#ifdef FAHM_SUPPORT*/

#ifdef IFS_CLM_SUPPORT
void phydm_ifs_clm_racing_release(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct ccx_info *ccx = &dm->dm_ccx_info;

	PHYDM_DBG(dm, DBG_ENV_MNTR, "[%s]===>\n", __func__);
	PHYDM_DBG(dm, DBG_ENV_MNTR, "ifs clm lv:(%d)->(0)\n",
		  ccx->ifs_clm_set_lv);

	ccx->ifs_clm_ongoing = false;
	ccx->ifs_clm_set_lv = IFS_CLM_RELEASE;
	ccx->ifs_clm_app = IFS_CLM_BACKGROUND;
}

void phydm_ifs_clm_trigger(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct ccx_info *ccx = &dm->dm_ccx_info;

	PHYDM_DBG(dm, DBG_ENV_MNTR, "[%s]===>\n", __func__);

	/*Trigger IFS_CLM*/
	pdm_set_reg(dm, R_0x1ee4, BIT(29), 0);
	pdm_set_reg(dm, R_0x1ee4, BIT(29), 1);
	ccx->ifs_clm_trigger_time = dm->phydm_sys_up_time;
	ccx->ifs_clm_rpt_stamp++;
	ccx->ifs_clm_ongoing = true;
}

void phydm_ifs_clm_get_utility(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct ccx_info *ccx = &dm->dm_ccx_info;
	u16 denom = 0;

	PHYDM_DBG(dm, DBG_ENV_MNTR, "[%s]===>\n", __func__);

	denom = ccx->ifs_clm_period;
	ccx->ifs_clm_tx_ratio = phydm_ccx_get_rpt_ratio(dm, ccx->ifs_clm_tx,
				denom);
	ccx->ifs_clm_edcca_excl_cca_ratio = phydm_ccx_get_rpt_ratio(dm,
					    ccx->ifs_clm_edcca_excl_cca,
					    denom);
	ccx->ifs_clm_cck_fa_ratio = phydm_ccx_get_rpt_ratio(dm,
				    ccx->ifs_clm_cckfa, denom);
	ccx->ifs_clm_ofdm_fa_ratio = phydm_ccx_get_rpt_ratio(dm,
				     ccx->ifs_clm_ofdmfa, denom);
	ccx->ifs_clm_cck_cca_excl_fa_ratio = phydm_ccx_get_rpt_ratio(dm,
					     ccx->ifs_clm_cckcca_excl_fa,
					     denom);
	ccx->ifs_clm_ofdm_cca_excl_fa_ratio = phydm_ccx_get_rpt_ratio(dm,
					      ccx->ifs_clm_ofdmcca_excl_fa,
					      denom);

	PHYDM_DBG(dm, DBG_ENV_MNTR,
		  "Tx_ratio = %d, EDCCA_exclude_CCA_ratio = %d \n",
		  ccx->ifs_clm_tx_ratio, ccx->ifs_clm_edcca_excl_cca_ratio);
	PHYDM_DBG(dm, DBG_ENV_MNTR,
		  "CCK : FA_ratio = %d, CCA_exclude_FA_ratio = %d \n",
		  ccx->ifs_clm_cck_fa_ratio,
		  ccx->ifs_clm_cck_cca_excl_fa_ratio);
	PHYDM_DBG(dm, DBG_ENV_MNTR,
		  "OFDM : FA_ratio = %d, CCA_exclude_FA_ratio = %d \n",
		  ccx->ifs_clm_ofdm_fa_ratio,
		  ccx->ifs_clm_ofdm_cca_excl_fa_ratio);
}

void phydm_ifs_clm_get_result(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct ccx_info *ccx = &dm->dm_ccx_info;
	u32 value32 = 0;
	u8 i = 0;

	PHYDM_DBG(dm, DBG_ENV_MNTR, "[%s]===>\n", __func__);

	/*Enhance CLM result*/
	value32 = odm_get_bb_reg(dm, R_0x2e60, MASKDWORD);
	ccx->ifs_clm_tx = (u16)(value32 & MASKLWORD);
	ccx->ifs_clm_edcca_excl_cca = (u16)((value32 & MASKHWORD) >> 16);
	value32 = odm_get_bb_reg(dm, R_0x2e64, MASKDWORD);
	ccx->ifs_clm_ofdmfa = (u16)(value32 & MASKLWORD);
	ccx->ifs_clm_ofdmcca_excl_fa = (u16)((value32 & MASKHWORD) >> 16);
	value32 = odm_get_bb_reg(dm, R_0x2e68, MASKDWORD);
	ccx->ifs_clm_cckfa = (u16)(value32 & MASKLWORD);
	ccx->ifs_clm_cckcca_excl_fa = (u16)((value32 & MASKHWORD) >> 16);
	value32 = odm_get_bb_reg(dm, R_0x2e6c, MASKDWORD);
	ccx->ifs_clm_total_cca = (u16)(value32 & MASKLWORD);

	/* IFS result */
	value32 = odm_get_bb_reg(dm, R_0x2e70, MASKDWORD);
	odm_move_memory(dm, &ccx->ifs_clm_his[0], &value32, 4);
	value32 = odm_get_bb_reg(dm, R_0x2e74, MASKDWORD);
	ccx->ifs_clm_avg[0] = (u16)(value32 & MASKLWORD);
	ccx->ifs_clm_avg[1] = (u16)((value32 & MASKHWORD) >> 16);
	value32 = odm_get_bb_reg(dm, R_0x2e78, MASKDWORD);
	ccx->ifs_clm_avg[2] = (u16)(value32 & MASKLWORD);
	ccx->ifs_clm_avg[3] = (u16)((value32 & MASKHWORD) >> 16);
	value32 = odm_get_bb_reg(dm, R_0x2e7c, MASKDWORD);
	ccx->ifs_clm_avg_cca[0] = (u16)(value32 & MASKLWORD);
	ccx->ifs_clm_avg_cca[1] = (u16)((value32 & MASKHWORD) >> 16);
	value32 = odm_get_bb_reg(dm, R_0x2e80, MASKDWORD);
	ccx->ifs_clm_avg_cca[2] = (u16)(value32 & MASKLWORD);
	ccx->ifs_clm_avg_cca[3] = (u16)((value32 & MASKHWORD) >> 16);

	/* Print Result */
	PHYDM_DBG(dm, DBG_ENV_MNTR,
		  "ECLM_Rpt[%d]: \nTx = %d, EDCCA_exclude_CCA = %d \n",
		  ccx->ifs_clm_rpt_stamp, ccx->ifs_clm_tx,
		  ccx->ifs_clm_edcca_excl_cca);
	PHYDM_DBG(dm, DBG_ENV_MNTR,
		  "[FA_cnt] {CCK, OFDM} = {%d, %d}\n",
		  ccx->ifs_clm_cckfa, ccx->ifs_clm_ofdmfa);
	PHYDM_DBG(dm, DBG_ENV_MNTR,
		  "[CCA_exclude_FA_cnt] {CCK, OFDM} = {%d, %d}\n",
		  ccx->ifs_clm_cckcca_excl_fa, ccx->ifs_clm_ofdmcca_excl_fa);
	PHYDM_DBG(dm, DBG_ENV_MNTR, "CCATotal = %d\n", ccx->ifs_clm_total_cca);
	PHYDM_DBG(dm, DBG_ENV_MNTR, "Time:[his, avg, avg_cca]\n");
	for (i = 0; i < IFS_CLM_NUM; i++)
		PHYDM_DBG(dm, DBG_ENV_MNTR,
			  "T%d:[%d, %d, %d]\n", i + 1,
			  ccx->ifs_clm_his[i], ccx->ifs_clm_avg[i],
			  ccx->ifs_clm_avg_cca[i]);

	phydm_ifs_clm_racing_release(dm);

	return;
}

#define IFS_CLM_NUM 4
void phydm_ifs_clm_init(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct ccx_info *ccx = &dm->dm_ccx_info;

	ccx->ifs_clm_app = IFS_CLM_BACKGROUND;

	/*Set IFS threshold*/
	ccx->ifs_clm_ongoing = false;
	ccx->ifs_clm_set_lv = IFS_CLM_RELEASE;
	ccx->ifs_clm_period = 0;
	ccx->ifs_clm_ctrl_unit = IFS_CLM_INIT;
	ccx->ifs_clm_manual_ctrl = 0;
	ccx->ifs_clm_rpt_stamp = 0;
}

#endif

void phydm_env_monitor_init(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	#if (defined(NHM_SUPPORT) && defined(CLM_SUPPORT))
	phydm_nhm_init(dm);
	phydm_clm_init(dm);
	#endif

	#ifdef FAHM_SUPPORT
	phydm_fahm_init(dm);
	#endif

	#ifdef IFS_CLM_SUPPORT
	phydm_ifs_clm_init(dm);
	#endif
}

