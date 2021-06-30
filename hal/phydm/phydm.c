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

const u16 phy_rate_table[] = {
	/*@20M*/
	1, 2, 5, 11,
	6, 9, 12, 18, 24, 36, 48, 54,
	6, 13, 19, 26, 39, 52, 58, 65, /*@MCS0~7*/
	13, 26, 39, 52, 78, 104, 117, 130, /*@MCS8~15*/
	19, 39, 58, 78, 117, 156, 175, 195, /*@MCS16~23*/
	26, 52, 78, 104, 156, 208, 234, 260, /*@MCS24~31*/
	6, 13, 19, 26, 39, 52, 58, 65, 78, 90, /*@1ss MCS0~9*/
	13, 26, 39, 52, 78, 104, 117, 130, 156, 180, /*@2ss MCS0~9*/
	19, 39, 58, 78, 117, 156, 175, 195, 234, 260, /*@3ss MCS0~9*/
	26, 52, 78, 104, 156, 208, 234, 260, 312, 360 /*@4ss MCS0~9*/
};

void phydm_traffic_load_decision(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u8 shift = 0;

	/*@---TP & Trafic-load calculation---*/

	if (dm->last_tx_ok_cnt > *dm->num_tx_bytes_unicast)
		dm->last_tx_ok_cnt = *dm->num_tx_bytes_unicast;

	if (dm->last_rx_ok_cnt > *dm->num_rx_bytes_unicast)
		dm->last_rx_ok_cnt = *dm->num_rx_bytes_unicast;

	dm->cur_tx_ok_cnt = *dm->num_tx_bytes_unicast - dm->last_tx_ok_cnt;
	dm->cur_rx_ok_cnt = *dm->num_rx_bytes_unicast - dm->last_rx_ok_cnt;
	dm->last_tx_ok_cnt = *dm->num_tx_bytes_unicast;
	dm->last_rx_ok_cnt = *dm->num_rx_bytes_unicast;

	/*@AP:  <<3(8bit), >>20(10^6,M), >>0(1sec)*/
	shift = 17 + (PHYDM_WATCH_DOG_PERIOD - 1);
	/*@WIN&CE:  <<3(8bit), >>20(10^6,M), >>1(2sec)*/

	dm->tx_tp = (dm->tx_tp >> 1) + (u32)((dm->cur_tx_ok_cnt >> shift) >> 1);
	dm->rx_tp = (dm->rx_tp >> 1) + (u32)((dm->cur_rx_ok_cnt >> shift) >> 1);

	dm->total_tp = dm->tx_tp + dm->rx_tp;

	/*@[Calculate TX/RX state]*/
	if (dm->tx_tp > (dm->rx_tp << 1))
		dm->txrx_state_all = TX_STATE;
	else if (dm->rx_tp > (dm->tx_tp << 1))
		dm->txrx_state_all = RX_STATE;
	else
		dm->txrx_state_all = BI_DIRECTION_STATE;

	/*@[Traffic load decision]*/
	dm->pre_traffic_load = dm->traffic_load;

	if (dm->cur_tx_ok_cnt > 1875000 || dm->cur_rx_ok_cnt > 1875000) {
		/* @( 1.875M * 8bit ) / 2sec= 7.5M bits /sec )*/
		dm->traffic_load = TRAFFIC_HIGH;
	} else if (dm->cur_tx_ok_cnt > 500000 || dm->cur_rx_ok_cnt > 500000) {
		/*@( 0.5M * 8bit ) / 2sec =  2M bits /sec )*/
		dm->traffic_load = TRAFFIC_MID;
	} else if (dm->cur_tx_ok_cnt > 100000 || dm->cur_rx_ok_cnt > 100000) {
		/*@( 0.1M * 8bit ) / 2sec =  0.4M bits /sec )*/
		dm->traffic_load = TRAFFIC_LOW;
	} else if (dm->cur_tx_ok_cnt > 25000 || dm->cur_rx_ok_cnt > 25000) {
		/*@( 0.025M * 8bit ) / 2sec =  0.1M bits /sec )*/
		dm->traffic_load = TRAFFIC_ULTRA_LOW;
	} else {
		dm->traffic_load = TRAFFIC_NO_TP;
	}

	/*@[Calculate consecutive idlel time]*/
	if (dm->traffic_load == 0)
		dm->consecutive_idlel_time += PHYDM_WATCH_DOG_PERIOD;
	else
		dm->consecutive_idlel_time = 0;

	#if 0
	PHYDM_DBG(dm, DBG_COMMON_FLOW,
		  "cur_tx_ok_cnt = %d, cur_rx_ok_cnt = %d, last_tx_ok_cnt = %d, last_rx_ok_cnt = %d\n",
		  dm->cur_tx_ok_cnt, dm->cur_rx_ok_cnt, dm->last_tx_ok_cnt,
		  dm->last_rx_ok_cnt);

	PHYDM_DBG(dm, DBG_COMMON_FLOW, "tx_tp = %d, rx_tp = %d\n", dm->tx_tp,
		  dm->rx_tp);
	#endif
}

void phydm_cck_new_agc_chk(struct dm_struct *dm)
{
	u32 new_agc_addr = 0x0;

	dm->cck_new_agc = false;
	new_agc_addr = R_0x1a9c;

	/*@1: new agc  0: old agc*/
	dm->cck_new_agc = (boolean)odm_get_bb_reg(dm, new_agc_addr, BIT(17));
}

/*select 3 or 4 bit LNA */
void phydm_cck_lna_bit_num_chk(struct dm_struct *dm)
{
	boolean report_type = 0;
	#if (RTL8192E_SUPPORT)
	u32 value_824, value_82c;
	#endif


	dm->cck_agc_report_type = report_type;

	PHYDM_DBG(dm, ODM_COMP_INIT, "cck_agc_report_type=((%d))\n",
		  dm->cck_agc_report_type);
}

#ifdef CONFIG_RFE_BY_HW_INFO
void phydm_init_hw_info_by_rfe(struct dm_struct *dm)
{
}
#endif

void phydm_common_info_self_init(struct dm_struct *dm)
{
	dm->run_in_drv_fw = RUN_IN_DRIVER;
	dm->ic_ip_series = PHYDM_IC_JGR3;
	dm->ic_phy_sts_type = PHYDM_PHYSTS_TYPE_3;
	dm->phydm_sys_up_time = 0;
	dm->num_rf_path = 2;
	dm->tx_rate = 0xFF;
	dm->rssi_min_by_path = 0xFF;
	dm->number_linked_client = 0;
	dm->pre_number_linked_client = 0;
	dm->number_active_client = 0;
	dm->pre_number_active_client = 0;
	dm->last_tx_ok_cnt = 0;
	dm->last_rx_ok_cnt = 0;
	dm->tx_tp = 0;
	dm->rx_tp = 0;
	dm->total_tp = 0;
	dm->traffic_load = TRAFFIC_LOW;
	dm->nbi_set_result = 0;
	dm->is_init_hw_info_by_rfe = false;
	dm->pre_dbg_priority = DBGPORT_RELEASE;
	dm->tp_active_th = 5;
	dm->u8_dummy = 0xf;
	dm->u16_dummy = 0xffff;
	dm->u32_dummy = 0xffffffff;
	dm->pre_is_linked = false;
	dm->is_linked = false;
	if (dm->en_auto_bw_th == 0)
		dm->en_auto_bw_th = 20;
}

void phydm_cmn_sta_info_update(void *dm_void, u8 macid)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct cmn_sta_info *sta = dm->phydm_sta_info[macid];
	struct ra_sta_info *ra = NULL;

	if (is_sta_active(sta)) {
		ra = &sta->ra_info;
	} else {
		PHYDM_DBG(dm, DBG_RA_MASK, "[Warning] %s invalid sta_info\n",
			  __func__);
		return;
	}

	PHYDM_DBG(dm, DBG_RA_MASK, "%s ======>\n", __func__);
	PHYDM_DBG(dm, DBG_RA_MASK, "MACID=%d\n", sta->mac_id);

	/*@[Calculate TX/RX state]*/
	if (sta->tx_moving_average_tp > (sta->rx_moving_average_tp << 1))
		ra->txrx_state = TX_STATE;
	else if (sta->rx_moving_average_tp > (sta->tx_moving_average_tp << 1))
		ra->txrx_state = RX_STATE;
	else
		ra->txrx_state = BI_DIRECTION_STATE;

	ra->is_noisy = dm->noisy_decision;
}

void phydm_common_info_self_update(struct dm_struct *dm)
{
	u8 sta_cnt = 0, num_active_client = 0;
	u32 i, one_entry_macid = 0;
	u32 ma_rx_tp = 0;
	u32 tp_diff = 0;
	struct cmn_sta_info *sta;
#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	PADAPTER adapter = (PADAPTER)dm->adapter;
	PMGNT_INFO mgnt_info = &((PADAPTER)adapter)->MgntInfo;

	sta = dm->phydm_sta_info[0];

	/* STA mode is linked to AP */
	if (is_sta_active(sta) && !ACTING_AS_AP(adapter))
		dm->bsta_state = true;
	else
		dm->bsta_state = false;
#endif

	for (i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++) {
		sta = dm->phydm_sta_info[i];
		if (is_sta_active(sta)) {
			sta_cnt++;

			if (sta_cnt == 1)
				one_entry_macid = i;

			phydm_cmn_sta_info_update(dm, (u8)i);

			ma_rx_tp = sta->rx_moving_average_tp +
				   sta->tx_moving_average_tp;

			PHYDM_DBG(dm, DBG_COMMON_FLOW,
				  "TP[%d]: ((%d )) bit/sec\n", i, ma_rx_tp);

			if (ma_rx_tp > ACTIVE_TP_THRESHOLD)
				num_active_client++;
		}
	}

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
	dm->is_linked = (sta_cnt != 0) ? true : false;
#endif

	if (sta_cnt == 1) {
		dm->is_one_entry_only = true;
		dm->one_entry_macid = one_entry_macid;
		dm->one_entry_tp = ma_rx_tp;

		dm->tp_active_occur = 0;

		PHYDM_DBG(dm, DBG_COMMON_FLOW,
			  "one_entry_tp=((%d)), pre_one_entry_tp=((%d))\n",
			  dm->one_entry_tp, dm->pre_one_entry_tp);

		if (dm->one_entry_tp > dm->pre_one_entry_tp &&
		    dm->pre_one_entry_tp <= 2) {
			tp_diff = dm->one_entry_tp - dm->pre_one_entry_tp;

			if (tp_diff > dm->tp_active_th)
				dm->tp_active_occur = 1;
		}
		dm->pre_one_entry_tp = dm->one_entry_tp;
	} else {
		dm->is_one_entry_only = false;
	}

	dm->pre_number_linked_client = dm->number_linked_client;
	dm->pre_number_active_client = dm->number_active_client;

	dm->number_linked_client = sta_cnt;
	dm->number_active_client = num_active_client;

	/*Traffic load information update*/
	phydm_traffic_load_decision(dm);

	dm->phydm_sys_up_time += PHYDM_WATCH_DOG_PERIOD;

	dm->is_dfs_band = phydm_is_dfs_band(dm);
	dm->phy_dbg_info.show_phy_sts_cnt = 0;

	/*[Link Status Check]*/
	dm->first_connect = dm->is_linked && !dm->pre_is_linked;
	dm->first_disconnect = !dm->is_linked && dm->pre_is_linked;
	dm->pre_is_linked = dm->is_linked;
}

void phydm_common_info_self_reset(struct dm_struct *dm)
{
	struct odm_phy_dbg_info		*dbg_t = &dm->phy_dbg_info;

	dbg_t->beacon_cnt_in_period = dbg_t->num_qry_beacon_pkt;
	dbg_t->num_qry_beacon_pkt = 0;

	dm->rxsc_l = 0xff;
	dm->rxsc_20 = 0xff;
	dm->rxsc_40 = 0xff;
	dm->rxsc_80 = 0xff;
}

void *
phydm_get_structure(struct dm_struct *dm, u8 structure_type)

{
	void *structure = NULL;

	switch (structure_type) {
	case PHYDM_FALSEALMCNT:
		structure = &dm->false_alm_cnt;
		break;

	case PHYDM_CFOTRACK:
		structure = &dm->dm_cfo_track;
		break;

	case PHYDM_ADAPTIVITY:
		structure = &dm->adaptivity;
		break;
#ifdef CONFIG_PHYDM_DFS_MASTER
	case PHYDM_DFS:
		structure = &dm->dfs;
		break;
#endif
	default:
		break;
	}

	return structure;
}

void phydm_phy_info_update(struct dm_struct *dm)
{
}

void phydm_hw_setting(struct dm_struct *dm)
{
	phydm_hwsetting_8822c(dm);
}

__odm_func__
boolean phydm_chk_bb_rf_pkg_set_valid(struct dm_struct *dm)
{
	boolean valid = true;

	valid = phydm_chk_pkg_set_valid_8822c(dm,
					      RELEASE_VERSION_8822C,
					      RF_RELEASE_VERSION_8822C);

	return valid;
}


void phydm_fwoffload_ability_init(struct dm_struct *dm,
				  enum phydm_offload_ability offload_ability)
{
	switch (offload_ability) {
	case PHYDM_PHY_PARAM_OFFLOAD:
		dm->fw_offload_ability |= PHYDM_PHY_PARAM_OFFLOAD;
		break;

	case PHYDM_RF_IQK_OFFLOAD:
		dm->fw_offload_ability |= PHYDM_RF_IQK_OFFLOAD;
		break;

	case PHYDM_RF_DPK_OFFLOAD:
		dm->fw_offload_ability |= PHYDM_RF_DPK_OFFLOAD;
		break;

	default:
		PHYDM_DBG(dm, ODM_COMP_INIT, "fwofflad, wrong init type!!\n");
		break;
	}

	PHYDM_DBG(dm, ODM_COMP_INIT, "fw_offload_ability = %x\n",
		  dm->fw_offload_ability);
}

void phydm_fwoffload_ability_clear(struct dm_struct *dm,
				   enum phydm_offload_ability offload_ability)
{
	switch (offload_ability) {
	case PHYDM_PHY_PARAM_OFFLOAD:
		dm->fw_offload_ability &= (~PHYDM_PHY_PARAM_OFFLOAD);
		break;

	case PHYDM_RF_IQK_OFFLOAD:
		dm->fw_offload_ability &= (~PHYDM_RF_IQK_OFFLOAD);
		break;

	case PHYDM_RF_DPK_OFFLOAD:
		dm->fw_offload_ability &= (~PHYDM_RF_DPK_OFFLOAD);
		break;	

	default:
		PHYDM_DBG(dm, ODM_COMP_INIT, "fwofflad, wrong init type!!\n");
		break;
	}

	PHYDM_DBG(dm, ODM_COMP_INIT, "fw_offload_ability = %x\n",
		  dm->fw_offload_ability);
}

void phydm_dm_early_init(struct dm_struct *dm)
{
	phydm_init_debug_setting(dm);
}

enum phydm_init_result odm_dm_init(struct dm_struct *dm)
{
	enum phydm_init_result result = PHYDM_INIT_SUCCESS;
	struct _hal_rf_ *rf = &dm->rf_table;

	rf->rf_supportability =
		HAL_RF_TX_PWR_TRACK |
		HAL_RF_IQK |
		HAL_RF_LCK |
		HAL_RF_DPK |
		HAL_RF_DACK |
		HAL_RF_DPK_TRACK |
		HAL_RF_RXDCK |
		HAL_RF_TXGAPK |
		0;

	dm->support_ability =
			ODM_BB_DIG |
			ODM_BB_RA_MASK |
			ODM_BB_DYNAMIC_TXPWR	|
			ODM_BB_FA_CNT |
			ODM_BB_RSSI_MONITOR |
			ODM_BB_CCK_PD |
			ODM_BB_RATE_ADAPTIVE |
			ODM_BB_ADAPTIVITY |
			ODM_BB_CFO_TRACKING |
			ODM_BB_ENV_MONITOR;

	phydm_pause_func_init(dm);
	phydm_common_info_self_init(dm);
	phydm_rx_phy_status_init(dm);

	phydm_dig_init(dm);
#ifdef PHYDM_SUPPORT_CCKPD
	phydm_cck_pd_init(dm);
#endif
	phydm_env_monitor_init(dm);
	phydm_adaptivity_init(dm);
	phydm_ra_info_init(dm);
	phydm_rssi_monitor_init(dm);
	phydm_cfo_tracking_init(dm);
	phydm_rf_init(dm);
#if (PHYDM_LA_MODE_SUPPORT)
	phydm_la_init(dm);
#endif

#ifdef CONFIG_PSD_TOOL
	phydm_psd_init(dm);
#endif

#ifdef PHYDM_CCK_RX_PATHDIV_SUPPORT
	phydm_cck_rx_pathdiv_init(dm);
#endif

	return result;
}

void odm_dm_reset(struct dm_struct *dm)
{
	#ifdef CONFIG_PHYDM_ANTENNA_DIVERSITY
	odm_ant_div_reset(dm);
	#endif
	phydm_set_edcca_threshold_api(dm);
}

void phydm_supportability_en(void *dm_void, char input[][16], u32 *_used,
			     char *output, u32 *_out_len)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 dm_value[10] = {0};
	u64 pre_support_ability, one = 1;
	u64 comp = 0;
	u32 used = *_used;
	u32 out_len = *_out_len;
	u8 i;

	for (i = 0; i < 5; i++) {
		PHYDM_SSCANF(input[i + 1], DCMD_DECIMAL, &dm_value[i]);
	}

	pre_support_ability = dm->support_ability;
	comp = dm->support_ability;

	PDM_SNPF(out_len, used, output + used, out_len - used,
		 "\n================================\n");

	if (dm_value[0] == 100) {
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "[Supportability] PhyDM Selection\n");
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "================================\n");
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "00. (( %s ))DIG\n",
			 ((comp & ODM_BB_DIG) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "01. (( %s ))RA_MASK\n",
			 ((comp & ODM_BB_RA_MASK) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "02. (( %s ))DYN_TXPWR\n",
			 ((comp & ODM_BB_DYNAMIC_TXPWR) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "03. (( %s ))FA_CNT\n",
			 ((comp & ODM_BB_FA_CNT) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "04. (( %s ))RSSI_MNTR\n",
			 ((comp & ODM_BB_RSSI_MONITOR) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "05. (( %s ))CCK_PD\n",
			 ((comp & ODM_BB_CCK_PD) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "06. (( %s ))ANT_DIV\n",
			 ((comp & ODM_BB_ANT_DIV) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "07. (( %s ))SMT_ANT\n",
			 ((comp & ODM_BB_SMT_ANT) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "08. (( %s ))PWR_TRAIN\n",
			 ((comp & ODM_BB_PWR_TRAIN) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "09. (( %s ))RA\n",
			 ((comp & ODM_BB_RATE_ADAPTIVE) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "10. (( %s ))PATH_DIV\n",
			 ((comp & ODM_BB_PATH_DIV) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "11. (( %s ))DFS\n",
			 ((comp & ODM_BB_DFS) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "12. (( %s ))DYN_ARFR\n",
			 ((comp & ODM_BB_DYNAMIC_ARFR) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "13. (( %s ))ADAPTIVITY\n",
			 ((comp & ODM_BB_ADAPTIVITY) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "14. (( %s ))CFO_TRACK\n",
			 ((comp & ODM_BB_CFO_TRACKING) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "15. (( %s ))ENV_MONITOR\n",
			 ((comp & ODM_BB_ENV_MONITOR) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "16. (( %s ))PRI_CCA\n",
			 ((comp & ODM_BB_PRIMARY_CCA) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "17. (( %s ))ADPTV_SOML\n",
			 ((comp & ODM_BB_ADAPTIVE_SOML) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "18. (( %s ))LNA_SAT_CHK\n",
			 ((comp & ODM_BB_LNA_SAT_CHK) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "================================\n");
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "[Supportability] PhyDM offload ability\n");
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "================================\n");

		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "00. (( %s ))PHY PARAM OFFLOAD\n",
			 ((dm->fw_offload_ability & PHYDM_PHY_PARAM_OFFLOAD) ?
			 ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "01. (( %s ))RF IQK OFFLOAD\n",
			 ((dm->fw_offload_ability & PHYDM_RF_IQK_OFFLOAD) ?
			 ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "================================\n");

	} else if (dm_value[0] == 101) {
		dm->support_ability = 0;
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "Disable all support_ability components\n");
	} else {
		if (dm_value[1] == 1) { /* @enable */
			dm->support_ability |= (one << dm_value[0]);
		} else if (dm_value[1] == 2) {/* @disable */
			dm->support_ability &= ~(one << dm_value[0]);
		} else {
			PDM_SNPF(out_len, used, output + used, out_len - used,
				 "[Warning!!!]  1:enable,  2:disable\n");
		}
	}
	PDM_SNPF(out_len, used, output + used, out_len - used,
		 "pre-supportability = 0x%llx\n", pre_support_ability);
	PDM_SNPF(out_len, used, output + used, out_len - used,
		 "Cur-supportability = 0x%llx\n", dm->support_ability);
	PDM_SNPF(out_len, used, output + used, out_len - used,
		 "================================\n");

	*_used = used;
	*_out_len = out_len;
}

void phydm_watchdog_mp(struct dm_struct *dm)
{
}

void phydm_pause_func_init(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	dm->pause_lv_table.lv_cckpd = PHYDM_PAUSE_RELEASE;
	dm->pause_lv_table.lv_dig = PHYDM_PAUSE_RELEASE;
	dm->pause_lv_table.lv_antdiv = PHYDM_PAUSE_RELEASE;
	dm->pause_lv_table.lv_dig = PHYDM_PAUSE_RELEASE;
	dm->pause_lv_table.lv_adapt = PHYDM_PAUSE_RELEASE;
	dm->pause_lv_table.lv_adsl = PHYDM_PAUSE_RELEASE;
}

void phydm_fw_dm_ctrl_en(void *dm_void, enum phydm_func_idx fun_idx,
			 boolean enable)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u8 h2c_val[H2C_MAX_LENGTH] = {0};
	u8 para4[4]; /*4 bit*/
	u8 para8[4]; /*8 bit*/
	u8 i = 0;

	for (i = 0; i < 4; i++) {
		para4[i] = 0;
		para8[i] = 0;
	}

	switch (fun_idx) {
	case F00_DIG:
		phydm_fill_fw_dig_info(dm, &enable, para4, para8);
		break;
	default:
		pr_debug("[Warning] %s\n", __func__);
		return;
	}

	h2c_val[0] = (u8)((fun_idx & 0x3f) | (enable << 6));
	h2c_val[1] = para8[0];
	h2c_val[2] = para8[1];
	h2c_val[3] = para8[2];
	h2c_val[4] = para8[3];
	h2c_val[5] = (para4[0] & 0xf) | ((para4[1] & 0xf) << 3);
	h2c_val[6] = (para4[2] & 0xf) | ((para4[3] & 0xf) << 3);

	PHYDM_DBG(dm, DBG_FW_DM,
		  "H2C[0x59] fun_idx=%d,en=%d,para8={%x %x %x %x},para4={%x %x %x %x}\n",
		  fun_idx, enable,
		  para8[0], para8[1], para8[2], para8[3],
		  para4[0], para4[1], para4[2], para4[3]);

	odm_fill_h2c_cmd(dm, PHYDM_H2C_FW_DM_CTRL, H2C_MAX_LENGTH, h2c_val);
}

/*@
 * Init /.. Fixed HW value. Only init time.
 */
void odm_cmn_info_init(struct dm_struct *dm, enum odm_cmninfo cmn_info,
		       u64 value)
{
	/* This section is used for init value */
	switch (cmn_info) {
	/* @Fixed ODM value. */
	case ODM_CMNINFO_ABILITY:
		dm->support_ability = (u64)value;
		break;

	case ODM_CMNINFO_RF_TYPE:
		dm->rf_type = (u8)value;
		break;

	case ODM_CMNINFO_PLATFORM:
		dm->support_platform = (u8)value;
		break;

	case ODM_CMNINFO_INTERFACE:
		dm->support_interface = (u8)value;
		break;

	case ODM_CMNINFO_MP_TEST_CHIP:
		dm->is_mp_chip = (u8)value;
		break;

	case ODM_CMNINFO_CUT_VER:
		break;

	case ODM_CMNINFO_FAB_VER:
		break;
	case ODM_CMNINFO_RFE_TYPE:
		dm->rfe_type = (u8)value;

#ifdef CONFIG_RFE_BY_HW_INFO
		phydm_init_hw_info_by_rfe(dm);
#endif
		break;

	case ODM_CMNINFO_RF_ANTENNA_TYPE:
		dm->ant_div_type = (u8)value;
		break;

	case ODM_CMNINFO_WITH_EXT_ANTENNA_SWITCH:
		dm->with_extenal_ant_switch = (u8)value;
		break;

#ifdef CONFIG_PHYDM_ANTENNA_DIVERSITY
	case ODM_CMNINFO_BE_FIX_TX_ANT:
		dm->dm_fat_table.b_fix_tx_ant = (u8)value;
		break;
#endif

	case ODM_CMNINFO_BOARD_TYPE:
		if (!dm->is_init_hw_info_by_rfe)
			dm->board_type = (u8)value;
		break;

	case ODM_CMNINFO_PACKAGE_TYPE:
		if (!dm->is_init_hw_info_by_rfe)
			dm->package_type = (u8)value;
		break;

	case ODM_CMNINFO_EXT_LNA:
		if (!dm->is_init_hw_info_by_rfe)
			dm->ext_lna = (u8)value;
		break;

	case ODM_CMNINFO_5G_EXT_LNA:
		if (!dm->is_init_hw_info_by_rfe)
			dm->ext_lna_5g = (u8)value;
		break;

	case ODM_CMNINFO_EXT_PA:
		if (!dm->is_init_hw_info_by_rfe)
			dm->ext_pa = (u8)value;
		break;

	case ODM_CMNINFO_5G_EXT_PA:
		if (!dm->is_init_hw_info_by_rfe)
			dm->ext_pa_5g = (u8)value;
		break;

	case ODM_CMNINFO_GPA:
		if (!dm->is_init_hw_info_by_rfe)
			dm->type_gpa = (u16)value;
		break;

	case ODM_CMNINFO_APA:
		if (!dm->is_init_hw_info_by_rfe)
			dm->type_apa = (u16)value;
		break;

	case ODM_CMNINFO_GLNA:
		if (!dm->is_init_hw_info_by_rfe)
			dm->type_glna = (u16)value;
		break;

	case ODM_CMNINFO_ALNA:
		if (!dm->is_init_hw_info_by_rfe)
			dm->type_alna = (u16)value;
		break;

	case ODM_CMNINFO_EXT_TRSW:
		if (!dm->is_init_hw_info_by_rfe)
			dm->ext_trsw = (u8)value;
		break;
	case ODM_CMNINFO_EXT_LNA_GAIN:
		dm->ext_lna_gain = (u8)value;
		break;
	case ODM_CMNINFO_BINHCT_TEST:
		dm->is_in_hct_test = (boolean)value;
		break;
	case ODM_CMNINFO_BWIFI_TEST:
		dm->wifi_test = (u8)value;
		break;
	case ODM_CMNINFO_SMART_CONCURRENT:
		dm->is_dual_mac_smart_concurrent = (boolean)value;
		break;
#if (DM_ODM_SUPPORT_TYPE & (ODM_AP))
	case ODM_CMNINFO_CONFIG_BB_RF:
		dm->config_bbrf = (boolean)value;
		break;
#endif
	case ODM_CMNINFO_IQKPAOFF:
		dm->rf_calibrate_info.is_iqk_pa_off = (boolean)value;
		break;
	case ODM_CMNINFO_REGRFKFREEENABLE:
		dm->rf_calibrate_info.reg_rf_kfree_enable = (u8)value;
		break;
	case ODM_CMNINFO_RFKFREEENABLE:
		dm->rf_calibrate_info.rf_kfree_enable = (u8)value;
		break;
	case ODM_CMNINFO_NORMAL_RX_PATH_CHANGE:
		dm->normal_rx_path = (u8)value;
		break;
	case ODM_CMNINFO_VALID_PATH_SET:
		dm->valid_path_set = (u8)value;
		break;
	case ODM_CMNINFO_EFUSE0X3D8:
		dm->efuse0x3d8 = (u8)value;
		break;
	case ODM_CMNINFO_EFUSE0X3D7:
		dm->efuse0x3d7 = (u8)value;
		break;
	case ODM_CMNINFO_ADVANCE_OTA:
		dm->p_advance_ota = (u8)value;
		break;

#ifdef CONFIG_PHYDM_DFS_MASTER
	case ODM_CMNINFO_DFS_REGION_DOMAIN:
		dm->dfs_region_domain = (u8)value;
		break;
#endif
	case ODM_CMNINFO_SOFT_AP_SPECIAL_SETTING:
		dm->soft_ap_special_setting = (u32)value;
		break;

	case ODM_CMNINFO_X_CAP_SETTING:
		dm->dm_cfo_track.crystal_cap_default = (u8)value;
		break;

	case ODM_CMNINFO_DPK_EN:
		/*@dm->dpk_en = (u1Byte)value;*/
		halrf_cmn_info_set(dm, HALRF_CMNINFO_DPK_EN, (u64)value);
		break;

	case ODM_CMNINFO_HP_HWID:
		dm->hp_hw_id = (boolean)value;
		break;
	case ODM_CMNINFO_TSSI_ENABLE:
		dm->en_tssi_mode = (u8)value;
		break;
	case ODM_CMNINFO_DIS_DPD:
		dm->en_dis_dpd = (boolean)value;
		break;
	case ODM_CMNINFO_EN_AUTO_BW_TH:
		dm->en_auto_bw_th = (u8)value;
		break;
#if (RTL8721D_SUPPORT)
	case ODM_CMNINFO_POWER_VOLTAGE:
		dm->power_voltage = (u8)value;
		break;
	case ODM_CMNINFO_ANTDIV_GPIO:
		dm->antdiv_gpio = (u8)value;
		break;
	case ODM_CMNINFO_PEAK_DETECT_MODE:
		dm->peak_detect_mode = (u8)value;
		break;
#endif
	default:
		break;
	}
}

void odm_cmn_info_hook(struct dm_struct *dm, enum odm_cmninfo cmn_info,
		       void *value)
{
	/* @Hook call by reference pointer. */
	switch (cmn_info) {
	/* @Dynamic call by reference pointer. */
	case ODM_CMNINFO_TX_UNI:
		dm->num_tx_bytes_unicast = (u64 *)value;
		break;

	case ODM_CMNINFO_RX_UNI:
		dm->num_rx_bytes_unicast = (u64 *)value;
		break;

	case ODM_CMNINFO_BAND:
		dm->band_type = (u8 *)value;
		break;

	case ODM_CMNINFO_SEC_CHNL_OFFSET:
		dm->sec_ch_offset = (u8 *)value;
		break;

	case ODM_CMNINFO_SEC_MODE:
		dm->security = (u8 *)value;
		break;

	case ODM_CMNINFO_BW:
		dm->band_width = (u8 *)value;
		break;

	case ODM_CMNINFO_CHNL:
		dm->channel = (u8 *)value;
		break;

	case ODM_CMNINFO_SCAN:
		dm->is_scan_in_process = (boolean *)value;
		break;

	case ODM_CMNINFO_POWER_SAVING:
		dm->is_power_saving = (boolean *)value;
		break;

	case ODM_CMNINFO_TDMA:
		dm->is_tdma = (boolean *)value;
		break;

	case ODM_CMNINFO_ONE_PATH_CCA:
		dm->one_path_cca = (u8 *)value;
		break;

	case ODM_CMNINFO_DRV_STOP:
		dm->is_driver_stopped = (boolean *)value;
		break;
	case ODM_CMNINFO_INIT_ON:
		dm->pinit_adpt_in_progress = (boolean *)value;
		break;

	case ODM_CMNINFO_ANT_TEST:
		dm->antenna_test = (u8 *)value;
		break;

	case ODM_CMNINFO_NET_CLOSED:
		dm->is_net_closed = (boolean *)value;
		break;

	case ODM_CMNINFO_FORCED_RATE:
		dm->forced_data_rate = (u16 *)value;
		break;
	case ODM_CMNINFO_ANT_DIV:
		dm->enable_antdiv = (u8 *)value;
		break;
	case ODM_CMNINFO_ADAPTIVE_SOML:
		dm->en_adap_soml = (u8 *)value;
		break;
	case ODM_CMNINFO_ADAPTIVITY:
		dm->edcca_mode = (u8 *)value;
		break;

	case ODM_CMNINFO_P2P_LINK:
		dm->dm_dig_table.is_p2p_in_process = (u8 *)value;
		break;

	case ODM_CMNINFO_IS1ANTENNA:
		dm->is_1_antenna = (boolean *)value;
		break;

	case ODM_CMNINFO_RFDEFAULTPATH:
		dm->rf_default_path = (u8 *)value;
		break;

	case ODM_CMNINFO_FCS_MODE: /* @fast channel switch (= MCC mode)*/
		dm->is_fcs_mode_enable = (boolean *)value;
		break;

	case ODM_CMNINFO_HUBUSBMODE:
		dm->hub_usb_mode = (u8 *)value;
		break;
	case ODM_CMNINFO_FWDWRSVDPAGEINPROGRESS:
		dm->is_fw_dw_rsvd_page_in_progress = (boolean *)value;
		break;
	case ODM_CMNINFO_TX_TP:
		dm->current_tx_tp = (u32 *)value;
		break;
	case ODM_CMNINFO_RX_TP:
		dm->current_rx_tp = (u32 *)value;
		break;
	case ODM_CMNINFO_SOUNDING_SEQ:
		dm->sounding_seq = (u8 *)value;
		break;
#ifdef CONFIG_PHYDM_DFS_MASTER
	case ODM_CMNINFO_DFS_MASTER_ENABLE:
		dm->dfs_master_enabled = (u8 *)value;
		break;
#endif

#ifdef CONFIG_PHYDM_ANTENNA_DIVERSITY
	case ODM_CMNINFO_FORCE_TX_ANT_BY_TXDESC:
		dm->dm_fat_table.p_force_tx_by_desc = (u8 *)value;
		break;
	case ODM_CMNINFO_SET_S0S1_DEFAULT_ANTENNA:
		dm->dm_fat_table.p_default_s0_s1 = (u8 *)value;
		break;
	case ODM_CMNINFO_BF_ANTDIV_DECISION:
		dm->dm_fat_table.is_no_csi_feedback = (boolean *)value;
		break;
#endif

	case ODM_CMNINFO_SOFT_AP_MODE:
		dm->soft_ap_mode = (u32 *)value;
		break;
	case ODM_CMNINFO_MP_MODE:
		dm->mp_mode = (u8 *)value;
		break;
	case ODM_CMNINFO_INTERRUPT_MASK:
		dm->interrupt_mask = (u32 *)value;
		break;
	case ODM_CMNINFO_BB_OPERATION_MODE:
		dm->bb_op_mode = (u8 *)value;
		break;
	case ODM_CMNINFO_MANUAL_SUPPORTABILITY:
		dm->manual_supportability = (u32 *)value;
		break;
	case ODM_CMNINFO_EN_DYM_BW_INDICATION:
		dm->dis_dym_bw_indication = (u8 *)value;
	default:
		/*do nothing*/
		break;
	}
}

/*@
 * Update band/CHannel/.. The values are dynamic but non-per-packet.
 */
void odm_cmn_info_update(struct dm_struct *dm, u32 cmn_info, u64 value)
{
	/* This init variable may be changed in run time. */
	switch (cmn_info) {
	case ODM_CMNINFO_LINK_IN_PROGRESS:
		dm->is_link_in_process = (boolean)value;
		break;

	case ODM_CMNINFO_ABILITY:
		dm->support_ability = (u64)value;
		break;

	case ODM_CMNINFO_RF_TYPE:
		dm->rf_type = (u8)value;
		break;

	case ODM_CMNINFO_WIFI_DIRECT:
		dm->is_wifi_direct = (boolean)value;
		break;

	case ODM_CMNINFO_WIFI_DISPLAY:
		dm->is_wifi_display = (boolean)value;
		break;

	case ODM_CMNINFO_LINK:
		dm->is_linked = (boolean)value;
		break;

	case ODM_CMNINFO_STATION_STATE:
		dm->bsta_state = (boolean)value;
		break;

	case ODM_CMNINFO_RSSI_MIN:
#if 0
		dm->rssi_min = (u8)value;
#endif
		break;

	case ODM_CMNINFO_RSSI_MIN_BY_PATH:
		dm->rssi_min_by_path = (u8)value;
		break;

	case ODM_CMNINFO_DBG_COMP:
		dm->debug_components = (u64)value;
		break;

#ifdef ODM_CONFIG_BT_COEXIST
	/* The following is for BT HS mode and BT coexist mechanism. */
	case ODM_CMNINFO_BT_ENABLED:
		dm->bt_info_table.is_bt_enabled = (boolean)value;
		break;

	case ODM_CMNINFO_BT_HS_CONNECT_PROCESS:
		dm->bt_info_table.is_bt_connect_process = (boolean)value;
		break;

	case ODM_CMNINFO_BT_HS_RSSI:
		dm->bt_info_table.bt_hs_rssi = (u8)value;
		break;

	case ODM_CMNINFO_BT_OPERATION:
		dm->bt_info_table.is_bt_hs_operation = (boolean)value;
		break;

	case ODM_CMNINFO_BT_LIMITED_DIG:
		dm->bt_info_table.is_bt_limited_dig = (boolean)value;
		break;
#endif

	case ODM_CMNINFO_AP_TOTAL_NUM:
		dm->ap_total_num = (u8)value;
		break;

#ifdef CONFIG_PHYDM_DFS_MASTER
	case ODM_CMNINFO_DFS_REGION_DOMAIN:
		dm->dfs_region_domain = (u8)value;
		break;
#endif

	case ODM_CMNINFO_BT_CONTINUOUS_TURN:
		dm->is_bt_continuous_turn = (boolean)value;
		break;
	case ODM_CMNINFO_IS_DOWNLOAD_FW:
		dm->is_download_fw = (boolean)value;
		break;
	case ODM_CMNINFO_RRSR_VAL:
		dm->dm_ra_table.rrsr_val_init = (u32)value;
		break;
	case ODM_CMNINFO_LINKED_BF_SUPPORT:
		dm->linked_bf_support = (u8)value;
		break;
	case ODM_CMNINFO_FLATNESS_TYPE:
		dm->flatness_type = (u8)value;
		break;
	case ODM_CMNINFO_TSSI_ENABLE:
		dm->en_tssi_mode = (u8)value;
		break;
	default:
		break;
	}
}

u32 phydm_cmn_info_query(struct dm_struct *dm, enum phydm_info_query info_type)
{
	struct phydm_fa_struct *fa_t = &dm->false_alm_cnt;
	struct phydm_dig_struct *dig_t = &dm->dm_dig_table;
	struct ccx_info *ccx_info = &dm->dm_ccx_info;

	switch (info_type) {
	/*@=== [FA Relative] ===========================================*/
	case PHYDM_INFO_FA_OFDM:
		return fa_t->cnt_ofdm_fail;

	case PHYDM_INFO_FA_CCK:
		return fa_t->cnt_cck_fail;

	case PHYDM_INFO_FA_TOTAL:
		return fa_t->cnt_all;

	case PHYDM_INFO_CCA_OFDM:
		return fa_t->cnt_ofdm_cca;

	case PHYDM_INFO_CCA_CCK:
		return fa_t->cnt_cck_cca;

	case PHYDM_INFO_CCA_ALL:
		return fa_t->cnt_cca_all;

	case PHYDM_INFO_CRC32_OK_VHT:
		return fa_t->cnt_vht_crc32_ok;

	case PHYDM_INFO_CRC32_OK_HT:
		return fa_t->cnt_ht_crc32_ok;

	case PHYDM_INFO_CRC32_OK_LEGACY:
		return fa_t->cnt_ofdm_crc32_ok;

	case PHYDM_INFO_CRC32_OK_CCK:
		return fa_t->cnt_cck_crc32_ok;

	case PHYDM_INFO_CRC32_ERROR_VHT:
		return fa_t->cnt_vht_crc32_error;

	case PHYDM_INFO_CRC32_ERROR_HT:
		return fa_t->cnt_ht_crc32_error;

	case PHYDM_INFO_CRC32_ERROR_LEGACY:
		return fa_t->cnt_ofdm_crc32_error;

	case PHYDM_INFO_CRC32_ERROR_CCK:
		return fa_t->cnt_cck_crc32_error;

	case PHYDM_INFO_EDCCA_FLAG:
		return fa_t->edcca_flag;

	case PHYDM_INFO_OFDM_ENABLE:
		return fa_t->ofdm_block_enable;

	case PHYDM_INFO_CCK_ENABLE:
		return fa_t->cck_block_enable;

	case PHYDM_INFO_DBG_PORT_0:
		return fa_t->dbg_port0;

	case PHYDM_INFO_CRC32_OK_HT_AGG:
		return fa_t->cnt_ht_crc32_ok_agg;

	case PHYDM_INFO_CRC32_ERROR_HT_AGG:
		return fa_t->cnt_ht_crc32_error_agg;

	/*@=== [DIG] ================================================*/

	case PHYDM_INFO_CURR_IGI:
		return dig_t->cur_ig_value;

	/*@=== [RSSI] ===============================================*/
	case PHYDM_INFO_RSSI_MIN:
		return (u32)dm->rssi_min;

	case PHYDM_INFO_RSSI_MAX:
		return (u32)dm->rssi_max;

	case PHYDM_INFO_CLM_RATIO:
		return (u32)ccx_info->clm_ratio;
	case PHYDM_INFO_NHM_RATIO:
		return (u32)ccx_info->nhm_ratio;
	case PHYDM_INFO_NHM_NOISE_PWR:
		return (u32)ccx_info->nhm_level;
	case PHYDM_INFO_NHM_PWR:
		return (u32)ccx_info->nhm_pwr;
	case PHYDM_INFO_NHM_ENV_RATIO:
		return (u32)ccx_info->nhm_env_ratio;
	case PHYDM_INFO_TXEN_CCK:
		return (u32)fa_t->cnt_cck_txen;
	case PHYDM_INFO_TXEN_OFDM:
		return (u32)fa_t->cnt_ofdm_txen;
	default:
		return 0xffffffff;
	}
}

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
void odm_init_all_work_items(struct dm_struct *dm)
{
	void *adapter = dm->adapter;
#if USE_WORKITEM

#ifdef CONFIG_ADAPTIVE_SOML
	odm_initialize_work_item(dm,
				 &dm->dm_soml_table.phydm_adaptive_soml_workitem,
				 (RT_WORKITEM_CALL_BACK)phydm_adaptive_soml_workitem_callback,
				 (void *)adapter,
				 "AdaptiveSOMLWorkitem");
#endif

#ifdef ODM_EVM_ENHANCE_ANTDIV
	odm_initialize_work_item(dm,
				 &dm->phydm_evm_antdiv_workitem,
				 (RT_WORKITEM_CALL_BACK)phydm_evm_antdiv_workitem_callback,
				 (void *)adapter,
				 "EvmAntdivWorkitem");
#endif

#ifdef CONFIG_S0S1_SW_ANTENNA_DIVERSITY
	odm_initialize_work_item(dm,
				 &dm->dm_swat_table.phydm_sw_antenna_switch_workitem,
				 (RT_WORKITEM_CALL_BACK)odm_sw_antdiv_workitem_callback,
				 (void *)adapter,
				 "AntennaSwitchWorkitem");
#endif
#if (defined(CONFIG_HL_SMART_ANTENNA))
	odm_initialize_work_item(dm,
				 &dm->dm_sat_table.hl_smart_antenna_workitem,
				 (RT_WORKITEM_CALL_BACK)phydm_beam_switch_workitem_callback,
				 (void *)adapter,
				 "hl_smart_ant_workitem");

	odm_initialize_work_item(dm,
				 &dm->dm_sat_table.hl_smart_antenna_decision_workitem,
				 (RT_WORKITEM_CALL_BACK)phydm_beam_decision_workitem_callback,
				 (void *)adapter,
				 "hl_smart_ant_decision_workitem");
#endif

	odm_initialize_work_item(
		dm,
		&dm->ra_rpt_workitem,
		(RT_WORKITEM_CALL_BACK)halrf_update_init_rate_work_item_callback,
		(void *)adapter,
		"ra_rpt_workitem");

#if (defined(CONFIG_5G_CG_SMART_ANT_DIVERSITY)) || (defined(CONFIG_2G_CG_SMART_ANT_DIVERSITY))
	odm_initialize_work_item(
		dm,
		&dm->fast_ant_training_workitem,
		(RT_WORKITEM_CALL_BACK)odm_fast_ant_training_work_item_callback,
		(void *)adapter,
		"fast_ant_training_workitem");
#endif

#endif /*#if USE_WORKITEM*/

#if (PHYDM_LA_MODE_SUPPORT == 1)
	odm_initialize_work_item(
		dm,
		&dm->adcsmp.adc_smp_work_item,
		(RT_WORKITEM_CALL_BACK)adc_smp_work_item_callback,
		(void *)adapter,
		"adc_smp_work_item");

	odm_initialize_work_item(
		dm,
		&dm->adcsmp.adc_smp_work_item_1,
		(RT_WORKITEM_CALL_BACK)adc_smp_work_item_callback,
		(void *)adapter,
		"adc_smp_work_item_1");
#endif
}

void odm_free_all_work_items(struct dm_struct *dm)
{
#if USE_WORKITEM

#ifdef CONFIG_S0S1_SW_ANTENNA_DIVERSITY
	odm_free_work_item(&dm->dm_swat_table.phydm_sw_antenna_switch_workitem);
#endif

#ifdef CONFIG_ADAPTIVE_SOML
	odm_free_work_item(&dm->dm_soml_table.phydm_adaptive_soml_workitem);
#endif

#ifdef ODM_EVM_ENHANCE_ANTDIV
	odm_free_work_item(&dm->phydm_evm_antdiv_workitem);
#endif

#if (defined(CONFIG_HL_SMART_ANTENNA))
	odm_free_work_item(&dm->dm_sat_table.hl_smart_antenna_workitem);
	odm_free_work_item(&dm->dm_sat_table.hl_smart_antenna_decision_workitem);
#endif

#if (defined(CONFIG_5G_CG_SMART_ANT_DIVERSITY)) || (defined(CONFIG_2G_CG_SMART_ANT_DIVERSITY))
	odm_free_work_item(&dm->fast_ant_training_workitem);
#endif
	odm_free_work_item(&dm->ra_rpt_workitem);
/*odm_free_work_item((&dm->sbdcnt_workitem));*/
#endif

#if (PHYDM_LA_MODE_SUPPORT == 1)
	odm_free_work_item((&dm->adcsmp.adc_smp_work_item));
	odm_free_work_item((&dm->adcsmp.adc_smp_work_item_1));
#endif
}
#endif /*#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)*/

void odm_init_all_timers(struct dm_struct *dm)
{
}

void odm_cancel_all_timers(struct dm_struct *dm)
{
}

void odm_release_all_timers(struct dm_struct *dm)
{
}

#if (DM_ODM_SUPPORT_TYPE == ODM_CE)
/* @Justin: According to the current RRSI to adjust Response Frame TX power,
 * 2012/11/05
 */
void odm_dtc(struct dm_struct *dm)
{
#ifdef CONFIG_DM_RESP_TXAGC
/* RSSI higher than this value, start to decade TX power */
#define DTC_BASE 35

/* RSSI lower than this value, start to increase TX power */
#define DTC_DWN_BASE (DTC_BASE - 5)

	/* RSSI vs TX power step mapping: decade TX power */
	static const u8 dtc_table_down[] = {
		DTC_BASE,
		(DTC_BASE + 5),
		(DTC_BASE + 10),
		(DTC_BASE + 15),
		(DTC_BASE + 20),
		(DTC_BASE + 25)};

	/* RSSI vs TX power step mapping: increase TX power */
	static const u8 dtc_table_up[] = {
		DTC_DWN_BASE,
		(DTC_DWN_BASE - 5),
		(DTC_DWN_BASE - 10),
		(DTC_DWN_BASE - 15),
		(DTC_DWN_BASE - 15),
		(DTC_DWN_BASE - 20),
		(DTC_DWN_BASE - 20),
		(DTC_DWN_BASE - 25),
		(DTC_DWN_BASE - 25),
		(DTC_DWN_BASE - 30),
		(DTC_DWN_BASE - 35)};

	u8 i;
	u8 dtc_steps = 0;
	u8 sign;
	u8 resp_txagc = 0;

	if (dm->rssi_min > DTC_BASE) {
		/* need to decade the CTS TX power */
		sign = 1;
		for (i = 0; i < ARRAY_SIZE(dtc_table_down); i++) {
			if (dtc_table_down[i] >= dm->rssi_min || dtc_steps >= 6)
				break;
			else
				dtc_steps++;
		}
	}
#if 0
	else if (dm->rssi_min > DTC_DWN_BASE) {
		/* needs to increase the CTS TX power */
		sign = 0;
		dtc_steps = 1;
		for (i = 0; i < ARRAY_SIZE(dtc_table_up); i++) {
			if (dtc_table_up[i] <= dm->rssi_min || dtc_steps >= 10)
				break;
			else
				dtc_steps++;
		}
	}
#endif
	else {
		sign = 0;
		dtc_steps = 0;
	}

	resp_txagc = dtc_steps | (sign << 4);
	resp_txagc = resp_txagc | (resp_txagc << 5);
	odm_write_1byte(dm, 0x06d9, resp_txagc);

	PHYDM_DBG(dm, ODM_COMP_PWR_TRAIN,
		  "%s rssi_min:%u, set RESP_TXAGC to %s %u\n", __func__,
		  dm->rssi_min, sign ? "minus" : "plus", dtc_steps);
#endif /* @CONFIG_RESP_TXAGC_ADJUST */
}

#endif /* @#if (DM_ODM_SUPPORT_TYPE == ODM_CE) */

void phydm_dyn_bw_indication(void *dm_void)
{
#ifdef CONFIG_BW_INDICATION
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u8 en_auto_bw_th = dm->en_auto_bw_th;

	/*driver decide bw cobime timing*/
	if (dm->dis_dym_bw_indication) {
		if (*dm->dis_dym_bw_indication)
			return;
	}

	/*check for auto bw*/
	if (dm->rssi_min <= en_auto_bw_th && dm->is_linked) {
		phydm_bw_fixed_enable(dm, FUNC_DISABLE);
		return;
	}

	phydm_bw_fixed_setting(dm);
#endif
}

