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

/*******************************************************
 * when antenna test utility is on or some testing need to disable antenna
 * diversity call this function to disable all ODM related mechanisms which
 * will switch antenna.
 *****************************************************
 */
#ifdef CONFIG_PHYDM_ANTENNA_DIVERSITY


void odm_stop_antenna_switch_dm(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_fat_struct *fat_tab = &dm->dm_fat_table;
	/* @disable ODM antenna diversity */
	dm->support_ability &= ~ODM_BB_ANT_DIV;
	if (fat_tab->div_path_type == ANT_PATH_A)
		odm_ant_div_on_off(dm, ANTDIV_OFF, ANT_PATH_A);
	else if (fat_tab->div_path_type == ANT_PATH_B)
		odm_ant_div_on_off(dm, ANTDIV_OFF, ANT_PATH_B);
	else if (fat_tab->div_path_type == ANT_PATH_AB)
		odm_ant_div_on_off(dm, ANTDIV_OFF, ANT_PATH_AB);
	odm_tx_by_tx_desc_or_reg(dm, TX_BY_REG);
	PHYDM_DBG(dm, DBG_ANT_DIV, "STOP Antenna Diversity\n");
}

void phydm_enable_antenna_diversity(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	dm->support_ability |= ODM_BB_ANT_DIV;
	dm->antdiv_select = 0;
	PHYDM_DBG(dm, DBG_ANT_DIV, "AntDiv is enabled & Re-Init AntDiv\n");
	odm_antenna_diversity_init(dm);
}

void odm_set_ant_config(void *dm_void, u8 ant_setting /* @0=A, 1=B, 2=C,...*/)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
}

/* ****************************************************** */

void odm_sw_ant_div_rest_after_link(void *dm_void)
{
#if (defined(CONFIG_PHYDM_ANTENNA_DIVERSITY))
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct sw_antenna_switch *swat_tab = &dm->dm_swat_table;
	struct phydm_fat_struct *fat_tab = &dm->dm_fat_table;
	u32 i;

	if (dm->ant_div_type == S0S1_SW_ANTDIV) {
		swat_tab->try_flag = SWAW_STEP_INIT;
		swat_tab->rssi_trying = 0;
		swat_tab->double_chk_flag = 0;
		fat_tab->rx_idle_ant = MAIN_ANT;

		for (i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++)
			phydm_antdiv_reset_statistic(dm, i);
	}

#endif
}

void phydm_n_on_off(void *dm_void, u8 swch, u8 path)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_fat_struct *fat_tab = &dm->dm_fat_table;

	if (path == ANT_PATH_A) {
		odm_set_bb_reg(dm, R_0xc50, BIT(7), swch);
	} else if (path == ANT_PATH_B) {
		odm_set_bb_reg(dm, R_0xc58, BIT(7), swch);
	} else if (path == ANT_PATH_AB) {
		odm_set_bb_reg(dm, R_0xc50, BIT(7), swch);
		odm_set_bb_reg(dm, R_0xc58, BIT(7), swch);
	}
	odm_set_bb_reg(dm, R_0xa00, BIT(15), swch);
}

void phydm_ac_on_off(void *dm_void, u8 swch, u8 path)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_fat_struct *fat_tab = &dm->dm_fat_table;

	odm_set_bb_reg(dm, R_0x8d4, BIT(24), swch);
	/* OFDM AntDiv function block enable */

	PHYDM_DBG(dm, DBG_ANT_DIV, "(Turn %s) CCK HW-AntDiv\n",
		  (swch == ANTDIV_ON) ? "ON" : "OFF");
	odm_set_bb_reg(dm, R_0x800, BIT(25), swch);
	odm_set_bb_reg(dm, R_0xa00, BIT(15), swch);
	/* @CCK AntDiv function block enable */
}

void phydm_jgr3_on_off(void *dm_void, u8 swch, u8 path)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_fat_struct *fat_tab = &dm->dm_fat_table;

	odm_set_bb_reg(dm, R_0x8a0, BIT(17), swch);
	/* OFDM AntDiv function block enable */
	odm_set_bb_reg(dm, R_0xa00, BIT(15), swch);
	/* @CCK AntDiv function block enable */
	PHYDM_DBG(dm, DBG_ANT_DIV,
		  "[8723F] AntDiv_on\n");
}

void odm_ant_div_on_off(void *dm_void, u8 swch, u8 path)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_fat_struct *fat_tab = &dm->dm_fat_table;

	if (fat_tab->ant_div_on_off != swch) {
		if (dm->ant_div_type == S0S1_SW_ANTDIV)
			return;

		PHYDM_DBG(dm, DBG_ANT_DIV,
			  "(( Turn %s )) JGR3 HW-AntDiv block\n",
			  (swch == ANTDIV_ON) ? "ON" : "OFF");
		phydm_jgr3_on_off(dm, swch, path);
	}
	fat_tab->ant_div_on_off = swch;
}

void odm_tx_by_tx_desc_or_reg(void *dm_void, u8 swch)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_fat_struct *fat_tab = &dm->dm_fat_table;
	u8 enable;

	if (fat_tab->b_fix_tx_ant == NO_FIX_TX_ANT)
		enable = (swch == TX_BY_DESC) ? 1 : 0;
	else
		enable = 0; /*@Force TX by Reg*/

	if (dm->ant_div_type != CGCS_RX_HW_ANTDIV) {
		odm_set_bb_reg(dm, R_0x186c, BIT(1), enable);

		PHYDM_DBG(dm, DBG_ANT_DIV, "[AntDiv] TX_Ant_BY (( %s ))\n",
			  (enable == TX_BY_DESC) ? "DESC" : "REG");
	}
}

void phydm_antdiv_reset_statistic(void *dm_void, u32 macid)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_fat_struct *fat_tab = &dm->dm_fat_table;

	fat_tab->main_sum[macid] = 0;
	fat_tab->aux_sum[macid] = 0;
	fat_tab->main_cnt[macid] = 0;
	fat_tab->aux_cnt[macid] = 0;
	fat_tab->main_sum_cck[macid] = 0;
	fat_tab->aux_sum_cck[macid] = 0;
	fat_tab->main_cnt_cck[macid] = 0;
	fat_tab->aux_cnt_cck[macid] = 0;
}

void phydm_fast_training_enable(void *dm_void, u8 swch)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u8 enable;

	if (swch == FAT_ON)
		enable = 1;
	else
		enable = 0;

	PHYDM_DBG(dm, DBG_ANT_DIV, "Fast ant Training_en = ((%d))\n", enable);

}

void phydm_keep_rx_ack_ant_by_tx_ant_time(void *dm_void, u32 time)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

}

void phydm_update_rx_idle_ac(void *dm_void, u8 ant, u32 default_ant,
			     u32 optional_ant, u32 default_tx_ant)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	u16 value16 = odm_read_2byte(dm, ODM_REG_TRMUX_11AC + 2);
	/* @2014/01/14 MH/Luke.Lee Add direct write for register 0xc0a to  */
	/* @prevnt incorrect 0xc08 bit0-15.We still not know why it is changed*/
	value16 &= ~(BIT(11) | BIT(10) | BIT(9) | BIT(8) | BIT(7) | BIT(6) |
		   BIT(5) | BIT(4) | BIT(3));
	value16 |= ((u16)default_ant << 3);
	value16 |= ((u16)optional_ant << 6);
	value16 |= ((u16)default_tx_ant << 9);
	odm_write_2byte(dm, ODM_REG_TRMUX_11AC + 2, value16);
}

void phydm_update_rx_idle_n(void *dm_void, u8 ant, u32 default_ant,
			    u32 optional_ant, u32 default_tx_ant)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 value32;

	odm_set_bb_reg(dm, R_0x864, 0x38, default_ant);/*@Default RX*/
	odm_set_bb_reg(dm, R_0x864, 0x1c0, optional_ant);
	odm_set_bb_reg(dm, R_0x860, 0x7000, default_tx_ant);
}

void phydm_update_rx_idle_jgr3(void *dm_void, u8 ant, u32 default_ant,
			       u32 optional_ant, u32 default_tx_ant)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 value32;

	odm_set_bb_reg(dm, R_0x1884, 0xf0, default_ant);/*@Default RX*/
	odm_set_bb_reg(dm, R_0x1884, 0xf00, optional_ant);
		/*Optional RX*/
	odm_set_bb_reg(dm, R_0x1884, 0xf000, default_tx_ant);
		/*@Default TX*/
}
void odm_update_rx_idle_ant(void *dm_void, u8 ant)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_fat_struct *fat_tab = &dm->dm_fat_table;
	u32 default_ant, optional_ant, value32, default_tx_ant;

	PHYDM_DBG(dm, DBG_ANT_DIV,"not suppoty JGR3 HW-AntDiv block\n");

	if (fat_tab->rx_idle_ant != ant) {
		PHYDM_DBG(dm, DBG_ANT_DIV,
			  "[ Update Rx-Idle-ant ] rx_idle_ant =%s\n",
			  (ant == MAIN_ANT) ? "MAIN_ANT" : "AUX_ANT");

		fat_tab->rx_idle_ant = ant;

		if (ant == MAIN_ANT) {
			default_ant = ANT1_2G;
			optional_ant = ANT2_2G;
		} else {
			default_ant = ANT2_2G;
			optional_ant = ANT1_2G;
		}

		if (fat_tab->b_fix_tx_ant != NO_FIX_TX_ANT)
			default_tx_ant = (fat_tab->b_fix_tx_ant ==
					 FIX_TX_AT_MAIN) ? 0 : 1;
		else
			default_tx_ant = default_ant;

		odm_set_mac_reg(dm, R_0x6d8, 0x700, default_tx_ant);

	} else { /* @fat_tab->rx_idle_ant == ant */
		PHYDM_DBG(dm, DBG_ANT_DIV,
			  "[ Stay in Ori-ant ]  rx_idle_ant =%s\n",
			  (ant == MAIN_ANT) ? "MAIN_ANT" : "AUX_ANT");
		fat_tab->rx_idle_ant = ant;
	}
}

void phydm_update_rx_idle_ant_pathb(void *dm_void, u8 ant)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_fat_struct *fat_tab = &dm->dm_fat_table;
	u32 default_ant, optional_ant, value32, default_tx_ant;

	if (fat_tab->rx_idle_ant2 != ant) {
		PHYDM_DBG(dm, DBG_ANT_DIV,
			  "[ Update Rx-Idle-ant2 ] rx_idle_ant2 =%s\n",
			  (ant == MAIN_ANT) ? "MAIN_ANT" : "AUX_ANT");
		if (ant == MAIN_ANT) {
			default_ant = ANT1_2G;
			optional_ant = ANT2_2G;
		} else {
			default_ant = ANT2_2G;
			optional_ant = ANT1_2G;
		}

		if (fat_tab->b_fix_tx_ant != NO_FIX_TX_ANT)
			default_tx_ant = (fat_tab->b_fix_tx_ant ==
					  FIX_TX_AT_MAIN) ? 0 : 1;
		else
			default_tx_ant = default_ant;
	} else {
		/* fat_tab->rx_idle_ant2 == ant */
		PHYDM_DBG(dm, DBG_ANT_DIV, "[Stay Ori Ant] rx_idle_ant2 = %s\n",
			  (ant == MAIN_ANT) ? "MAIN_ANT" : "AUX_ANT");
		fat_tab->rx_idle_ant2 = ant;
	}
}

void phydm_set_antdiv_val(void *dm_void, u32 *val_buf,	u8 val_len)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	if (!(dm->support_ability & ODM_BB_ANT_DIV))
		return;

	if (val_len != 1) {
		PHYDM_DBG(dm, ODM_COMP_API, "[Error][antdiv]Need val_len=1\n");
		return;
	}

	odm_update_rx_idle_ant(dm, (u8)(*val_buf));
}

void odm_update_tx_ant(void *dm_void, u8 ant, u32 mac_id)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_fat_struct *fat_tab = &dm->dm_fat_table;
	u8 tx_ant;

	if (fat_tab->b_fix_tx_ant != NO_FIX_TX_ANT)
		ant = (fat_tab->b_fix_tx_ant == FIX_TX_AT_MAIN) ?
		       MAIN_ANT : AUX_ANT;

	if (dm->ant_div_type == CG_TRX_SMART_ANTDIV)
		tx_ant = ant;
	else {
		if (ant == MAIN_ANT)
			tx_ant = ANT1_2G;
		else
			tx_ant = ANT2_2G;
	}
#if (RTL8721D_SUPPORT)
	if (dm->antdiv_gpio != ANTDIV_GPIO_PB1PB2PB26) {
		if (ant == MAIN_ANT)
			tx_ant = ANT1_2G;
		else
			tx_ant = ANT2_2G;
		}
	else
		tx_ant = fat_tab->ant_idx_vec[0]-1;
#endif
	fat_tab->antsel_a[mac_id] = tx_ant & BIT(0);
	fat_tab->antsel_b[mac_id] = (tx_ant & BIT(1)) >> 1;
	fat_tab->antsel_c[mac_id] = (tx_ant & BIT(2)) >> 2;

	PHYDM_DBG(dm, DBG_ANT_DIV,
		  "[Set TX-DESC value]: mac_id:(( %d )),  tx_ant = (( %s ))\n",
		  mac_id, (ant == MAIN_ANT) ? "MAIN_ANT" : "AUX_ANT");
#if 0
	PHYDM_DBG(dm, DBG_ANT_DIV,
		  "antsel_tr_mux=(( 3'b%d%d%d ))\n",
		  fat_tab->antsel_c[mac_id], fat_tab->antsel_b[mac_id],
		  fat_tab->antsel_a[mac_id]);
#endif
}

void odm_hw_ant_div(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 i, min_max_rssi = 0xFF, ant_div_max_rssi = 0, max_rssi = 0;
	u32 main_rssi, aux_rssi, mian_cnt, aux_cnt, local_max_rssi;
	struct phydm_fat_struct *fat_tab = &dm->dm_fat_table;
	u8 rx_idle_ant = fat_tab->rx_idle_ant, target_ant = 7;
	struct phydm_dig_struct *dig_t = &dm->dm_dig_table;
	struct cmn_sta_info *sta;

#ifdef PHYDM_BEAMFORMING_SUPPORT
#if (DM_ODM_SUPPORT_TYPE == ODM_AP)
	struct _BF_DIV_COEX_ *dm_bdc_table = &dm->dm_bdc_table;
	u32 TH1 = 500000;
	u32 TH2 = 10000000;
	u32 ma_rx_temp, degrade_TP_temp, improve_TP_temp;
	u8 monitor_rssi_threshold = 30;

	dm_bdc_table->BF_pass = true;
	dm_bdc_table->DIV_pass = true;
	dm_bdc_table->is_all_div_sta_idle = true;
	dm_bdc_table->is_all_bf_sta_idle = true;
	dm_bdc_table->num_bf_tar = 0;
	dm_bdc_table->num_div_tar = 0;
	dm_bdc_table->num_client = 0;
#endif
#endif

	if (!dm->is_linked) { /* @is_linked==False */
		PHYDM_DBG(dm, DBG_ANT_DIV, "[No Link!!!]\n");

		if (fat_tab->is_become_linked) {
			if (fat_tab->div_path_type == ANT_PATH_A)
				odm_ant_div_on_off(dm, ANTDIV_OFF, ANT_PATH_A);
			else if (fat_tab->div_path_type == ANT_PATH_B)
				odm_ant_div_on_off(dm, ANTDIV_OFF, ANT_PATH_B);
			else if (fat_tab->div_path_type == ANT_PATH_AB)
				odm_ant_div_on_off(dm, ANTDIV_OFF, ANT_PATH_AB);
			odm_update_rx_idle_ant(dm, MAIN_ANT);
			odm_tx_by_tx_desc_or_reg(dm, TX_BY_REG);
			dm->antdiv_period = 0;

			fat_tab->is_become_linked = dm->is_linked;
		}
		return;
	} else {
		if (!fat_tab->is_become_linked) {
			PHYDM_DBG(dm, DBG_ANT_DIV, "[Linked !!!]\n");
			if (fat_tab->div_path_type == ANT_PATH_A)
				odm_ant_div_on_off(dm, ANTDIV_ON, ANT_PATH_A);
			else if (fat_tab->div_path_type == ANT_PATH_B)
				odm_ant_div_on_off(dm, ANTDIV_ON, ANT_PATH_B);
			else if (fat_tab->div_path_type == ANT_PATH_AB)
				odm_ant_div_on_off(dm, ANTDIV_ON, ANT_PATH_AB);

			fat_tab->is_become_linked = dm->is_linked;

			/* @ BDC Init */
			#ifdef PHYDM_BEAMFORMING_SUPPORT
			#if (DM_ODM_SUPPORT_TYPE == ODM_AP)
			odm_bdc_init(dm);
			#endif
			#endif
		}
	}

	if (!(*fat_tab->p_force_tx_by_desc)) {
		if (dm->is_one_entry_only)
			odm_tx_by_tx_desc_or_reg(dm, TX_BY_REG);
		else
			odm_tx_by_tx_desc_or_reg(dm, TX_BY_DESC);
	}

/* @2 BDC mode Arbitration */
#ifdef PHYDM_BEAMFORMING_SUPPORT
#if (DM_ODM_SUPPORT_TYPE == ODM_AP)
	if (dm->antdiv_evm_en == 0 || fat_tab->evm_method_enable == 0)
		odm_bf_ant_div_mode_arbitration(dm);
#endif
#endif

	for (i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++) {
		sta = dm->phydm_sta_info[i];
		if (!is_sta_active(sta)) {
			phydm_antdiv_reset_statistic(dm, i);
			continue;
		}

		/* @2 Caculate RSSI per Antenna */
		if (fat_tab->main_cnt[i] != 0 || fat_tab->aux_cnt[i] != 0) {
			mian_cnt = fat_tab->main_cnt[i];
			aux_cnt = fat_tab->aux_cnt[i];
			main_rssi = (mian_cnt != 0) ?
				    (fat_tab->main_sum[i] / mian_cnt) : 0;
			aux_rssi = (aux_cnt != 0) ?
				   (fat_tab->aux_sum[i] / aux_cnt) : 0;
			target_ant = (mian_cnt == aux_cnt) ?
				     fat_tab->rx_idle_ant :
				     ((mian_cnt >= aux_cnt) ?
				     fat_tab->ant_idx_vec[0]:fat_tab->ant_idx_vec[1]);
				     /*Use counter number for OFDM*/

		} else { /*@CCK only case*/
			mian_cnt = fat_tab->main_cnt_cck[i];
			aux_cnt = fat_tab->aux_cnt_cck[i];
			main_rssi = (mian_cnt != 0) ?
				    (fat_tab->main_sum_cck[i] / mian_cnt) : 0;
			aux_rssi = (aux_cnt != 0) ?
				   (fat_tab->aux_sum_cck[i] / aux_cnt) : 0;
			target_ant = (main_rssi == aux_rssi) ?
				     fat_tab->rx_idle_ant :
				     ((main_rssi >= aux_rssi) ?
				     fat_tab->ant_idx_vec[0]:fat_tab->ant_idx_vec[1]);
				     /*Use RSSI for CCK only case*/
		}
#if (RTL8721D_SUPPORT)
	if(dm->antdiv_gpio == ANTDIV_GPIO_PB1PB2PB26) { /* added by Jiao Qi on May.25,2020, only for 3 antenna diversity */
		u8 tmp;
		if(target_ant == fat_tab->ant_idx_vec[0]){/* switch the second & third ant index */
			tmp = fat_tab->ant_idx_vec[1];
			fat_tab->ant_idx_vec[1] = fat_tab->ant_idx_vec[2];
			fat_tab->ant_idx_vec[2] = tmp;
		}else{
			/* switch the first & second ant index */
			tmp = fat_tab->ant_idx_vec[0];
			fat_tab->ant_idx_vec[0] = fat_tab->ant_idx_vec[1];
			fat_tab->ant_idx_vec[1] = tmp;
			/* switch the second & third ant index */
			tmp = fat_tab->ant_idx_vec[1];
			fat_tab->ant_idx_vec[1] = fat_tab->ant_idx_vec[2];
			fat_tab->ant_idx_vec[2] = tmp;
		}
	}
#endif

		PHYDM_DBG(dm, DBG_ANT_DIV,
			  "*** Client[ %d ] : Main_Cnt = (( %d ))  ,  CCK_Main_Cnt = (( %d )) ,  main_rssi= ((  %d ))\n",
			  i, fat_tab->main_cnt[i],
			  fat_tab->main_cnt_cck[i], main_rssi);
		PHYDM_DBG(dm, DBG_ANT_DIV,
			  "*** Client[ %d ] : Aux_Cnt   = (( %d ))  , CCK_Aux_Cnt   = (( %d )) ,  aux_rssi = ((  %d ))\n",
			  i, fat_tab->aux_cnt[i],
			  fat_tab->aux_cnt_cck[i], aux_rssi);

		local_max_rssi = (main_rssi > aux_rssi) ? main_rssi : aux_rssi;
		/* @ Select max_rssi for DIG */
		if (local_max_rssi > ant_div_max_rssi && local_max_rssi < 40)
			ant_div_max_rssi = local_max_rssi;
		if (local_max_rssi > max_rssi)
			max_rssi = local_max_rssi;

		/* @ Select RX Idle Antenna */
		if (local_max_rssi != 0 && local_max_rssi < min_max_rssi) {
			rx_idle_ant = target_ant;
			min_max_rssi = local_max_rssi;
		}

		/* @2 Select TX Antenna */
		if (dm->ant_div_type != CGCS_RX_HW_ANTDIV) {
			#ifdef PHYDM_BEAMFORMING_SUPPORT
			#if (DM_ODM_SUPPORT_TYPE == ODM_AP)
			if (dm_bdc_table->w_bfee_client[i] == 0)
			#endif
			#endif
			{
				odm_update_tx_ant(dm, target_ant, i);
			}
		}

/* @------------------------------------------------------------ */

		#ifdef PHYDM_BEAMFORMING_SUPPORT
		#if (DM_ODM_SUPPORT_TYPE == ODM_AP)

		dm_bdc_table->num_client++;

		if (dm_bdc_table->bdc_mode == BDC_MODE_2 || dm_bdc_table->bdc_mode == BDC_MODE_3) {
			/* @2 Byte counter */

			ma_rx_temp = sta->rx_moving_average_tp; /* RX  TP   ( bit /sec) */

			if (dm_bdc_table->BDC_state == bdc_bfer_train_state)
				dm_bdc_table->MA_rx_TP_DIV[i] = ma_rx_temp;
			else
				dm_bdc_table->MA_rx_TP[i] = ma_rx_temp;

			if (ma_rx_temp < TH2 && ma_rx_temp > TH1 && local_max_rssi <= monitor_rssi_threshold) {
				if (dm_bdc_table->w_bfer_client[i] == 1) { /* @Bfer_Target */
					dm_bdc_table->num_bf_tar++;

					if (dm_bdc_table->BDC_state == BDC_DECISION_STATE && dm_bdc_table->bdc_try_flag == 0) {
						improve_TP_temp = (dm_bdc_table->MA_rx_TP_DIV[i] * 9) >> 3; /* @* 1.125 */
						dm_bdc_table->BF_pass = (dm_bdc_table->MA_rx_TP[i] > improve_TP_temp) ? true : false;
						PHYDM_DBG(dm, DBG_ANT_DIV, "*** Client[ %d ] :  { MA_rx_TP,improve_TP_temp, MA_rx_TP_DIV,  BF_pass}={ %d,  %d, %d , %d }\n", i, dm_bdc_table->MA_rx_TP[i], improve_TP_temp, dm_bdc_table->MA_rx_TP_DIV[i], dm_bdc_table->BF_pass);
					}
				} else { /* @DIV_Target */
					dm_bdc_table->num_div_tar++;

					if (dm_bdc_table->BDC_state == BDC_DECISION_STATE && dm_bdc_table->bdc_try_flag == 0) {
						degrade_TP_temp = (dm_bdc_table->MA_rx_TP_DIV[i] * 5) >> 3; /* @* 0.625 */
						dm_bdc_table->DIV_pass = (dm_bdc_table->MA_rx_TP[i] > degrade_TP_temp) ? true : false;
						PHYDM_DBG(dm, DBG_ANT_DIV, "*** Client[ %d ] :  { MA_rx_TP, degrade_TP_temp, MA_rx_TP_DIV,  DIV_pass}=\n{ %d,  %d, %d , %d }\n", i, dm_bdc_table->MA_rx_TP[i], degrade_TP_temp, dm_bdc_table->MA_rx_TP_DIV[i], dm_bdc_table->DIV_pass);
					}
				}
			}

			if (ma_rx_temp > TH1) {
				if (dm_bdc_table->w_bfer_client[i] == 1) /* @Bfer_Target */
					dm_bdc_table->is_all_bf_sta_idle = false;
				else /* @DIV_Target */
					dm_bdc_table->is_all_div_sta_idle = false;
			}

			PHYDM_DBG(dm, DBG_ANT_DIV,
				  "*** Client[ %d ] :  { BFmeeCap, BFmerCap}  = { %d , %d }\n",
				  i, dm_bdc_table->w_bfee_client[i],
				  dm_bdc_table->w_bfer_client[i]);

			if (dm_bdc_table->BDC_state == bdc_bfer_train_state)
				PHYDM_DBG(dm, DBG_ANT_DIV, "*** Client[ %d ] :    MA_rx_TP_DIV = (( %d ))\n", i, dm_bdc_table->MA_rx_TP_DIV[i]);

			else
				PHYDM_DBG(dm, DBG_ANT_DIV, "*** Client[ %d ] :    MA_rx_TP = (( %d ))\n", i, dm_bdc_table->MA_rx_TP[i]);
		}
		#endif
		#endif

		#ifdef PHYDM_BEAMFORMING_SUPPORT
		#if (DM_ODM_SUPPORT_TYPE == ODM_AP)
		if (dm_bdc_table->bdc_try_flag == 0)
		#endif
		#endif
		{
			phydm_antdiv_reset_statistic(dm, i);
		}
	}

/* @2 Set RX Idle Antenna & TX Antenna(Because of HW Bug ) */
#if (DM_ODM_SUPPORT_TYPE == ODM_AP)
	PHYDM_DBG(dm, DBG_ANT_DIV, "*** rx_idle_ant = (( %s ))\n",
		  (rx_idle_ant == MAIN_ANT) ? "MAIN_ANT" : "AUX_ANT");

#ifdef PHYDM_BEAMFORMING_SUPPORT
#if (DM_ODM_SUPPORT_TYPE == ODM_AP)
	if (dm_bdc_table->bdc_mode == BDC_MODE_1 || dm_bdc_table->bdc_mode == BDC_MODE_3) {
		PHYDM_DBG(dm, DBG_ANT_DIV,
			  "*** bdc_rx_idle_update_counter = (( %d ))\n",
			  dm_bdc_table->bdc_rx_idle_update_counter);

		if (dm_bdc_table->bdc_rx_idle_update_counter == 1) {
			PHYDM_DBG(dm, DBG_ANT_DIV,
				  "***Update RxIdle Antenna!!!\n");
			dm_bdc_table->bdc_rx_idle_update_counter = 30;
			odm_update_rx_idle_ant(dm, rx_idle_ant);
		} else {
			dm_bdc_table->bdc_rx_idle_update_counter--;
			PHYDM_DBG(dm, DBG_ANT_DIV,
				  "***NOT update RxIdle Antenna because of BF  ( need to fix TX-ant)\n");
		}
	} else
#endif
#endif
		odm_update_rx_idle_ant(dm, rx_idle_ant);
#else
#if (RTL8721D_SUPPORT)
if (dm->antdiv_gpio == ANTDIV_GPIO_PB1PB2PB26) {
	if(odm_get_bb_reg(dm,R_0xc50,0x80) || odm_get_bb_reg(dm, R_0xa00, 0x8000))
		odm_update_rx_idle_ant_sp3t(dm, rx_idle_ant);
}
else
#endif
	odm_update_rx_idle_ant(dm, rx_idle_ant);

#endif /* @#if(DM_ODM_SUPPORT_TYPE  == ODM_AP) */

/* @2 BDC Main Algorithm */
#ifdef PHYDM_BEAMFORMING_SUPPORT
#if (DM_ODM_SUPPORT_TYPE == ODM_AP)
	if (dm->antdiv_evm_en == 0 || fat_tab->evm_method_enable == 0)
		odm_bd_ccoex_bfee_rx_div_arbitration(dm);

	dm_bdc_table->num_txbfee_client = 0;
	dm_bdc_table->num_txbfer_client = 0;
#endif
#endif

	if (ant_div_max_rssi == 0)
		dig_t->ant_div_rssi_max = dm->rssi_min;
	else
		dig_t->ant_div_rssi_max = ant_div_max_rssi;

	PHYDM_DBG(dm, DBG_ANT_DIV, "***AntDiv End***\n\n");
}

#ifdef CONFIG_S0S1_SW_ANTENNA_DIVERSITY

void odm_s0s1_sw_ant_div_reset(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct sw_antenna_switch *swat_tab = &dm->dm_swat_table;
	struct phydm_fat_struct *fat_tab = &dm->dm_fat_table;

	fat_tab->is_become_linked = false;
	swat_tab->try_flag = SWAW_STEP_INIT;
	swat_tab->double_chk_flag = 0;

	PHYDM_DBG(dm, DBG_ANT_DIV, "%s: fat_tab->is_become_linked = %d\n",
		  __func__, fat_tab->is_become_linked);
}

void phydm_sw_antdiv_train_time(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct sw_antenna_switch *swat_tab = &dm->dm_swat_table;
	u8 high_traffic_train_time_u = 0x32, high_traffic_train_time_l = 0;
	u8 low_traffic_train_time_u = 200, low_traffic_train_time_l = 0;
	u8 train_time_temp;

	if (dm->traffic_load == TRAFFIC_HIGH) {
		train_time_temp = swat_tab->train_time;

		if (swat_tab->train_time_flag == 3) {
			high_traffic_train_time_l = 0xa;

			if (train_time_temp <= 16)
				train_time_temp = high_traffic_train_time_l;
			else
				train_time_temp -= 16;

		} else if (swat_tab->train_time_flag == 2) {
			train_time_temp -= 8;
			high_traffic_train_time_l = 0xf;
		} else if (swat_tab->train_time_flag == 1) {
			train_time_temp -= 4;
			high_traffic_train_time_l = 0x1e;
		} else if (swat_tab->train_time_flag == 0) {
			train_time_temp += 8;
			high_traffic_train_time_l = 0x28;
		}

		/* @-- */
		if (train_time_temp > high_traffic_train_time_u)
			train_time_temp = high_traffic_train_time_u;

		else if (train_time_temp < high_traffic_train_time_l)
			train_time_temp = high_traffic_train_time_l;

		swat_tab->train_time = train_time_temp; /*@10ms~200ms*/

		PHYDM_DBG(dm, DBG_ANT_DIV,
			  "train_time_flag=((%d)), train_time=((%d))\n",
			  swat_tab->train_time_flag,
			  swat_tab->train_time);

	} else if ((dm->traffic_load == TRAFFIC_MID) ||
		   (dm->traffic_load == TRAFFIC_LOW)) {
		train_time_temp = swat_tab->train_time;

		if (swat_tab->train_time_flag == 3) {
			low_traffic_train_time_l = 10;
			if (train_time_temp < 50)
				train_time_temp = low_traffic_train_time_l;
			else
				train_time_temp -= 50;
		} else if (swat_tab->train_time_flag == 2) {
			train_time_temp -= 30;
			low_traffic_train_time_l = 36;
		} else if (swat_tab->train_time_flag == 1) {
			train_time_temp -= 10;
			low_traffic_train_time_l = 40;
		} else {
			train_time_temp += 10;
			low_traffic_train_time_l = 50;
		}

		/* @-- */
		if (train_time_temp >= low_traffic_train_time_u)
			train_time_temp = low_traffic_train_time_u;

		else if (train_time_temp <= low_traffic_train_time_l)
			train_time_temp = low_traffic_train_time_l;

		swat_tab->train_time = train_time_temp; /*@10ms~200ms*/

		PHYDM_DBG(dm, DBG_ANT_DIV,
			  "train_time_flag=((%d)) , train_time=((%d))\n",
			  swat_tab->train_time_flag, swat_tab->train_time);

	} else {
		swat_tab->train_time = 0xc8; /*@200ms*/
	}
}

void phydm_sw_antdiv_decision(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct sw_antenna_switch *swat_tab = &dm->dm_swat_table;
	struct phydm_fat_struct *fat_tab = &dm->dm_fat_table;
	u32 i, min_max_rssi = 0xFF, local_max_rssi, local_min_rssi;
	u32 main_rssi, aux_rssi;
	u8 rx_idle_ant = swat_tab->pre_ant;
	u8 target_ant = swat_tab->pre_ant, next_ant = 0;
	struct cmn_sta_info *entry = NULL;
	u32 main_cnt = 0, aux_cnt = 0, main_sum = 0, aux_sum = 0;
	u32 main_ctrl_cnt = 0, aux_ctrl_cnt = 0;
	boolean is_by_ctrl_frame = false;
	boolean cond_23d_main, cond_23d_aux;
	u64 pkt_cnt_total = 0;

	for (i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++) {
		entry = dm->phydm_sta_info[i];
		if (!is_sta_active(entry)) {
			phydm_antdiv_reset_statistic(dm, i);
			continue;
		}

		/* @2 Caculate RSSI per Antenna */
		if (fat_tab->main_cnt[i] != 0 || fat_tab->aux_cnt[i] != 0) {
			main_cnt = (u32)fat_tab->main_cnt[i];
			aux_cnt = (u32)fat_tab->aux_cnt[i];
			main_rssi = (main_cnt != 0) ?
				    (fat_tab->main_sum[i] / main_cnt) : 0;
			aux_rssi = (aux_cnt != 0) ?
				   (fat_tab->aux_sum[i] / aux_cnt) : 0;
			if (swat_tab->pre_ant == MAIN_ANT) {
				target_ant = (aux_rssi > main_rssi) ?
					     AUX_ANT :
					     swat_tab->pre_ant;
			} else if (swat_tab->pre_ant == AUX_ANT) {
				target_ant = (main_rssi > aux_rssi) ?
					     MAIN_ANT :
					     swat_tab->pre_ant;
			}
		} else { /*@CCK only case*/
			main_cnt = fat_tab->main_cnt_cck[i];
			aux_cnt = fat_tab->aux_cnt_cck[i];
			main_rssi = (main_cnt != 0) ?
				    (fat_tab->main_sum_cck[i] / main_cnt) : 0;
			aux_rssi = (aux_cnt != 0) ?
				   (fat_tab->aux_sum_cck[i] / aux_cnt) : 0;
			target_ant = (main_rssi == aux_rssi) ?
				     swat_tab->pre_ant :
				     ((main_rssi >= aux_rssi) ?
				     MAIN_ANT : AUX_ANT);
				     /*Use RSSI for CCK only case*/
		}
		local_max_rssi = (main_rssi >= aux_rssi) ? main_rssi : aux_rssi;
		local_min_rssi = (main_rssi >= aux_rssi) ? aux_rssi : main_rssi;

		PHYDM_DBG(dm, DBG_ANT_DIV,
			  "***  CCK_counter_main = (( %d ))  , CCK_counter_aux= ((  %d ))\n",
			  fat_tab->main_cnt_cck[i], fat_tab->aux_cnt_cck[i]);
		PHYDM_DBG(dm, DBG_ANT_DIV,
			  "***  OFDM_counter_main = (( %d ))  , OFDM_counter_aux= ((  %d ))\n",
			  fat_tab->main_cnt[i], fat_tab->aux_cnt[i]);
		PHYDM_DBG(dm, DBG_ANT_DIV,
			  "***  main_Cnt = (( %d ))  , aux_Cnt   = (( %d ))\n",
			  main_cnt, aux_cnt);
		PHYDM_DBG(dm, DBG_ANT_DIV,
			  "***  main_rssi= ((  %d )) , aux_rssi = ((  %d ))\n",
			  main_rssi, aux_rssi);
		PHYDM_DBG(dm, DBG_ANT_DIV,
			  "*** MAC ID:[ %d ] , target_ant = (( %s ))\n", i,
			  (target_ant == MAIN_ANT) ? "MAIN_ANT" : "AUX_ANT");

		/* @2 Select RX Idle Antenna */

		if (local_max_rssi != 0 && local_max_rssi < min_max_rssi) {
			rx_idle_ant = target_ant;
			min_max_rssi = local_max_rssi;
			PHYDM_DBG(dm, DBG_ANT_DIV,
				  "*** local_max_rssi-local_min_rssi = ((%d))\n",
				  (local_max_rssi - local_min_rssi));

			if ((local_max_rssi - local_min_rssi) > 8) {
				if (local_min_rssi != 0) {
					swat_tab->train_time_flag = 3;
				} else {
					if (min_max_rssi > RSSI_CHECK_THRESHOLD)
						swat_tab->train_time_flag = 0;
					else
						swat_tab->train_time_flag = 3;
				}
			} else if ((local_max_rssi - local_min_rssi) > 5) {
				swat_tab->train_time_flag = 2;
			} else if ((local_max_rssi - local_min_rssi) > 2) {
				swat_tab->train_time_flag = 1;
			} else {
				swat_tab->train_time_flag = 0;
			}
		}

		/* @2 Select TX Antenna */
		if (target_ant == MAIN_ANT)
			fat_tab->antsel_a[i] = ANT1_2G;
		else
			fat_tab->antsel_a[i] = ANT2_2G;

		phydm_antdiv_reset_statistic(dm, i);
		pkt_cnt_total += (main_cnt + aux_cnt);
	}

	if (swat_tab->is_sw_ant_div_by_ctrl_frame) {
		odm_s0s1_sw_ant_div_by_ctrl_frame(dm, SWAW_STEP_DETERMINE);
		is_by_ctrl_frame = true;
	}

	PHYDM_DBG(dm, DBG_ANT_DIV,
		  "Control frame packet counter = %d, data frame packet counter = %llu\n",
		  swat_tab->pkt_cnt_sw_ant_div_by_ctrl_frame, pkt_cnt_total);

	if (min_max_rssi == 0xff || ((pkt_cnt_total <
	    (swat_tab->pkt_cnt_sw_ant_div_by_ctrl_frame >> 1)) &&
	    dm->phy_dbg_info.num_qry_beacon_pkt < 2)) {
		min_max_rssi = 0;
		PHYDM_DBG(dm, DBG_ANT_DIV,
			  "Check RSSI of control frame because min_max_rssi == 0xff\n");
		PHYDM_DBG(dm, DBG_ANT_DIV, "is_by_ctrl_frame = %d\n",
			  is_by_ctrl_frame);

		if (is_by_ctrl_frame) {
			main_ctrl_cnt = fat_tab->main_ctrl_cnt;
			aux_ctrl_cnt = fat_tab->aux_ctrl_cnt;
			main_rssi = (main_ctrl_cnt != 0) ?
				    (fat_tab->main_ctrl_sum / main_ctrl_cnt) :
				    0;
			aux_rssi = (aux_ctrl_cnt != 0) ?
				   (fat_tab->aux_ctrl_sum / aux_ctrl_cnt) : 0;

			if (main_ctrl_cnt <= 1 &&
			    fat_tab->cck_ctrl_frame_cnt_main >= 1)
				main_rssi = 0;

			if (aux_ctrl_cnt <= 1 &&
			    fat_tab->cck_ctrl_frame_cnt_aux >= 1)
				aux_rssi = 0;

			if (main_rssi != 0 || aux_rssi != 0) {
				rx_idle_ant = (main_rssi == aux_rssi) ?
					      swat_tab->pre_ant :
					      ((main_rssi >= aux_rssi) ?
					      MAIN_ANT : AUX_ANT);
				local_max_rssi = (main_rssi >= aux_rssi) ?
						 main_rssi : aux_rssi;
				local_min_rssi = (main_rssi >= aux_rssi) ?
						 aux_rssi : main_rssi;

				if ((local_max_rssi - local_min_rssi) > 8)
					swat_tab->train_time_flag = 3;
				else if ((local_max_rssi - local_min_rssi) > 5)
					swat_tab->train_time_flag = 2;
				else if ((local_max_rssi - local_min_rssi) > 2)
					swat_tab->train_time_flag = 1;
				else
					swat_tab->train_time_flag = 0;

				PHYDM_DBG(dm, DBG_ANT_DIV,
					  "Control frame: main_rssi = %d, aux_rssi = %d\n",
					  main_rssi, aux_rssi);
				PHYDM_DBG(dm, DBG_ANT_DIV,
					  "rx_idle_ant decided by control frame = %s\n",
					  (rx_idle_ant == MAIN_ANT ?
					  "MAIN" : "AUX"));
			}
		}
	}

	fat_tab->min_max_rssi = min_max_rssi;
	swat_tab->try_flag = SWAW_STEP_PEEK;

	if (swat_tab->double_chk_flag == 1) {
		swat_tab->double_chk_flag = 0;

		if (fat_tab->min_max_rssi > RSSI_CHECK_THRESHOLD) {
			PHYDM_DBG(dm, DBG_ANT_DIV,
				  " [Double check] min_max_rssi ((%d)) > %d again!!\n",
				  fat_tab->min_max_rssi, RSSI_CHECK_THRESHOLD);

			odm_update_rx_idle_ant(dm, rx_idle_ant);

			PHYDM_DBG(dm, DBG_ANT_DIV,
				  "[reset try_flag = 0] Training accomplished !!!]\n\n\n");
		} else {
			PHYDM_DBG(dm, DBG_ANT_DIV,
				  " [Double check] min_max_rssi ((%d)) <= %d !!\n",
				  fat_tab->min_max_rssi, RSSI_CHECK_THRESHOLD);

			next_ant = (fat_tab->rx_idle_ant == MAIN_ANT) ?
				   AUX_ANT : MAIN_ANT;
			swat_tab->try_flag = SWAW_STEP_PEEK;
			swat_tab->reset_idx = RSSI_CHECK_RESET_PERIOD;
			PHYDM_DBG(dm, DBG_ANT_DIV,
				  "[set try_flag=0]  Normal state:  Need to tryg again!!\n\n\n");
		}
	} else {
		if (fat_tab->min_max_rssi < RSSI_CHECK_THRESHOLD)
			swat_tab->reset_idx = RSSI_CHECK_RESET_PERIOD;

		swat_tab->pre_ant = rx_idle_ant;
		odm_update_rx_idle_ant(dm, rx_idle_ant);
		PHYDM_DBG(dm, DBG_ANT_DIV,
			  "[reset try_flag = 0] Training accomplished !!!]\n\n\n");
	}
}

void odm_s0s1_sw_ant_div(void *dm_void, u8 step)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct sw_antenna_switch *swat_tab = &dm->dm_swat_table;
	struct phydm_fat_struct *fat_tab = &dm->dm_fat_table;
	u32 value32;
	u8 next_ant = 0;

	if (!dm->is_linked) { /* @is_linked==False */
		PHYDM_DBG(dm, DBG_ANT_DIV, "[No Link!!!]\n");
		if (fat_tab->is_become_linked == true) {
			odm_tx_by_tx_desc_or_reg(dm, TX_BY_REG);
			fat_tab->is_become_linked = dm->is_linked;
		}
		return;
	} else {
		if (fat_tab->is_become_linked == false) {
			PHYDM_DBG(dm, DBG_ANT_DIV, "[Linked !!!]\n");

			fat_tab->is_become_linked = dm->is_linked;
		}
	}

	if (!(*fat_tab->p_force_tx_by_desc)) {
		if (dm->is_one_entry_only == true)
			odm_tx_by_tx_desc_or_reg(dm, TX_BY_REG);
		else
			odm_tx_by_tx_desc_or_reg(dm, TX_BY_DESC);
	}

	PHYDM_DBG(dm, DBG_ANT_DIV,
		  "[%d] { try_flag=(( %d )), step=(( %d )), double_chk_flag = (( %d )) }\n",
		  __LINE__, swat_tab->try_flag, step,
		  swat_tab->double_chk_flag);

	/* @ Handling step mismatch condition. */
	/* @ Peak step is not finished at last time. */
	/* @ Recover the variable and check again. */
	if (step != swat_tab->try_flag) {
		PHYDM_DBG(dm, DBG_ANT_DIV,
			  "[step != try_flag]    Need to Reset After Link\n");
		odm_sw_ant_div_rest_after_link(dm);
	}

	if (swat_tab->try_flag == SWAW_STEP_INIT) {
		swat_tab->try_flag = SWAW_STEP_PEEK;
		swat_tab->train_time_flag = 0;
		PHYDM_DBG(dm, DBG_ANT_DIV,
			  "[set try_flag = 0]  Prepare for peek!\n\n");
		return;

	} else {
		/* @1 Normal state (Begin Trying) */
		if (swat_tab->try_flag == SWAW_STEP_PEEK) {
			PHYDM_DBG(dm, DBG_ANT_DIV,
				  "TxOkCnt=(( %llu )), RxOkCnt=(( %llu )), traffic_load = (%d))\n",
				  dm->cur_tx_ok_cnt, dm->cur_rx_ok_cnt,
				  dm->traffic_load);
			phydm_sw_antdiv_train_time(dm);

			PHYDM_DBG(dm, DBG_ANT_DIV,
				  "Current min_max_rssi is ((%d))\n",
				  fat_tab->min_max_rssi);

			/* @---reset index--- */
			if (swat_tab->reset_idx >= RSSI_CHECK_RESET_PERIOD) {
				fat_tab->min_max_rssi = 0;
				swat_tab->reset_idx = 0;
			}
			PHYDM_DBG(dm, DBG_ANT_DIV, "reset_idx = (( %d ))\n",
				  swat_tab->reset_idx);

			swat_tab->reset_idx++;

			/* @---double check flag--- */
			if (fat_tab->min_max_rssi > RSSI_CHECK_THRESHOLD &&
			    swat_tab->double_chk_flag == 0) {
				PHYDM_DBG(dm, DBG_ANT_DIV,
					  " min_max_rssi is ((%d)), and > %d\n",
					  fat_tab->min_max_rssi,
					  RSSI_CHECK_THRESHOLD);

				swat_tab->double_chk_flag = 1;
				swat_tab->try_flag = SWAW_STEP_DETERMINE;
				swat_tab->rssi_trying = 0;

				PHYDM_DBG(dm, DBG_ANT_DIV,
					  "Test the current ant for (( %d )) ms again\n",
					  swat_tab->train_time);
				odm_update_rx_idle_ant(dm,
						       fat_tab->rx_idle_ant);
				odm_set_timer(dm, &swat_tab->sw_antdiv_timer,
					      swat_tab->train_time); /*@ms*/
				return;
			}

			next_ant = (fat_tab->rx_idle_ant == MAIN_ANT) ?
				   AUX_ANT : MAIN_ANT;

			swat_tab->try_flag = SWAW_STEP_DETERMINE;

			if (swat_tab->reset_idx <= 1)
				swat_tab->rssi_trying = 2;
			else
				swat_tab->rssi_trying = 1;

			odm_s0s1_sw_ant_div_by_ctrl_frame(dm, SWAW_STEP_PEEK);
			PHYDM_DBG(dm, DBG_ANT_DIV,
				  "[set try_flag=1]  Normal state:  Begin Trying!!\n");

		} else if ((swat_tab->try_flag == SWAW_STEP_DETERMINE) &&
			   (swat_tab->double_chk_flag == 0)) {
			next_ant = (fat_tab->rx_idle_ant == MAIN_ANT) ?
				   AUX_ANT : MAIN_ANT;
			swat_tab->rssi_trying--;
		}

		/* @1 Decision state */
		if (swat_tab->try_flag == SWAW_STEP_DETERMINE &&
		    swat_tab->rssi_trying == 0) {
			phydm_sw_antdiv_decision(dm);
			return;
		}
	}

	/* @1 4.Change TRX antenna */

	PHYDM_DBG(dm, DBG_ANT_DIV,
		  "rssi_trying = (( %d )),    ant: (( %s )) >>> (( %s ))\n",
		  swat_tab->rssi_trying,
		  (fat_tab->rx_idle_ant == MAIN_ANT ? "MAIN" : "AUX"),
		  (next_ant == MAIN_ANT ? "MAIN" : "AUX"));

	odm_update_rx_idle_ant(dm, next_ant);

	/* @1 5.Reset Statistics */

	fat_tab->rx_idle_ant = next_ant;

	/* @1 6.Set next timer   (Trying state) */
	PHYDM_DBG(dm, DBG_ANT_DIV, " Test ((%s)) ant for (( %d )) ms\n",
		  (next_ant == MAIN_ANT ? "MAIN" : "AUX"),
		  swat_tab->train_time);
	odm_set_timer(dm, &swat_tab->sw_antdiv_timer, swat_tab->train_time);
								/*@ms*/
}


void odm_sw_antdiv_workitem_callback(void *context)
{
	void *
		adapter = (void *)context;
	HAL_DATA_TYPE
	*hal_data = GET_HAL_DATA(((PADAPTER)adapter));

	odm_s0s1_sw_ant_div(&hal_data->odmpriv, SWAW_STEP_DETERMINE);
}

void odm_sw_antdiv_callback(void *function_context)
{
	struct dm_struct *dm = (struct dm_struct *)function_context;
	void *padapter = dm->adapter;
	if (*dm->is_net_closed == true)
		return;

	rtw_run_in_thread_cmd(padapter, odm_sw_antdiv_workitem_callback,
			      padapter);
}

void odm_s0s1_sw_ant_div_by_ctrl_frame(void *dm_void, u8 step)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct sw_antenna_switch *swat_tab = &dm->dm_swat_table;
	struct phydm_fat_struct *fat_tab = &dm->dm_fat_table;

	switch (step) {
	case SWAW_STEP_PEEK:
		swat_tab->pkt_cnt_sw_ant_div_by_ctrl_frame = 0;
		swat_tab->is_sw_ant_div_by_ctrl_frame = true;
		fat_tab->main_ctrl_cnt = 0;
		fat_tab->aux_ctrl_cnt = 0;
		fat_tab->main_ctrl_sum = 0;
		fat_tab->aux_ctrl_sum = 0;
		fat_tab->cck_ctrl_frame_cnt_main = 0;
		fat_tab->cck_ctrl_frame_cnt_aux = 0;
		fat_tab->ofdm_ctrl_frame_cnt_main = 0;
		fat_tab->ofdm_ctrl_frame_cnt_aux = 0;
		PHYDM_DBG(dm, DBG_ANT_DIV,
			  "odm_S0S1_SwAntDivForAPMode(): Start peek and reset counter\n");
		break;
	case SWAW_STEP_DETERMINE:
		swat_tab->is_sw_ant_div_by_ctrl_frame = false;
		PHYDM_DBG(dm, DBG_ANT_DIV,
			  "odm_S0S1_SwAntDivForAPMode(): Stop peek\n");
		break;
	default:
		swat_tab->is_sw_ant_div_by_ctrl_frame = false;
		break;
	}
}

void odm_antsel_statistics_ctrl(void *dm_void, u8 antsel_tr_mux,
				u32 rx_pwdb_all)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_fat_struct *fat_tab = &dm->dm_fat_table;

	if (antsel_tr_mux == ANT1_2G) {
		fat_tab->main_ctrl_sum += rx_pwdb_all;
		fat_tab->main_ctrl_cnt++;
	} else {
		fat_tab->aux_ctrl_sum += rx_pwdb_all;
		fat_tab->aux_ctrl_cnt++;
	}
}

void odm_s0s1_sw_ant_div_by_ctrl_frame_process_rssi(void *dm_void,
						    void *phy_info_void,
						    void *pkt_info_void
	/*	struct phydm_phyinfo_struct*		phy_info, */
	/*	struct phydm_perpkt_info_struct*		pktinfo */
	)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_phyinfo_struct *phy_info = NULL;
	struct phydm_perpkt_info_struct *pktinfo = NULL;
	struct sw_antenna_switch *swat_tab = &dm->dm_swat_table;
	struct phydm_fat_struct *fat_tab = &dm->dm_fat_table;
	u8 rssi_cck;

	phy_info = (struct phydm_phyinfo_struct *)phy_info_void;
	pktinfo = (struct phydm_perpkt_info_struct *)pkt_info_void;

	if (!(dm->support_ability & ODM_BB_ANT_DIV))
		return;

	if (dm->ant_div_type != S0S1_SW_ANTDIV)
		return;

	/* @In try state */
	if (!swat_tab->is_sw_ant_div_by_ctrl_frame)
		return;

	/* No HW error and match receiver address */
	if (!pktinfo->is_to_self)
		return;

	swat_tab->pkt_cnt_sw_ant_div_by_ctrl_frame++;

	if (pktinfo->is_cck_rate) {
		rssi_cck = phy_info->rx_mimo_signal_strength[RF_PATH_A];
		fat_tab->antsel_rx_keep_0 = (fat_tab->rx_idle_ant == MAIN_ANT) ?
					    ANT1_2G : ANT2_2G;

		if (fat_tab->antsel_rx_keep_0 == ANT1_2G)
			fat_tab->cck_ctrl_frame_cnt_main++;
		else
			fat_tab->cck_ctrl_frame_cnt_aux++;

		odm_antsel_statistics_ctrl(dm, fat_tab->antsel_rx_keep_0,
					   rssi_cck);
	} else {
		fat_tab->antsel_rx_keep_0 = (fat_tab->rx_idle_ant == MAIN_ANT) ?
					    ANT1_2G : ANT2_2G;

		if (fat_tab->antsel_rx_keep_0 == ANT1_2G)
			fat_tab->ofdm_ctrl_frame_cnt_main++;
		else
			fat_tab->ofdm_ctrl_frame_cnt_aux++;

		odm_antsel_statistics_ctrl(dm, fat_tab->antsel_rx_keep_0,
					   phy_info->rx_pwdb_all);
	}
}

#endif /* @#if (RTL8723B_SUPPORT == 1) || (RTL8821A_SUPPORT == 1) */

void odm_set_next_mac_addr_target(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_fat_struct *fat_tab = &dm->dm_fat_table;
	struct cmn_sta_info *entry;
	u32 value32, i;

	PHYDM_DBG(dm, DBG_ANT_DIV, "%s ==>\n", __func__);

	if (dm->is_linked) {
		for (i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++) {
			if ((fat_tab->train_idx + 1) == ODM_ASSOCIATE_ENTRY_NUM)
				fat_tab->train_idx = 0;
			else
				fat_tab->train_idx++;

			entry = dm->phydm_sta_info[fat_tab->train_idx];

			if (is_sta_active(entry)) {
				/*@Match MAC ADDR*/
				value32 = (entry->mac_addr[5] << 8) | entry->mac_addr[4];

				odm_set_mac_reg(dm, R_0x7b4, 0xFFFF, value32); /*@0x7b4~0x7b5*/

				value32 = (entry->mac_addr[3] << 24) | (entry->mac_addr[2] << 16) | (entry->mac_addr[1] << 8) | entry->mac_addr[0];

				odm_set_mac_reg(dm, R_0x7b0, MASKDWORD, value32); /*@0x7b0~0x7b3*/

				PHYDM_DBG(dm, DBG_ANT_DIV,
					  "fat_tab->train_idx=%d\n",
					  fat_tab->train_idx);

				PHYDM_DBG(dm, DBG_ANT_DIV,
					  "Training MAC addr = %x:%x:%x:%x:%x:%x\n",
					  entry->mac_addr[5],
					  entry->mac_addr[4],
					  entry->mac_addr[3],
					  entry->mac_addr[2],
					  entry->mac_addr[1],
					  entry->mac_addr[0]);

				break;
			}
		}
	}
}

void odm_ant_div_init(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_fat_struct *fat_tab = &dm->dm_fat_table;
	struct sw_antenna_switch *swat_tab = &dm->dm_swat_table;
	u8 i;

	if (!(dm->support_ability & ODM_BB_ANT_DIV)) {
		PHYDM_DBG(dm, DBG_ANT_DIV,
			  "[Return!!!]   Not Support Antenna Diversity Function\n");
		return;
	}

	/* @2 [--General---] */
	dm->antdiv_period = 0;

	fat_tab->is_become_linked = false;
	fat_tab->ant_div_on_off = 0xff;

	for(i=0;i<3;i++)
		fat_tab->ant_idx_vec[i]=i+1; /* initialize ant_idx_vec for SP3T */

	/* @2 [---Set MAIN_ANT as default antenna if Auto-ant enable---] */
	if (fat_tab->div_path_type == ANT_PATH_A)
		odm_ant_div_on_off(dm, ANTDIV_OFF, ANT_PATH_A);
	else if (fat_tab->div_path_type == ANT_PATH_B)
		odm_ant_div_on_off(dm, ANTDIV_OFF, ANT_PATH_B);
	else if (fat_tab->div_path_type == ANT_PATH_AB)
		odm_ant_div_on_off(dm, ANTDIV_OFF, ANT_PATH_AB);

	dm->ant_type = ODM_AUTO_ANT;

	fat_tab->rx_idle_ant = 0xff;

	/*to make RX-idle-antenna will be updated absolutly*/
	odm_update_rx_idle_ant(dm, MAIN_ANT);
	phydm_keep_rx_ack_ant_by_tx_ant_time(dm, 0);
	/* Timming issue: keep Rx ant after tx for ACK(5 x 3.2 mu = 16mu sec)*/

	/* @2 [---Set TX Antenna---] */
	if (!fat_tab->p_force_tx_by_desc) {
		fat_tab->force_tx_by_desc = 0;
		fat_tab->p_force_tx_by_desc = &fat_tab->force_tx_by_desc;
	}
	PHYDM_DBG(dm, DBG_ANT_DIV, "p_force_tx_by_desc = %d\n",
		  *fat_tab->p_force_tx_by_desc);

	if (*fat_tab->p_force_tx_by_desc)
		odm_tx_by_tx_desc_or_reg(dm, TX_BY_DESC);
	else
		odm_tx_by_tx_desc_or_reg(dm, TX_BY_REG);
}

void odm_ant_div(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	void *adapter = dm->adapter;
	struct phydm_fat_struct *fat_tab = &dm->dm_fat_table;
#if (defined(CONFIG_HL_SMART_ANTENNA))
	struct smt_ant_honbo *sat_tab = &dm->dm_sat_table;
#endif

	if (!(dm->support_ability & ODM_BB_ANT_DIV))
		return;

	if (*dm->band_type == ODM_BAND_5G) {
		if (fat_tab->idx_ant_div_counter_5g < dm->antdiv_period) {
			fat_tab->idx_ant_div_counter_5g++;
			return;
		} else
			fat_tab->idx_ant_div_counter_5g = 0;
	} else if (*dm->band_type == ODM_BAND_2_4G) {
		if (fat_tab->idx_ant_div_counter_2g < dm->antdiv_period) {
			fat_tab->idx_ant_div_counter_2g++;
			return;
		} else
			fat_tab->idx_ant_div_counter_2g = 0;
	}

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN || DM_ODM_SUPPORT_TYPE == ODM_CE)

	if (fat_tab->enable_ctrl_frame_antdiv) {
		if (dm->data_frame_num <= 10 && dm->is_linked)
			fat_tab->use_ctrl_frame_antdiv = 1;
		else
			fat_tab->use_ctrl_frame_antdiv = 0;

		PHYDM_DBG(dm, DBG_ANT_DIV,
			  "use_ctrl_frame_antdiv = (( %d )), data_frame_num = (( %d ))\n",
			  fat_tab->use_ctrl_frame_antdiv, dm->data_frame_num);
		dm->data_frame_num = 0;
	}

	{
#ifdef PHYDM_BEAMFORMING_SUPPORT

		enum beamforming_cap beamform_cap = phydm_get_beamform_cap(dm);
		PHYDM_DBG(dm, DBG_ANT_DIV, "is_bt_continuous_turn = ((%d))\n",
			  dm->is_bt_continuous_turn);
		PHYDM_DBG(dm, DBG_ANT_DIV,
			  "[ AntDiv Beam Cap ]   cap= ((%d))\n", beamform_cap);
		if (!dm->is_bt_continuous_turn) {
			if ((beamform_cap & BEAMFORMEE_CAP) &&
			    (!(*fat_tab->is_no_csi_feedback))) {
			    /* @BFmee On  &&   Div On->Div Off */
				PHYDM_DBG(dm, DBG_ANT_DIV,
					  "[ AntDiv : OFF ]   BFmee ==1; cap= ((%d))\n",
					  beamform_cap);
				PHYDM_DBG(dm, DBG_ANT_DIV,
					  "[ AntDiv BF]   is_no_csi_feedback= ((%d))\n",
					  *(fat_tab->is_no_csi_feedback));
				if (fat_tab->fix_ant_bfee == 0) {
					odm_ant_div_on_off(dm, ANTDIV_OFF,
							   ANT_PATH_A);
					fat_tab->fix_ant_bfee = 1;
				}
				return;
			} else { /* @BFmee Off   &&   Div Off->Div On */
				if (fat_tab->fix_ant_bfee == 1 &&
				    dm->is_linked) {
					PHYDM_DBG(dm, DBG_ANT_DIV,
						  "[ AntDiv : ON ]   BFmee ==0; cap=((%d))\n",
						  beamform_cap);
					PHYDM_DBG(dm, DBG_ANT_DIV,
						  "[ AntDiv BF]   is_no_csi_feedback= ((%d))\n",
						  *fat_tab->is_no_csi_feedback);
					if (dm->ant_div_type != S0S1_SW_ANTDIV)
						odm_ant_div_on_off(dm, ANTDIV_ON
								   , ANT_PATH_A)
								   ;
					fat_tab->fix_ant_bfee = 0;
				}
			}
		} else {
			if (fat_tab->div_path_type == ANT_PATH_A)
				odm_ant_div_on_off(dm, ANTDIV_ON, ANT_PATH_A);
			else if (fat_tab->div_path_type == ANT_PATH_B)
				odm_ant_div_on_off(dm, ANTDIV_ON, ANT_PATH_B);
			else if (fat_tab->div_path_type == ANT_PATH_AB)
				odm_ant_div_on_off(dm, ANTDIV_ON, ANT_PATH_AB);
		}
#endif
	}
#endif

	/* @---------- */

	if (dm->antdiv_select == 1)
		dm->ant_type = ODM_FIX_MAIN_ANT;
	else if (dm->antdiv_select == 2)
		dm->ant_type = ODM_FIX_AUX_ANT;
	else { /* @if (dm->antdiv_select==0) */
		dm->ant_type = ODM_AUTO_ANT;
	}

	/*PHYDM_DBG(dm, DBG_ANT_DIV,"ant_type= (%d), pre_ant_type= (%d)\n",*/
	/*dm->ant_type,dm->pre_ant_type); */

	if (dm->ant_type != ODM_AUTO_ANT) {
		PHYDM_DBG(dm, DBG_ANT_DIV, "Fix Antenna at (( %s ))\n",
			  (dm->ant_type == ODM_FIX_MAIN_ANT) ? "MAIN" : "AUX");

		if (dm->ant_type != dm->pre_ant_type) {
			odm_ant_div_on_off(dm, ANTDIV_OFF, ANT_PATH_A);
			odm_tx_by_tx_desc_or_reg(dm, TX_BY_REG);

			if (dm->ant_type == ODM_FIX_MAIN_ANT)
				odm_update_rx_idle_ant(dm, MAIN_ANT);
			else if (dm->ant_type == ODM_FIX_AUX_ANT)
				odm_update_rx_idle_ant(dm, AUX_ANT);
		}
		dm->pre_ant_type = dm->ant_type;
		return;
	} else {
		if (dm->ant_type != dm->pre_ant_type) {
			odm_ant_div_on_off(dm, ANTDIV_ON, ANT_PATH_A);
			odm_tx_by_tx_desc_or_reg(dm, TX_BY_DESC);
		}
		dm->pre_ant_type = dm->ant_type;
	}
#if (defined(CONFIG_2T4R_ANTENNA))
	if (dm->ant_type2 != ODM_AUTO_ANT) {
		PHYDM_DBG(dm, DBG_ANT_DIV, "PathB Fix Ant at (( %s ))\n",
			  (dm->ant_type2 == ODM_FIX_MAIN_ANT) ? "MAIN" : "AUX");

		if (dm->ant_type2 != dm->pre_ant_type2) {
			odm_ant_div_on_off(dm, ANTDIV_OFF, ANT_PATH_B);
			odm_tx_by_tx_desc_or_reg(dm, TX_BY_REG);

			if (dm->ant_type2 == ODM_FIX_MAIN_ANT)
				phydm_update_rx_idle_ant_pathb(dm, MAIN_ANT);
			else if (dm->ant_type2 == ODM_FIX_AUX_ANT)
				phydm_update_rx_idle_ant_pathb(dm, AUX_ANT);
		}
		dm->pre_ant_type2 = dm->ant_type2;
		return;
	}
	if (dm->ant_type2 != dm->pre_ant_type2) {
		odm_ant_div_on_off(dm, ANTDIV_ON, ANT_PATH_B);
		odm_tx_by_tx_desc_or_reg(dm, TX_BY_DESC);
	}
	dm->pre_ant_type2 = dm->ant_type2;

#endif

}

void odm_antsel_statistics(void *dm_void, void *phy_info_void,
			   u8 antsel_tr_mux, u32 mac_id, u32 utility, u8 method,
			   u8 is_cck_rate)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_fat_struct *fat_tab = &dm->dm_fat_table;
	struct phydm_phyinfo_struct *phy_info = NULL;

	phy_info = (struct phydm_phyinfo_struct *)phy_info_void;

	if (method == RSSI_METHOD) {
		if (is_cck_rate) {
			if (antsel_tr_mux == fat_tab->ant_idx_vec[0]-1) {
	/*to prevent u16 overflow, max(RSSI)=100, 65435+100 = 65535 (u16)*/
				if (fat_tab->main_sum_cck[mac_id] > 65435)
					return;

				fat_tab->main_sum_cck[mac_id] += (u16)utility;
				fat_tab->main_cnt_cck[mac_id]++;
			} else {
				if (fat_tab->aux_sum_cck[mac_id] > 65435)
					return;

				fat_tab->aux_sum_cck[mac_id] += (u16)utility;
				fat_tab->aux_cnt_cck[mac_id]++;
			}

		} else { /*ofdm rate*/

			if (antsel_tr_mux == fat_tab->ant_idx_vec[0]-1) {
				if (fat_tab->main_sum[mac_id] > 65435)
					return;

				fat_tab->main_sum[mac_id] += (u16)utility;
				fat_tab->main_cnt[mac_id]++;
			} else {
				if (fat_tab->aux_sum[mac_id] > 65435)
					return;

				fat_tab->aux_sum[mac_id] += (u16)utility;
				fat_tab->aux_cnt[mac_id]++;
			}
		}
	}
}

void odm_process_rssi_smart(void *dm_void, void *phy_info_void,
			    void *pkt_info_void, u8 rx_power_ant0)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_phyinfo_struct *phy_info = NULL;
	struct phydm_perpkt_info_struct *pktinfo = NULL;
	struct phydm_fat_struct *fat_tab = &dm->dm_fat_table;

	phy_info = (struct phydm_phyinfo_struct *)phy_info_void;
	pktinfo = (struct phydm_perpkt_info_struct *)pkt_info_void;

}

void odm_process_rssi_normal(void *dm_void, void *phy_info_void,
			     void *pkt_info_void, u8 rx_pwr0)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_phyinfo_struct *phy_info = NULL;
	struct phydm_perpkt_info_struct *pktinfo = NULL;
	struct phydm_fat_struct *fat_tab = &dm->dm_fat_table;
	u8 rx_evm0, rx_evm1;
	boolean b_main;

	phy_info = (struct phydm_phyinfo_struct *)phy_info_void;
	pktinfo = (struct phydm_perpkt_info_struct *)pkt_info_void;
	rx_evm0 = phy_info->rx_mimo_signal_quality[0];
	rx_evm1 = phy_info->rx_mimo_signal_quality[1];

	if (!(pktinfo->is_packet_to_self || fat_tab->use_ctrl_frame_antdiv))
		return;

	if (dm->ant_div_type == S0S1_SW_ANTDIV) {
		if (pktinfo->is_cck_rate) {

			b_main = (fat_tab->rx_idle_ant == MAIN_ANT);
			fat_tab->antsel_rx_keep_0 = b_main ? ANT1_2G : ANT2_2G;
		}

		odm_antsel_statistics(dm, phy_info, fat_tab->antsel_rx_keep_0,
				      pktinfo->station_id, rx_pwr0, RSSI_METHOD,
				      pktinfo->is_cck_rate);
	} else {
		odm_antsel_statistics(dm, phy_info, fat_tab->antsel_rx_keep_0,
				      pktinfo->station_id, rx_pwr0, RSSI_METHOD,
				      pktinfo->is_cck_rate);
	}
}

void odm_process_rssi_for_ant_div(void *dm_void, void *phy_info_void,
				  void *pkt_info_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_phyinfo_struct *phy_info = NULL;
	struct phydm_perpkt_info_struct *pktinfo = NULL;
	struct phydm_fat_struct *fat_tab = &dm->dm_fat_table;
#if (defined(CONFIG_HL_SMART_ANTENNA))
	struct smt_ant_honbo *sat_tab = &dm->dm_sat_table;
	u32 beam_tmp;
	u8 next_ant;
	u8 train_pkt_number;
#endif
	boolean b_main;
	u8 rx_power_ant0, rx_power_ant1;
	u8 rx_evm_ant0, rx_evm_ant1;
	u8 rssi_avg;
	u64 rssi_linear = 0;

	phy_info = (struct phydm_phyinfo_struct *)phy_info_void;
	pktinfo = (struct phydm_perpkt_info_struct *)pkt_info_void;
	rx_power_ant0 = phy_info->rx_mimo_signal_strength[0];
	rx_power_ant1 = phy_info->rx_mimo_signal_strength[1];
	rx_evm_ant0 = phy_info->rx_mimo_signal_quality[0];
	rx_evm_ant1 = phy_info->rx_mimo_signal_quality[1];

	if ((!pktinfo->is_cck_rate) {
		if (rx_power_ant1 < 100) {
			rssi_linear = phydm_db_2_linear(rx_power_ant0) +
				      phydm_db_2_linear(rx_power_ant1);
			/* @Rounding and removing fractional bits */
			rssi_linear = (rssi_linear +
				       (1 << (FRAC_BITS - 1))) >> FRAC_BITS;
			/* @Calculate average RSSI */
			rssi_linear = DIVIDED_2(rssi_linear);
			/* @averaged PWDB */
			rssi_avg = (u8)odm_convert_to_db(rssi_linear);
		}

	} else {
		rx_power_ant0 = (u8)phy_info->rx_pwdb_all;
		rssi_avg = rx_power_ant0;
	}

#ifdef CONFIG_HL_SMART_ANTENNA_TYPE2
	if ((dm->ant_div_type == HL_SW_SMART_ANT_TYPE2) && (fat_tab->fat_state == FAT_TRAINING_STATE))
		phydm_process_rssi_for_hb_smtant_type2(dm, phy_info, pktinfo, rssi_avg); /*@for 8822B*/
	else
#endif

#ifdef CONFIG_HL_SMART_ANTENNA_TYPE1
#ifdef CONFIG_FAT_PATCH
		if (dm->ant_div_type == HL_SW_SMART_ANT_TYPE1 && fat_tab->fat_state == FAT_TRAINING_STATE) {
		/*@[Beacon]*/
		if (pktinfo->is_packet_beacon) {
			sat_tab->beacon_counter++;
			PHYDM_DBG(dm, DBG_ANT_DIV,
				  "MatchBSSID_beacon_counter = ((%d))\n",
				  sat_tab->beacon_counter);

			if (sat_tab->beacon_counter >= sat_tab->pre_beacon_counter + 2) {
				if (sat_tab->ant_num > 1) {
					next_ant = (fat_tab->rx_idle_ant == MAIN_ANT) ? AUX_ANT : MAIN_ANT;
					odm_update_rx_idle_ant(dm, next_ant);
				}

				sat_tab->update_beam_idx++;

				PHYDM_DBG(dm, DBG_ANT_DIV,
					  "pre_beacon_counter = ((%d)), pkt_counter = ((%d)), update_beam_idx = ((%d))\n",
					  sat_tab->pre_beacon_counter,
					  sat_tab->pkt_counter,
					  sat_tab->update_beam_idx);

				sat_tab->pre_beacon_counter = sat_tab->beacon_counter;
				sat_tab->pkt_counter = 0;
			}
		}
		/*@[data]*/
		else if (pktinfo->is_packet_to_self) {
			if (sat_tab->pkt_skip_statistic_en == 0) {
				/*@
				PHYDM_DBG(dm, DBG_ANT_DIV, "StaID[%d]:  antsel_pathA = ((%d)), hw_antsw_occur = ((%d)), Beam_num = ((%d)), RSSI = ((%d))\n",
					pktinfo->station_id, fat_tab->antsel_rx_keep_0, fat_tab->hw_antsw_occur, sat_tab->fast_training_beam_num, rx_power_ant0);
				*/
				PHYDM_DBG(dm, DBG_ANT_DIV,
					  "ID[%d][pkt_cnt = %d]: {ANT, Beam} = {%d, %d}, RSSI = ((%d))\n",
					  pktinfo->station_id,
					  sat_tab->pkt_counter,
					  fat_tab->antsel_rx_keep_0,
					  sat_tab->fast_training_beam_num,
					  rx_power_ant0);

				sat_tab->pkt_rssi_sum[fat_tab->antsel_rx_keep_0][sat_tab->fast_training_beam_num] += rx_power_ant0;
				sat_tab->pkt_rssi_cnt[fat_tab->antsel_rx_keep_0][sat_tab->fast_training_beam_num]++;
				sat_tab->pkt_counter++;

				train_pkt_number = sat_tab->beam_train_cnt[fat_tab->rx_idle_ant - 1][sat_tab->fast_training_beam_num];

				/*Swich Antenna erery N pkts*/
				if (sat_tab->pkt_counter == train_pkt_number) {
					if (sat_tab->ant_num > 1) {
						PHYDM_DBG(dm, DBG_ANT_DIV, "packet enugh ((%d ))pkts ---> Switch antenna\n", train_pkt_number);
						next_ant = (fat_tab->rx_idle_ant == MAIN_ANT) ? AUX_ANT : MAIN_ANT;
						odm_update_rx_idle_ant(dm, next_ant);
					}

					sat_tab->update_beam_idx++;
					PHYDM_DBG(dm, DBG_ANT_DIV, "pre_beacon_counter = ((%d)), update_beam_idx_counter = ((%d))\n",
						  sat_tab->pre_beacon_counter, sat_tab->update_beam_idx);

					sat_tab->pre_beacon_counter = sat_tab->beacon_counter;
					sat_tab->pkt_counter = 0;
				}
			}
		}

		/*Swich Beam after switch "sat_tab->ant_num" antennas*/
		if (sat_tab->update_beam_idx == sat_tab->ant_num) {
			sat_tab->update_beam_idx = 0;
			sat_tab->pkt_counter = 0;
			beam_tmp = sat_tab->fast_training_beam_num;

			if (sat_tab->fast_training_beam_num >= (sat_tab->beam_patten_num_each_ant - 1)) {
				fat_tab->fat_state = FAT_DECISION_STATE;

#if DEV_BUS_TYPE == RT_USB_INTERFACE || DEV_BUS_TYPE == RT_SDIO_INTERFACE
				if (dm->support_interface == ODM_ITRF_USB || dm->support_interface == ODM_ITRF_SDIO)
					odm_schedule_work_item(&sat_tab->hl_smart_antenna_decision_workitem);
#endif

			} else {
				sat_tab->fast_training_beam_num++;
				PHYDM_DBG(dm, DBG_ANT_DIV,
					  "Update Beam_num (( %d )) -> (( %d ))\n",
					  beam_tmp,
					  sat_tab->fast_training_beam_num);
				phydm_set_all_ant_same_beam_num(dm);

				fat_tab->fat_state = FAT_TRAINING_STATE;
			}
		}
	}
#else

	}
#endif
	else
#endif
		if (dm->ant_div_type == CG_TRX_SMART_ANTDIV) {
			odm_process_rssi_smart(dm, phy_info, pktinfo,
					       rx_power_ant0);
		} else { /* @ant_div_type != CG_TRX_SMART_ANTDIV */
			odm_process_rssi_normal(dm, phy_info, pktinfo,
						rx_power_ant0);
		}
#if 0
/* PHYDM_DBG(dm,DBG_ANT_DIV,"is_cck_rate=%d, pwdb_all=%d\n",
 *	     pktinfo->is_cck_rate, phy_info->rx_pwdb_all);
 * PHYDM_DBG(dm,DBG_ANT_DIV,"antsel_tr_mux=3'b%d%d%d\n",
 *	     fat_tab->antsel_rx_keep_2, fat_tab->antsel_rx_keep_1,
 *	     fat_tab->antsel_rx_keep_0);
 */
#endif
}

void odm_set_tx_ant_by_tx_info(void *dm_void, u8 *desc, u8 mac_id)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_fat_struct *fat_tab = &dm->dm_fat_table;

	if (!(dm->support_ability & ODM_BB_ANT_DIV))
		return;

	if (dm->ant_div_type == CGCS_RX_HW_ANTDIV)
		return;
}

void odm_ant_div_config(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_fat_struct *fat_tab = &dm->dm_fat_table;
	PHYDM_DBG(dm, DBG_ANT_DIV, "CE Config Antenna Diversity\n");

	PHYDM_DBG(dm, DBG_ANT_DIV,
		  "[AntDiv Config Info] AntDiv_SupportAbility = (( %x ))\n",
		  ((dm->support_ability & ODM_BB_ANT_DIV) ? 1 : 0));
	PHYDM_DBG(dm, DBG_ANT_DIV,
		  "[AntDiv Config Info] be_fix_tx_ant = ((%d))\n",
		  dm->dm_fat_table.b_fix_tx_ant);
}

void odm_ant_div_timers(void *dm_void, u8 state)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	if (state == INIT_ANTDIV_TIMMER) {
#ifdef CONFIG_S0S1_SW_ANTENNA_DIVERSITY
		odm_initialize_timer(dm,
				     &dm->dm_swat_table.sw_antdiv_timer,
				     (void *)odm_sw_antdiv_callback, NULL,
				     "sw_antdiv_timer");
#elif (defined(CONFIG_5G_CG_SMART_ANT_DIVERSITY)) ||\
	(defined(CONFIG_2G_CG_SMART_ANT_DIVERSITY))
		odm_initialize_timer(dm, &dm->fast_ant_training_timer,
				     (void *)odm_fast_ant_training_callback,
				     NULL, "fast_ant_training_timer");
#endif

	} else if (state == CANCEL_ANTDIV_TIMMER) {
#ifdef CONFIG_S0S1_SW_ANTENNA_DIVERSITY
		odm_cancel_timer(dm,
				 &dm->dm_swat_table.sw_antdiv_timer);
#elif (defined(CONFIG_5G_CG_SMART_ANT_DIVERSITY)) ||\
	(defined(CONFIG_2G_CG_SMART_ANT_DIVERSITY))
		odm_cancel_timer(dm, &dm->fast_ant_training_timer);
#endif
	} else if (state == RELEASE_ANTDIV_TIMMER) {
#ifdef CONFIG_S0S1_SW_ANTENNA_DIVERSITY
		odm_release_timer(dm,
				  &dm->dm_swat_table.sw_antdiv_timer);
#elif (defined(CONFIG_5G_CG_SMART_ANT_DIVERSITY)) ||\
	(defined(CONFIG_2G_CG_SMART_ANT_DIVERSITY))
		odm_release_timer(dm, &dm->fast_ant_training_timer);
#endif
	}
}

void phydm_antdiv_debug(void *dm_void, char input[][16], u32 *_used,
			char *output, u32 *_out_len)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_fat_struct	*fat_tab = &dm->dm_fat_table;
	u32 used = *_used;
	u32 out_len = *_out_len;
	u32 dm_value[10] = {0};
	char help[] = "-h";
	u8 i, input_idx = 0;

	for (i = 0; i < 5; i++) {
		if (input[i + 1]) {
			PHYDM_SSCANF(input[i + 1], DCMD_HEX, &dm_value[i]);
			input_idx++;
		}
	}

	if (input_idx == 0)
		return;

	if ((strcmp(input[1], help) == 0)) {
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "{1} {0:auto, 1:fix main, 2:fix auto}\n");
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "{2} {antdiv_period}\n");
		#if (RTL8821C_SUPPORT == 1)
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "{3} {en} {0:Default, 1:HW_Div, 2:SW_Div}\n");
		#endif

	} else if (dm_value[0] == 1) {
	/*@fixed or auto antenna*/
		if (dm_value[1] == 0) {
			dm->ant_type = ODM_AUTO_ANT;
			PDM_SNPF(out_len, used, output + used, out_len - used,
				 "AntDiv: Auto\n");
		} else if (dm_value[1] == 1) {
			dm->ant_type = ODM_FIX_MAIN_ANT;

		#if (RTL8710C_SUPPORT == 1)
			dm->antdiv_select = 1;
		#endif

			PDM_SNPF(out_len, used, output + used, out_len - used,
				 "AntDiv: Fix Main\n");
		} else if (dm_value[1] == 2) {
			dm->ant_type = ODM_FIX_AUX_ANT;

		#if (RTL8710C_SUPPORT == 1)
			dm->antdiv_select = 2;
		#endif

			PDM_SNPF(out_len, used, output + used, out_len - used,
				 "AntDiv: Fix Aux\n");
		}

		if (dm->ant_type != ODM_AUTO_ANT) {
			odm_stop_antenna_switch_dm(dm);
			if (dm->ant_type == ODM_FIX_MAIN_ANT)
				odm_update_rx_idle_ant(dm, MAIN_ANT);
			else if (dm->ant_type == ODM_FIX_AUX_ANT)
				odm_update_rx_idle_ant(dm, AUX_ANT);
		} else {
			phydm_enable_antenna_diversity(dm);
		}
		dm->pre_ant_type = dm->ant_type;
	} else if (dm_value[0] == 2) {
	/*@dynamic period for AntDiv*/
		dm->antdiv_period = (u8)dm_value[1];
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "AntDiv_period=((%d))\n", dm->antdiv_period);
	}
	*_used = used;
	*_out_len = out_len;
}

void odm_ant_div_reset(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	if (!(dm->support_ability & ODM_BB_ANT_DIV))
		return;

	#ifdef CONFIG_S0S1_SW_ANTENNA_DIVERSITY
	if (dm->ant_div_type == S0S1_SW_ANTDIV)
		odm_s0s1_sw_ant_div_reset(dm);
	#endif
}

void odm_antenna_diversity_init(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	odm_ant_div_config(dm);
	odm_ant_div_init(dm);
}

void odm_antenna_diversity(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	if (*dm->mp_mode)
		return;

	if (!(dm->support_ability & ODM_BB_ANT_DIV)) {
		PHYDM_DBG(dm, DBG_ANT_DIV,
			  "[Return!!!]   Not Support Antenna Diversity Function\n");
		return;
	}

	if (dm->pause_ability & ODM_BB_ANT_DIV) {
		return;
	}

	odm_ant_div(dm);
}
#endif /*@#ifdef CONFIG_PHYDM_ANTENNA_DIVERSITY*/
