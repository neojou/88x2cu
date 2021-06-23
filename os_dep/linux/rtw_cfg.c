/******************************************************************************
 *
 * Copyright(c) 2007 - 2019 Realtek Corporation.
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
#define _OS_INTFS_C_

#include <drv_types.h>
#include <hal_data.h>

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Realtek Wireless Lan Driver");
MODULE_AUTHOR("Realtek Semiconductor Corp.");
MODULE_VERSION(DRIVERVERSION);

/* module param defaults */
int rtw_chip_version = 0x00;
int rtw_rfintfs = HWPI;
int rtw_lbkmode = 0;/* RTL8712_AIR_TRX; */
#ifdef DBG_LA_MODE
int rtw_la_mode_en=0;
module_param(rtw_la_mode_en, int, 0644);
#endif
int rtw_network_mode = Ndis802_11IBSS;/* Ndis802_11Infrastructure; */ /* infra, ad-hoc, auto */
/* NDIS_802_11_SSID	ssid; */
int rtw_channel = 1;/* ad-hoc support requirement */
int rtw_wireless_mode = WIRELESS_MODE_MAX;
int rtw_band_type = BAND_CAP_2G | BAND_CAP_5G;
module_param(rtw_wireless_mode, int, 0644);
module_param(rtw_band_type, int, 0644);
int rtw_vrtl_carrier_sense = AUTO_VCS;
int rtw_vcs_type = RTS_CTS;
int rtw_rts_thresh = 2347;
int rtw_frag_thresh = 2346;
int rtw_preamble = PREAMBLE_LONG;/* long, short, auto */
int rtw_scan_mode = 1;/* active, passive */
/* int smart_ps = 1; */
#ifdef CONFIG_POWER_SAVING
	/* IPS configuration */
	int rtw_ips_mode = RTW_IPS_MODE;

	/* LPS configuration */
/* RTW_LPS_MODE=0:disable, 1:LPS , 2:LPS with clock gating, 3: power gating */
#if (RTW_LPS_MODE > 0)
	int rtw_power_mgnt = PM_PS_MODE_MAX;

	#ifdef CONFIG_USB_HCI
		int rtw_lps_level = LPS_NORMAL; /*USB default LPS level*/
	#else /*SDIO,PCIE*/
		int rtw_lps_level = (RTW_LPS_MODE - 1);
	#endif/*CONFIG_USB_HCI*/
#else
	int rtw_power_mgnt = PM_PS_MODE_ACTIVE;
	int rtw_lps_level = LPS_NORMAL;
#endif

	int rtw_lps_chk_by_tp = 1;

	/* WOW LPS configuration */
#ifdef CONFIG_WOWLAN
/* RTW_WOW_LPS_MODE=0:disable, 1:LPS , 2:LPS with clock gating, 3: power gating */
#if (RTW_WOW_LPS_MODE > 0)
	int rtw_wow_power_mgnt = PM_PS_MODE_MAX;
	int rtw_wow_lps_level = (RTW_WOW_LPS_MODE - 1);
#else
	int rtw_wow_power_mgnt = PM_PS_MODE_ACTIVE;
	int rtw_wow_lps_level = LPS_NORMAL;
#endif	
#endif /* CONFIG_WOWLAN */

#else /* !CONFIG_POWER_SAVING */
	int rtw_ips_mode = IPS_NONE;
	int rtw_power_mgnt = PM_PS_MODE_ACTIVE;
	int rtw_lps_level = LPS_NORMAL;
	int rtw_lps_chk_by_tp = 0;
#ifdef CONFIG_WOWLAN
	int rtw_wow_power_mgnt = PM_PS_MODE_ACTIVE;
	int rtw_wow_lps_level = LPS_NORMAL;
#endif /* CONFIG_WOWLAN */
#endif /* CONFIG_POWER_SAVING */

#ifdef CONFIG_NARROWBAND_SUPPORTING
int rtw_nb_config = CONFIG_NB_VALUE;
module_param(rtw_nb_config, int, 0644);
MODULE_PARM_DESC(rtw_nb_config, "5M/10M/Normal bandwidth configuration");
#endif

module_param(rtw_ips_mode, int, 0644);
MODULE_PARM_DESC(rtw_ips_mode, "The default IPS mode");

module_param(rtw_lps_level, int, 0644);
MODULE_PARM_DESC(rtw_lps_level, "The default LPS level");

#ifdef CONFIG_LPS_1T1R
int rtw_lps_1t1r = RTW_LPS_1T1R;
module_param(rtw_lps_1t1r, int, 0644);
MODULE_PARM_DESC(rtw_lps_1t1r, "The default LPS 1T1R setting");
#endif

module_param(rtw_lps_chk_by_tp, int, 0644);

#ifdef CONFIG_WOWLAN
module_param(rtw_wow_power_mgnt, int, 0644);
MODULE_PARM_DESC(rtw_wow_power_mgnt, "The default WOW LPS mode");
module_param(rtw_wow_lps_level, int, 0644);
MODULE_PARM_DESC(rtw_wow_lps_level, "The default WOW LPS level");
#ifdef CONFIG_LPS_1T1R
int rtw_wow_lps_1t1r = RTW_WOW_LPS_1T1R;
module_param(rtw_wow_lps_1t1r, int, 0644);
MODULE_PARM_DESC(rtw_wow_lps_1t1r, "The default WOW LPS 1T1R setting");
#endif
#endif /* CONFIG_WOWLAN */

/* LPS: 
 * rtw_smart_ps = 0 => TX: pwr bit = 1, RX: PS_Poll
 * rtw_smart_ps = 1 => TX: pwr bit = 0, RX: PS_Poll
 * rtw_smart_ps = 2 => TX: pwr bit = 0, RX: NullData with pwr bit = 0
*/
int rtw_smart_ps = 2;

int rtw_max_bss_cnt = 0;
module_param(rtw_max_bss_cnt, int, 0644);
#ifdef CONFIG_WMMPS_STA	
/* WMMPS: 
 * rtw_smart_ps = 0 => Only for fw test
 * rtw_smart_ps = 1 => Refer to Beacon's TIM Bitmap
 * rtw_smart_ps = 2 => Don't refer to Beacon's TIM Bitmap
*/
int rtw_wmm_smart_ps = 2;
#endif /* CONFIG_WMMPS_STA */

int rtw_check_fw_ps = 1;

#ifdef CONFIG_TX_EARLY_MODE
int rtw_early_mode = 1;
#endif

int rtw_usb_rxagg_mode = 2;/* RX_AGG_DMA=1, RX_AGG_USB=2 */
module_param(rtw_usb_rxagg_mode, int, 0644);

int rtw_dynamic_agg_enable = 1;
module_param(rtw_dynamic_agg_enable, int, 0644);

/* set log level when inserting driver module, default log level is _DRV_INFO_ = 4,
* please refer to "How_to_set_driver_debug_log_level.doc" to set the available level.
*/
#ifdef CONFIG_RTW_DEBUG
#ifdef RTW_LOG_LEVEL
	uint rtw_drv_log_level = (uint)RTW_LOG_LEVEL; /* from Makefile */
#else
	uint rtw_drv_log_level = _DRV_INFO_;
#endif
module_param(rtw_drv_log_level, uint, 0644);
MODULE_PARM_DESC(rtw_drv_log_level, "set log level when insert driver module, default log level is _DRV_INFO_ = 4");
#endif
int rtw_radio_enable = 1;
int rtw_long_retry_lmt = 7;
int rtw_short_retry_lmt = 7;
int rtw_busy_thresh = 40;
/* int qos_enable = 0; */ /* * */
int rtw_ack_policy = NORMAL_ACK;

int rtw_mp_mode = 0;

#if defined(CONFIG_MP_INCLUDED) && defined(CONFIG_RTW_CUSTOMER_STR)
uint rtw_mp_customer_str = 0;
module_param(rtw_mp_customer_str, uint, 0644);
MODULE_PARM_DESC(rtw_mp_customer_str, "Whether or not to enable customer str support on MP mode");
#endif

int rtw_software_encrypt = 0;
int rtw_software_decrypt = 0;

int rtw_acm_method = 0;/* 0:By SW 1:By HW. */

int rtw_wmm_enable = 1;/* default is set to enable the wmm. */

#ifdef CONFIG_WMMPS_STA
/* uapsd (unscheduled automatic power-save delivery) = a kind of wmmps */
/* 0: NO_LIMIT, 1: TWO_MSDU, 2: FOUR_MSDU, 3: SIX_MSDU */
int rtw_uapsd_max_sp = NO_LIMIT;
/* BIT0: AC_VO UAPSD, BIT1: AC_VI UAPSD, BIT2: AC_BK UAPSD, BIT3: AC_BE UAPSD */
int rtw_uapsd_ac_enable = 0x0;
#endif /* CONFIG_WMMPS_STA */

#if defined(CONFIG_RTL8814A)
	int rtw_pwrtrim_enable = 2; /* disable kfree , rename to power trim disable */
#elif defined(CONFIG_RTL8821C) || defined(CONFIG_RTL8822B) || defined(CONFIG_RTL8822C)
	/*PHYDM API, must enable by default*/
	int rtw_pwrtrim_enable = 1;
#else
	int rtw_pwrtrim_enable = 0; /* Default Enalbe  power trim by efuse config */
#endif

#if CONFIG_TX_AC_LIFETIME
uint rtw_tx_aclt_flags = CONFIG_TX_ACLT_FLAGS;
module_param(rtw_tx_aclt_flags, uint, 0644);
MODULE_PARM_DESC(rtw_tx_aclt_flags, "device TX AC queue packet lifetime control flags");

static uint rtw_tx_aclt_conf_default[3] = CONFIG_TX_ACLT_CONF_DEFAULT;
static uint rtw_tx_aclt_conf_default_num = 0;
module_param_array(rtw_tx_aclt_conf_default, uint, &rtw_tx_aclt_conf_default_num, 0644);
MODULE_PARM_DESC(rtw_tx_aclt_conf_default, "device TX AC queue lifetime config for default status");

#ifdef CONFIG_AP_MODE
#if CONFIG_RTW_AP_DATA_BMC_TO_UC
static uint rtw_tx_aclt_conf_ap_m2u[3] = CONFIG_TX_ACLT_CONF_AP_M2U;
static uint rtw_tx_aclt_conf_ap_m2u_num = 0;
module_param_array(rtw_tx_aclt_conf_ap_m2u, uint, &rtw_tx_aclt_conf_ap_m2u_num, 0644);
MODULE_PARM_DESC(rtw_tx_aclt_conf_ap_m2u, "device TX AC queue lifetime config for AP mode M2U status");
#endif
#endif /* CONFIG_AP_MODE */

#ifdef CONFIG_RTW_MESH
static uint rtw_tx_aclt_conf_mesh[3] = CONFIG_TX_ACLT_CONF_MESH;
static uint rtw_tx_aclt_conf_mesh_num = 0;
module_param_array(rtw_tx_aclt_conf_mesh, uint, &rtw_tx_aclt_conf_mesh_num, 0644);
MODULE_PARM_DESC(rtw_tx_aclt_conf_mesh, "device TX AC queue lifetime config for MESH status");
#endif
#endif /* CONFIG_TX_AC_LIFETIME */

uint rtw_tx_bw_mode = 0x21;
module_param(rtw_tx_bw_mode, uint, 0644);
MODULE_PARM_DESC(rtw_tx_bw_mode, "The max tx bw for 2.4G and 5G. format is the same as rtw_bw_mode");

#ifdef CONFIG_FW_HANDLE_TXBCN
uint rtw_tbtt_rpt = 0;	/*ROOT AP - BIT0, VAP1 - BIT1, VAP2 - BIT2, VAP3 - VAP3, FW report TBTT INT by C2H*/
module_param(rtw_tbtt_rpt, uint, 0644);
#endif

#ifdef CONFIG_80211N_HT
int rtw_ht_enable = 1;
/* 0: 20 MHz, 1: 40 MHz, 2: 80 MHz, 3: 160MHz, 4: 80+80MHz
* 2.4G use bit 0 ~ 3, 5G use bit 4 ~ 7
* 0x21 means enable 2.4G 40MHz & 5G 80MHz */
#ifdef CONFIG_RTW_CUSTOMIZE_BWMODE
int rtw_bw_mode = CONFIG_RTW_CUSTOMIZE_BWMODE;
#else
int rtw_bw_mode = 0x21;
#endif
int rtw_ampdu_enable = 1;/* for enable tx_ampdu , */ /* 0: disable, 0x1:enable */
int rtw_rx_stbc = 1;/* 0: disable, bit(0):enable 2.4g, bit(1):enable 5g, default is set to enable 2.4GHZ for IOT issue with bufflao's AP at 5GHZ */
#if (defined(CONFIG_RTL8814A) || defined(CONFIG_RTL8814B) || defined(CONFIG_RTL8822B) || defined(CONFIG_RTL8822C)) && defined(CONFIG_PCI_HCI)
int rtw_rx_ampdu_amsdu = 2;/* 0: disabled, 1:enabled, 2:auto . There is an IOT issu with DLINK DIR-629 when the flag turn on */
#elif ((defined(CONFIG_RTL8822B) || defined(CONFIG_RTL8822C)) && defined(CONFIG_SDIO_HCI))
int rtw_rx_ampdu_amsdu = 1;
#else
int rtw_rx_ampdu_amsdu;/* 0: disabled, 1:enabled, 2:auto . There is an IOT issu with DLINK DIR-629 when the flag turn on */
#endif
/*
* 2: Follow the AMSDU filed in ADDBA Resp. (Deault)
* 0: Force the AMSDU filed in ADDBA Resp. to be disabled.
* 1: Force the AMSDU filed in ADDBA Resp. to be enabled.
*/
int rtw_tx_ampdu_amsdu = 2;

int rtw_quick_addba_req = 0;

static uint rtw_rx_ampdu_sz_limit_1ss[4] = CONFIG_RTW_RX_AMPDU_SZ_LIMIT_1SS;
static uint rtw_rx_ampdu_sz_limit_1ss_num = 0;
module_param_array(rtw_rx_ampdu_sz_limit_1ss, uint, &rtw_rx_ampdu_sz_limit_1ss_num, 0644);
MODULE_PARM_DESC(rtw_rx_ampdu_sz_limit_1ss, "RX AMPDU size limit for 1SS link of each BW, 0xFF: no limitation");

static uint rtw_rx_ampdu_sz_limit_2ss[4] = CONFIG_RTW_RX_AMPDU_SZ_LIMIT_2SS;
static uint rtw_rx_ampdu_sz_limit_2ss_num = 0;
module_param_array(rtw_rx_ampdu_sz_limit_2ss, uint, &rtw_rx_ampdu_sz_limit_2ss_num, 0644);
MODULE_PARM_DESC(rtw_rx_ampdu_sz_limit_2ss, "RX AMPDU size limit for 2SS link of each BW, 0xFF: no limitation");

static uint rtw_rx_ampdu_sz_limit_3ss[4] = CONFIG_RTW_RX_AMPDU_SZ_LIMIT_3SS;
static uint rtw_rx_ampdu_sz_limit_3ss_num = 0;
module_param_array(rtw_rx_ampdu_sz_limit_3ss, uint, &rtw_rx_ampdu_sz_limit_3ss_num, 0644);
MODULE_PARM_DESC(rtw_rx_ampdu_sz_limit_3ss, "RX AMPDU size limit for 3SS link of each BW, 0xFF: no limitation");

static uint rtw_rx_ampdu_sz_limit_4ss[4] = CONFIG_RTW_RX_AMPDU_SZ_LIMIT_4SS;
static uint rtw_rx_ampdu_sz_limit_4ss_num = 0;
module_param_array(rtw_rx_ampdu_sz_limit_4ss, uint, &rtw_rx_ampdu_sz_limit_4ss_num, 0644);
MODULE_PARM_DESC(rtw_rx_ampdu_sz_limit_4ss, "RX AMPDU size limit for 4SS link of each BW, 0xFF: no limitation");

/* Short GI support Bit Map
* BIT0 - 20MHz, 0: non-support, 1: support
* BIT1 - 40MHz, 0: non-support, 1: support
* BIT2 - 80MHz, 0: non-support, 1: support
* BIT3 - 160MHz, 0: non-support, 1: support */
int rtw_short_gi = 0x7;
/* BIT0: Enable VHT LDPC Rx, BIT1: Enable VHT LDPC Tx, BIT4: Enable HT LDPC Rx, BIT5: Enable HT LDPC Tx */
int rtw_ldpc_cap = 0x33;
/* BIT0: Enable VHT STBC Rx, BIT1: Enable VHT STBC Tx, BIT4: Enable HT STBC Rx, BIT5: Enable HT STBC Tx */
#ifdef CONFIG_RTL8192F
int rtw_stbc_cap = 0x30;
#else
int rtw_stbc_cap = 0x13;
#endif
module_param(rtw_stbc_cap, int, 0644);
/*
* BIT0: Enable VHT SU Beamformer
* BIT1: Enable VHT SU Beamformee
* BIT2: Enable VHT MU Beamformer, depend on VHT SU Beamformer
* BIT3: Enable VHT MU Beamformee, depend on VHT SU Beamformee
* BIT4: Enable HT Beamformer
* BIT5: Enable HT Beamformee
*/
int rtw_beamform_cap = BIT(1) | BIT(3);
int rtw_bfer_rf_number = 0; /*BeamformerCapRfNum Rf path number, 0 for auto, others for manual*/
int rtw_bfee_rf_number = 0; /*BeamformeeCapRfNum  Rf path number, 0 for auto, others for manual*/

#endif /* CONFIG_80211N_HT */

#ifdef CONFIG_80211AC_VHT
int rtw_vht_enable = 1; /* 0:disable, 1:enable, 2:force auto enable */
module_param(rtw_vht_enable, int, 0644);

int rtw_vht_24g_enable = 1; /* 0:disable, 1:enable */
module_param(rtw_vht_24g_enable, int, 0644);

int rtw_ampdu_factor = 7;

uint rtw_vht_rx_mcs_map = 0xaaaa;
module_param(rtw_vht_rx_mcs_map, uint, 0644);
MODULE_PARM_DESC(rtw_vht_rx_mcs_map, "VHT RX MCS map");
#endif /* CONFIG_80211AC_VHT */


/* 0: not check in watch dog, 1: check in watch dog  */
int rtw_check_hw_status = 0;

int rtw_low_power = 0;
int rtw_wifi_spec = 0;


int rtw_trx_path_bmp = 0x00;
module_param(rtw_trx_path_bmp, int, 0644); /* [7:4]TX path bmp, [0:3]RX path bmp, 0: not specified */

#ifdef CONFIG_SPECIAL_RF_PATH /* configure Nss/xTxR IC to 1ss/1T1R */
int rtw_tx_path_lmt = 1;
int rtw_rx_path_lmt = 1;
int rtw_tx_nss = 1;
int rtw_rx_nss = 1;
#elif defined(CONFIG_CUSTOMER01_SMART_ANTENNA)
int rtw_tx_path_lmt = 2;
int rtw_rx_path_lmt = 2;
int rtw_tx_nss = 1;
int rtw_rx_nss = 1;
#else
int rtw_tx_path_lmt = 0;
int rtw_rx_path_lmt = 0;
int rtw_tx_nss = 0;
int rtw_rx_nss = 0;
#endif
module_param(rtw_tx_path_lmt, int, 0644); /* limit of TX path number, 0: not specified */
module_param(rtw_rx_path_lmt, int, 0644); /* limit of RX path number, 0: not specified */
module_param(rtw_tx_nss, int, 0644);
module_param(rtw_rx_nss, int, 0644);

#ifdef CONFIG_REGD_SRC_FROM_OS
static uint rtw_regd_src = CONFIG_RTW_REGD_SRC;
module_param(rtw_regd_src, uint, 0644);
MODULE_PARM_DESC(rtw_regd_src, "The default regd source selection, 0:Realtek defined, 1: OS");
#endif

char rtw_country_unspecified[] = {0xFF, 0xFF, 0x00};
char *rtw_country_code = rtw_country_unspecified;
module_param(rtw_country_code, charp, 0644);
MODULE_PARM_DESC(rtw_country_code, "The default country code (in alpha2)");

int rtw_channel_plan = CONFIG_RTW_CHPLAN;
module_param(rtw_channel_plan, int, 0644);
MODULE_PARM_DESC(rtw_channel_plan, "The default chplan ID when rtw_alpha2 is not specified or valid");

static uint rtw_excl_chs[MAX_CHANNEL_NUM] = CONFIG_RTW_EXCL_CHS;
static int rtw_excl_chs_num = 0;
module_param_array(rtw_excl_chs, uint, &rtw_excl_chs_num, 0644);
MODULE_PARM_DESC(rtw_excl_chs, "exclusive channel array");

/*if concurrent softap + p2p(GO) is needed, this param lets p2p response full channel list.
But Softap must be SHUT DOWN once P2P decide to set up connection and become a GO.*/
#ifdef CONFIG_FULL_CH_IN_P2P_HANDSHAKE
	int rtw_full_ch_in_p2p_handshake = 1; /* reply full channel list*/
#else
	int rtw_full_ch_in_p2p_handshake = 0; /* reply only softap channel*/
#endif

#ifdef CONFIG_BT_COEXIST
int rtw_btcoex_enable = 2;
module_param(rtw_btcoex_enable, int, 0644);
MODULE_PARM_DESC(rtw_btcoex_enable, "BT co-existence on/off, 0:off, 1:on, 2:by efuse");

int rtw_ant_num = 0;
module_param(rtw_ant_num, int, 0644);
MODULE_PARM_DESC(rtw_ant_num, "Antenna number setting, 0:by efuse");

int rtw_bt_iso = 2;/* 0:Low, 1:High, 2:From Efuse */
int rtw_bt_sco = 3;/* 0:Idle, 1:None-SCO, 2:SCO, 3:From Counter, 4.Busy, 5.OtherBusy */
int rtw_bt_ampdu = 1 ; /* 0:Disable BT control A-MPDU, 1:Enable BT control A-MPDU. */
#endif /* CONFIG_BT_COEXIST */

int rtw_AcceptAddbaReq = _TRUE;/* 0:Reject AP's Add BA req, 1:Accept AP's Add BA req. */

int rtw_antdiv_cfg = 2; /* 0:OFF , 1:ON, 2:decide by Efuse config */
int rtw_antdiv_type = 0
	; /* 0:decide by efuse  1: for 88EE, 1Tx and 1RxCG are diversity.(2 Ant with SPDT), 2:  for 88EE, 1Tx and 2Rx are diversity.( 2 Ant, Tx and RxCG are both on aux port, RxCS is on main port ), 3: for 88EE, 1Tx and 1RxCG are fixed.(1Ant, Tx and RxCG are both on aux port) */

int rtw_drv_ant_band_switch = 1; /* 0:OFF , 1:ON, Driver control antenna band switch*/

int rtw_single_ant_path; /*0:main ant , 1:aux ant , Fixed single antenna path, default main ant*/

/* 0: doesn't switch, 1: switch from usb2.0 to usb 3.0 2: switch from usb3.0 to usb 2.0 */
int rtw_switch_usb_mode = 0;

#ifdef CONFIG_USB_AUTOSUSPEND
int rtw_enusbss = 1;/* 0:disable,1:enable */
#else
int rtw_enusbss = 0;/* 0:disable,1:enable */
#endif

int rtw_hwpdn_mode = 2; /* 0:disable,1:enable,2: by EFUSE config */

#ifdef CONFIG_HW_PWRP_DETECTION
int rtw_hwpwrp_detect = 1;
#else
int rtw_hwpwrp_detect = 0; /* HW power  ping detect 0:disable , 1:enable */
#endif

#ifdef CONFIG_USB_HCI
int rtw_hw_wps_pbc = 1;
#else
int rtw_hw_wps_pbc = 0;
#endif

#ifdef CONFIG_80211D
int rtw_80211d = 0;
#endif

#ifdef CONFIG_PCI_ASPM
/* CLK_REQ:BIT0 L0s:BIT1 ASPM_L1:BIT2 L1Off:BIT3*/
int	rtw_pci_aspm_enable = 0x5;
#else
int	rtw_pci_aspm_enable;
#endif

/*
 * BIT [15:12] mask of ps mode
 * BIT [11:8] val of ps mode
 * BIT [7:4] mask of perf mode
 * BIT [3:0] val of perf mode
 *
 * L0s:BIT[+0] L1:BIT[+1]
 *
 * 0x0030: change value only if perf mode
 * 0x3300: change value only if ps mode
 * 0x3330: change value in both perf and ps mode
 */
#ifdef CONFIG_PCI_DYNAMIC_ASPM
#ifdef CONFIG_PCI_ASPM
int rtw_pci_dynamic_aspm_linkctrl = 0x3330;
#else
int rtw_pci_dynamic_aspm_linkctrl = 0x0030;
#endif
#else
int rtw_pci_dynamic_aspm_linkctrl = 0x0000;
#endif
module_param(rtw_pci_dynamic_aspm_linkctrl, int, 0644);

#ifdef CONFIG_QOS_OPTIMIZATION
int rtw_qos_opt_enable = 1; /* 0: disable,1:enable */
#else
int rtw_qos_opt_enable = 0; /* 0: disable,1:enable */
#endif
module_param(rtw_qos_opt_enable, int, 0644);

#ifdef CONFIG_RTW_ACS
int rtw_acs_auto_scan = 0; /*0:disable, 1:enable*/
module_param(rtw_acs_auto_scan, int, 0644);

int rtw_acs = 1;
module_param(rtw_acs, int, 0644);
#endif

#ifdef CONFIG_BACKGROUND_NOISE_MONITOR
int rtw_nm = 1;/*noise monitor*/
module_param(rtw_nm, int, 0644);
#endif

char *ifname = "wlan%d";
module_param(ifname, charp, 0644);
MODULE_PARM_DESC(ifname, "The default name to allocate for first interface");

#ifdef CONFIG_PLATFORM_ANDROID
	char *if2name = "p2p%d";
#else /* CONFIG_PLATFORM_ANDROID */
	char *if2name = "wlan%d";
#endif /* CONFIG_PLATFORM_ANDROID */
module_param(if2name, charp, 0644);
MODULE_PARM_DESC(if2name, "The default name to allocate for second interface");

char *rtw_initmac = 0;  /* temp mac address if users want to use instead of the mac address in Efuse */

#ifdef CONFIG_CONCURRENT_MODE

	#if (CONFIG_IFACE_NUMBER > 2)
		int rtw_virtual_iface_num = CONFIG_IFACE_NUMBER - 1;
		module_param(rtw_virtual_iface_num, int, 0644);
	#else
		int rtw_virtual_iface_num = 1;
	#endif

#ifdef CONFIG_P2P

	#ifdef CONFIG_SEL_P2P_IFACE
	int rtw_sel_p2p_iface = CONFIG_SEL_P2P_IFACE;
	#else
	int rtw_sel_p2p_iface = IFACE_ID1;
	#endif

	module_param(rtw_sel_p2p_iface, int, 0644);

#endif

#endif

#ifdef CONFIG_AP_MODE
u8 rtw_bmc_tx_rate = MGN_UNKNOWN;

#if CONFIG_RTW_AP_DATA_BMC_TO_UC
int rtw_ap_src_b2u_flags = CONFIG_RTW_AP_SRC_B2U_FLAGS;
module_param(rtw_ap_src_b2u_flags, int, 0644);

int rtw_ap_fwd_b2u_flags = CONFIG_RTW_AP_FWD_B2U_FLAGS;
module_param(rtw_ap_fwd_b2u_flags, int, 0644);
#endif /* CONFIG_RTW_AP_DATA_BMC_TO_UC */
#endif /* CONFIG_AP_MODE */

#ifdef CONFIG_RTW_MESH
#if CONFIG_RTW_MESH_DATA_BMC_TO_UC
int rtw_msrc_b2u_flags = CONFIG_RTW_MSRC_B2U_FLAGS;
module_param(rtw_msrc_b2u_flags, int, 0644);

int rtw_mfwd_b2u_flags = CONFIG_RTW_MFWD_B2U_FLAGS;
module_param(rtw_mfwd_b2u_flags, int, 0644);
#endif /* CONFIG_RTW_MESH_DATA_BMC_TO_UC */
#endif /* CONFIG_RTW_MESH */

#ifdef RTW_WOW_STA_MIX
int rtw_wowlan_sta_mix_mode = 1;
#else
int rtw_wowlan_sta_mix_mode = 0;
#endif
module_param(rtw_wowlan_sta_mix_mode, int, 0644);
module_param(rtw_pwrtrim_enable, int, 0644);
module_param(rtw_initmac, charp, 0644);
module_param(rtw_chip_version, int, 0644);
module_param(rtw_rfintfs, int, 0644);
module_param(rtw_lbkmode, int, 0644);
module_param(rtw_network_mode, int, 0644);
module_param(rtw_channel, int, 0644);
module_param(rtw_mp_mode, int, 0644);
module_param(rtw_wmm_enable, int, 0644);
#ifdef CONFIG_WMMPS_STA
module_param(rtw_uapsd_max_sp, int, 0644);
module_param(rtw_uapsd_ac_enable, int, 0644);
module_param(rtw_wmm_smart_ps, int, 0644);
#endif /* CONFIG_WMMPS_STA */
module_param(rtw_vrtl_carrier_sense, int, 0644);
module_param(rtw_vcs_type, int, 0644);
module_param(rtw_busy_thresh, int, 0644);

#ifdef CONFIG_80211N_HT
module_param(rtw_ht_enable, int, 0644);
module_param(rtw_bw_mode, int, 0644);
module_param(rtw_ampdu_enable, int, 0644);
module_param(rtw_rx_stbc, int, 0644);
module_param(rtw_rx_ampdu_amsdu, int, 0644);
module_param(rtw_tx_ampdu_amsdu, int, 0644);
module_param(rtw_quick_addba_req, int, 0644);
#endif /* CONFIG_80211N_HT */

#ifdef CONFIG_BEAMFORMING
module_param(rtw_beamform_cap, int, 0644);
#endif

module_param(rtw_power_mgnt, int, 0644);
module_param(rtw_smart_ps, int, 0644);
module_param(rtw_low_power, int, 0644);
module_param(rtw_wifi_spec, int, 0644);

module_param(rtw_full_ch_in_p2p_handshake, int, 0644);
module_param(rtw_antdiv_cfg, int, 0644);
module_param(rtw_antdiv_type, int, 0644);

module_param(rtw_drv_ant_band_switch, int, 0644);
module_param(rtw_single_ant_path, int, 0644);

module_param(rtw_switch_usb_mode, int, 0644);

module_param(rtw_enusbss, int, 0644);
module_param(rtw_hwpdn_mode, int, 0644);
module_param(rtw_hwpwrp_detect, int, 0644);

module_param(rtw_hw_wps_pbc, int, 0644);
module_param(rtw_check_hw_status, int, 0644);

#ifdef CONFIG_PCI_HCI
module_param(rtw_pci_aspm_enable, int, 0644);
#endif

#ifdef CONFIG_TX_EARLY_MODE
module_param(rtw_early_mode, int, 0644);
#endif
#ifdef CONFIG_ADAPTOR_INFO_CACHING_FILE
char *rtw_adaptor_info_caching_file_path = "/data/misc/wifi/rtw_cache";
module_param(rtw_adaptor_info_caching_file_path, charp, 0644);
MODULE_PARM_DESC(rtw_adaptor_info_caching_file_path, "The path of adapter info cache file");
#endif /* CONFIG_ADAPTOR_INFO_CACHING_FILE */

#ifdef CONFIG_LAYER2_ROAMING
uint rtw_max_roaming_times = 2;
module_param(rtw_max_roaming_times, uint, 0644);
MODULE_PARM_DESC(rtw_max_roaming_times, "The max roaming times to try");
#endif /* CONFIG_LAYER2_ROAMING */

#ifdef CONFIG_IOL
int rtw_fw_iol = 1;
module_param(rtw_fw_iol, int, 0644);
MODULE_PARM_DESC(rtw_fw_iol, "FW IOL. 0:Disable, 1:enable, 2:by usb speed");
#endif /* CONFIG_IOL */

#ifdef CONFIG_FILE_FWIMG
char *rtw_fw_file_path = "/system/etc/firmware/rtlwifi/FW_NIC.BIN";
module_param(rtw_fw_file_path, charp, 0644);
MODULE_PARM_DESC(rtw_fw_file_path, "The path of fw image");

char *rtw_fw_wow_file_path = "/system/etc/firmware/rtlwifi/FW_WoWLAN.BIN";
module_param(rtw_fw_wow_file_path, charp, 0644);
MODULE_PARM_DESC(rtw_fw_wow_file_path, "The path of fw for Wake on Wireless image");

#ifdef CONFIG_MP_INCLUDED
char *rtw_fw_mp_bt_file_path = "";
module_param(rtw_fw_mp_bt_file_path, charp, 0644);
MODULE_PARM_DESC(rtw_fw_mp_bt_file_path, "The path of fw for MP-BT image");
#endif /* CONFIG_MP_INCLUDED */
#endif /* CONFIG_FILE_FWIMG */

#ifdef CONFIG_80211D
module_param(rtw_80211d, int, 0644);
MODULE_PARM_DESC(rtw_80211d, "Enable 802.11d mechanism");
#endif

#ifdef CONFIG_ADVANCE_OTA
/*	BIT(0): OTA continuous rotated test within low RSSI,1R CCA in path B
	BIT(1) & BIT(2): OTA continuous rotated test with low high RSSI */
/* Experimental environment: shielding room with half of absorber and 2~3 rotation per minute */
int rtw_advnace_ota;
module_param(rtw_advnace_ota, int, 0644);
#endif

uint rtw_notch_filter = RTW_NOTCH_FILTER;
module_param(rtw_notch_filter, uint, 0644);
MODULE_PARM_DESC(rtw_notch_filter, "0:Disable, 1:Enable, 2:Enable only for P2P");

uint rtw_hiq_filter = CONFIG_RTW_HIQ_FILTER;
module_param(rtw_hiq_filter, uint, 0644);
MODULE_PARM_DESC(rtw_hiq_filter, "0:allow all, 1:allow special, 2:deny all");

uint rtw_adaptivity_en = CONFIG_RTW_ADAPTIVITY_EN;
module_param(rtw_adaptivity_en, uint, 0644);
MODULE_PARM_DESC(rtw_adaptivity_en, "0:disable, 1:enable");

uint rtw_adaptivity_mode = CONFIG_RTW_ADAPTIVITY_MODE;
module_param(rtw_adaptivity_mode, uint, 0644);
MODULE_PARM_DESC(rtw_adaptivity_mode, "0:normal, 1:carrier sense");

int rtw_adaptivity_th_l2h_ini = CONFIG_RTW_ADAPTIVITY_TH_L2H_INI;
module_param(rtw_adaptivity_th_l2h_ini, int, 0644);
MODULE_PARM_DESC(rtw_adaptivity_th_l2h_ini, "th_l2h_ini for Adaptivity");

int rtw_adaptivity_th_edcca_hl_diff = CONFIG_RTW_ADAPTIVITY_TH_EDCCA_HL_DIFF;
module_param(rtw_adaptivity_th_edcca_hl_diff, int, 0644);
MODULE_PARM_DESC(rtw_adaptivity_th_edcca_hl_diff, "th_edcca_hl_diff for Adaptivity");

#ifdef CONFIG_DFS_MASTER
uint rtw_dfs_region_domain = CONFIG_RTW_DFS_REGION_DOMAIN;
module_param(rtw_dfs_region_domain, uint, 0644);
MODULE_PARM_DESC(rtw_dfs_region_domain, "0:NONE, 1:FCC, 2:MKK, 3:ETSI");
#endif

uint rtw_amplifier_type_2g = CONFIG_RTW_AMPLIFIER_TYPE_2G;
module_param(rtw_amplifier_type_2g, uint, 0644);
MODULE_PARM_DESC(rtw_amplifier_type_2g, "BIT3:2G ext-PA, BIT4:2G ext-LNA");

uint rtw_amplifier_type_5g = CONFIG_RTW_AMPLIFIER_TYPE_5G;
module_param(rtw_amplifier_type_5g, uint, 0644);
MODULE_PARM_DESC(rtw_amplifier_type_5g, "BIT6:5G ext-PA, BIT7:5G ext-LNA");

uint rtw_rfe_type = CONFIG_RTW_RFE_TYPE;
module_param(rtw_rfe_type, uint, 0644);
MODULE_PARM_DESC(rtw_rfe_type, "default init value:64");

uint rtw_powertracking_type = 64;
module_param(rtw_powertracking_type, uint, 0644);
MODULE_PARM_DESC(rtw_powertracking_type, "default init value:64");

uint rtw_GLNA_type = CONFIG_RTW_GLNA_TYPE;
module_param(rtw_GLNA_type, uint, 0644);
MODULE_PARM_DESC(rtw_GLNA_type, "default init value:0");

uint rtw_TxBBSwing_2G = 0xFF;
module_param(rtw_TxBBSwing_2G, uint, 0644);
MODULE_PARM_DESC(rtw_TxBBSwing_2G, "default init value:0xFF");

uint rtw_TxBBSwing_5G = 0xFF;
module_param(rtw_TxBBSwing_5G, uint, 0644);
MODULE_PARM_DESC(rtw_TxBBSwing_5G, "default init value:0xFF");

uint rtw_OffEfuseMask = 0;
module_param(rtw_OffEfuseMask, uint, 0644);
MODULE_PARM_DESC(rtw_OffEfuseMask, "default open Efuse Mask value:0");

uint rtw_FileMaskEfuse = 0;
module_param(rtw_FileMaskEfuse, uint, 0644);
MODULE_PARM_DESC(rtw_FileMaskEfuse, "default drv Mask Efuse value:0");

uint rtw_rxgain_offset_2g = 0;
module_param(rtw_rxgain_offset_2g, uint, 0644);
MODULE_PARM_DESC(rtw_rxgain_offset_2g, "default RF Gain 2G Offset value:0");

uint rtw_rxgain_offset_5gl = 0;
module_param(rtw_rxgain_offset_5gl, uint, 0644);
MODULE_PARM_DESC(rtw_rxgain_offset_5gl, "default RF Gain 5GL Offset value:0");

uint rtw_rxgain_offset_5gm = 0;
module_param(rtw_rxgain_offset_5gm, uint, 0644);
MODULE_PARM_DESC(rtw_rxgain_offset_5gm, "default RF Gain 5GM Offset value:0");

uint rtw_rxgain_offset_5gh = 0;
module_param(rtw_rxgain_offset_5gh, uint, 0644);
MODULE_PARM_DESC(rtw_rxgain_offset_5gm, "default RF Gain 5GL Offset value:0");

uint rtw_pll_ref_clk_sel = CONFIG_RTW_PLL_REF_CLK_SEL;
module_param(rtw_pll_ref_clk_sel, uint, 0644);
MODULE_PARM_DESC(rtw_pll_ref_clk_sel, "force pll_ref_clk_sel, 0xF:use autoload value");

int rtw_tx_pwr_by_rate = CONFIG_TXPWR_BY_RATE_EN;
module_param(rtw_tx_pwr_by_rate, int, 0644);
MODULE_PARM_DESC(rtw_tx_pwr_by_rate, "0:Disable, 1:Enable, 2: Depend on efuse");

#if CONFIG_TXPWR_LIMIT
int rtw_tx_pwr_lmt_enable = CONFIG_TXPWR_LIMIT_EN;
module_param(rtw_tx_pwr_lmt_enable, int, 0644);
MODULE_PARM_DESC(rtw_tx_pwr_lmt_enable, "0:Disable, 1:Enable, 2: Depend on efuse");
#endif

static int rtw_target_tx_pwr_2g_a[RATE_SECTION_NUM] = CONFIG_RTW_TARGET_TX_PWR_2G_A;
static int rtw_target_tx_pwr_2g_a_num = 0;
module_param_array(rtw_target_tx_pwr_2g_a, int, &rtw_target_tx_pwr_2g_a_num, 0644);
MODULE_PARM_DESC(rtw_target_tx_pwr_2g_a, "2.4G target tx power (unit:dBm) of RF path A for each rate section, should match the real calibrate power, -1: undefined");

static int rtw_target_tx_pwr_2g_b[RATE_SECTION_NUM] = CONFIG_RTW_TARGET_TX_PWR_2G_B;
static int rtw_target_tx_pwr_2g_b_num = 0;
module_param_array(rtw_target_tx_pwr_2g_b, int, &rtw_target_tx_pwr_2g_b_num, 0644);
MODULE_PARM_DESC(rtw_target_tx_pwr_2g_b, "2.4G target tx power (unit:dBm) of RF path B for each rate section, should match the real calibrate power, -1: undefined");

static int rtw_target_tx_pwr_2g_c[RATE_SECTION_NUM] = CONFIG_RTW_TARGET_TX_PWR_2G_C;
static int rtw_target_tx_pwr_2g_c_num = 0;
module_param_array(rtw_target_tx_pwr_2g_c, int, &rtw_target_tx_pwr_2g_c_num, 0644);
MODULE_PARM_DESC(rtw_target_tx_pwr_2g_c, "2.4G target tx power (unit:dBm) of RF path C for each rate section, should match the real calibrate power, -1: undefined");

static int rtw_target_tx_pwr_2g_d[RATE_SECTION_NUM] = CONFIG_RTW_TARGET_TX_PWR_2G_D;
static int rtw_target_tx_pwr_2g_d_num = 0;
module_param_array(rtw_target_tx_pwr_2g_d, int, &rtw_target_tx_pwr_2g_d_num, 0644);
MODULE_PARM_DESC(rtw_target_tx_pwr_2g_d, "2.4G target tx power (unit:dBm) of RF path D for each rate section, should match the real calibrate power, -1: undefined");

#if CONFIG_IEEE80211_BAND_5GHZ
static int rtw_target_tx_pwr_5g_a[RATE_SECTION_NUM - 1] = CONFIG_RTW_TARGET_TX_PWR_5G_A;
static int rtw_target_tx_pwr_5g_a_num = 0;
module_param_array(rtw_target_tx_pwr_5g_a, int, &rtw_target_tx_pwr_5g_a_num, 0644);
MODULE_PARM_DESC(rtw_target_tx_pwr_5g_a, "5G target tx power (unit:dBm) of RF path A for each rate section, should match the real calibrate power, -1: undefined");

static int rtw_target_tx_pwr_5g_b[RATE_SECTION_NUM - 1] = CONFIG_RTW_TARGET_TX_PWR_5G_B;
static int rtw_target_tx_pwr_5g_b_num = 0;
module_param_array(rtw_target_tx_pwr_5g_b, int, &rtw_target_tx_pwr_5g_b_num, 0644);
MODULE_PARM_DESC(rtw_target_tx_pwr_5g_b, "5G target tx power (unit:dBm) of RF path B for each rate section, should match the real calibrate power, -1: undefined");

static int rtw_target_tx_pwr_5g_c[RATE_SECTION_NUM - 1] = CONFIG_RTW_TARGET_TX_PWR_5G_C;
static int rtw_target_tx_pwr_5g_c_num = 0;
module_param_array(rtw_target_tx_pwr_5g_c, int, &rtw_target_tx_pwr_5g_c_num, 0644);
MODULE_PARM_DESC(rtw_target_tx_pwr_5g_c, "5G target tx power (unit:dBm) of RF path C for each rate section, should match the real calibrate power, -1: undefined");

static int rtw_target_tx_pwr_5g_d[RATE_SECTION_NUM - 1] = CONFIG_RTW_TARGET_TX_PWR_5G_D;
static int rtw_target_tx_pwr_5g_d_num = 0;
module_param_array(rtw_target_tx_pwr_5g_d, int, &rtw_target_tx_pwr_5g_d_num, 0644);
MODULE_PARM_DESC(rtw_target_tx_pwr_5g_d, "5G target tx power (unit:dBm) of RF path D for each rate section, should match the real calibrate power, -1: undefined");
#endif /* CONFIG_IEEE80211_BAND_5GHZ */

int rtw_antenna_gain = CONFIG_RTW_ANTENNA_GAIN;
module_param(rtw_antenna_gain, int, 0644);
MODULE_PARM_DESC(rtw_antenna_gain, "Antenna gain in mBi. 0x7FFF: unspecifed");

#ifdef CONFIG_RTW_TX_NPATH_EN
/*0:disable ,1: 2path*/
int rtw_tx_npath_enable = 1;
module_param(rtw_tx_npath_enable, int, 0644);
MODULE_PARM_DESC(rtw_tx_npath_enable, "0:Disable, 1:TX-2PATH");
#endif

int rtw_tsf_update_pause_factor = CONFIG_TSF_UPDATE_PAUSE_FACTOR;
module_param(rtw_tsf_update_pause_factor, int, 0644);
MODULE_PARM_DESC(rtw_tsf_update_pause_factor, "num of bcn intervals to stay TSF update pause status");

int rtw_tsf_update_restore_factor = CONFIG_TSF_UPDATE_RESTORE_FACTOR;
module_param(rtw_tsf_update_restore_factor, int, 0644);
MODULE_PARM_DESC(rtw_tsf_update_restore_factor, "num of bcn intervals to stay TSF update restore status");

#ifdef CONFIG_LOAD_PHY_PARA_FROM_FILE
char *rtw_phy_file_path = REALTEK_CONFIG_PATH;
module_param(rtw_phy_file_path, charp, 0644);
MODULE_PARM_DESC(rtw_phy_file_path, "The path of phy parameter");
/* PHY FILE Bit Map
* BIT0 - MAC,				0: non-support, 1: support
* BIT1 - BB,					0: non-support, 1: support
* BIT2 - BB_PG,				0: non-support, 1: support
* BIT3 - BB_MP,				0: non-support, 1: support
* BIT4 - RF,					0: non-support, 1: support
* BIT5 - RF_TXPWR_TRACK,	0: non-support, 1: support
* BIT6 - RF_TXPWR_LMT,		0: non-support, 1: support */
int rtw_load_phy_file = (BIT2 | BIT6);
module_param(rtw_load_phy_file, int, 0644);
MODULE_PARM_DESC(rtw_load_phy_file, "PHY File Bit Map");
int rtw_decrypt_phy_file = 0;
module_param(rtw_decrypt_phy_file, int, 0644);
MODULE_PARM_DESC(rtw_decrypt_phy_file, "Enable Decrypt PHY File");
#endif

uint rtw_recvbuf_nr = NR_RECVBUFF;
module_param(rtw_recvbuf_nr, int, 0644);
MODULE_PARM_DESC(rtw_recvbuf_nr, "Preallocated number of struct recv_buf");

#ifdef CONFIG_SUPPORT_TRX_SHARED
#ifdef DFT_TRX_SHARE_MODE
int rtw_trx_share_mode = DFT_TRX_SHARE_MODE;
#else
int rtw_trx_share_mode = 0;
#endif
module_param(rtw_trx_share_mode, int, 0644);
MODULE_PARM_DESC(rtw_trx_share_mode, "TRx FIFO Shared");
#endif

#ifdef CONFIG_DYNAMIC_SOML
uint rtw_dynamic_soml_en = 1;
module_param(rtw_dynamic_soml_en, int, 0644);
MODULE_PARM_DESC(rtw_dynamic_soml_en, "0: disable, 1: enable with default param, 2: enable with specified param.");

uint rtw_dynamic_soml_train_num = 0;
module_param(rtw_dynamic_soml_train_num, int, 0644);
MODULE_PARM_DESC(rtw_dynamic_soml_train_num, "SOML training number");

uint rtw_dynamic_soml_interval = 0;
module_param(rtw_dynamic_soml_interval, int, 0644);
MODULE_PARM_DESC(rtw_dynamic_soml_interval, "SOML training interval");

uint rtw_dynamic_soml_period = 0;
module_param(rtw_dynamic_soml_period, int, 0644);
MODULE_PARM_DESC(rtw_dynamic_soml_period, "SOML training period");

uint rtw_dynamic_soml_delay = 0;
module_param(rtw_dynamic_soml_delay, int, 0644);
MODULE_PARM_DESC(rtw_dynamic_soml_delay, "SOML training delay");
#endif

uint rtw_phydm_ability = 0xffffffff;
module_param(rtw_phydm_ability, uint, 0644);

uint rtw_halrf_ability = 0xffffffff;
module_param(rtw_halrf_ability, uint, 0644);

#ifdef CONFIG_RTW_MESH
uint rtw_peer_alive_based_preq = 1;
module_param(rtw_peer_alive_based_preq, uint, 0644);
MODULE_PARM_DESC(rtw_peer_alive_based_preq,
	"On demand PREQ will reference peer alive status. 0: Off, 1: On");
#endif

#ifdef CONFIG_PLATFORM_INTEL_BYT
extern int rtw_sdio_set_power(int on);
#endif /* CONFIG_PLATFORM_INTEL_BYT */

#ifdef CONFIG_MCC_MODE
/* enable MCC mode or not */
int rtw_en_mcc = 1;
/* can referece following value before insmod driver */
int rtw_mcc_ap_bw20_target_tx_tp = MCC_AP_BW20_TARGET_TX_TP;
int rtw_mcc_ap_bw40_target_tx_tp = MCC_AP_BW40_TARGET_TX_TP;
int rtw_mcc_ap_bw80_target_tx_tp = MCC_AP_BW80_TARGET_TX_TP;
int rtw_mcc_sta_bw20_target_tx_tp = MCC_STA_BW20_TARGET_TX_TP;
int rtw_mcc_sta_bw40_target_tx_tp = MCC_STA_BW40_TARGET_TX_TP;
int rtw_mcc_sta_bw80_target_tx_tp = MCC_STA_BW80_TARGET_TX_TP;
int rtw_mcc_single_tx_cri = MCC_SINGLE_TX_CRITERIA;
int rtw_mcc_policy_table_idx = 0;
int rtw_mcc_duration = 0;
int rtw_mcc_enable_runtime_duration = 1;
#ifdef CONFIG_MCC_PHYDM_OFFLOAD
int rtw_mcc_phydm_offload = 1;
#else
int rtw_mcc_phydm_offload = 0;
#endif
module_param(rtw_en_mcc, int, 0644);
module_param(rtw_mcc_single_tx_cri, int, 0644);
module_param(rtw_mcc_ap_bw20_target_tx_tp, int, 0644);
module_param(rtw_mcc_ap_bw40_target_tx_tp, int, 0644);
module_param(rtw_mcc_ap_bw80_target_tx_tp, int, 0644);
module_param(rtw_mcc_sta_bw20_target_tx_tp, int, 0644);
module_param(rtw_mcc_sta_bw40_target_tx_tp, int, 0644);
module_param(rtw_mcc_sta_bw80_target_tx_tp, int, 0644);
module_param(rtw_mcc_policy_table_idx, int, 0644);
module_param(rtw_mcc_duration, int, 0644);
module_param(rtw_mcc_phydm_offload, int, 0644);
#endif /*CONFIG_MCC_MODE */

#ifdef CONFIG_RTW_NAPI
/*following setting should define NAPI in Makefile
enable napi only = 1, disable napi = 0*/
int rtw_en_napi = 1;
module_param(rtw_en_napi, int, 0644);
#ifdef CONFIG_RTW_NAPI_DYNAMIC
int rtw_napi_threshold = 100; /* unit: Mbps */
module_param(rtw_napi_threshold, int, 0644);
#endif /* CONFIG_RTW_NAPI_DYNAMIC */
#ifdef CONFIG_RTW_GRO
/*following setting should define GRO in Makefile
enable gro = 1, disable gro = 0*/
int rtw_en_gro = 1;
module_param(rtw_en_gro, int, 0644);
#endif /* CONFIG_RTW_GRO */
#endif /* CONFIG_RTW_NAPI */

#ifdef RTW_IQK_FW_OFFLOAD
int rtw_iqk_fw_offload = 1;
#else
int rtw_iqk_fw_offload;
#endif /* RTW_IQK_FW_OFFLOAD */
module_param(rtw_iqk_fw_offload, int, 0644);

#ifdef RTW_CHANNEL_SWITCH_OFFLOAD
int rtw_ch_switch_offload = 0;
#else
int rtw_ch_switch_offload;
#endif /* RTW_CHANNEL_SWITCH_OFFLOAD */
module_param(rtw_ch_switch_offload, int, 0644);

#ifdef CONFIG_TDLS
int rtw_en_tdls = 1;
module_param(rtw_en_tdls, int, 0644);
#endif

#ifdef CONFIG_TDMADIG
int rtw_tdmadig_en = 1;
/*
1:MODE_PERFORMANCE
2:MODE_COVERAGE
*/
int rtw_tdmadig_mode = 1;
int rtw_dynamic_tdmadig = 0;
module_param(rtw_tdmadig_en, int, 0644);
module_param(rtw_tdmadig_mode, int, 0644);
module_param(rtw_dynamic_tdmadig, int, 0644);
#endif/*CONFIG_TDMADIG*/

/*dynamic RRSR default enable*/
int rtw_en_dyn_rrsr = 1;
int rtw_rrsr_value = 0xFFFFFFFF;
module_param(rtw_en_dyn_rrsr, int, 0644);
module_param(rtw_rrsr_value, int, 0644);

#ifdef CONFIG_WOWLAN
/*
 * 0: disable, 1: enable
 */
uint rtw_wow_enable = 1;
module_param(rtw_wow_enable, uint, 0644);
/*
 * bit[0]: magic packet wake up
 * bit[1]: unucast packet(HW/FW unuicast)
 * bit[2]: deauth wake up
 */
uint rtw_wakeup_event = RTW_WAKEUP_EVENT;
module_param(rtw_wakeup_event, uint, 0644);
/*
 * 0: common WOWLAN
 * bit[0]: disable BB RF
 * bit[1]: For wireless remote controller with or without connection
 */
uint rtw_suspend_type = RTW_SUSPEND_TYPE;
module_param(rtw_suspend_type, uint, 0644);
#endif

#ifdef RTW_BUSY_DENY_SCAN
uint rtw_scan_interval_thr = BUSY_TRAFFIC_SCAN_DENY_PERIOD;
module_param(rtw_scan_interval_thr, uint, 0644);
MODULE_PARM_DESC(rtw_scan_interval_thr, "Threshold used to judge if scan " \
		 "request comes from scan UI, unit is ms.");
#endif /* RTW_BUSY_DENY_SCAN */

#ifdef CONFIG_RTL8822C_XCAP_NEW_POLICY
uint rtw_8822c_xcap_overwrite = 1;
module_param(rtw_8822c_xcap_overwrite, uint, 0644);
#endif

#ifdef CONFIG_RTW_MULTI_AP
static int rtw_unassoc_sta_mode_of_stype[UNASOC_STA_SRC_NUM] = CONFIG_RTW_UNASOC_STA_MODE_OF_STYPE;
static int rtw_unassoc_sta_mode_of_stype_num = 0;
module_param_array(rtw_unassoc_sta_mode_of_stype, int, &rtw_unassoc_sta_mode_of_stype_num, 0644);

uint rtw_max_unassoc_sta_cnt = 0;
module_param(rtw_max_unassoc_sta_cnt, uint, 0644);
#endif

#if CONFIG_TX_AC_LIFETIME
static void rtw_regsty_load_tx_ac_lifetime(struct registry_priv *regsty)
{
	int i, j;
	struct tx_aclt_conf_t *conf;
	uint *parm;

	regsty->tx_aclt_flags = (u8)rtw_tx_aclt_flags;

	for (i = 0; i < TX_ACLT_CONF_NUM; i++) {
		conf = &regsty->tx_aclt_confs[i];
		if (i == TX_ACLT_CONF_DEFAULT)
			parm = rtw_tx_aclt_conf_default;
		#ifdef CONFIG_AP_MODE
		#if CONFIG_RTW_AP_DATA_BMC_TO_UC
		else if (i == TX_ACLT_CONF_AP_M2U)
			parm = rtw_tx_aclt_conf_ap_m2u;
		#endif
		#endif /* CONFIG_AP_MODE */
		#ifdef CONFIG_RTW_MESH
		else if (i == TX_ACLT_CONF_MESH)
			parm = rtw_tx_aclt_conf_mesh;
		#endif
		else
			parm = NULL;

		if (parm) {
			conf->en = parm[0] & 0xF;
			conf->vo_vi = parm[1];
			conf->be_bk = parm[2];
		}	
	}
}
#endif

void rtw_regsty_load_target_tx_power(struct registry_priv *regsty)
{
	int path, rs;
	int *target_tx_pwr;

	for (path = RF_PATH_A; path < RF_PATH_MAX; path++) {
		if (path == RF_PATH_A)
			target_tx_pwr = rtw_target_tx_pwr_2g_a;
		else if (path == RF_PATH_B)
			target_tx_pwr = rtw_target_tx_pwr_2g_b;
		else if (path == RF_PATH_C)
			target_tx_pwr = rtw_target_tx_pwr_2g_c;
		else if (path == RF_PATH_D)
			target_tx_pwr = rtw_target_tx_pwr_2g_d;

		for (rs = CCK; rs < RATE_SECTION_NUM; rs++)
			regsty->target_tx_pwr_2g[path][rs] = target_tx_pwr[rs];
	}

#if CONFIG_IEEE80211_BAND_5GHZ
	for (path = RF_PATH_A; path < RF_PATH_MAX; path++) {
		if (path == RF_PATH_A)
			target_tx_pwr = rtw_target_tx_pwr_5g_a;
		else if (path == RF_PATH_B)
			target_tx_pwr = rtw_target_tx_pwr_5g_b;
		else if (path == RF_PATH_C)
			target_tx_pwr = rtw_target_tx_pwr_5g_c;
		else if (path == RF_PATH_D)
			target_tx_pwr = rtw_target_tx_pwr_5g_d;

		for (rs = OFDM; rs < RATE_SECTION_NUM; rs++)
			regsty->target_tx_pwr_5g[path][rs - 1] = target_tx_pwr[rs - 1];
	}
#endif /* CONFIG_IEEE80211_BAND_5GHZ */
}

inline void rtw_regsty_load_excl_chs(struct registry_priv *regsty)
{
	int i;
	int ch_num = 0;

	for (i = 0; i < MAX_CHANNEL_NUM; i++)
		if (((u8)rtw_excl_chs[i]) != 0)
			regsty->excl_chs[ch_num++] = (u8)rtw_excl_chs[i];

	if (ch_num < MAX_CHANNEL_NUM)
		regsty->excl_chs[ch_num] = 0;
}

#ifdef CONFIG_80211N_HT
inline void rtw_regsty_init_rx_ampdu_sz_limit(struct registry_priv *regsty)
{
	int i, j;
	uint *sz_limit;

	for (i = 0; i < 4; i++) {
		if (i == 0)
			sz_limit = rtw_rx_ampdu_sz_limit_1ss;
		else if (i == 1)
			sz_limit = rtw_rx_ampdu_sz_limit_2ss;
		else if (i == 2)
			sz_limit = rtw_rx_ampdu_sz_limit_3ss;
		else if (i == 3)
			sz_limit = rtw_rx_ampdu_sz_limit_4ss;

		for (j = 0; j < 4; j++)
			regsty->rx_ampdu_sz_limit_by_nss_bw[i][j] = sz_limit[j];
	}
}
#endif /* CONFIG_80211N_HT */

#ifdef CONFIG_RTW_MULTI_AP
inline void rtw_regsty_init_unassoc_sta_param(struct registry_priv *regsty)
{
	int i;

	for (i = 0; i < UNASOC_STA_SRC_NUM; i++)
		regsty->unassoc_sta_mode_of_stype[i] = rtw_unassoc_sta_mode_of_stype[i];

	regsty->max_unassoc_sta_cnt = (u16) rtw_max_unassoc_sta_cnt;
}
#endif

uint rtw_load_registry(_adapter *padapter)
{
	uint status = _SUCCESS;
	struct registry_priv  *registry_par = &padapter->registrypriv;


#ifdef CONFIG_RTW_DEBUG
	if (rtw_drv_log_level >= _DRV_MAX_)
		rtw_drv_log_level = _DRV_DEBUG_;
#endif

	registry_par->chip_version = (u8)rtw_chip_version;
	registry_par->rfintfs = (u8)rtw_rfintfs;
	registry_par->lbkmode = (u8)rtw_lbkmode;
	/* registry_par->hci = (u8)hci; */
	registry_par->network_mode  = (u8)rtw_network_mode;

	_rtw_memcpy(registry_par->ssid.Ssid, "ANY", 3);
	registry_par->ssid.SsidLength = 3;

	registry_par->channel = (u8)rtw_channel;
#ifdef CONFIG_NARROWBAND_SUPPORTING
	if (rtw_nb_config != RTW_NB_CONFIG_NONE)
		rtw_wireless_mode &= ~WIRELESS_11B;
#endif
	registry_par->wireless_mode = (u8)rtw_wireless_mode;
	registry_par->band_type = (u8)rtw_band_type;

	if (is_supported_24g(registry_par->wireless_mode) && (!is_supported_5g(registry_par->wireless_mode))
	    && (registry_par->channel > 14))
		registry_par->channel = 1;
	else if (is_supported_5g(registry_par->wireless_mode) && (!is_supported_24g(registry_par->wireless_mode))
		 && (registry_par->channel <= 14))
		registry_par->channel = 36;

	registry_par->vrtl_carrier_sense = (u8)rtw_vrtl_carrier_sense ;
	registry_par->vcs_type = (u8)rtw_vcs_type;
	registry_par->rts_thresh = (u16)rtw_rts_thresh;
	registry_par->frag_thresh = (u16)rtw_frag_thresh;
	registry_par->preamble = (u8)rtw_preamble;
	registry_par->scan_mode = (u8)rtw_scan_mode;
	registry_par->smart_ps = (u8)rtw_smart_ps;
	registry_par->check_fw_ps = (u8)rtw_check_fw_ps;
	#ifdef CONFIG_TDMADIG
		registry_par->tdmadig_en = (u8)rtw_tdmadig_en;
		registry_par->tdmadig_mode = (u8)rtw_tdmadig_mode;
		registry_par->tdmadig_dynamic = (u8) rtw_dynamic_tdmadig;
		registry_par->power_mgnt = PM_PS_MODE_ACTIVE;
		registry_par->ips_mode = IPS_NONE;
	#else
		registry_par->power_mgnt = (u8)rtw_power_mgnt;
		registry_par->ips_mode = (u8)rtw_ips_mode;
	#endif/*CONFIG_TDMADIG*/
	registry_par->lps_level = (u8)rtw_lps_level;
	registry_par->en_dyn_rrsr = (u8)rtw_en_dyn_rrsr;
	registry_par->set_rrsr_value = (u32)rtw_rrsr_value;
#ifdef CONFIG_LPS_1T1R
	registry_par->lps_1t1r = (u8)(rtw_lps_1t1r ? 1 : 0);
#endif
	registry_par->lps_chk_by_tp = (u8)rtw_lps_chk_by_tp;
#ifdef CONFIG_WOWLAN
	registry_par->wow_power_mgnt = (u8)rtw_wow_power_mgnt;
	registry_par->wow_lps_level = (u8)rtw_wow_lps_level;
	#ifdef CONFIG_LPS_1T1R
	registry_par->wow_lps_1t1r = (u8)(rtw_wow_lps_1t1r ? 1 : 0);
	#endif
#endif /* CONFIG_WOWLAN */
	registry_par->radio_enable = (u8)rtw_radio_enable;
	registry_par->long_retry_lmt = (u8)rtw_long_retry_lmt;
	registry_par->short_retry_lmt = (u8)rtw_short_retry_lmt;
	registry_par->busy_thresh = (u16)rtw_busy_thresh;
	registry_par->max_bss_cnt = (u16)rtw_max_bss_cnt;
	/* registry_par->qos_enable = (u8)rtw_qos_enable; */
	registry_par->ack_policy = (u8)rtw_ack_policy;
	registry_par->mp_mode = (u8)rtw_mp_mode;
#if defined(CONFIG_MP_INCLUDED) && defined(CONFIG_RTW_CUSTOMER_STR)
	registry_par->mp_customer_str = (u8)rtw_mp_customer_str;
#endif
	registry_par->software_encrypt = (u8)rtw_software_encrypt;
	registry_par->software_decrypt = (u8)rtw_software_decrypt;

	registry_par->acm_method = (u8)rtw_acm_method;
	registry_par->usb_rxagg_mode = (u8)rtw_usb_rxagg_mode;
	registry_par->dynamic_agg_enable = (u8)rtw_dynamic_agg_enable;

	/* WMM */
	registry_par->wmm_enable = (u8)rtw_wmm_enable;

#ifdef CONFIG_WMMPS_STA
	/* UAPSD */
	registry_par->uapsd_max_sp_len= (u8)rtw_uapsd_max_sp;
	registry_par->uapsd_ac_enable = (u8)rtw_uapsd_ac_enable;
	registry_par->wmm_smart_ps = (u8)rtw_wmm_smart_ps;
#endif /* CONFIG_WMMPS_STA */

	registry_par->RegPwrTrimEnable = (u8)rtw_pwrtrim_enable;

#if CONFIG_TX_AC_LIFETIME
	rtw_regsty_load_tx_ac_lifetime(registry_par);
#endif

	registry_par->tx_bw_mode = (u8)rtw_tx_bw_mode;

#ifdef CONFIG_80211N_HT
	registry_par->ht_enable = (u8)rtw_ht_enable;
	if (registry_par->ht_enable && is_supported_ht(registry_par->wireless_mode)) {
#ifdef CONFIG_NARROWBAND_SUPPORTING
	if (rtw_nb_config != RTW_NB_CONFIG_NONE)
		rtw_bw_mode = 0;
#endif
		registry_par->bw_mode = (u8)rtw_bw_mode;
		registry_par->ampdu_enable = (u8)rtw_ampdu_enable;
		registry_par->rx_stbc = (u8)rtw_rx_stbc;
		registry_par->rx_ampdu_amsdu = (u8)rtw_rx_ampdu_amsdu;
		registry_par->tx_ampdu_amsdu = (u8)rtw_tx_ampdu_amsdu;
		registry_par->tx_quick_addba_req = (u8)rtw_quick_addba_req;
		registry_par->short_gi = (u8)rtw_short_gi;
		registry_par->ldpc_cap = (u8)rtw_ldpc_cap;
#if defined(CONFIG_CUSTOMER01_SMART_ANTENNA)
		rtw_stbc_cap = 0x0;
#endif
#ifdef CONFIG_RTW_TX_NPATH_EN
		registry_par->tx_npath = (u8)rtw_tx_npath_enable;
#endif
		registry_par->stbc_cap = (u8)rtw_stbc_cap;
		registry_par->beamform_cap = (u8)rtw_beamform_cap;
		registry_par->beamformer_rf_num = (u8)rtw_bfer_rf_number;
		registry_par->beamformee_rf_num = (u8)rtw_bfee_rf_number;
		rtw_regsty_init_rx_ampdu_sz_limit(registry_par);
	}
#endif
#ifdef DBG_LA_MODE
	registry_par->la_mode_en = (u8)rtw_la_mode_en;
#endif
#ifdef CONFIG_NARROWBAND_SUPPORTING
	registry_par->rtw_nb_config = (u8)rtw_nb_config;
#endif

#ifdef CONFIG_80211AC_VHT
	registry_par->vht_enable = (u8)rtw_vht_enable;
	registry_par->vht_24g_enable = (u8)rtw_vht_24g_enable;
	registry_par->ampdu_factor = (u8)rtw_ampdu_factor;
	registry_par->vht_rx_mcs_map[0] = (u8)(rtw_vht_rx_mcs_map & 0xFF);
	registry_par->vht_rx_mcs_map[1] = (u8)((rtw_vht_rx_mcs_map & 0xFF00) >> 8);
#endif

#ifdef CONFIG_TX_EARLY_MODE
	registry_par->early_mode = (u8)rtw_early_mode;
#endif
	registry_par->trx_path_bmp = (u8)rtw_trx_path_bmp;
	registry_par->tx_path_lmt = (u8)rtw_tx_path_lmt;
	registry_par->rx_path_lmt = (u8)rtw_rx_path_lmt;
	registry_par->tx_nss = (u8)rtw_tx_nss;
	registry_par->rx_nss = (u8)rtw_rx_nss;
	registry_par->low_power = (u8)rtw_low_power;

	registry_par->check_hw_status = (u8)rtw_check_hw_status;

	registry_par->wifi_spec = (u8)rtw_wifi_spec;

#ifdef CONFIG_REGD_SRC_FROM_OS
	if (regd_src_is_valid(rtw_regd_src))
		registry_par->regd_src = (u8)rtw_regd_src;
	else {
		RTW_WARN("%s invalid rtw_regd_src(%u), use REGD_SRC_RTK_PRIV instead\n", __func__, rtw_regd_src);
		registry_par->regd_src = REGD_SRC_RTK_PRIV;
	}
#endif

	if (strlen(rtw_country_code) != 2
		|| is_alpha(rtw_country_code[0]) == _FALSE
		|| is_alpha(rtw_country_code[1]) == _FALSE
	) {
		if (rtw_country_code != rtw_country_unspecified)
			RTW_ERR("%s discard rtw_country_code not in alpha2\n", __func__);
		_rtw_memset(registry_par->alpha2, 0xFF, 2);
	} else
		_rtw_memcpy(registry_par->alpha2, rtw_country_code, 2);

	registry_par->channel_plan = (u8)rtw_channel_plan;
	rtw_regsty_load_excl_chs(registry_par);

	registry_par->full_ch_in_p2p_handshake = (u8)rtw_full_ch_in_p2p_handshake;
#ifdef CONFIG_BT_COEXIST
	registry_par->btcoex = (u8)rtw_btcoex_enable;
	registry_par->bt_iso = (u8)rtw_bt_iso;
	registry_par->bt_sco = (u8)rtw_bt_sco;
	registry_par->bt_ampdu = (u8)rtw_bt_ampdu;
	registry_par->ant_num = (u8)rtw_ant_num;
	registry_par->single_ant_path = (u8) rtw_single_ant_path;
#endif

	registry_par->bAcceptAddbaReq = (u8)rtw_AcceptAddbaReq;

	registry_par->antdiv_cfg = (u8)rtw_antdiv_cfg;
	registry_par->antdiv_type = (u8)rtw_antdiv_type;

	registry_par->drv_ant_band_switch = (u8) rtw_drv_ant_band_switch;

	registry_par->switch_usb_mode = (u8)rtw_switch_usb_mode;
#ifdef SUPPORT_HW_RFOFF_DETECTED
	registry_par->hwpdn_mode = (u8)rtw_hwpdn_mode;/* 0:disable,1:enable,2:by EFUSE config */
	registry_par->hwpwrp_detect = (u8)rtw_hwpwrp_detect;/* 0:disable,1:enable */
#endif

	registry_par->hw_wps_pbc = (u8)rtw_hw_wps_pbc;

#ifdef CONFIG_ADAPTOR_INFO_CACHING_FILE
	snprintf(registry_par->adaptor_info_caching_file_path, PATH_LENGTH_MAX, "%s", rtw_adaptor_info_caching_file_path);
	registry_par->adaptor_info_caching_file_path[PATH_LENGTH_MAX - 1] = 0;
#endif

#ifdef CONFIG_LAYER2_ROAMING
	registry_par->max_roaming_times = (u8)rtw_max_roaming_times;
#endif

#ifdef CONFIG_IOL
	registry_par->fw_iol = rtw_fw_iol;
#endif

#ifdef CONFIG_80211D
	registry_par->enable80211d = (u8)rtw_80211d;
#endif

	snprintf(registry_par->ifname, 16, "%s", ifname);
	snprintf(registry_par->if2name, 16, "%s", if2name);

	registry_par->notch_filter = (u8)rtw_notch_filter;

#ifdef CONFIG_CONCURRENT_MODE
	registry_par->virtual_iface_num = (u8)rtw_virtual_iface_num;
#ifdef CONFIG_P2P
	registry_par->sel_p2p_iface = (u8)rtw_sel_p2p_iface;
	RTW_INFO("%s, Select P2P interface: iface_id:%d\n", __func__, registry_par->sel_p2p_iface);
#endif
#endif
	registry_par->pll_ref_clk_sel = (u8)rtw_pll_ref_clk_sel;

#if CONFIG_TXPWR_LIMIT
	registry_par->RegEnableTxPowerLimit = (u8)rtw_tx_pwr_lmt_enable;
#endif
	registry_par->RegEnableTxPowerByRate = (u8)rtw_tx_pwr_by_rate;

	rtw_regsty_load_target_tx_power(registry_par);

	registry_par->antenna_gain = (s16)rtw_antenna_gain;

	registry_par->tsf_update_pause_factor = (u8)rtw_tsf_update_pause_factor;
	registry_par->tsf_update_restore_factor = (u8)rtw_tsf_update_restore_factor;

	registry_par->TxBBSwing_2G = (s8)rtw_TxBBSwing_2G;
	registry_par->TxBBSwing_5G = (s8)rtw_TxBBSwing_5G;
	registry_par->bEn_RFE = 1;
	registry_par->RFE_Type = (u8)rtw_rfe_type;
	registry_par->PowerTracking_Type = (u8)rtw_powertracking_type;
	registry_par->AmplifierType_2G = (u8)rtw_amplifier_type_2g;
	registry_par->AmplifierType_5G = (u8)rtw_amplifier_type_5g;
	registry_par->GLNA_Type = (u8)rtw_GLNA_type;
#ifdef CONFIG_LOAD_PHY_PARA_FROM_FILE
	registry_par->load_phy_file = (u8)rtw_load_phy_file;
	registry_par->RegDecryptCustomFile = (u8)rtw_decrypt_phy_file;
#endif
	registry_par->qos_opt_enable = (u8)rtw_qos_opt_enable;

	registry_par->hiq_filter = (u8)rtw_hiq_filter;

	registry_par->adaptivity_en = (u8)rtw_adaptivity_en;
	registry_par->adaptivity_mode = (u8)rtw_adaptivity_mode;
	registry_par->adaptivity_th_l2h_ini = (s8)rtw_adaptivity_th_l2h_ini;
	registry_par->adaptivity_th_edcca_hl_diff = (s8)rtw_adaptivity_th_edcca_hl_diff;

#ifdef CONFIG_DYNAMIC_SOML
	registry_par->dyn_soml_en = (u8)rtw_dynamic_soml_en;
	registry_par->dyn_soml_train_num = (u8)rtw_dynamic_soml_train_num;
	registry_par->dyn_soml_interval = (u8)rtw_dynamic_soml_interval;
	registry_par->dyn_soml_period = (u8)rtw_dynamic_soml_period;
	registry_par->dyn_soml_delay = (u8)rtw_dynamic_soml_delay;
#endif

	registry_par->boffefusemask = (u8)rtw_OffEfuseMask;
	registry_par->bFileMaskEfuse = (u8)rtw_FileMaskEfuse;
	registry_par->bBTFileMaskEfuse = (u8)rtw_FileMaskEfuse;

#ifdef CONFIG_RTW_ACS
	registry_par->acs_mode = (u8)rtw_acs;
	registry_par->acs_auto_scan = (u8)rtw_acs_auto_scan;
#endif
#ifdef CONFIG_BACKGROUND_NOISE_MONITOR
	registry_par->nm_mode = (u8)rtw_nm;
#endif
	registry_par->reg_rxgain_offset_2g = (u32) rtw_rxgain_offset_2g;
	registry_par->reg_rxgain_offset_5gl = (u32) rtw_rxgain_offset_5gl;
	registry_par->reg_rxgain_offset_5gm = (u32) rtw_rxgain_offset_5gm;
	registry_par->reg_rxgain_offset_5gh = (u32) rtw_rxgain_offset_5gh;

#ifdef CONFIG_DFS_MASTER
	registry_par->dfs_region_domain = (u8)rtw_dfs_region_domain;
	#ifdef CONFIG_REGD_SRC_FROM_OS
	if (rtw_regd_src == REGD_SRC_OS && registry_par->dfs_region_domain != RTW_DFS_REGD_NONE) {
		RTW_WARN("%s force disable radar detection capability when regd_src is OS\n", __func__);
		registry_par->dfs_region_domain = RTW_DFS_REGD_NONE;
	}
	#endif
#endif

#ifdef CONFIG_MCC_MODE
	registry_par->en_mcc = (u8)rtw_en_mcc;
	registry_par->rtw_mcc_ap_bw20_target_tx_tp = (u32)rtw_mcc_ap_bw20_target_tx_tp;
	registry_par->rtw_mcc_ap_bw40_target_tx_tp = (u32)rtw_mcc_ap_bw40_target_tx_tp;
	registry_par->rtw_mcc_ap_bw80_target_tx_tp = (u32)rtw_mcc_ap_bw80_target_tx_tp;
	registry_par->rtw_mcc_sta_bw20_target_tx_tp = (u32)rtw_mcc_sta_bw20_target_tx_tp;
	registry_par->rtw_mcc_sta_bw40_target_tx_tp = (u32)rtw_mcc_sta_bw40_target_tx_tp;
	registry_par->rtw_mcc_sta_bw80_target_tx_tp = (u32)rtw_mcc_sta_bw80_target_tx_tp;
	registry_par->rtw_mcc_single_tx_cri = (u32)rtw_mcc_single_tx_cri;
	registry_par->rtw_mcc_policy_table_idx = rtw_mcc_policy_table_idx;
	registry_par->rtw_mcc_duration = (u8)rtw_mcc_duration;
	registry_par->rtw_mcc_enable_runtime_duration = rtw_mcc_enable_runtime_duration;
	registry_par->rtw_mcc_phydm_offload = rtw_mcc_phydm_offload;
#endif /*CONFIG_MCC_MODE */

#ifdef CONFIG_WOWLAN
	registry_par->wowlan_enable = rtw_wow_enable;
	registry_par->wakeup_event = rtw_wakeup_event;
	registry_par->suspend_type = rtw_suspend_type;
#endif

#if defined(CONFIG_SDIO_HCI) && defined(CONFIG_PREALLOC_RX_SKB_BUFFER)
	if (rtw_recvbuf_nr != NR_RECVBUFF) {
		RTW_WARN("CONFIG_PREALLOC_RX_SKB_BUFFER && CONFIG_SDIO_HCI, force recvbuf_nr to NR_RECVBUFF(%d)\n", NR_RECVBUFF);
		rtw_recvbuf_nr = NR_RECVBUFF;
	}
#endif
	registry_par->recvbuf_nr = rtw_recvbuf_nr;

#ifdef CONFIG_SUPPORT_TRX_SHARED
	registry_par->trx_share_mode = rtw_trx_share_mode;
#endif
	registry_par->wowlan_sta_mix_mode = rtw_wowlan_sta_mix_mode;

#ifdef CONFIG_PCI_HCI
	registry_par->pci_aspm_config = rtw_pci_aspm_enable;
	registry_par->pci_dynamic_aspm_linkctrl = rtw_pci_dynamic_aspm_linkctrl;
#endif

#ifdef CONFIG_RTW_NAPI
	registry_par->en_napi = (u8)rtw_en_napi;
#ifdef CONFIG_RTW_NAPI_DYNAMIC
	registry_par->napi_threshold = (u32)rtw_napi_threshold;
#endif /* CONFIG_RTW_NAPI_DYNAMIC */
#ifdef CONFIG_RTW_GRO
	registry_par->en_gro = (u8)rtw_en_gro;
	if (!registry_par->en_napi && registry_par->en_gro) {
		registry_par->en_gro = 0;
		RTW_WARN("Disable GRO because NAPI is not enabled\n");
	}
#endif /* CONFIG_RTW_GRO */
#endif /* CONFIG_RTW_NAPI */

	registry_par->iqk_fw_offload = (u8)rtw_iqk_fw_offload;
	registry_par->ch_switch_offload = (u8)rtw_ch_switch_offload;

#ifdef CONFIG_TDLS
	registry_par->en_tdls = rtw_en_tdls;
#endif

#ifdef CONFIG_ADVANCE_OTA
	registry_par->adv_ota = rtw_advnace_ota;
#endif
#ifdef CONFIG_AP_MODE
	registry_par->bmc_tx_rate = rtw_bmc_tx_rate;
	#if CONFIG_RTW_AP_DATA_BMC_TO_UC
	registry_par->ap_src_b2u_flags = rtw_ap_src_b2u_flags;
	registry_par->ap_fwd_b2u_flags = rtw_ap_fwd_b2u_flags;
	#endif
#endif /* CONFIG_AP_MODE */

#ifdef CONFIG_RTW_MESH
	#if CONFIG_RTW_MESH_DATA_BMC_TO_UC
	registry_par->msrc_b2u_flags = rtw_msrc_b2u_flags;
	registry_par->mfwd_b2u_flags = rtw_mfwd_b2u_flags;
	#endif
#endif /* CONFIG_RTW_MESH */

#ifdef CONFIG_FW_HANDLE_TXBCN
	registry_par->fw_tbtt_rpt = rtw_tbtt_rpt;
#endif
	registry_par->phydm_ability = rtw_phydm_ability;
	registry_par->halrf_ability = rtw_halrf_ability;
#ifdef CONFIG_RTW_MESH
	registry_par->peer_alive_based_preq = rtw_peer_alive_based_preq;
#endif

#ifdef RTW_BUSY_DENY_SCAN
	registry_par->scan_interval_thr = rtw_scan_interval_thr;
#endif

#ifdef CONFIG_RTL8822C_XCAP_NEW_POLICY
	registry_par->rtw_8822c_xcap_overwrite = (u8)rtw_8822c_xcap_overwrite;
#endif

#ifdef CONFIG_RTW_MULTI_AP
	rtw_regsty_init_unassoc_sta_param(registry_par);
#endif

	return status;
}

void rtw_core_update_default_setting (struct dvobj_priv *dvobj)
{
	struct rtw_phl_com_t *phl_com = GET_PHL_COM(dvobj);

	#ifdef CONFIG_LOAD_PHY_PARA_FROM_FILE
	rtw_load_phy_file_path(dvobj);
	#endif /* CONFIG_LOAD_PHY_PARA_FROM_FILE */

	phl_com->dev_sw_cap.fw_cap.dlram_en = true;
	phl_com->dev_sw_cap.fw_cap.dlrom_en = false;
	#ifdef CONFIG_FILE_FWIMG
	phl_com->dev_sw_cap.fw_cap.fw_src = RTW_FW_SRC_EXTNAL;
	#else /* !CONFIG_FILE_FWIMG */
	phl_com->dev_sw_cap.fw_cap.fw_src = RTW_FW_SRC_INTNAL;
	#endif /* !CONFIG_FILE_FWIMG */

	phl_com->phy_sw_cap[0].proto_sup = rtw_wireless_mode;
	phl_com->phy_sw_cap[0].band_sup = rtw_band_type;
	phl_com->phy_sw_cap[0].bw_sup = BW_CAP_80M | BW_CAP_40M | BW_CAP_20M; //rtw_bw_mode;
	phl_com->phy_sw_cap[0].txss = rtw_tx_nss;
	phl_com->phy_sw_cap[0].rxss = rtw_rx_nss;

#if 0 // NEO : 8822c has one phy only
	phl_com->phy_sw_cap[1].proto_sup = rtw_wireless_mode;
	phl_com->phy_sw_cap[1].band_sup = rtw_band_type;
	phl_com->phy_sw_cap[1].bw_sup = rtw_bw_mode;
	phl_com->phy_sw_cap[1].txss = rtw_tx_nss;
	phl_com->phy_sw_cap[1].rxss = rtw_rx_nss;
#endif

	/*phl_com->dev_sw_cap.pkg_type = rtw_pkg_type;*/
	phl_com->dev_sw_cap.rfe_type = rtw_rfe_type;
#ifdef DBG_LA_MODE
	phl_com->dev_sw_cap.la_mode = rtw_la_mode_en;
#endif
#ifdef CONFIG_DBCC_SUPPORT
	phl_com->dev_sw_cap.dbcc_sup = rtw_dbcc_en;
#endif

	#if 0 //NEO
	phl_com->dev_sw_cap.hw_hdr_conv = rtw_hw_hdr_conv;

	phl_com->proto_sw_cap[0].max_amsdu_len = rtw_max_amsdu_len;
	phl_com->proto_sw_cap[1].max_amsdu_len = rtw_max_amsdu_len;
	#endif

#if defined(CONFIG_PCI_HCI)
	#if !defined(CONFIG_PCI_ASPM)
	/* Disable all PCIE Backdoor to avoid PCIE IOT */
	phl_com->bus_sw_cap.l0s_ctrl = RTW_PCIE_BUS_FUNC_DISABLE;
	phl_com->bus_sw_cap.l1_ctrl = RTW_PCIE_BUS_FUNC_DISABLE;
	phl_com->bus_sw_cap.l1ss_ctrl = RTW_PCIE_BUS_FUNC_DISABLE;
	phl_com->bus_sw_cap.wake_ctrl = RTW_PCIE_BUS_FUNC_DEFAULT;
	phl_com->bus_sw_cap.crq_ctrl = RTW_PCIE_BUS_FUNC_DISABLE;
	#endif
	phl_com->bus_sw_cap.txbd_num = 256;
	phl_com->bus_sw_cap.rxbd_num = 256;
	phl_com->bus_sw_cap.rpbd_num = 0;	/* by default */
#ifdef CONFIG_RXBUF_NUM_1024
	phl_com->bus_sw_cap.rxbuf_num = 1024;
#else
	phl_com->bus_sw_cap.rxbuf_num = 512;
#endif
	phl_com->bus_sw_cap.rpbuf_num = 0;	/* by default */
#endif /*CONFIG_PCI_HCI*/

#ifdef CONFIG_BTC
	phl_com->dev_sw_cap.btc_mode = BTC_MODE_NORMAL;
#else
	phl_com->dev_sw_cap.btc_mode = BTC_MODE_WL;
#endif
#ifdef CONFIG_MCC
	phl_com->dev_sw_cap.mcc_sup = rtw_mcc_en;
#endif
#ifdef CONFIG_USB_HCI
	phl_com->bus_sw_cap.tx_buf_size = MAX_XMITBUF_SZ;
	phl_com->bus_sw_cap.tx_buf_num = NR_XMITBUFF;
	phl_com->bus_sw_cap.tx_mgnt_buf_size = MAX_MGNT_XMITBUF_SZ;
	phl_com->bus_sw_cap.tx_mgnt_buf_num = NR_MGNT_XMITBUFF;
	phl_com->bus_sw_cap.rx_buf_size = MAX_RECVBUF_SZ;
	phl_com->bus_sw_cap.rx_buf_num = NR_RECVBUFF;
	phl_com->bus_sw_cap.in_token_num = NR_RECV_URB;
#endif
#ifdef CONFIG_BEAMFORMING
	phl_com->role_sw_cap.bf_cap = 0;
	phl_com->role_sw_cap.bf_cap |= (rtw_beamform_cap & BIT0) ? HW_CAP_BFER_VHT_SU : 0;
	phl_com->role_sw_cap.bf_cap |= (rtw_beamform_cap & BIT1) ? HW_CAP_BFEE_VHT_SU: 0;
	phl_com->role_sw_cap.bf_cap |= (rtw_beamform_cap & BIT2) ? HW_CAP_BFER_VHT_MU: 0;
	phl_com->role_sw_cap.bf_cap |= (rtw_beamform_cap & BIT3) ? HW_CAP_BFEE_VHT_MU: 0;
	phl_com->role_sw_cap.bf_cap |= (rtw_beamform_cap & BIT4) ? HW_CAP_BFER_HT_SU: 0;
	phl_com->role_sw_cap.bf_cap |= (rtw_beamform_cap & BIT5) ? HW_CAP_BFEE_HT_SU: 0;
	phl_com->role_sw_cap.bf_cap |= (rtw_beamform_cap & BIT6) ? HW_CAP_BFER_HE_SU: 0;
	phl_com->role_sw_cap.bf_cap |= (rtw_beamform_cap & BIT7) ? HW_CAP_BFEE_HE_SU: 0;
	phl_com->role_sw_cap.bf_cap |= (rtw_beamform_cap & BIT8) ? HW_CAP_BFER_HE_MU: 0;
	phl_com->role_sw_cap.bf_cap |= (rtw_beamform_cap & BIT9) ? HW_CAP_BFEE_HE_MU: 0;

	/*Band0*/
	phl_com->proto_sw_cap[0].vht_su_bfmr = (rtw_sw_proto_bf_cap_phy0 & BIT0) ? 1 : 0;
	phl_com->proto_sw_cap[0].vht_su_bfme = (rtw_sw_proto_bf_cap_phy0 & BIT1) ? 1 : 0;
	phl_com->proto_sw_cap[0].vht_mu_bfmr = (rtw_sw_proto_bf_cap_phy0 & BIT2) ? 1 : 0;
	phl_com->proto_sw_cap[0].vht_mu_bfme = (rtw_sw_proto_bf_cap_phy0 & BIT3) ? 1 : 0;
	phl_com->proto_sw_cap[0].ht_su_bfmr = (rtw_sw_proto_bf_cap_phy0 & BIT4) ? 1 : 0;
	phl_com->proto_sw_cap[0].ht_su_bfme = (rtw_sw_proto_bf_cap_phy0 & BIT5) ? 1 : 0;
	phl_com->proto_sw_cap[0].he_su_bfmr = (rtw_sw_proto_bf_cap_phy0 & BIT6) ? 1 : 0;
	phl_com->proto_sw_cap[0].he_su_bfme = (rtw_sw_proto_bf_cap_phy0 & BIT7) ? 1 : 0;
	phl_com->proto_sw_cap[0].he_mu_bfmr = (rtw_sw_proto_bf_cap_phy0 & BIT8) ? 1 : 0;
	phl_com->proto_sw_cap[0].he_mu_bfme = (rtw_sw_proto_bf_cap_phy0 & BIT9) ? 1 : 0;

	/*Band1*/
	phl_com->proto_sw_cap[1].vht_su_bfmr = (rtw_sw_proto_bf_cap_phy1 & BIT0) ? 1 : 0;
	phl_com->proto_sw_cap[1].vht_su_bfme = (rtw_sw_proto_bf_cap_phy1 & BIT1) ? 1 : 0;
	phl_com->proto_sw_cap[1].vht_mu_bfmr = (rtw_sw_proto_bf_cap_phy1 & BIT2) ? 1 : 0;
	phl_com->proto_sw_cap[1].vht_mu_bfme = (rtw_sw_proto_bf_cap_phy1 & BIT3) ? 1 : 0;
	phl_com->proto_sw_cap[1].ht_su_bfmr = (rtw_sw_proto_bf_cap_phy1 & BIT4) ? 1 : 0;
	phl_com->proto_sw_cap[1].ht_su_bfme = (rtw_sw_proto_bf_cap_phy1 & BIT5) ? 1 : 0;
	phl_com->proto_sw_cap[1].he_su_bfmr = (rtw_sw_proto_bf_cap_phy1 & BIT6) ? 1 : 0;
	phl_com->proto_sw_cap[1].he_su_bfme = (rtw_sw_proto_bf_cap_phy1 & BIT7) ? 1 : 0;
	phl_com->proto_sw_cap[1].he_mu_bfmr = (rtw_sw_proto_bf_cap_phy1 & BIT8) ? 1 : 0;
	phl_com->proto_sw_cap[1].he_mu_bfme = (rtw_sw_proto_bf_cap_phy1 & BIT9) ? 1 : 0;
#endif
}

