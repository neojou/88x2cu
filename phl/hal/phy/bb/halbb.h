/******************************************************************************
 *
 * Copyright(c) 2007 - 2020  Realtek Corporation.
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

#ifndef __HALBB_H__
#define __HALBB_H__

/*@--------------------------[Define] ---------------------------------------*/

#ifndef PHL_MAX_STA_NUM
	#define PHL_MAX_STA_NUM	16 /* will move out of halbb*/
#endif

#define	ACTIVE_TP_THRESHOLD	1
#define BB_WATCH_DOG_PERIOD	2 /*sec*/
#define is_sta_active(sta)	((sta) && (sta->active))
#define HALBB_SNPRINT_SIZE	200
#define HALBB_SNPRINT_SIZE_S	20

#define BB_GAIN_BAND_NUM	4
#define BB_EFUSE_BAND_NUM	5
#define IC_LNA_NUM		7
#define IC_TIA_NUM		2

/*@--------------------------[Enum]------------------------------------------*/

enum bb_trx_state_t {
	BB_TX_STATE	= 0,
	BB_RX_STATE	= 1,
	BB_BI_DIR_STATE	= 2
};

enum bb_trafic_t {
	TRAFFIC_NO_TP		= 0,
	TRAFFIC_ULTRA_LOW	= 1,
	TRAFFIC_LOW		= 2,
	TRAFFIC_MID		= 3,
	TRAFFIC_HIGH		= 4
};

enum efuse_bit_mask {
	LOW_MASK = 0,
	HIGH_MASK = 1
};
/*@--------------------------[Structure]-------------------------------------*/

struct halbb_pause_lv {
	s8			lv_dig;
	s8			lv_cfo;
};

struct bb_func_hooker_info {
	void (*pause_bb_dm_handler)(struct bb_info *bb, u32 *val_buf, u8 val_len);
};

struct	bb_iot_info {
	u8		is_linked_cmw500:1;
	u8		patch_id_00000000:1;
	u8		rsvd:6;
};

struct bb_path_info {
	/*[Path info]*/
	u8			tx_path_en; /*TX path enable*/
	u8			rx_path_en; /*RX path enable*/
	#ifdef HALBB_COMPILE_ABOVE_4SS
	enum bb_path		tx_4ss_path_map; /*@Use N-X for 4STS rate*/
	#endif
	#ifdef HALBB_COMPILE_ABOVE_3SS
	enum bb_path		tx_3ss_path_map; /*@Use N-X for 3STS rate*/
	#endif
	#ifdef HALBB_COMPILE_ABOVE_2SS
	enum bb_path		tx_2ss_path_map; /*@Use N-X for 2STS rate*/
	#endif
	enum bb_path		tx_1ss_path_map; /*@Use N-X for 1STS rate*/
};

struct bb_link_info {
	/*[Link Info]*/
	bool			is_linked;
	bool			is_linked_pre;
	bool			first_connect;
	bool			first_disconnect;
	enum bb_trx_state_t	txrx_state_all;
	/*[One Entry TP Info]*/
	bool			is_one_entry_only;
	u32			one_entry_macid;
	u32			one_entry_tp;
	u32			one_entry_tp_pre;
	u16			tp_active_th;
	bool			one_entry_tp_active_occur;
	bool			is_match_bssid;
	/*[Client Number]*/
	u8			num_linked_client;
	u8			num_linked_client_pre;
	u8			num_active_client;
	u8			num_active_client_pre;
	/*[TP & Traffic]*/
	u8			traffic_load;
	u8			traffic_load_pre;
	u16			tx_rate;
	u16			rx_rate_plurality;
	u16			rx_rate_plurality_mu;
	u32			tx_tp;			/*@Mbps*/
	u32			rx_tp;			/*@Mbps*/
	u32			total_tp;		/*@Mbps*/
	u16			consecutive_idle_time;	/*@unit: second*/
};

struct bb_ch_info {
	u8			rssi_min;
	u16			rssi_min_macid;
	u8			pre_rssi_min;
	u8			rssi_max;
	u16			rssi_max_macid;
	u8			rxsc_160;
	u8			rxsc_80;
	u8			rxsc_40;
	u8			rxsc_20;
	u8			rxsc_l;
	u8			is_noisy;
	u8			rf_central_ch_cfg; /*report in phy-sts*/
};

struct bb_cmn_backup_info {
	u8			cur_tx_path;
	u8			cur_rx_path;
	s16			cur_tx_pwr;
	u8			cur_pd_lower_bound;
	u8			last_rssi;
};

struct bb_gain_info {
	s8 lna_gain[BB_GAIN_BAND_NUM][HALBB_MAX_PATH][IC_LNA_NUM];
	s8 tia_gain[BB_GAIN_BAND_NUM][HALBB_MAX_PATH][IC_TIA_NUM];
};

struct bb_efuse_info{
	bool normal_efuse_check;
	bool hidden_efuse_check;
	s8 gain_offset[HALBB_MAX_PATH][BB_EFUSE_BAND_NUM]; // S(8,0)
	s8 gain_cs[HALBB_MAX_PATH][BB_GAIN_BAND_NUM]; // S(8,0)
	s8 gain_cg[HALBB_MAX_PATH][BB_GAIN_BAND_NUM]; // S(8,0)

	s8 lna_err_2g[HALBB_MAX_PATH][7]; // S(6,2)
	s8 lna_err_5g[HALBB_MAX_PATH][7]; // S(6,2)

	s8 frontend_loss[HALBB_MAX_PATH];
	s8 rpl_bias_comp[HALBB_MAX_PATH];
	s8 rssi_bias_comp[HALBB_MAX_PATH];

	s8 efuse_ofst; // 8852A:S(5,2) 8852B:S(8,4)
	s8 efuse_ofst_tb; // 8852A:S(7,4) 8852B:S(8,4)
};

struct bb_cmn_info {
	u8 bb_dm_number;
#if 0 //NEO
#ifdef HALBB_PSD_SUPPORT
	struct bb_psd_info	bb_psd_i;
#endif
#ifdef HALBB_LA_MODE_SUPPORT
	struct bb_la_mode_info	bb_la_mode_i;
#endif
#ifdef HALBB_DYN_CSI_RSP_SUPPORT
	struct bf_ch_raw_info bf_ch_raw_i;
#endif
	struct bb_echo_cmd_info	bb_echo_cmd_i;
	struct bb_func_hooker_info bb_func_hooker_i;
#endif //NEO
};

struct bb_info {
	struct rtw_phl_com_t	*phl_com;
	struct rtw_hal_com_t	*hal_com;
	struct rtw_phl_stainfo_t *phl_sta_info[PHL_MAX_STA_NUM];
	u8			phl2bb_macid_table[PHL_MAX_STA_NUM];
	bool			sta_exist[PHL_MAX_STA_NUM];
	/*[DBCC]*/
#ifdef HALBB_DBCC_SUPPORT
	struct bb_info		*bb_phy_hooker;
#endif
	enum phl_phy_idx	bb_phy_idx;
	struct bb_cmn_info	*bb_cmn_hooker;
	/*[Common Info]*/
	struct bb_gain_info	bb_gain_i;
	struct bb_efuse_info	bb_efuse_i;
	enum bb_ic_t		ic_type;
	enum bb_cr_t		cr_type;
	u8			num_rf_path;
	/*[System Info]*/
	//NEO
	//enum bb_watchdog_mode_t bb_watchdog_mode;
	bool			bb_cmn_info_init_ready;
	bool			bb_dm_init_ready;
	u32			bb_sys_up_time;
	bool			bb_watchdog_en;
	u8			bb_watchdog_period; /*2s, 4s, 8s...,254s*/
	bool			bb_ic_api_en;
	u8			pre_dbg_priority;
	char			dbg_buf[HALBB_SNPRINT_SIZE];
	/*[DM Info]*/
	bool			is_noisy;
	bool			adv_bb_dm_en;
	u64			support_ability;	/*HALBB function Supportability*/
	u64			manual_support_ability;
	u64			pause_ability;		/*HALBB function pause Supportability*/
	struct halbb_pause_lv	pause_lv_table;
	/*[FW Info]*/
	u8			fwofld_last_cmd;
	u64			fw_dbg_component;
	/*[Drv Dbg Info]*/
	u64			dbg_component;
	u8			cmn_dbg_msg_period;
	u8			cmn_dbg_msg_cnt;
	/*[Dummy]*/
	bool			bool_dummy;
	u8			u8_dummy;
	u16			u16_dummy;
	u32			u32_dummy;
	/*[Link Info]*/
	enum rf_path 		tx_path; /*PMAC Tx Path*/
	enum rf_path 		rx_path;
	/*[Timer]*/
	/* Timer */
	//NEO
	//halbb_timer_list	antdiv_timer;

	/*@=== [HALBB Structure] ============================================*/
#ifdef BB_8852A_CAV_SUPPORT
	struct bb_8852a_info	bb_8852a_i;
#endif
#ifdef BB_8852A_2_SUPPORT
	struct bb_8852a_2_info	bb_8852a_2_i;
	struct bb_h2c_fw_cmw	bb_fw_cmw_i;
#endif
#ifdef HALBB_RA_SUPPORT
	struct bb_ra_info	bb_ra_i[PHL_MAX_STA_NUM];
#endif


#ifdef HALBB_ENV_MNTR_SUPPORT
	struct bb_env_mntr_info bb_env_mntr_i;
#endif

#if 0 //NEO
#ifdef HALBB_EDCCA_SUPPORT
	struct bb_edcca_info	bb_edcca_i;
#endif
#ifdef HALBB_DFS_SUPPORT
	struct bb_dfs_info	bb_dfs_i;
#endif
#ifdef HALBB_STATISTICS_SUPPORT
	struct bb_stat_info	bb_stat_i;
#endif
#endif //NEO

#ifdef HALBB_DIG_SUPPORT
	struct bb_dig_info	bb_dig_i;
#endif

#if 0 //NEO
#ifdef HALBB_CFO_TRK_SUPPORT
	struct bb_cfo_trk_info	bb_cfo_trk_i;
#endif
#ifdef HALBB_PHYSTS_PARSING_SUPPORT
	struct bb_physts_info	bb_physts_i;
#endif
#ifdef HALBB_LA_MODE_SUPPORT
	struct bb_la_mode_info	bb_la_mode_i;
#endif

#ifdef HALBB_PWR_CTRL_SUPPORT
	struct bb_pwr_ctrl_info	bb_pwr_ctrl_i;
	struct bb_dyncca_info	bb_dyncca_i;
#endif

#ifdef HALBB_PMAC_TX_SUPPORT
	struct bb_plcp_info bb_plcp_i;
#endif
#endif //NEO

	struct bb_link_info	bb_link_i;
	struct bb_path_info	bb_path_i;
	struct bb_ch_info	bb_ch_i;
#if 0 //NEO
	struct bb_api_info	bb_api_i;
	struct bb_iot_info	bb_iot_i;
#endif //NEO
	struct bb_dbg_info	bb_dbg_i;
#if 0 //NEO
	struct bb_c2h_fw_tx_rpt	bb_fwtx_c2h_i;
	struct bb_h2c_fw_tx_setting	bb_fwtx_h2c_i;
	struct bb_h2c_fw_edcca	bb_fw_edcca_i;
	struct bb_h2c_he_sigb	bb_h2c_he_sigb_i;
	struct bb_fw_dbg_info	bb_fwdbg_i;
	struct bb_cmn_rpt_info	bb_cmn_rpt_i;
	struct bb_rpt_info bb_rpt_i;
	struct rxevm_physts	rxevm;
	struct bb_cmn_backup_info	bb_cmn_backup_i;
#ifdef HALBB_CH_INFO_SUPPORT
	struct bb_ch_rpt_info	bb_ch_rpt_i;
#endif
#ifdef HALBB_AUTO_DBG_SUPPORT
	struct bb_auto_dbg_info bb_auto_dbg_i;
#endif
#ifdef HALBB_ANT_DIV_SUPPORT
	struct bb_antdiv_info bb_ant_div_i;
#endif
#ifdef HALBB_DYN_L2H_SUPPORT
	struct bb_dyn_l2h_info bb_dyn_l2h_i;
#endif
	/*@=== [HALBB Timer] ================================================*/
#ifdef HALBB_RUA_SUPPORT
	/*struct rtw_rua_tbl rtw_rua_t;*/
#endif

#endif //NEO
};

 
/*@--------------------------[Prptotype]-------------------------------------*/
void halbb_cmn_info_self_reset(struct bb_info *bb);
void halbb_sta_info_dbg(struct bb_info *bb, char input[][16], u32 *_used,
			char *output, u32 *_out_len);
void halbb_supportability_dbg(struct bb_info *bb, char input[][16], u32 *_used,
			     char *output, u32 *_out_len);
void halbb_pause_func_dbg(struct bb_info *bb, char input[][16], u32 *_used,
			  char *output, u32 *_out_len);
#endif
