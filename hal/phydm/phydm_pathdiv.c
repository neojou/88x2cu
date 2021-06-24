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

#ifdef CONFIG_PATH_DIVERSITY

#ifdef PHYDM_IC_JGR3_SERIES_SUPPORT
void phydm_set_resp_tx_path_by_fw_jgr3(void *dm_void, u8 macid,
				       enum bb_path path, boolean enable)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _ODM_PATH_DIVERSITY_ *p_div = &dm->dm_path_div;
	u8 h2c_para[7] = {0};
	u8 path_map[4] = {0}; /* tx logic map*/
	u8 num_enable_path = 0;
	u8 n_tx_path_ctrl_map = 0;
	u8 i = 0, n_sts = 0;

	/*Response TX is controlled in FW ctrl info*/

	PHYDM_DBG(dm, DBG_PATH_DIV, "[%s] =====>\n", __func__);

	if (enable) {
		n_tx_path_ctrl_map = path;

		for (i = 0; i < 4; i++) {
			path_map[i] = 0;
			if (path & BIT(i))
				num_enable_path++;
		}

		for (i = 0; i < 4; i++) {
			if (path & BIT(i)) {
				path_map[i] = n_sts;
				n_sts++;

				if (n_sts == num_enable_path)
					break;
			}
		}
	}

	PHYDM_DBG(dm, DBG_PATH_DIV, "ctrl_map=0x%x Map[D:A]={%d, %d, %d, %d}\n",
		  n_tx_path_ctrl_map,
		  path_map[3], path_map[2], path_map[1], path_map[0]);

	h2c_para[0] = macid;
	h2c_para[1] = n_tx_path_ctrl_map;
	h2c_para[2] = (path_map[3] << 6) | (path_map[2] << 4) |
		      (path_map[1] << 2) | path_map[0];

	odm_fill_h2c_cmd(dm, PHYDM_H2C_DYNAMIC_TX_PATH, 7, h2c_para);
}

void phydm_get_tx_path_txdesc_jgr3(void *dm_void, u8 macid,
				   struct path_txdesc_ctrl *desc)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _ODM_PATH_DIVERSITY_ *p_div = &dm->dm_path_div;
	u8 ant_map_a = 0, ant_map_b = 0;
	u8 ntx_map = 0;

	if (p_div->path_sel[macid] == BB_PATH_A) {
		desc->ant_map_a = 0; /*offest24[23:22]*/
		desc->ant_map_b = 0; /*offest24[25:24]*/
		desc->ntx_map = BB_PATH_A; /*offest28[23:20]*/
	} else if (p_div->path_sel[macid] == BB_PATH_B) {
		desc->ant_map_a = 0; /*offest24[23:22]*/
		desc->ant_map_b = 0; /*offest24[25:24]*/
		desc->ntx_map = BB_PATH_B; /*offest28[23:20]*/
	} else {
		desc->ant_map_a = 0; /*offest24[23:22]*/
		desc->ant_map_b = 1; /*offest24[25:24]*/
		desc->ntx_map = BB_PATH_AB; /*offest28[23:20]*/
	}
}
#endif

void phydm_tx_path_by_mac_or_reg(void *dm_void, enum phydm_path_ctrl ctrl)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _ODM_PATH_DIVERSITY_ *p_div = &dm->dm_path_div;

	PHYDM_DBG(dm, DBG_PATH_DIV, "[%s] ctrl=%s\n",
		  __func__, (ctrl == TX_PATH_BY_REG) ? "REG" : "DESC");

	if (ctrl == p_div->tx_path_ctrl)
		return;

	p_div->tx_path_ctrl = ctrl;

	if (ctrl == TX_PATH_BY_REG) {
		odm_set_bb_reg(dm, R_0x1e24, BIT(16), 0); /*OFDM*/
		odm_set_bb_reg(dm, R_0x1a84, 0xe0, 0); /*CCK*/
	} else {
		odm_set_bb_reg(dm, R_0x1e24, BIT(16), 1); /*OFDM*/
		odm_set_bb_reg(dm, R_0x1a84, 0xe0, 7); /*CCK*/
	}
}

void phydm_fix_1ss_tx_path_by_bb_reg(void *dm_void,
				     enum bb_path tx_path_sel_1ss,
				     enum bb_path tx_path_sel_cck)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _ODM_PATH_DIVERSITY_ *p_div = &dm->dm_path_div;

	if (tx_path_sel_1ss != BB_PATH_AUTO) {
		p_div->ofdm_fix_path_en = true;
		p_div->ofdm_fix_path_sel = tx_path_sel_1ss;
	} else {
		p_div->ofdm_fix_path_en = false;
		p_div->ofdm_fix_path_sel = dm->tx_1ss_status;
	}

	if (tx_path_sel_cck != BB_PATH_AUTO) {
		p_div->cck_fix_path_en = true;
		p_div->cck_fix_path_sel = tx_path_sel_cck;
	} else {
		p_div->cck_fix_path_en = false;
		p_div->cck_fix_path_sel = dm->tx_1ss_status;
	}

	p_div->force_update = true;

	PHYDM_DBG(dm, DBG_PATH_DIV,
		  "{OFDM_fix_en=%d, path=%d} {CCK_fix_en=%d, path=%d}\n",
		  p_div->ofdm_fix_path_en, p_div->ofdm_fix_path_sel,
		  p_div->cck_fix_path_en, p_div->cck_fix_path_sel);
}

void phydm_tx_path_diversity_init_v2(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _ODM_PATH_DIVERSITY_ *p_div = &dm->dm_path_div;
	u32 i = 0;

	PHYDM_DBG(dm, DBG_PATH_DIV, "[%s] ====>\n", __func__);

	/*BB_PATH_AB is a invalid value used for init state*/
	p_div->default_tx_path = BB_PATH_A;
	p_div->tx_path_ctrl = TX_PATH_CTRL_INIT;
	p_div->path_div_in_progress = false;

	p_div->cck_fix_path_en = false;
	p_div->ofdm_fix_path_en = false;
	p_div->force_update = false;

	for (i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++)
		p_div->path_sel[i] = BB_PATH_A; /* TxInfo default at path-A */

	phydm_tx_path_by_mac_or_reg(dm, TX_PATH_BY_REG);
}

void phydm_tx_path_diversity_init(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	if (!(dm->support_ability & ODM_BB_PATH_DIV))
		return;

	phydm_tx_path_diversity_init_v2(dm); /*@ After 8822B*/
}

void phydm_process_rssi_for_path_div(void *dm_void, void *phy_info_void,
				     void *pkt_info_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_phyinfo_struct *phy_info = NULL;
	struct phydm_perpkt_info_struct *pktinfo = NULL;
	struct _ODM_PATH_DIVERSITY_ *p_div = &dm->dm_path_div;
	u8 id = 0;

	phy_info = (struct phydm_phyinfo_struct *)phy_info_void;
	pktinfo = (struct phydm_perpkt_info_struct *)pkt_info_void;

	if (!(pktinfo->is_packet_to_self || pktinfo->is_packet_match_bssid))
		return;

	if (pktinfo->is_cck_rate)
		return;

	id = pktinfo->station_id;
	p_div->path_a_sum[id] += phy_info->rx_mimo_signal_strength[0];
	p_div->path_a_cnt[id]++;

	p_div->path_b_sum[id] += phy_info->rx_mimo_signal_strength[1];
	p_div->path_b_cnt[id]++;
}

void phydm_c2h_dtp_handler(void *dm_void, u8 *cmd_buf, u8 cmd_len)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _ODM_PATH_DIVERSITY_ *p_div = &dm->dm_path_div;

	u8 macid = cmd_buf[0];
	u8 target = cmd_buf[1];
	u8 nsc_1 = cmd_buf[2];
	u8 nsc_2 = cmd_buf[3];
	u8 nsc_3 = cmd_buf[4];

	PHYDM_DBG(dm, DBG_PATH_DIV, "Target_candidate = (( %d ))\n", target);
/*@
	if( (nsc_1 >= nsc_2) &&  (nsc_1 >= nsc_3))
	{
		phydm_dtp_fix_tx_path(dm, p_div->ant_candidate_1);
	}
	else	if( nsc_2 >= nsc_3)
	{
		phydm_dtp_fix_tx_path(dm, p_div->ant_candidate_2);
	}
	else
	{
		phydm_dtp_fix_tx_path(dm, p_div->ant_candidate_3);
	}
	*/
}

#endif /*  @#ifdef CONFIG_PATH_DIVERSITY */
