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

void phydm_init_debug_setting(struct dm_struct *dm)
{
	dm->fw_debug_components = 0;
	dm->debug_components =

#if DBG
	/*@BB Functions*/
	/*@DBG_DIG					|*/
	/*@DBG_RA_MASK					|*/
	/*@DBG_DYN_TXPWR				|*/
	/*@DBG_FA_CNT					|*/
	/*@DBG_RSSI_MNTR				|*/
	/*@DBG_CCKPD					|*/
	/*@DBG_ANT_DIV					|*/
	/*@DBG_SMT_ANT					|*/
	/*@DBG_PWR_TRAIN				|*/
	/*@DBG_RA					|*/
	/*@DBG_PATH_DIV					|*/
	/*@DBG_DFS					|*/
	/*@DBG_DYN_ARFR					|*/
	/*@DBG_ADPTVTY					|*/
	/*@DBG_CFO_TRK					|*/
	/*@DBG_ENV_MNTR					|*/
	/*@DBG_PRI_CCA					|*/
	/*@DBG_ADPTV_SOML				|*/
	/*@DBG_LNA_SAT_CHK				|*/
	/*@DBG_PHY_STATUS				|*/
	/*@DBG_TMP					|*/
	/*@DBG_FW_TRACE					|*/
	/*@DBG_TXBF					|*/
	/*@DBG_COMMON_FLOW				|*/
	/*@ODM_PHY_CONFIG				|*/
	/*@ODM_COMP_INIT				|*/
	/*@DBG_CMN					|*/
	/*@ODM_COMP_API					|*/
#endif
	0;

	dm->fw_buff_is_enpty = true;
	dm->pre_c2h_seq = 0;
	dm->c2h_cmd_start = 0;
	dm->cmn_dbg_msg_cnt = PHYDM_WATCH_DOG_PERIOD;
	dm->cmn_dbg_msg_period = PHYDM_WATCH_DOG_PERIOD;
	phydm_reset_rx_rate_distribution(dm);
}

void phydm_bb_dbg_port_header_sel(void *dm_void, u32 header_idx)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
}

void phydm_bb_dbg_port_clock_en(void *dm_void, u8 enable)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 reg_value = 0;

}

u32 phydm_get_bb_dbg_port_idx(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 val = 0;

	val = odm_get_bb_reg(dm, R_0x1c3c, 0xfff00);
	return val;
}

u8 phydm_set_bb_dbg_port(void *dm_void, u8 curr_dbg_priority, u32 debug_port)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u8 dbg_port_result = false;

	if (curr_dbg_priority > dm->pre_dbg_priority) {
		odm_set_bb_reg(dm, R_0x1c3c, 0xfff00, debug_port);
		PHYDM_DBG(dm, ODM_COMP_API,
			  "DbgPort ((0x%x)) set success, Cur_priority=((%d)), Pre_priority=((%d))\n",
			  debug_port, curr_dbg_priority, dm->pre_dbg_priority);
		dm->pre_dbg_priority = curr_dbg_priority;
		dbg_port_result = true;
	}

	return dbg_port_result;
}

void phydm_release_bb_dbg_port(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	phydm_bb_dbg_port_clock_en(dm, false);
	phydm_bb_dbg_port_header_sel(dm, 0);

	dm->pre_dbg_priority = DBGPORT_RELEASE;
	PHYDM_DBG(dm, ODM_COMP_API, "Release BB dbg_port\n");
}

u32 phydm_get_bb_dbg_port_val(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 dbg_port_value = 0;

	dbg_port_value = odm_get_bb_reg(dm, R_0x2dbc, MASKDWORD);

	PHYDM_DBG(dm, ODM_COMP_API, "dbg_port_value = 0x%x\n", dbg_port_value);
	return dbg_port_value;
}

void phydm_reset_rx_rate_distribution(struct dm_struct *dm)
{
	struct odm_phy_dbg_info *dbg = &dm->phy_dbg_info;

	odm_memory_set(dm, &dbg->num_qry_legacy_pkt[0], 0,
		       (LEGACY_RATE_NUM * 2));
	odm_memory_set(dm, &dbg->num_qry_ht_pkt[0], 0,
		       (HT_RATE_NUM * 2));
	odm_memory_set(dm, &dbg->num_qry_pkt_sc_20m[0], 0,
		       (LOW_BW_RATE_NUM * 2));

	dbg->ht_pkt_not_zero = false;
	dbg->low_bw_20_occur = false;

#if (ODM_IC_11AC_SERIES_SUPPORT || defined(PHYDM_IC_JGR3_SERIES_SUPPORT))
	odm_memory_set(dm, &dbg->num_qry_vht_pkt[0], 0, VHT_RATE_NUM * 2);
	odm_memory_set(dm, &dbg->num_qry_pkt_sc_40m[0], 0, LOW_BW_RATE_NUM * 2);
	#if (ODM_PHY_STATUS_NEW_TYPE_SUPPORT == 1) || (defined(PHYSTS_3RD_TYPE_SUPPORT))
	odm_memory_set(dm, &dbg->num_mu_vht_pkt[0], 0, VHT_RATE_NUM * 2);
	#endif
	dbg->vht_pkt_not_zero = false;
	dbg->low_bw_40_occur = false;
#endif
}

void phydm_rx_rate_distribution(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct odm_phy_dbg_info *dbg = &dm->phy_dbg_info;
	u8 i = 0;
	u8 rate_num = dm->num_rf_path, ss_ofst = 0;

	PHYDM_DBG(dm, DBG_CMN, "[RxRate Cnt] =============>\n");

	/*@======CCK=========================================================*/
	if (*dm->channel <= 14) {
		PHYDM_DBG(dm, DBG_CMN, "* CCK = {%d, %d, %d, %d}\n",
			  dbg->num_qry_legacy_pkt[0],
			  dbg->num_qry_legacy_pkt[1],
			  dbg->num_qry_legacy_pkt[2],
			  dbg->num_qry_legacy_pkt[3]);
	}
	/*@======OFDM========================================================*/
	PHYDM_DBG(dm, DBG_CMN, "* OFDM = {%d, %d, %d, %d, %d, %d, %d, %d}\n",
		  dbg->num_qry_legacy_pkt[4], dbg->num_qry_legacy_pkt[5],
		  dbg->num_qry_legacy_pkt[6], dbg->num_qry_legacy_pkt[7],
		  dbg->num_qry_legacy_pkt[8], dbg->num_qry_legacy_pkt[9],
		  dbg->num_qry_legacy_pkt[10], dbg->num_qry_legacy_pkt[11]);

	/*@======HT==========================================================*/
	if (dbg->ht_pkt_not_zero) {
		for (i = 0; i < rate_num; i++) {
			ss_ofst = (i << 3);

			PHYDM_DBG(dm, DBG_CMN,
				  "* HT MCS[%d :%d ] = {%d, %d, %d, %d, %d, %d, %d, %d}\n",
				  (ss_ofst), (ss_ofst + 7),
				  dbg->num_qry_ht_pkt[ss_ofst + 0],
				  dbg->num_qry_ht_pkt[ss_ofst + 1],
				  dbg->num_qry_ht_pkt[ss_ofst + 2],
				  dbg->num_qry_ht_pkt[ss_ofst + 3],
				  dbg->num_qry_ht_pkt[ss_ofst + 4],
				  dbg->num_qry_ht_pkt[ss_ofst + 5],
				  dbg->num_qry_ht_pkt[ss_ofst + 6],
				  dbg->num_qry_ht_pkt[ss_ofst + 7]);
		}

		if (dbg->low_bw_20_occur) {
			for (i = 0; i < rate_num; i++) {
				ss_ofst = (i << 3);

				PHYDM_DBG(dm, DBG_CMN,
					  "* [Low BW 20M] HT MCS[%d :%d ] = {%d, %d, %d, %d, %d, %d, %d, %d}\n",
					  (ss_ofst), (ss_ofst + 7),
					  dbg->num_qry_pkt_sc_20m[ss_ofst + 0],
					  dbg->num_qry_pkt_sc_20m[ss_ofst + 1],
					  dbg->num_qry_pkt_sc_20m[ss_ofst + 2],
					  dbg->num_qry_pkt_sc_20m[ss_ofst + 3],
					  dbg->num_qry_pkt_sc_20m[ss_ofst + 4],
					  dbg->num_qry_pkt_sc_20m[ss_ofst + 5],
					  dbg->num_qry_pkt_sc_20m[ss_ofst + 6],
					  dbg->num_qry_pkt_sc_20m[ss_ofst + 7]);
			}
		}
	}

#if (ODM_IC_11AC_SERIES_SUPPORT || defined(PHYDM_IC_JGR3_SERIES_SUPPORT))
	/*@======VHT==========================================================*/
	if (dbg->vht_pkt_not_zero) {
		for (i = 0; i < rate_num; i++) {
			ss_ofst = 10 * i;

			PHYDM_DBG(dm, DBG_CMN,
				  "* VHT-%d ss MCS[0:9] = {%d, %d, %d, %d, %d, %d, %d, %d, %d, %d}\n",
				  (i + 1),
				  dbg->num_qry_vht_pkt[ss_ofst + 0],
				  dbg->num_qry_vht_pkt[ss_ofst + 1],
				  dbg->num_qry_vht_pkt[ss_ofst + 2],
				  dbg->num_qry_vht_pkt[ss_ofst + 3],
				  dbg->num_qry_vht_pkt[ss_ofst + 4],
				  dbg->num_qry_vht_pkt[ss_ofst + 5],
				  dbg->num_qry_vht_pkt[ss_ofst + 6],
				  dbg->num_qry_vht_pkt[ss_ofst + 7],
				  dbg->num_qry_vht_pkt[ss_ofst + 8],
				  dbg->num_qry_vht_pkt[ss_ofst + 9]);
		}

		if (dbg->low_bw_20_occur) {
			for (i = 0; i < rate_num; i++) {
				ss_ofst = 10 * i;

				PHYDM_DBG(dm, DBG_CMN,
					  "*[Low BW 20M] VHT-%d ss MCS[0:9] = {%d, %d, %d, %d, %d, %d, %d, %d, %d, %d}\n",
					  (i + 1),
					  dbg->num_qry_pkt_sc_20m[ss_ofst + 0],
					  dbg->num_qry_pkt_sc_20m[ss_ofst + 1],
					  dbg->num_qry_pkt_sc_20m[ss_ofst + 2],
					  dbg->num_qry_pkt_sc_20m[ss_ofst + 3],
					  dbg->num_qry_pkt_sc_20m[ss_ofst + 4],
					  dbg->num_qry_pkt_sc_20m[ss_ofst + 5],
					  dbg->num_qry_pkt_sc_20m[ss_ofst + 6],
					  dbg->num_qry_pkt_sc_20m[ss_ofst + 7],
					  dbg->num_qry_pkt_sc_20m[ss_ofst + 8],
					  dbg->num_qry_pkt_sc_20m[ss_ofst + 9]);
			}
		}

		if (dbg->low_bw_40_occur) {
			for (i = 0; i < rate_num; i++) {
				ss_ofst = 10 * i;

				PHYDM_DBG(dm, DBG_CMN,
					  "*[Low BW 40M] VHT-%d ss MCS[0:9] = {%d, %d, %d, %d, %d, %d, %d, %d, %d, %d}\n",
					  (i + 1),
					  dbg->num_qry_pkt_sc_40m[ss_ofst + 0],
					  dbg->num_qry_pkt_sc_40m[ss_ofst + 1],
					  dbg->num_qry_pkt_sc_40m[ss_ofst + 2],
					  dbg->num_qry_pkt_sc_40m[ss_ofst + 3],
					  dbg->num_qry_pkt_sc_40m[ss_ofst + 4],
					  dbg->num_qry_pkt_sc_40m[ss_ofst + 5],
					  dbg->num_qry_pkt_sc_40m[ss_ofst + 6],
					  dbg->num_qry_pkt_sc_40m[ss_ofst + 7],
					  dbg->num_qry_pkt_sc_40m[ss_ofst + 8],
					  dbg->num_qry_pkt_sc_40m[ss_ofst + 9]);
			}
		}
	}
#endif
}

u16 phydm_rx_utility(void *dm_void, u16 avg_phy_rate, u8 rx_max_ss,
		     enum channel_width bw)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct odm_phy_dbg_info *dbg = &dm->phy_dbg_info;
	u16 utility_primitive = 0, utility = 0;

	if (dbg->ht_pkt_not_zero) {
	/*@ MCS7 20M: tp = 65, 1000/65 = 15.38, 65*15.5 = 1007*/
		utility_primitive = avg_phy_rate * 15 + (avg_phy_rate >> 1);
	}
#if (ODM_IC_11AC_SERIES_SUPPORT || defined(PHYDM_IC_JGR3_SERIES_SUPPORT))
	else if (dbg->vht_pkt_not_zero) {
	/*@ VHT 1SS MCS9(fake) 20M: tp = 90, 1000/90 = 11.11, 65*11.125 = 1001*/
		utility_primitive = avg_phy_rate * 11 + (avg_phy_rate >> 3);
	}
#endif
	else {
	/*@ 54M, 1000/54 = 18.5, 54*18.5 = 999*/
		utility_primitive = avg_phy_rate * 18 + (avg_phy_rate >> 1);
	}

	utility = (utility_primitive / rx_max_ss) >> bw;

	if (utility > 1000)
		utility = 1000;

	return utility;
}

u16 phydm_rx_avg_phy_rate(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct odm_phy_dbg_info *dbg = &dm->phy_dbg_info;
	u8 i = 0, rate_num = 0, rate_base = 0;
	u16 rate = 0, avg_phy_rate = 0;
	u32 pkt_cnt = 0, phy_rate_sum = 0;

	if (dbg->ht_pkt_not_zero) {
		rate_num = HT_RATE_NUM;
		rate_base = ODM_RATEMCS0;
		for (i = 0; i < rate_num; i++) {
			rate = phy_rate_table[i + rate_base] << *dm->band_width;
			phy_rate_sum += dbg->num_qry_ht_pkt[i] * rate;
			pkt_cnt += dbg->num_qry_ht_pkt[i];
		}
	}
#if (ODM_IC_11AC_SERIES_SUPPORT || defined(PHYDM_IC_JGR3_SERIES_SUPPORT))
	else if (dbg->vht_pkt_not_zero) {
		rate_num = VHT_RATE_NUM;
		rate_base = ODM_RATEVHTSS1MCS0;
		for (i = 0; i < rate_num; i++) {
			rate = phy_rate_table[i + rate_base] << *dm->band_width;
			phy_rate_sum += dbg->num_qry_vht_pkt[i] * rate;
			pkt_cnt += dbg->num_qry_vht_pkt[i];
		}
	}
#endif
	else {
		for (i = ODM_RATE1M; i <= ODM_RATE54M; i++) {
			/*SKIP 1M & 6M for beacon case*/
			if (*dm->channel < 36 && i == ODM_RATE1M)
				continue;

			if (*dm->channel >= 36 && i == ODM_RATE6M)
				continue;

			rate = phy_rate_table[i];
			phy_rate_sum += dbg->num_qry_legacy_pkt[i] * rate;
			pkt_cnt += dbg->num_qry_legacy_pkt[i];
		}
	}

#if (ODM_IC_11AC_SERIES_SUPPORT || defined(PHYDM_IC_JGR3_SERIES_SUPPORT))
	if (dbg->low_bw_40_occur) {
		for (i = 0; i < LOW_BW_RATE_NUM; i++) {
			rate = phy_rate_table[i + rate_base]
			       << CHANNEL_WIDTH_40;
			phy_rate_sum += dbg->num_qry_pkt_sc_40m[i] * rate;
			pkt_cnt += dbg->num_qry_pkt_sc_40m[i];
		}
	}
#endif

	if (dbg->low_bw_20_occur) {
		for (i = 0; i < LOW_BW_RATE_NUM; i++) {
			rate = phy_rate_table[i + rate_base];
			phy_rate_sum += dbg->num_qry_pkt_sc_20m[i] * rate;
			pkt_cnt += dbg->num_qry_pkt_sc_20m[i];
		}
	}

	avg_phy_rate = (pkt_cnt == 0) ? 0 : (u16)(phy_rate_sum / pkt_cnt);

	return avg_phy_rate;
}

void phydm_print_hist_2_buf(void *dm_void, u16 *val, u16 len, char *buf,
			    u16 buf_size)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	if (len == PHY_HIST_SIZE) {
		PHYDM_SNPRINTF(buf, buf_size,
			       "[%.2d, %.2d, %.2d, %.2d, %.2d, %.2d, %.2d, %.2d, %.2d, %.2d, %.2d, %.2d]",
			       val[0], val[1], val[2], val[3], val[4],
			       val[5], val[6], val[7], val[8], val[9],
			       val[10], val[11]);
	} else if (len == (PHY_HIST_SIZE - 1)) {
		PHYDM_SNPRINTF(buf, buf_size,
			       "[%.2d, %.2d, %.2d, %.2d, %.2d, %.2d, %.2d, %.2d, %.2d, %.2d, %.2d]",
			       val[0], val[1], val[2], val[3], val[4],
			       val[5], val[6], val[7], val[8], val[9],
			       val[10]);
	}
}

void phydm_nss_hitogram(void *dm_void, enum PDM_RATE_TYPE rate_type)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct odm_phy_dbg_info *dbg_i = &dm->phy_dbg_info;
	struct phydm_phystatus_statistic *dbg_s = &dbg_i->physts_statistic_info;
	char buf[PHYDM_SNPRINT_SIZE] = {0};
	u16 buf_size = PHYDM_SNPRINT_SIZE;
	u16 h_size = PHY_HIST_SIZE;
	u16 *evm_hist = &dbg_s->evm_1ss_hist[0];
	u16 *snr_hist = &dbg_s->snr_1ss_hist[0];
	u8 i = 0;
	u8 ss = phydm_rate_type_2_num_ss(dm, rate_type);

	for (i = 0; i < ss; i++) {
		if (rate_type == PDM_1SS) {
			evm_hist = &dbg_s->evm_1ss_hist[0];
			snr_hist = &dbg_s->snr_1ss_hist[0];
		} else if (rate_type == PDM_2SS) {
			#if (defined(PHYDM_COMPILE_ABOVE_2SS))
			evm_hist = &dbg_s->evm_2ss_hist[i][0];
			snr_hist = &dbg_s->snr_2ss_hist[i][0];
			#endif
		} else if (rate_type == PDM_3SS) {
			#if (defined(PHYDM_COMPILE_ABOVE_3SS))
			evm_hist = &dbg_s->evm_3ss_hist[i][0];
			snr_hist = &dbg_s->snr_3ss_hist[i][0];
			#endif
		} else if (rate_type == PDM_4SS) {
			#if (defined(PHYDM_COMPILE_ABOVE_4SS))
			evm_hist = &dbg_s->evm_4ss_hist[i][0];
			snr_hist = &dbg_s->snr_4ss_hist[i][0];
			#endif
		}

		phydm_print_hist_2_buf(dm, evm_hist, h_size, buf, buf_size);
		PHYDM_DBG(dm, DBG_CMN, "[%d-SS][EVM][%d]=%s\n", ss, i, buf);
		phydm_print_hist_2_buf(dm, snr_hist, h_size, buf, buf_size);
		PHYDM_DBG(dm, DBG_CMN, "[%d-SS][SNR][%d]=%s\n",  ss, i, buf);
	}
}

#ifdef PHYDM_PHYSTAUS_AUTO_SWITCH
void phydm_show_cn_hitogram(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct odm_phy_dbg_info *dbg_i = &dm->phy_dbg_info;
	struct phydm_phystatus_statistic *dbg_s = &dbg_i->physts_statistic_info;
	u16 th_tmp[PHY_HIST_TH_SIZE];
	char buf[PHYDM_SNPRINT_SIZE] = {0};
	u8 i = 0;
	u16 *cn_hist = NULL;
	u32 cn_avg = 0;

	if (!dm->pkt_proc_struct.physts_auto_swch_en)
		return;

	if (dm->num_rf_path == 1)
		return;

	PHYDM_DBG(dm, DBG_CMN, "[Condition number Histogram] ========>\n");
/*@===[Threshold]=============================================================*/
	for (i = 0; i < PHY_HIST_TH_SIZE; i++)
		th_tmp[i] = dbg_i->cn_hist_th[i] >> 1;

	phydm_print_hist_2_buf(dm, th_tmp,
			       PHY_HIST_TH_SIZE, buf, PHYDM_SNPRINT_SIZE);
	PHYDM_DBG(dm, DBG_CMN, "%-24s=%s\n", "[CN_TH]", buf);

/*@===[Histogram]=============================================================*/

	for (i = 1; i <= dm->num_rf_path; i++) {
		if (dbg_s->p4_cnt[i] == 0)
			continue;

		cn_avg = PHYDM_DIV((dbg_s->cn_sum[i] +
				   (dbg_s->p4_cnt[i] >> 1)) << 2,
				   dbg_s->p4_cnt[i]); /*u(8,1)<<2 -> u(10,3)*/

		cn_hist = &dbg_s->cn_hist[i][0];
		phydm_print_hist_2_buf(dm, cn_hist,
				       PHY_HIST_SIZE, buf, PHYDM_SNPRINT_SIZE);
		PHYDM_DBG(dm, DBG_CMN, "[%d-SS]%s=(avg:%d.%4d)%s\n",
			  i + 1, "[CN]", cn_avg >> 3,
			  phydm_show_fraction_num(cn_avg & 0x7, 3), buf);
	}
}
#endif

void phydm_show_phy_hitogram(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct odm_phy_dbg_info *dbg_i = &dm->phy_dbg_info;
	struct phydm_phystatus_statistic *dbg_s = &dbg_i->physts_statistic_info;
	char buf[PHYDM_SNPRINT_SIZE] = {0};
	u16 buf_size = PHYDM_SNPRINT_SIZE;
	u16 th_size = PHY_HIST_SIZE - 1;
	u8 i = 0;

	PHYDM_DBG(dm, DBG_CMN, "[PHY Histogram] ==============>\n");
/*@===[Threshold]=============================================================*/
	phydm_print_hist_2_buf(dm, dbg_i->evm_hist_th, th_size, buf, buf_size);
	PHYDM_DBG(dm, DBG_CMN, "%-16s=%s\n", "[EVM_TH]", buf);

	phydm_print_hist_2_buf(dm, dbg_i->snr_hist_th, th_size, buf, buf_size);
	PHYDM_DBG(dm, DBG_CMN, "%-16s=%s\n", "[SNR_TH]", buf);
/*@===[OFDM]==================================================================*/
	if (dbg_s->rssi_ofdm_cnt) {
		phydm_print_hist_2_buf(dm, dbg_s->evm_ofdm_hist, PHY_HIST_SIZE,
				       buf, buf_size);
		PHYDM_DBG(dm, DBG_CMN, "%-14s=%s\n", "[OFDM][EVM]", buf);

		phydm_print_hist_2_buf(dm, dbg_s->snr_ofdm_hist, PHY_HIST_SIZE,
				       buf, buf_size);
		PHYDM_DBG(dm, DBG_CMN, "%-14s=%s\n", "[OFDM][SNR]", buf);
	}
/*@===[1-SS]==================================================================*/
	if (dbg_s->rssi_1ss_cnt)
		phydm_nss_hitogram(dm, PDM_1SS);
/*@===[2-SS]==================================================================*/
	#if (defined(PHYDM_COMPILE_ABOVE_2SS))
	if (dbg_s->rssi_2ss_cnt)
		phydm_nss_hitogram(dm, PDM_2SS);
	#endif
}

void phydm_avg_phy_val_nss(void *dm_void, u8 nss)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct odm_phy_dbg_info *dbg_i = &dm->phy_dbg_info;
	struct phydm_phystatus_statistic *dbg_s = &dbg_i->physts_statistic_info;
	struct phydm_phystatus_avg *dbg_avg = &dbg_i->phystatus_statistic_avg;
	char *rate_type = NULL;
	u32 *tmp_cnt = NULL;
	u8 *tmp_rssi_avg = NULL;
	u32 *tmp_rssi_sum = NULL;
	u8 *tmp_snr_avg = NULL;
	u32 *tmp_snr_sum = NULL;
	u8 *tmp_evm_avg = NULL;
	u32 *tmp_evm_sum = NULL;
	u8 evm_rpt_show[RF_PATH_MEM_SIZE];
	u8 i = 0;

	odm_memory_set(dm, &evm_rpt_show[0], 0, RF_PATH_MEM_SIZE);

	switch (nss) {
	#if (defined(PHYDM_COMPILE_ABOVE_4SS))
	case 4:
		rate_type = "[4-SS]";
		tmp_cnt = &dbg_s->rssi_4ss_cnt;
		tmp_rssi_avg = &dbg_avg->rssi_4ss_avg[0];
		tmp_snr_avg = &dbg_avg->snr_4ss_avg[0];
		tmp_rssi_sum = &dbg_s->rssi_4ss_sum[0];
		tmp_snr_sum = &dbg_s->snr_4ss_sum[0];
		tmp_evm_avg = &dbg_avg->evm_4ss_avg[0];
		tmp_evm_sum = &dbg_s->evm_4ss_sum[0];
		break;
	#endif
	#if (defined(PHYDM_COMPILE_ABOVE_3SS))
	case 3:
		rate_type = "[3-SS]";
		tmp_cnt = &dbg_s->rssi_3ss_cnt;
		tmp_rssi_avg = &dbg_avg->rssi_3ss_avg[0];
		tmp_snr_avg = &dbg_avg->snr_3ss_avg[0];
		tmp_rssi_sum = &dbg_s->rssi_3ss_sum[0];
		tmp_snr_sum = &dbg_s->snr_3ss_sum[0];
		tmp_evm_avg = &dbg_avg->evm_3ss_avg[0];
		tmp_evm_sum = &dbg_s->evm_3ss_sum[0];
		break;
	#endif
	#if (defined(PHYDM_COMPILE_ABOVE_2SS))
	case 2:
		rate_type = "[2-SS]";
		tmp_cnt = &dbg_s->rssi_2ss_cnt;
		tmp_rssi_avg = &dbg_avg->rssi_2ss_avg[0];
		tmp_snr_avg = &dbg_avg->snr_2ss_avg[0];
		tmp_rssi_sum = &dbg_s->rssi_2ss_sum[0];
		tmp_snr_sum = &dbg_s->snr_2ss_sum[0];
		tmp_evm_avg = &dbg_avg->evm_2ss_avg[0];
		tmp_evm_sum = &dbg_s->evm_2ss_sum[0];
		break;
	#endif
	case 1:
		rate_type = "[1-SS]";
		tmp_cnt = &dbg_s->rssi_1ss_cnt;
		tmp_rssi_avg = &dbg_avg->rssi_1ss_avg[0];
		tmp_snr_avg = &dbg_avg->snr_1ss_avg[0];
		tmp_rssi_sum = &dbg_s->rssi_1ss_sum[0];
		tmp_snr_sum = &dbg_s->snr_1ss_sum[0];
		tmp_evm_avg = &dbg_avg->evm_1ss_avg;
		tmp_evm_sum = &dbg_s->evm_1ss_sum;
		break;
	case 0:
		rate_type = "[L-OFDM]";
		tmp_cnt = &dbg_s->rssi_ofdm_cnt;
		tmp_rssi_avg = &dbg_avg->rssi_ofdm_avg[0];
		tmp_snr_avg = &dbg_avg->snr_ofdm_avg[0];
		tmp_rssi_sum = &dbg_s->rssi_ofdm_sum[0];
		tmp_snr_sum = &dbg_s->snr_ofdm_sum[0];
		tmp_evm_avg = &dbg_avg->evm_ofdm_avg;
		tmp_evm_sum = &dbg_s->evm_ofdm_sum;
		break;
	default:
		PHYDM_DBG(dm, DBG_CMN, "[warning] %s\n", __func__);
		return;
	}

	if (*tmp_cnt != 0) {
		for (i = 0; i < dm->num_rf_path; i++) {
			tmp_rssi_avg[i] = (u8)(tmp_rssi_sum[i] / *tmp_cnt);
			tmp_snr_avg[i] = (u8)(tmp_snr_sum[i] / *tmp_cnt);
		}

		if (nss == 0 || nss == 1) {
			*tmp_evm_avg = (u8)(*tmp_evm_sum / *tmp_cnt);
			evm_rpt_show[0] = *tmp_evm_avg;
		} else {
			for (i = 0; i < nss; i++) {
				tmp_evm_avg[i] = (u8)(tmp_evm_sum[i] /
						      *tmp_cnt);
				evm_rpt_show[i] = tmp_evm_avg[i];
			}
		}
	}

#if (defined(PHYDM_COMPILE_ABOVE_4SS))
	PHYDM_DBG(dm, DBG_CMN,
		  "* %-8s Cnt=((%.3d)) RSSI:{%.2d, %.2d, %.2d, %.2d} SNR:{%.2d, %.2d, %.2d, %.2d} EVM:{-%.2d, -%.2d, -%.2d, -%.2d}\n",
		  rate_type, *tmp_cnt,
		  tmp_rssi_avg[0], tmp_rssi_avg[1], tmp_rssi_avg[2],
		  tmp_rssi_avg[3], tmp_snr_avg[0], tmp_snr_avg[1],
		  tmp_snr_avg[2], tmp_snr_avg[3], evm_rpt_show[0],
		  evm_rpt_show[1], evm_rpt_show[2], evm_rpt_show[3]);
#elif (defined(PHYDM_COMPILE_ABOVE_3SS))
	PHYDM_DBG(dm, DBG_CMN,
		  "* %-8s Cnt=((%.3d)) RSSI:{%.2d, %.2d, %.2d} SNR:{%.2d, %.2d, %.2d} EVM:{-%.2d, -%.2d, -%.2d}\n",
		  rate_type, *tmp_cnt,
		  tmp_rssi_avg[0], tmp_rssi_avg[1], tmp_rssi_avg[2],
		  tmp_snr_avg[0], tmp_snr_avg[1], tmp_snr_avg[2],
		  evm_rpt_show[0], evm_rpt_show[1], evm_rpt_show[2]);
#elif (defined(PHYDM_COMPILE_ABOVE_2SS))
	PHYDM_DBG(dm, DBG_CMN,
		  "* %-8s Cnt= ((%.3d)) RSSI:{%.2d, %.2d} SNR:{%.2d, %.2d} EVM:{-%.2d, -%.2d}\n",
		  rate_type, *tmp_cnt,
		  tmp_rssi_avg[0], tmp_rssi_avg[1],
		  tmp_snr_avg[0], tmp_snr_avg[1],
		  evm_rpt_show[0], evm_rpt_show[1]);
#else
	PHYDM_DBG(dm, DBG_CMN,
		  "* %-8s Cnt= ((%.3d)) RSSI:{%.2d} SNR:{%.2d} EVM:{-%.2d}\n",
		  rate_type, *tmp_cnt,
		  tmp_rssi_avg[0], tmp_snr_avg[0], evm_rpt_show[0]);
#endif
}

void phydm_get_avg_phystatus_val(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct odm_phy_dbg_info *dbg_i = &dm->phy_dbg_info;
	struct phydm_phystatus_statistic *dbg_s = &dbg_i->physts_statistic_info;
	struct phydm_phystatus_avg *dbg_avg = &dbg_i->phystatus_statistic_avg;
	u32 avg_tmp = 0;
	u8 i = 0;

	PHYDM_DBG(dm, DBG_CMN, "[PHY Avg] ==============>\n");
	phydm_reset_phystatus_avg(dm);

	/*@===[Beacon]===*/
	if (dbg_s->rssi_beacon_cnt) {
		for (i = 0; i < dm->num_rf_path; i++) {
			avg_tmp = dbg_s->rssi_beacon_sum[i] /
				  dbg_s->rssi_beacon_cnt;
			dbg_avg->rssi_beacon_avg[i] = (u8)avg_tmp;
		}
	}

	switch (dm->num_rf_path) {
#if (defined(PHYDM_COMPILE_ABOVE_4SS))
	case 4:
		PHYDM_DBG(dm, DBG_CMN,
			  "* %-8s Cnt=((%.3d)) RSSI:{%.2d, %.2d, %.2d, %.2d}\n",
			  "[Beacon]", dbg_s->rssi_beacon_cnt,
			  dbg_avg->rssi_beacon_avg[0],
			  dbg_avg->rssi_beacon_avg[1],
			  dbg_avg->rssi_beacon_avg[2],
			  dbg_avg->rssi_beacon_avg[3]);
		break;
#endif
#if (defined(PHYDM_COMPILE_ABOVE_3SS))
	case 3:
		PHYDM_DBG(dm, DBG_CMN,
			  "* %-8s Cnt=((%.3d)) RSSI:{%.2d, %.2d, %.2d}\n",
			  "[Beacon]", dbg_s->rssi_beacon_cnt,
			  dbg_avg->rssi_beacon_avg[0],
			  dbg_avg->rssi_beacon_avg[1],
			  dbg_avg->rssi_beacon_avg[2]);
		break;
#endif
#if (defined(PHYDM_COMPILE_ABOVE_2SS))
	case 2:
		PHYDM_DBG(dm, DBG_CMN,
			  "* %-8s Cnt=((%.3d)) RSSI:{%.2d, %.2d}\n",
			  "[Beacon]", dbg_s->rssi_beacon_cnt,
			  dbg_avg->rssi_beacon_avg[0],
			  dbg_avg->rssi_beacon_avg[1]);
		break;
#endif
	default:
		PHYDM_DBG(dm, DBG_CMN, "* %-8s Cnt=((%.3d)) RSSI:{%.2d}\n",
			  "[Beacon]", dbg_s->rssi_beacon_cnt,
			  dbg_avg->rssi_beacon_avg[0]);
		break;
	}

	/*@===[CCK]===*/
	if (dbg_s->rssi_cck_cnt) {
		dbg_avg->rssi_cck_avg = (u8)(dbg_s->rssi_cck_sum /
					     dbg_s->rssi_cck_cnt);
		for (i = 0; i < dm->num_rf_path - 1; i++) {
			avg_tmp = dbg_s->rssi_cck_sum_abv_2ss[i] /
				  dbg_s->rssi_cck_cnt;
			dbg_avg->rssi_cck_avg_abv_2ss[i] = (u8)avg_tmp;
		}
	}

	switch (dm->num_rf_path) {
#ifdef PHYSTS_3RD_TYPE_SUPPORT
	#if (defined(PHYDM_COMPILE_ABOVE_4SS))
	case 4:
		PHYDM_DBG(dm, DBG_CMN,
			  "* %-8s Cnt=((%.3d)) RSSI:{%.2d, %.2d, %.2d, %.2d}\n",
			  "[CCK]", dbg_s->rssi_cck_cnt, dbg_avg->rssi_cck_avg,
			  dbg_avg->rssi_cck_avg_abv_2ss[0],
			  dbg_avg->rssi_cck_avg_abv_2ss[1],
			  dbg_avg->rssi_cck_avg_abv_2ss[2]);
		break;
	#endif
	#if (defined(PHYDM_COMPILE_ABOVE_3SS))
	case 3:
		PHYDM_DBG(dm, DBG_CMN,
			  "* %-8s Cnt=((%.3d)) RSSI:{%.2d, %.2d, %.2d}\n",
			  "[CCK]", dbg_s->rssi_cck_cnt, dbg_avg->rssi_cck_avg,
			  dbg_avg->rssi_cck_avg_abv_2ss[0],
			  dbg_avg->rssi_cck_avg_abv_2ss[1]);
		break;
	#endif
	#if (defined(PHYDM_COMPILE_ABOVE_2SS))
	case 2:
		PHYDM_DBG(dm, DBG_CMN,
			  "* %-8s Cnt=((%.3d)) RSSI:{%.2d, %.2d}\n",
			  "[CCK]", dbg_s->rssi_cck_cnt, dbg_avg->rssi_cck_avg,
			  dbg_avg->rssi_cck_avg_abv_2ss[0]);
		break;
	#endif
#endif
	default:
		PHYDM_DBG(dm, DBG_CMN, "* %-8s Cnt=((%.3d)) RSSI:{%.2d}\n",
			  "[CCK]", dbg_s->rssi_cck_cnt, dbg_avg->rssi_cck_avg);
		break;
	}

	for (i = 0; i <= dm->num_rf_path; i++)
		phydm_avg_phy_val_nss(dm, i);
}

void phydm_dm_summary(void *dm_void, u8 macid)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_dig_struct *dig_t = &dm->dm_dig_table;
	struct phydm_cfo_track_struct *cfo_t = &dm->dm_cfo_track;
	struct cmn_sta_info *sta = NULL;
	struct ra_sta_info *ra = NULL;
	struct dtp_info *dtp = NULL;
	u64 comp = dm->support_ability;
	u64 pause_comp = dm->pause_ability;

	if (!(dm->debug_components & DBG_DM_SUMMARY))
		return;

	if (!dm->is_linked) {
		pr_debug("[%s]No Link !!!\n", __func__);
		return;
	}

	sta = dm->phydm_sta_info[macid];

	if (!is_sta_active(sta)) {
		pr_debug("[Warning] %s invalid STA, macid=%d\n",
			 __func__, macid);
		return;
	}

	ra = &sta->ra_info;
	dtp = &sta->dtp_stat;
	pr_debug("[%s]===========>\n", __func__);

	pr_debug("00.(%s) %-12s: IGI=0x%x, Dyn_Rng=0x%x~0x%x, FA_th={%d,%d,%d}\n",
		 ((comp & ODM_BB_DIG) ?
		 ((pause_comp & ODM_BB_DIG) ? "P" : "V") : "."),
		 "DIG",
		 dig_t->cur_ig_value,
		 dig_t->rx_gain_range_min, dig_t->rx_gain_range_max,
		 dig_t->fa_th[0], dig_t->fa_th[1], dig_t->fa_th[2]);

	pr_debug("01.(%s) %-12s: rssi_lv=%d, mask=0x%llx\n",
		 ((comp & ODM_BB_RA_MASK) ?
		 ((pause_comp & ODM_BB_RA_MASK) ? "P" : "V") : "."),
		 "RaMask",
		 ra->rssi_level, ra->ramask);

#ifdef CONFIG_DYNAMIC_TX_TWR
	pr_debug("02.(%s) %-12s: pwr_lv=%d\n",
		 ((comp & ODM_BB_DYNAMIC_TXPWR) ?
		 ((pause_comp & ODM_BB_DYNAMIC_TXPWR) ? "P" : "V") : "."),
		 "DynTxPwr",
		 dtp->sta_tx_high_power_lvl);
#endif

	pr_debug("05.(%s) %-12s: cck_pd_lv=%d\n",
		 ((comp & ODM_BB_CCK_PD) ?
		 ((pause_comp & ODM_BB_CCK_PD) ? "P" : "V") : "."),
		 "CCK_PD", dm->dm_cckpd_table.cck_pd_lv);

#ifdef CONFIG_PHYDM_ANTENNA_DIVERSITY
	pr_debug("06.(%s) %-12s: div_type=%d, curr_ant=%s\n",
		 ((comp & ODM_BB_ANT_DIV) ?
		 ((pause_comp & ODM_BB_ANT_DIV) ? "P" : "V") : "."),
		 "ANT_DIV",
		 dm->ant_div_type,
		 (dm->dm_fat_table.rx_idle_ant == MAIN_ANT) ? "MAIN" : "AUX");
#endif

#ifdef PHYDM_POWER_TRAINING_SUPPORT
	pr_debug("08.(%s) %-12s: PT_score=%d, disable_PT=%d\n",
		 ((comp & ODM_BB_PWR_TRAIN) ?
		 ((pause_comp & ODM_BB_PWR_TRAIN) ? "P" : "V") : "."),
		 "PwrTrain",
		 dm->pow_train_table.pow_train_score,
		 dm->is_disable_power_training);
#endif

#ifdef CONFIG_PHYDM_DFS_MASTER
	pr_debug("11.(%s) %-12s: dbg_mode=%d, region_domain=%d\n",
		 ((comp & ODM_BB_DFS) ?
		 ((pause_comp & ODM_BB_DFS) ? "P" : "V") : "."),
		 "DFS",
		 dm->dfs.dbg_mode, dm->dfs_region_domain);
#endif
#ifdef PHYDM_SUPPORT_ADAPTIVITY
	pr_debug("13.(%s) %-12s: th{l2h, h2l}={%d, %d}, edcca_flag=%d\n",
		 ((comp & ODM_BB_ADAPTIVITY) ?
		 ((pause_comp & ODM_BB_ADAPTIVITY) ? "P" : "V") : "."),
		 "Adaptivity",
		 dm->adaptivity.th_l2h, dm->adaptivity.th_h2l,
		 dm->false_alm_cnt.edcca_flag);
#endif
	pr_debug("14.(%s) %-12s: CFO_avg=%d kHz, CFO_traking=%s%d\n",
		 ((comp & ODM_BB_CFO_TRACKING) ?
		 ((pause_comp & ODM_BB_CFO_TRACKING) ? "P" : "V") : "."),
		 "CfoTrack",
		 cfo_t->CFO_ave_pre,
		 ((cfo_t->crystal_cap > cfo_t->def_x_cap) ? "+" : "-"),
		 DIFF_2(cfo_t->crystal_cap, cfo_t->def_x_cap));

	pr_debug("15.(%s) %-12s: ratio{nhm, clm}={%d, %d}\n",
		 ((comp & ODM_BB_ENV_MONITOR) ?
		 ((pause_comp & ODM_BB_ENV_MONITOR) ? "P" : "V") : "."),
		 "EnvMntr",
		 dm->dm_ccx_info.nhm_ratio, dm->dm_ccx_info.clm_ratio);

#ifdef PHYDM_PRIMARY_CCA
	pr_debug("16.(%s) %-12s: CCA @ (%s SB)\n",
		 ((comp & ODM_BB_PRIMARY_CCA) ?
		 ((pause_comp & ODM_BB_PRIMARY_CCA) ? "P" : "V") : "."),
		 "PriCCA",
		 ((dm->dm_pri_cca.mf_state == MF_USC_LSC) ? "D" :
		 ((dm->dm_pri_cca.mf_state == MF_LSC) ? "L" : "U")));
#endif
#ifdef CONFIG_ADAPTIVE_SOML
	pr_debug("17.(%s) %-12s: soml_en = %s\n",
		 ((comp & ODM_BB_ADAPTIVE_SOML) ?
		 ((pause_comp & ODM_BB_ADAPTIVE_SOML) ? "P" : "V") : "."),
		 "A-SOML",
		 (dm->dm_soml_table.soml_last_state == SOML_ON) ?
		 "ON" : "OFF");
#endif
#ifdef PHYDM_LNA_SAT_CHK_SUPPORT
	pr_debug("18.(%s) %-12s:\n",
		 ((comp & ODM_BB_LNA_SAT_CHK) ?
		 ((pause_comp & ODM_BB_LNA_SAT_CHK) ? "P" : "V") : "."),
		 "LNA_SAT_CHK");
#endif
}

void phydm_basic_dbg_message(void *dm_void)
{
}

void phydm_basic_profile(void *dm_void, u32 *_used, char *output, u32 *_out_len)
{
}

void phydm_cmd_parser(struct dm_struct *dm, char input[][MAX_ARGV],
		      u32 input_num, u8 flag, char *output, u32 out_len)
{
}

#if defined __ECOS || defined __ICCARM__
#ifndef strsep
char *strsep(char **s, const char *ct)
{
	char *sbegin = *s;
	char *end;

	if (!sbegin)
		return NULL;

	end = strpbrk(sbegin, ct);
	if (end)
		*end++ = '\0';
	*s = end;
	return sbegin;
}
#endif
#endif

#if (DM_ODM_SUPPORT_TYPE & (ODM_CE | ODM_AP | ODM_IOT))
s32 phydm_cmd(struct dm_struct *dm, char *input, u32 in_len, u8 flag,
	      char *output, u32 out_len)
{
	char *token;
	u32 argc = 0;
	char argv[MAX_ARGC][MAX_ARGV];

	do {
		token = strsep(&input, ", ");
		if (token) {
			if (strlen(token) <= MAX_ARGV)
				strcpy(argv[argc], token);

			argc++;
		} else {
			break;
		}
	} while (argc < MAX_ARGC);

	if (argc == 1)
		argv[0][strlen(argv[0]) - 1] = '\0';

	phydm_cmd_parser(dm, argv, argc, flag, output, out_len);

	return 0;
}
#endif

void phydm_fw_trace_handler(void *dm_void, u8 *cmd_buf, u8 cmd_len)
{
}

void phydm_fw_trace_handler_code(void *dm_void, u8 *buffer, u8 cmd_len)
{
}

void phydm_fw_trace_handler_8051(void *dm_void, u8 *buffer, u8 cmd_len)
{
}
