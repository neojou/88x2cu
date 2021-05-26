/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
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
 ******************************************************************************/


#include "init_8822c.h"
#include "../../mac_def.h"

#include "../pwr.h"
#include "../efuse.h"
#include "../init.h"
#if 0 // NEO
#include "../trxcfg.h"
#endif // if 0 NEO
#include "pwr_seq_8822c.h"

#include "../hw.h"

#if 0 // NEO
#include "../security_cam.h"
#include "../../feature_cfg.h"
#endif // if 0 NEO

#include "../trx_desc.h"
#include "../fwcmd.h"
#include "../fwdl.h"

#if 0 // NEO
#include "../fwofld.h"
#include "../role.h"
#include "../tblupd.h"
#include "../rx_forwarding.h"
#include "../rx_filter.h"
#include "../phy_rpt.h"
#include "../hwamsdu.h"
#include "../status.h"
#include "../hdr_conv.h"
#include "../hw_seq.h"
#include "gpio_8852a.h"
#include "../gpio.h"
#include "../cpuio.h"
#include "../sounding.h"
#include "../power_saving.h"
#include "../wowlan.h"
#include "../tcpip_checksum_offload.h"
#include "../la_mode.h"
#include "../dle.h"
#include "../coex.h"
#include "../mcc.h"
#include "../twt.h"
#include "../mport.h"
#include "../p2p.h"
#endif // if 0 NEO

#if MAC_SDIO_SUPPORT
#include "../_sdio.h"
#endif
#if MAC_USB_SUPPORT
#include "../_usb.h"
#endif
#if MAC_PCIE_SUPPORT
#include "../_pcie.h"
#endif
#if MAC_AX_FEATURE_DBGPKG
#include "../dbgpkg.h"
#endif


#if MAC_SDIO_SUPPORT
static struct mac_ax_intf_ops mac8852a_sdio_ops = {
	reg_read8_sdio, /* reg_read8 */
	reg_write8_sdio, /* reg_write8 */
	reg_read16_sdio, /* reg_read16 */
	reg_write16_sdio, /* reg_write16 */
	reg_read32_sdio, /* reg_read32 */
	reg_write32_sdio, /* reg_write32 */
	tx_allow_sdio, /* tx_allow_sdio */
	tx_cmd_addr_sdio, /* tx_cmd_addr_sdio */
	sdio_pre_init, /* intf_pre_init */
	sdio_init, /* intf_init */
	sdio_deinit, /* intf_deinit */
	reg_read_n_sdio, /* reg_read_n_sdio */
	NULL, /*get_bulkout_id*/
	NULL, /* ltr_set_pcie */
	NULL, /*u2u3_switch*/
	NULL, /*get_usb_mode*/
	NULL, /*get_usb_support_ability*/
	NULL, /*usb_tx_agg_cfg*/
	NULL, /*usb_rx_agg_cfg*/
	NULL, /*set_wowlan*/
};
#endif

#if MAC_USB_SUPPORT
static struct mac_intf_ops mac8822c_usb_ops = {
	reg_read8_usb, /* reg_read8 */
	reg_write8_usb, /* reg_write8 */
	reg_read16_usb, /* reg_read16 */
	reg_write16_usb, /* reg_write16 */
	reg_read32_usb, /* reg_read32 */
	reg_write32_usb, /* reg_write32 */
	NULL, /* tx_allow_sdio */
	NULL, /* tx_cmd_addr_sdio */
	usb_pre_init, /* intf_pre_init */
	usb_init, /* intf_init */
	usb_deinit, /* intf_deinit */
	NULL, /* reg_read_n_sdio */
	get_bulkout_id, /*get_bulkout_id*/
	NULL, /* ltr_set_pcie */
	u2u3_switch, /*u2u3_switch*/
	get_usb_mode, /*get_usb_mode*/
	get_usb_support_ability,/*get_usb_support_ability*/
#if 0 // NEO
	usb_tx_agg_cfg, /*usb_tx_agg_cfg*/
	usb_rx_agg_cfg, /*usb_rx_agg_cfg*/
	set_usb_wowlan, /*set_wowlan*/
#endif // if 0 NEO
};
#endif

#if MAC_PCIE_SUPPORT
static struct mac_ax_intf_ops mac8852a_pcie_ops = {
	reg_read8_pcie, /* reg_read8 */
	reg_write8_pcie, /* reg_write8 */
	reg_read16_pcie, /* reg_read16 */
	reg_write16_pcie, /* reg_write16 */
	reg_read32_pcie, /* reg_read32 */
	reg_write32_pcie, /* reg_write32 */
	NULL, /* tx_allow_sdio */
	NULL, /* tx_cmd_addr_sdio */
	pcie_pre_init, /* intf_pre_init */
	pcie_init, /* intf_init */
	pcie_deinit, /* intf_deinit */
	NULL, /* reg_read_n_sdio */
	NULL, /*get_bulkout_id*/
	ltr_set_pcie, /* ltr_set_pcie */
	NULL, /*u2u3_switch*/
	NULL, /*get_usb_mode*/
	NULL,/*get_usb_support_ability*/
	NULL, /*usb_tx_agg_cfg*/
	NULL, /*usb_rx_agg_cfg*/
	set_pcie_wowlan, /*set_wowlan*/
};
#endif

static struct mac_ops mac8822c_ops = {
	NULL, /* intf_ops */
	/*System level*/
	mac_hal_init, /* hal_init */
	mac_hal_fast_init, /* hal_fast_init */
#if 0 // NEO
	NULL, /* mac_hal_deinit,*/ /* hal_deinit */
	NULL, /* mac_add_role, *//* add_role */
	NULL, /* mac_remove_role, *//* remove_role */
	NULL, /* mac_change_role, */ /* change_role */
#endif // NEO if 0
	mac_pwr_switch, /* pwr_switch */
#if 0 // NEO
	NULL, /* mac_sys_init, */ /* sys_init */
	NULL, /* mac_trx_init, */ /* init */
	NULL, /* mac_romdl, */ /* romdl */
	NULL, /* mac_enable_cpu, */ /* enable_cpu */
	NULL, /* mac_disable_cpu, */ /* disable_cpu */
	NULL, /* mac_fwredl, */ /* fwredl */
	NULL, /* mac_fwdl, */ /* fwdl */
#endif // NEO if 0
	mac_enable_fw, /* enable_fw */
#if 0 // NEO
	NULL, /* mac_lv1_rcvy, */ /* lv1_rcvy */
	NULL, /* mac_get_macaddr, */
#endif // NEO if 0
	mac_build_txdesc, /* build_txdesc */
#if 0 // NEO
	NULL, /* mac_refill_txdesc, */ /* refill_txdesc */
	NULL, /* mac_parse_rxdesc, */ /* parse_rxdesc */
	/*FW offload related*/
	NULL, /* mac_reset_fwofld_state, */
	NULL, /* mac_check_fwofld_done, */
	NULL, /* mac_read_pkt_ofld, */
	NULL, /* mac_del_pkt_ofld, */
	NULL, /* mac_add_pkt_ofld, */
	NULL, /* mac_pkt_ofld_packet, */
	NULL, /* mac_dump_efuse_ofld, */
	NULL, /* mac_efuse_ofld_map, */
	NULL, /* mac_upd_dctl_info, */ /*update dmac ctrl info*/
	NULL, /* mac_upd_cctl_info, */ /*update cmac ctrl info*/
	NULL, /* mac_ie_cam_upd, */ /* ie_cam_upd */
	NULL, /* mac_twt_info_upd_h2c, */ /* twt info update h2c */
	NULL, /* mac_twt_act_h2c, */ /* twt act h2c */
	NULL, /* mac_twt_staanno_h2c, */ /* twt anno h2c */
	NULL, /* mac_twt_wait_anno, */
	NULL, /* mac_host_getpkt_h2c, */
	NULL, /* mac_p2p_act_h2c, */ /* p2p act h2c */
	NULL, /* mac_get_p2p_stat, */ /* get p2p state */
	/*Association, de-association related*/
	NULL, /* mac_sta_add_key, */ /* add station key */
	NULL, /* mac_sta_del_key, */ /* del station key */
	NULL, /* mac_sta_search_key_idx, */ /* search station key index */
	NULL, /* mac_sta_hw_security_support, */ /* control hw security support */
	NULL, /* mac_set_mu_table, */ /*set mu score table*/
	NULL, /* mac_ss_dl_grp_upd, */ /* update SS dl group info*/
	NULL, /* mac_ss_ul_grp_upd, */ /* update SS ul group info*/
	NULL, /* mac_ss_ul_sta_upd, */ /* add sta into SS ul link*/
	NULL, /* mac_bacam_info, */ /*update BA CAM info*/
#endif // NEO if 0
	/*TRX related*/
	mac_txdesc_len, /* txdesc_len */
#if 0 // NEO
	NULL, /* mac_upd_shcut_mhdr, */ /*update short cut mac header*/
	NULL, /* mac_enable_hwmasdu, */ /* enable_hwmasdu */
	NULL, /* mac_enable_cut_hwamsdu, */ /* enable_cut_hwamsdu */
	NULL, /* mac_hdr_conv, */ /* hdr_conv */
	NULL, /* mac_set_hwseq_reg, */ /* set hw seq by reg */
#endif // NEO if 0
	mac_process_c2h, /* process_c2h */
#if 0 // NEO
	NULL, /* mac_parse_dfs, */ /* parse_dfs */
	NULL, /* mac_parse_ppdu, */ /* parse_ppdu */
	NULL, /* mac_cfg_phy_rpt, */ /* cfg_phy_rpt */
	NULL, /* mac_set_rx_forwarding, */ /* rx_forwarding */
	NULL, /* mac_get_rx_fltr_opt, */ /* set rx fltr mac, pclp header opt */
	NULL, /* mac_set_rx_fltr_opt, */ /* get rx fltr mac, pclp header opt */
	NULL, /* mac_set_typ_fltr_opt, */ /* set machdr type fltr opt */
	NULL, /* mac_set_typsbtyp_fltr_opt, */ /* set machdr typ subtyp fltr opt */
	NULL, /* mac_sr_update, */ /* set sr parameter */
	NULL, /* mac_two_nav_cfg,  */ /* config 2NAV hw setting */
	NULL, /* mac_wde_pkt_drop, */ /* pkt_drop */
	NULL, /* mac_send_bcn_h2c, */ /* send beacon h2c */
	NULL, /* mac_tx_mode_sel, */ /*tx mode sel*/
	NULL, /* mac_tcpip_chksum_ofd, */ /* tcpip_chksum_ofd */
	NULL, /* mac_chk_rx_tcpip_chksum_ofd, */ /* chk_rx_tcpip_chksum_ofd */
	NULL, /* mac_chk_allq_empty, */ /*chk_allq_tmpty*/
	NULL, /* mac_is_txq_empty, */ /*chk_txq_tmpty*/
	NULL, /* mac_is_rxq_empty, */ /*chk_txq_tmpty*/
	NULL, /* mac_parse_bcn_stats_c2h, */ /*parse tx bcn statistics*/
	/*frame exchange related*/
	NULL, /* mac_upd_mudecision_para, */ /* upd_ba_infotbl */
	NULL, /* mac_mu_sta_upd, */ /* upd_mu_sta */
	NULL, /* mac_upd_ul_fixinfo, */ /* upd_ul_fixinfo */
	NULL, /* mac_f2p_test_cmd, */ /*f2p test cmd para*/
	NULL, /* mac_snd_test_cmd, */ /* f2p test cmd para */
	NULL, /* mac_set_fixmode_mib, */ /* set_fw_testmode */
	NULL, /* mac_dumpwlanc, */
	NULL, /* mac_dumpwlans, */
	NULL, /* mac_dumpwland, */
	/*outsrcing related */
	NULL, /* mac_outsrc_h2c_common, */ /* outsrc common h2c */
	NULL, /* mac_read_pwr_reg, */ /* for read tx power reg*/
	NULL, /* mac_write_pwr_reg, */ /* for write tx power reg*/
	NULL, /* mac_write_pwr_ofst_mode, */ /* for write tx power mode offset reg*/
	NULL, /* mac_write_pwr_ofst_bw, */ /* for write tx power BW offset reg*/
	NULL, /* mac_write_pwr_ref_reg, */ /* for write tx power ref reg*/
	NULL, /* mac_write_pwr_limit_en, */ /* for write tx power limit enable reg*/
	NULL, /* mac_write_pwr_limit_rua_reg, */ /* for write tx power limit rua reg*/
	NULL, /* mac_write_pwr_limit_reg, */ /* for write tx power limit reg*/
	NULL, /* mac_write_pwr_by_rate_reg, */ /* for write tx power by rate reg*/
	NULL, /* mac_lamode_cfg, */ /*cfg la mode para*/
	NULL, /* mac_lamode_trigger, */ /*trigger la mode start*/
	NULL, /* mac_lamode_buf_cfg, */ /*la mode buf size cfg */
	NULL, /* mac_get_lamode_st, */ /*get la mode status*/
	NULL, /* mac_read_xcap_reg, */ /*read xcap xo/xi reg*/
	NULL, /* mac_write_xcap_reg, */ /*write xcap xo/xi reg*/
	NULL, /* mac_write_bbrst_reg, */ /*write bb rst reg*/
	/*sounding related*/
	NULL, /* mac_get_csi_buffer_index, */ /* get CSI buffer index */
	NULL, /* mac_set_csi_buffer_index, */ /* set CSI buffer index */
	NULL, /* mac_get_snd_sts_index, */ /* get MACID SND status */
	NULL, /* mac_set_snd_sts_index, */ /* set SND status MACID */
	NULL, /* mac_init_snd_mer, */ /* init SND MER */
	NULL, /* mac_init_snd_mee, */ /* init SND MEE */
	NULL, /* mac_csi_force_rate, */ /*CSI fix rate reg*/
	NULL, /* mac_csi_rrsc, */ /*CSI RRSC*/
	NULL, /* mac_set_snd_para, */ /*set sound parameter*/
	NULL, /* mac_set_csi_para_reg, */ /*set reg csi para*/
	NULL, /* mac_set_csi_para_cctl, */ /*set csi para in cmac ctrl info*/
	NULL, /* mac_hw_snd_pause_release, */ /*HW SND pause release*/
	NULL, /* mac_bypass_snd_sts, */ /*bypass SND status*/
	NULL, /* mac_deinit_mee, */ /*deinit mee*/
	/*lps related*/
	NULL, /* mac_cfg_lps, */ /*config LPS*/
	NULL, /* mac_lps_pwr_state, */ /*set or check lps power state*/
	NULL, /* mac_chk_leave_lps, */ /*check already leave protocol ps*/
	NULL, /* mac_lps_chk_access, */ /*check the register can be accessed in lps */
	/* Wowlan related*/
	NULL, /* mac_cfg_wow_wake, */ /*config wowlan wake*/
	NULL, /* mac_cfg_disconnect_det, */ /*config disconnect det*/
	NULL, /* mac_cfg_keep_alive, */ /*config keep alive*/
	NULL, /* mac_cfg_gtk_ofld, */ /*config gtk ofld*/
	NULL, /* mac_cfg_arp_ofld, */ /*config arp ofld*/
	NULL, /* mac_cfg_ndp_ofld, */ /*config ndp ofld*/
	NULL, /* mac_cfg_realwow, */ /*config realwow*/
	NULL, /* mac_cfg_nlo, */ /*config nlo*/
	NULL, /* mac_cfg_dev2hst_gpio, */ /*config dev2hst gpio*/
	NULL, /* mac_cfg_uphy_ctrl, */ /*config uphy ctrl*/
	NULL, /* mac_cfg_wowcam_upd, */ /*config wowcam update*/
	NULL, /* mac_cfg_wow_sleep, */ /*config wowlan before sleep/after wake*/
	NULL, /* mac_get_wow_fw_status, */ /*get wowlan fw status*/
	NULL, /* mac_request_aoac_report, */
	NULL, /* mac_read_aoac_report, */
	/*system related*/
	NULL, /* mac_dbcc_enable, */ /*enable / disable dbcc */
	NULL, /* mac_port_cfg, */ /* cofig port para */
	NULL, /* mac_port_init, */ /* init port para */
	NULL, /* mac_enable_imr, */ /* enable CMAC/DMAC IMR */
	NULL, /* mac_dump_efuse_map_wl, */ /* dump_wl_efuse*/
	NULL, /* mac_dump_efuse_map_bt, */ /* dump_bt_efuse */
	NULL, /* mac_write_efuse, */ /* write_wl_bt_efuse */
	NULL, /* mac_read_efuse, */ /* read_wl_bt_efuse */
	NULL, /* mac_get_efuse_avl_size, */ /* get_available_efuse_size */
	NULL, /* mac_get_efuse_avl_size_bt, */ /* get_available_efuse_size_bt */
#endif // NEO
	mac_dump_log_efuse, /* dump_log_efuse */
#if 0 //NEO
	NULL, /* mac_read_log_efuse, */ /* read_logical_efuse */
	NULL, /* mac_write_log_efuse, */ /* write_logical_efuse */
	NULL, /* mac_dump_log_efuse_bt, */ /* dump_logical_efuse_bt */
	NULL, /* mac_read_log_efuse_bt, */ /* read_logical_efuse_bt */
	NULL, /* mac_write_log_efuse_bt, */ /* write_logical_efuse_bt */
	NULL, /* mac_pg_efuse_by_map, */ /* program_efuse_map */
	NULL, /* mac_pg_efuse_by_map_bt, */ /* program_efuse_map_bt */
	NULL, /* mac_mask_log_efuse, */ /* mask_logical_efuse_map */
	NULL, /* mac_pg_sec_data_by_map, */ /* program_secure_data_map */
	NULL, /* mac_cmp_sec_data_by_map, */ /* compare_secure_data_map */
#endif // NEO
	mac_get_efuse_info, /* get_efuse_info */
#if 0 //NEO
	NULL, /* mac_set_efuse_info, */ /* set_efuse_info */
#endif // NEO
	mac_read_hidden_rpt, /* read_hidden_rpt */
#if 0 //NEO
	NULL, /* mac_check_efuse_autoload, */ /* check_efuse_autoload */
	NULL, /* mac_pg_simulator, */ /* efuse pg simulator */
	NULL, /* mac_checksum_update, */ /* checksum update */
	NULL, /* mac_checksum_rpt, */ /*report checksum comparison result*/
	NULL, /* mac_set_efuse_ctrl, */ /*set efuse ctrl 0x30 or 0xC30*/
	NULL, /* mac_otp_test, */ /*efuse OTP test R/W to 0x7ff*/
	NULL, /* mac_get_ft_status, */ /* get_mac_ft_status */
	NULL, /* mac_fw_log_cfg, */ /* fw_log_cfg */
	NULL, /* mac_pinmux_set_func_8852a, */ /* pinmux_set_func */
	NULL, /* mac_pinmux_free_func, */ /* pinmux_free_func */
	NULL, /* mac_sel_uart_tx_pin, */ /* sel_uart_tx_pin */
	NULL, /* mac_sel_uart_rx_pin, */ /* sel_uart_rx_pin */
	NULL, /* mac_set_gpio_func_8852a, */ /* set_gpio_func */
#endif // NEO
	mac_get_hw_info, /* get_hw_info */
#if 0 //NEO
	NULL, /* mac_set_hw_value, */ /* set_hw_value */
#endif // NEO
	mac_get_hw_value, /* get_hw_value */
#if 0 //NEO
	NULL, /* mac_get_err_status, */ /* get_err_status */
	NULL, /* mac_set_err_status, */ /* set_err_status */
	NULL, /* mac_general_pkt_ids, */ /*general_pkt_ids */
	NULL, /* mac_coex_init, */ /* coex_init */
	NULL, /* mac_read_coex_reg, */ /* coex_read */
	NULL, /* mac_write_coex_reg, */ /* coex_write */
	NULL, /* mac_trigger_cmac_err, */ /*trigger_cmac_err*/
	NULL, /* mac_trigger_cmac1_err, */ /*trigger_cmac1_err*/
	NULL, /* mac_trigger_dmac_err, */ /*trigger_dmac_err*/
	NULL, /* mac_tsf_sync, */ /*tsf_sync*/
	/* mcc */
	NULL, /* mac_reset_mcc_group, */
	NULL, /* mac_reset_mcc_request, */
	NULL, /* mac_add_mcc, */ /* add_mcc */
	NULL, /* mac_start_mcc, */ /* start_mcc */
	NULL, /* mac_stop_mcc, */ /* stop_mcc */
	NULL, /* mac_del_mcc_group, */ /* del_mcc_group */
	NULL, /* mac_mcc_request_tsf, */ /* mcc_request_tsf */
	NULL, /* mac_mcc_macid_bitmap, */ /* mcc_macid_bitmap */
	NULL, /* mac_mcc_sync_enable, */ /* mcc_sync_enable */
	NULL, /* mac_mcc_set_duration, */ /* mcc_set_duration */
	NULL, /* mac_get_mcc_tsf_rpt, */
	NULL, /* mac_get_mcc_status_rpt, */
	NULL, /* mac_check_add_mcc_done, */
	NULL, /* mac_check_start_mcc_done, */
	NULL, /* mac_check_stop_mcc_done, */
	NULL, /* mac_check_del_mcc_group_done, */
	NULL, /* mac_check_mcc_request_tsf_done, */
	NULL, /* mac_check_mcc_macid_bitmap_done, */
	NULL, /* mac_check_mcc_sync_enable_done, */
	NULL, /* mac_check_mcc_set_duration_done, */
	/* not mcc */
	NULL, /* mac_check_access, */
	NULL, /* mac_set_led_mode, */ /* set_led_mode */
	NULL, /* mac_led_ctrl, */ /* led_ctrl */
	NULL, /* mac_set_sw_gpio_mode, */ /* set_sw_gpio_mode */
	NULL, /* mac_sw_gpio_ctrl, */ /* sw_gpio_ctrl */
#if MAC_AX_FEATURE_DBGPKG
	mac_fwcmd_lb, /* fwcmd_lb */
	mac_mem_dump, /* sram mem dump */
	mac_get_mem_size, /* get mem size */
	mac_dbg_status_dump, /* mac dbg status dump */
	mac_reg_dump, /* debug reg dump for MAC/BB/RF*/
	mac_rx_cnt,
	mac_dump_fw_rsvd_ple,
	mac_fw_dbg_dump,
#endif
#if MAC_AX_FEATURE_HV
	mac_ram_boot, /* ram_boot */
	/*fw offload related*/
	mac_clear_write_request, /* clear_write_request */
	mac_add_write_request, /* add_write_request */
	mac_write_ofld, /* write_ofld */
	mac_clear_conf_request, /* clear_conf_request */
	mac_add_conf_request, /* add_conf_request */
	mac_conf_ofld, /* conf_ofld */
	mac_clear_read_request, /* clear_read_request */
	mac_add_read_request, /* add_read_request */
	mac_read_ofld, /* read_ofld */
	mac_read_ofld_value, /* read_ofld_value */
#endif
#endif // NEO if 0
};


static struct mac_hw_info mac8822c_hw_info = {
	0, /* done */
	MAC_CHIP_ID_8822C, /* chip_id */
	0xFF, /* chip_cut */
	MAC_INTF_INVALID, /* intf */
	19, /* tx_ch_num */
	10, /* tx_data_ch_num */
	0, /* WD_BODY_LEN, */ /* wd_body_len */
	0, /* WD_INFO_LEN, */ /* wd_info_len */
	pwr_on_seq_8822c, /* pwr_on_seq */
	pwr_off_seq_8822c, /* pwr_off_seq */
	NULL, /* PWR_SEQ_VER_8852A, */ /* pwr_seq_ver */
	262144, /* txff_size */
	24576, /* rxff_size */
	50, /* csi_buf_pg_num */
	128, /* macid_num */
	20, /* bssid_num */
	1536, /* wl_efuse_size */
	1216, /* wl_zone2_efuse_size */
	768, /* log_efuse_size */
	1152, /* limit_efuse_size_PCIE */
	1152, /* limit_efuse_size_USB */
	1184, /* limit_efuse_size_SDIO */
	512, /* bt_efuse_size */
	1024, /* bt_log_efuse_size */
	32, /*hidden_efuse_size*/
	4, /* sec_ctrl_efuse_size */
	192, /* sec_data_efuse_size */
	NULL, /* sec_cam_table_t pointer */
	32, /* ple_rsvd_space */
	24, /* payload_desc_size */
	6, /* efuse_version_size */
	0, /* wl_efuse_size_DAV */
	0, /* wl_zone2_efuse_size_DAV */
	0, /* hidden_efuse_size_DAV */
	0, /* log_efuse_size_DAV */
	0, /* wl_efuse_start_addr */
	0, /* wl_efuse_start_addr_DAV */
	0, /* bt_efuse_start_addr */
	0, /* wd_checksum_en */
	0, /* sw_amsdu_max_size */
	NULL, /* pwr_on */
	NULL, /* pwr_off */
	0, /* ind_aces_cnt */
	0, /* dbg_port_cnt */
	0, /* core_swr_volt */
	0, /* core_swr_volt_sel */
};

#if 0 // NEO
static struct mac_ax_ft_status mac_8852a_ft_status[] = {
	{MAC_AX_FT_DUMP_EFUSE, MAC_AX_STATUS_IDLE, NULL, 0},
	{MAC_AX_FT_MAX, MAC_AX_STATUS_ERR, NULL, 0},
};

#endif // if 0 NEO

static struct mac_adapter mac_8822c_adapter = {
	&mac8822c_ops, /* ops */
	NULL, /* drv_adapter */
	NULL, /* phl_adapter */
	NULL, /* pltfm_cb */
	NULL, /* MAC_AX_DFLT_SM, */ /* sm */
	NULL, /* hw_info */
	{0, 0, 0, 0, 0, 0, 0, 0, 0}, /* fw_info */
	{NULL, NULL, NULL, 0, 0, 0, 0, 0}, /* efuse_param */
	{0, 0, NULL}, /* mac_pwr_info */
	NULL, /* mac_8852a_ft_status, */ /* ft_stat */
	NULL, /* hfc_param */
	NULL, /* {MAC_AX_QTA_SCC, 64, 128}, */ /* dle_info */
	NULL, /*
	{0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, 0, 0, 0,
	 0, 0, 0, 0, 0, DFLT_GPIO_STATE, DFLT_SW_IO_MODE}, */ /* gpio_info */
	NULL, /* role table */
	{NULL, NULL, NULL, 0, 0, 0, 0}, /* read_ofld_info */
	{0, 0, NULL}, /* read_ofld_value */
	{NULL, NULL, NULL, 0, 0, 0, 0}, /* write_ofld_info */
	{NULL}, /* efuse_ofld_info */
	{NULL, NULL, 0, 0, 0, 0}, /* conf_ofld_info */
	NULL, /* {PKT_OFLD_OP_MAX, PKT_OFLD_MAX_COUNT - 1, 0, {0}}, */ /* pkt_ofld_info */
	{0, 0, 0, NULL}, /* pkt_ofld_pkt */
	{{{0}, {0}, {0}, {0}}}, /* mcc_group_info */
	{NULL}, /* wowlan_info */
#if MAC_SDIO_SUPPORT
	{MAC_AX_SDIO_4BYTE_MODE_DISABLE, MAC_AX_SDIO_TX_MODE_AGG,
	MAC_AX_SDIO_SPEC_VER_2_00, 512, 1, 8, 0}, /* sdio_info */
#endif
#if MAC_AX_FEATURE_HV
	NULL, /*hv_ax_ops*/
#endif
};


#ifdef CONFIG_NEW_HALMAC_INTERFACE
aa
struct mac_ax_adapter *get_mac_8852a_adapter(enum mac_ax_intf intf,
					     u8 chip_cut, void *phl_adapter,
					     void *drv_adapter,
					     struct mac_ax_pltfm_cb *pltfm_cb)
{
	struct mac_ax_adapter *adapter = NULL;
	struct mac_ax_mac_pwr_info *pwr_info;

	adapter =
	(struct mac_ax_adapter *)hal_mem_alloc(drv_adapter,
		sizeof(struct mac_ax_adapter));
	if (!adapter)
		return NULL;

	hal_mem_cpy(drv_adapter, adapter, &mac_8852a_adapter,
		    sizeof(struct mac_ax_adapter));
	pwr_info = &adapter->mac_pwr_info;

	adapter->phl_adapter = phl_adapter;
	adapter->drv_adapter = drv_adapter;
	adapter->pltfm_cb = pltfm_cb;
	adapter->hw_info->chip_cut = chip_cut;
	adapter->hw_info->intf = intf;
	adapter->hw_info->done = 1;

	switch (intf) {
#if MAC_AX_SDIO_SUPPORT
	case MAC_AX_INTF_SDIO:
		adapter->ops->intf_ops = &mac8852a_sdio_ops;
		pwr_info->intf_pwr_switch = sdio_pwr_switch;
		break;
#endif
#if MAC_AX_USB_SUPPORT
	case MAC_AX_INTF_USB:
		adapter->ops->intf_ops = &mac8852a_usb_ops;
		pwr_info->intf_pwr_switch = usb_pwr_switch;
		break;
#endif
#if MAC_AX_PCIE_SUPPORT
	case MAC_AX_INTF_PCIE:
		adapter->ops->intf_ops = &mac8852a_pcie_ops;
		pwr_info->intf_pwr_switch = pcie_pwr_switch;
		break;
#endif
	default:
		return NULL;
	}

	return adapter;
}
#else
struct mac_adapter *get_mac_8822c_adapter(enum mac_intf intf,
					     u8 cv, void *drv_adapter,
					     struct mac_pltfm_cb *pltfm_cb)
{
	struct mac_adapter *adapter = NULL;
	struct mac_hw_info *hw_info = NULL;
	//struct mac_ax_mac_pwr_info *pwr_info;

	if (!pltfm_cb)
		return NULL;

	adapter = (struct mac_adapter *)pltfm_cb->rtl_malloc(drv_adapter,
		sizeof(struct mac_adapter));
	if (!adapter) {
		pltfm_cb->msg_print(drv_adapter, "Malloc adapter fail\n");
		return NULL;
	}

	pltfm_cb->rtl_memcpy(drv_adapter, adapter, &mac_8822c_adapter,
		     sizeof(struct mac_adapter));

	/*Alloc HW INFO */
	hw_info = (struct mac_hw_info *)pltfm_cb->rtl_malloc(drv_adapter,
		sizeof(struct mac_hw_info));

	if (!hw_info) {
		pltfm_cb->msg_print(drv_adapter, "Malloc hw info fail\n");
		return NULL;
	}

	pltfm_cb->rtl_memcpy(drv_adapter, hw_info, &mac8822c_hw_info,
		sizeof(struct mac_hw_info));

	//pwr_info = &adapter->mac_pwr_info;

	adapter->drv_adapter = drv_adapter;
	adapter->pltfm_cb = pltfm_cb;
	adapter->hw_info = hw_info;
	adapter->hw_info->cv = cv;
	adapter->hw_info->intf = intf;
	adapter->hw_info->done = 1;

	switch (intf) {
#if MAC_SDIO_SUPPORT
	case MAC_INTF_SDIO:
		adapter->ops->intf_ops = &mac8852a_sdio_ops;
		//pwr_info->intf_pwr_switch = sdio_pwr_switch;
		break;
#endif
#if MAC_USB_SUPPORT
	case MAC_INTF_USB:
		adapter->ops->intf_ops = &mac8822c_usb_ops;
		// NEO
		//pwr_info->intf_pwr_switch = usb_pwr_switch;
		break;
#endif
#if MAC_PCIE_SUPPORT
	case MAC_INTF_PCIE:
		adapter->ops->intf_ops = &mac8852a_pcie_ops;
		//pwr_info->intf_pwr_switch = pcie_pwr_switch;
		break;
#endif
	default:
		return NULL;
	}

	return adapter;
}
#endif /* CONFIG_NEW_HALMAC_INTERFACE */


