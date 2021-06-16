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
#ifndef __HAL_IC_CFG_H__
#define __HAL_IC_CFG_H__

#define RTL8188E_SUPPORT				0
#define RTL8812A_SUPPORT				0
#define RTL8821A_SUPPORT				0
#define RTL8723B_SUPPORT				0
#define RTL8723D_SUPPORT				0
#define RTL8723F_SUPPORT				0
#define RTL8192E_SUPPORT				0
#define RTL8192F_SUPPORT				0
#define RTL8814A_SUPPORT				0
#define RTL8195A_SUPPORT				0
#define RTL8197F_SUPPORT				0
#define RTL8703B_SUPPORT				0
#define RTL8188F_SUPPORT				0
#define RTL8822B_SUPPORT				0
#define RTL8821B_SUPPORT				0
#define RTL8821C_SUPPORT				0
#define RTL8710B_SUPPORT				0
#define RTL8814B_SUPPORT				0
#define RTL8824B_SUPPORT				0
#define RTL8198F_SUPPORT				0
#define RTL8195B_SUPPORT				0
#define RTL8822C_SUPPORT				0
#define RTL8721D_SUPPORT				0
#define RTL8812F_SUPPORT				0
#define RTL8197G_SUPPORT				0
#define RTL8710C_SUPPORT				0


/*#if (RTL8188E_SUPPORT==1)*/
#define RATE_ADAPTIVE_SUPPORT			0
#define POWER_TRAINING_ACTIVE			0

#ifdef CONFIG_MULTIDRV
#endif

#ifdef CONFIG_RTL8822C
	#undef RTL8822C_SUPPORT
	#define RTL8822C_SUPPORT				1
	/*#define DBG_LA_MODE*/
	#ifndef CONFIG_FW_C2H_PKT
		#define CONFIG_FW_C2H_PKT
	#endif /* CONFIG_FW_C2H_PKT */
	#define RTW_TX_PA_BIAS	/* Adjust TX PA Bias from eFuse */

	#ifdef CONFIG_WOWLAN
		#define CONFIG_GTK_OL
		/*#define CONFIG_ARP_KEEP_ALIVE*/

		#ifdef CONFIG_GPIO_WAKEUP
			#ifndef WAKEUP_GPIO_IDX
				#define WAKEUP_GPIO_IDX	6	/* WIFI Chip Side */
			#endif /* !WAKEUP_GPIO_IDX */
		#endif /* CONFIG_GPIO_WAKEUP */
	#endif /* CONFIG_WOWLAN */

	#ifdef CONFIG_CONCURRENT_MODE
		#define CONFIG_AP_PORT_SWAP
		#define CONFIG_FW_MULTI_PORT_SUPPORT
	#endif /* CONFIG_CONCURRENT_MODE */

	/*
	 * Beamforming related definition
	 */
	/* Only support new beamforming mechanism */
	#ifdef CONFIG_BEAMFORMING
		#define RTW_BEAMFORMING_VERSION_2
	#endif /* CONFIG_BEAMFORMING */

	#ifdef CONFIG_NO_FW
		#ifdef CONFIG_RTW_MAC_HIDDEN_RPT
			#undef CONFIG_RTW_MAC_HIDDEN_RPT
		#endif
	#else
		#ifndef CONFIG_RTW_MAC_HIDDEN_RPT
			#define CONFIG_RTW_MAC_HIDDEN_RPT
		#endif
	#endif

#if 0 //NEO
	#ifndef DBG_RX_DFRAME_RAW_DATA
		#define DBG_RX_DFRAME_RAW_DATA
	#endif /* DBG_RX_DFRAME_RAW_DATA */
#endif //NEO

	#ifndef RTW_IQK_FW_OFFLOAD
		/* #define RTW_IQK_FW_OFFLOAD */
	#endif /* RTW_IQK_FW_OFFLOAD */
	#define CONFIG_ADVANCE_OTA

	#ifdef CONFIG_MCC_MODE
		#define CONFIG_MCC_MODE_V2
		#define CONFIG_MCC_PHYDM_OFFLOAD
	#endif /* CONFIG_MCC_MODE */

	#if defined(CONFIG_TDLS) && defined(CONFIG_TDLS_CH_SW)
		#define CONFIG_TDLS_CH_SW_V2
	#endif

	#ifndef RTW_CHANNEL_SWITCH_OFFLOAD
		#ifdef CONFIG_TDLS_CH_SW_V2
			#define RTW_CHANNEL_SWITCH_OFFLOAD
		#endif
	#endif /* RTW_CHANNEL_SWITCH_OFFLOAD */

	#define CONFIG_SUPPORT_FIFO_DUMP
	#define CONFIG_HW_P0_TSF_SYNC
	#define CONFIG_BCN_RECV_TIME

	/*#define CONFIG_TCP_CSUM_OFFLOAD_TX*/
	#if defined(CONFIG_TCP_CSUM_OFFLOAD_TX) && !defined(CONFIG_RTW_NETIF_SG)
		#define CONFIG_RTW_NETIF_SG
	#endif
	#define CONFIG_TCP_CSUM_OFFLOAD_RX

	#ifdef CONFIG_P2P_PS
		#define CONFIG_P2P_PS_NOA_USE_MACID_SLEEP
	#endif
	//NEO
	//#define CONFIG_RTS_FULL_BW
	
	#ifdef CONFIG_LPS
		#define CONFIG_LPS_ACK	/* Supported after FW v07 */
		#define CONFIG_LPS_1T1R /* Supported after FW v07 */
	#endif

	#define CONFIG_BT_EFUSE_MASK

	#ifndef CONFIG_TXPWR_PG_WITH_PWR_IDX
	#define CONFIG_TXPWR_PG_WITH_PWR_IDX
	#endif
	#ifndef CONFIG_TXPWR_PG_WITH_TSSI_OFFSET
	#define CONFIG_TXPWR_PG_WITH_TSSI_OFFSET
	#endif

	#define CONFIG_RTL8822C_XCAP_NEW_POLICY

//NEO
//	#define CONFIG_SUPPORT_DYNAMIC_TXPWR
#endif /* CONFIG_RTL8822C */

#endif /*__HAL_IC_CFG_H__*/
