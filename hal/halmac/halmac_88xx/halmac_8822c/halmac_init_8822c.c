/******************************************************************************
 *
 * Copyright(c) 2017 - 2019 Realtek Corporation. All rights reserved.
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

#include "halmac_init_8822c.h"
#include "halmac_8822c_cfg.h"

#if HALMAC_PCIE_SUPPORT
#include "halmac_pcie_8822c.h"
#endif
#if HALMAC_SDIO_SUPPORT
#include "halmac_sdio_8822c.h"
#include "../halmac_sdio_88xx.h"
#endif
#include "halmac_gpio_8822c.h"
#include "halmac_common_8822c.h"
#include "halmac_cfg_wmac_8822c.h"
#include "../halmac_common_88xx.h"
#include "../halmac_init_88xx.h"
#include "../halmac_cfg_wmac_88xx.h"

#if HALMAC_8822C_SUPPORT

#define SYS_FUNC_EN		0xD8

#define RSVD_PG_DRV_NUM			16
#define RSVD_PG_H2C_EXTRAINFO_NUM	24
#define RSVD_PG_H2C_STATICINFO_NUM	8
#define RSVD_PG_H2CQ_NUM		8
#define RSVD_PG_CPU_INSTRUCTION_NUM	0
#define RSVD_PG_FW_TXBUF_NUM		4
#define RSVD_PG_CSIBUF_NUM		50
#define RSVD_PG_DLLB_NUM		(TX_FIFO_SIZE_8822C / 3 >> \
					TX_PAGE_SIZE_SHIFT_88XX)

#define MAC_TRX_ENABLE	(BIT_HCI_TXDMA_EN | BIT_HCI_RXDMA_EN | BIT_TXDMA_EN | \
			BIT_RXDMA_EN | BIT_PROTOCOL_EN | BIT_SCHEDULE_EN | \
			BIT_MACTXEN | BIT_MACRXEN)

#define WLAN_TXQ_RPT_EN		0x1F

#define BLK_DESC_NUM	0x3
#define RX_DLK_TIME	0x14

#define WLAN_SLOT_TIME		0x09
#define WLAN_PIFS_TIME		0x1C
#define WLAN_SIFS_CCK_CONT_TX	0x0A
#define WLAN_SIFS_OFDM_CONT_TX	0x0E
#define WLAN_SIFS_CCK_TRX	0x0A
#define WLAN_SIFS_OFDM_TRX	0x10
#define WLAN_NAV_MAX		0xC8
#define WLAN_RDG_NAV		0x05
#define WLAN_TXOP_NAV		0x1B
#define WLAN_CCK_RX_TSF		0x30
#define WLAN_OFDM_RX_TSF	0x30
#define WLAN_TBTT_PROHIBIT	0x04 /* unit : 32us */
#define WLAN_TBTT_HOLD_TIME	0x064 /* unit : 32us */
#define WLAN_DRV_EARLY_INT	0x04
#define WLAN_BCN_CTRL_CLT0	0x10
#define WLAN_BCN_DMA_TIME	0x02
#define WLAN_BCN_MAX_ERR	0xFF
#define WLAN_SIFS_CCK_DUR_TUNE	0x0A
#define WLAN_SIFS_OFDM_DUR_TUNE	0x10
#define WLAN_SIFS_CCK_CTX	0x0A
#define WLAN_SIFS_CCK_IRX	0x0A
#define WLAN_SIFS_OFDM_CTX	0x0E
#define WLAN_SIFS_OFDM_IRX	0x0E
#define WLAN_EIFS_DUR_TUNE	0x40
#define WLAN_EDCA_VO_PARAM	0x002FA226
#define WLAN_EDCA_VI_PARAM	0x005EA328
#define WLAN_EDCA_BE_PARAM	0x005EA42B
#define WLAN_EDCA_BK_PARAM	0x0000A44F

#define WLAN_RX_FILTER0		0xFFFFFFFF
#define WLAN_RX_FILTER2		0xFFFF
#define WLAN_RCR_CFG		0xE410220E
#define WLAN_RXPKT_MAX_SZ	12288
#define WLAN_RXPKT_MAX_SZ_512	(WLAN_RXPKT_MAX_SZ >> 9)

#define WLAN_AMPDU_MAX_TIME		0x70
#define WLAN_RTS_LEN_TH			0xFF
#define WLAN_RTS_TX_TIME_TH		0x08
#define WLAN_MAX_AGG_PKT_LIMIT		0x3F
#define WLAN_RTS_MAX_AGG_PKT_LIMIT	0x20
#define WLAN_PRE_TXCNT_TIME_TH		0x1E4
#define WALN_FAST_EDCA_VO_TH		0x06
#define WLAN_FAST_EDCA_VI_TH		0x06
#define WLAN_FAST_EDCA_BE_TH		0x06
#define WLAN_FAST_EDCA_BK_TH		0x06
#define WLAN_BAR_RETRY_LIMIT		0x01
#define WLAN_BAR_ACK_TYPE		0x05
#define WLAN_RA_TRY_RATE_AGG_LIMIT	0x08
#define WLAN_RESP_TXRATE		0x84
#define WLAN_ACK_TO			0x21
#define WLAN_ACK_TO_CCK			0x6A
#define WLAN_DATA_RATE_FB_CNT_1_4	0x01000000
#define WLAN_DATA_RATE_FB_CNT_5_8	0x08070504
#define WLAN_RTS_RATE_FB_CNT_5_8	0x08070504
#define WLAN_DATA_RATE_FB_RATE0		0xFE01F010
#define WLAN_DATA_RATE_FB_RATE0_H	0x40000000
#define WLAN_RTS_RATE_FB_RATE1		0x003FF010
#define WLAN_RTS_RATE_FB_RATE1_H	0x40000000
#define WLAN_RTS_RATE_FB_RATE4		0x0600F010
#define WLAN_RTS_RATE_FB_RATE4_H	0x400003E0
#define WLAN_RTS_RATE_FB_RATE5		0x0600F015
#define WLAN_RTS_RATE_FB_RATE5_H	0x000000E0

#define WLAN_TX_FUNC_CFG1		0x30
#define WLAN_TX_FUNC_CFG2		0x30
#define WLAN_MAC_OPT_NORM_FUNC1		0x98
#define WLAN_MAC_OPT_LB_FUNC1		0x80
#define WLAN_MAC_OPT_FUNC2		0xB1810041

#define WLAN_SIFS_CFG	(WLAN_SIFS_CCK_CONT_TX | \
			(WLAN_SIFS_OFDM_CONT_TX << BIT_SHIFT_SIFS_OFDM_CTX) | \
			(WLAN_SIFS_CCK_TRX << BIT_SHIFT_SIFS_CCK_TRX) | \
			(WLAN_SIFS_OFDM_TRX << BIT_SHIFT_SIFS_OFDM_TRX))

#define WLAN_SIFS_DUR_TUNE	(WLAN_SIFS_CCK_DUR_TUNE | \
				(WLAN_SIFS_OFDM_DUR_TUNE << 8))

#define WLAN_TBTT_TIME	(WLAN_TBTT_PROHIBIT |\
			(WLAN_TBTT_HOLD_TIME << BIT_SHIFT_TBTT_HOLD_TIME_AP))

#define WLAN_NAV_CFG		(WLAN_RDG_NAV | (WLAN_TXOP_NAV << 16))
#define WLAN_RX_TSF_CFG		(WLAN_CCK_RX_TSF | (WLAN_OFDM_RX_TSF) << 8)

#if HALMAC_PLATFORM_WINDOWS
/*SDIO RQPN Mapping for Windows, extra queue is not implemented in Driver code*/
static struct halmac_rqpn HALMAC_RQPN_SDIO_8822C[] = {
	/* { mode, vo_map, vi_map, be_map, bk_map, mg_map, hi_map } */
	{HALMAC_TRX_MODE_NORMAL,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_TRXSHARE,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_WMM,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_NQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_P2P,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_LOOPBACK,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
};
#else
/*SDIO RQPN Mapping*/
static struct halmac_rqpn HALMAC_RQPN_SDIO_8822C[] = {
	/* { mode, vo_map, vi_map, be_map, bk_map, mg_map, hi_map } */
	{HALMAC_TRX_MODE_NORMAL,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_TRXSHARE,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_WMM,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_NQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_P2P,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_LOOPBACK,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
};
#endif

/*PCIE RQPN Mapping*/
static struct halmac_rqpn HALMAC_RQPN_PCIE_8822C[] = {
	/* { mode, vo_map, vi_map, be_map, bk_map, mg_map, hi_map } */
	{HALMAC_TRX_MODE_NORMAL,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_TRXSHARE,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_WMM,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_NQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_P2P,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_LOOPBACK,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
};

/*USB 2 Bulkout RQPN Mapping*/
static struct halmac_rqpn HALMAC_RQPN_2BULKOUT_8822C[] = {
	/* { mode, vo_map, vi_map, be_map, bk_map, mg_map, hi_map } */
	{HALMAC_TRX_MODE_NORMAL,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_HQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_TRXSHARE,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_HQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_WMM,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_HQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_P2P,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ, HALMAC_MAP2_NQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_LOOPBACK,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ, HALMAC_MAP2_NQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ, HALMAC_MAP2_HQ, HALMAC_MAP2_NQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
};

/*USB 3 Bulkout RQPN Mapping*/
static struct halmac_rqpn HALMAC_RQPN_3BULKOUT_8822C[] = {
	/* { mode, vo_map, vi_map, be_map, bk_map, mg_map, hi_map } */
	{HALMAC_TRX_MODE_NORMAL,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_TRXSHARE,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_WMM,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_NQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_P2P,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ, HALMAC_MAP2_LQ, HALMAC_MAP2_NQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_LOOPBACK,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ, HALMAC_MAP2_LQ, HALMAC_MAP2_NQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ, HALMAC_MAP2_LQ, HALMAC_MAP2_NQ,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_HQ},
};

/*USB 4 Bulkout RQPN Mapping*/
static struct halmac_rqpn HALMAC_RQPN_4BULKOUT_8822C[] = {
	/* { mode, vo_map, vi_map, be_map, bk_map, mg_map, hi_map } */
	{HALMAC_TRX_MODE_NORMAL,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_TRXSHARE,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_WMM,
	 HALMAC_MAP2_HQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_NQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_P2P,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_LOOPBACK,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK,
	 HALMAC_MAP2_NQ, HALMAC_MAP2_NQ, HALMAC_MAP2_LQ, HALMAC_MAP2_LQ,
	 HALMAC_MAP2_EXQ, HALMAC_MAP2_HQ},
};

#if HALMAC_PLATFORM_WINDOWS
/*SDIO Page Number*/
static struct halmac_pg_num HALMAC_PG_NUM_SDIO_8822C[] = {
	/* { mode, hq_num, nq_num, lq_num, exq_num, gap_num} */
	{HALMAC_TRX_MODE_NORMAL, 64, 64, 64, 0, 1},
	{HALMAC_TRX_MODE_TRXSHARE, 32, 32, 32, 0, 1},
	{HALMAC_TRX_MODE_WMM, 64, 64, 64, 0, 1},
	{HALMAC_TRX_MODE_P2P, 64, 64, 64, 0, 1},
	{HALMAC_TRX_MODE_LOOPBACK, 64, 64, 64, 0, 1},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK, 64, 64, 64, 0, 1},
};
#else
/*SDIO Page Number*/
static struct halmac_pg_num HALMAC_PG_NUM_SDIO_8822C[] = {
	/* { mode, hq_num, nq_num, lq_num, exq_num, gap_num} */
	{HALMAC_TRX_MODE_NORMAL, 64, 64, 64, 64, 1},
	{HALMAC_TRX_MODE_TRXSHARE, 32, 32, 32, 32, 1},
	{HALMAC_TRX_MODE_WMM, 64, 64, 64, 64, 1},
	{HALMAC_TRX_MODE_P2P, 64, 64, 64, 64, 1},
	{HALMAC_TRX_MODE_LOOPBACK, 64, 64, 64, 64, 1},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK, 64, 64, 64, 64, 1},
};
#endif

/*PCIE Page Number*/
static struct halmac_pg_num HALMAC_PG_NUM_PCIE_8822C[] = {
	/* { mode, hq_num, nq_num, lq_num, exq_num, gap_num} */
	{HALMAC_TRX_MODE_NORMAL, 64, 64, 64, 64, 1},
	{HALMAC_TRX_MODE_TRXSHARE, 64, 64, 64, 64, 1},
	{HALMAC_TRX_MODE_WMM, 64, 64, 64, 64, 1},
	{HALMAC_TRX_MODE_P2P, 64, 64, 64, 64, 1},
	{HALMAC_TRX_MODE_LOOPBACK, 64, 64, 64, 64, 1},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK, 64, 64, 64, 64, 1},
};

/*USB 2 Bulkout Page Number*/
static struct halmac_pg_num HALMAC_PG_NUM_2BULKOUT_8822C[] = {
	/* { mode, hq_num, nq_num, lq_num, exq_num, gap_num} */
	{HALMAC_TRX_MODE_NORMAL, 64, 64, 0, 0, 1},
	{HALMAC_TRX_MODE_TRXSHARE, 64, 64, 0, 0, 1},
	{HALMAC_TRX_MODE_WMM, 64, 64, 0, 0, 1},
	{HALMAC_TRX_MODE_P2P, 64, 64, 0, 0, 1},
	{HALMAC_TRX_MODE_LOOPBACK, 64, 64, 0, 0, 1},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK, 64, 64, 0, 0, 1},
};

/*USB 3 Bulkout Page Number*/
static struct halmac_pg_num HALMAC_PG_NUM_3BULKOUT_8822C[] = {
	/* { mode, hq_num, nq_num, lq_num, exq_num, gap_num} */
	{HALMAC_TRX_MODE_NORMAL, 64, 64, 64, 0, 1},
	{HALMAC_TRX_MODE_TRXSHARE, 64, 64, 64, 0, 1},
	{HALMAC_TRX_MODE_WMM, 64, 64, 64, 0, 1},
	{HALMAC_TRX_MODE_P2P, 64, 64, 64, 0, 1},
	{HALMAC_TRX_MODE_LOOPBACK, 64, 64, 64, 0, 1},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK, 64, 64, 64, 0, 1},
};

/*USB 4 Bulkout Page Number*/
static struct halmac_pg_num HALMAC_PG_NUM_4BULKOUT_8822C[] = {
	/* { mode, hq_num, nq_num, lq_num, exq_num, gap_num} */
	{HALMAC_TRX_MODE_NORMAL, 64, 64, 64, 64, 1},
	{HALMAC_TRX_MODE_TRXSHARE, 64, 64, 64, 64, 1},
	{HALMAC_TRX_MODE_WMM, 64, 64, 64, 64, 1},
	{HALMAC_TRX_MODE_P2P, 64, 64, 64, 64, 1},
	{HALMAC_TRX_MODE_LOOPBACK, 64, 64, 64, 64, 1},
	{HALMAC_TRX_MODE_DELAY_LOOPBACK, 64, 64, 64, 64, 1},
};

enum halmac_ret_status
mount_api_8822c(struct halmac_adapter *adapter)
{
	struct halmac_api *api = (struct halmac_api *)adapter->halmac_api;

	//adapter->chip_id = HALMAC_CHIP_ID_8822C;
	adapter->hw_cfg_info.efuse_size = EFUSE_SIZE_8822C;
	adapter->hw_cfg_info.eeprom_size = EEPROM_SIZE_8822C;
	adapter->hw_cfg_info.bt_efuse_size = BT_EFUSE_SIZE_8822C;
	adapter->hw_cfg_info.prtct_efuse_size = PRTCT_EFUSE_SIZE_8822C;
	adapter->hw_cfg_info.cam_entry_num = SEC_CAM_NUM_8822C;
	adapter->hw_cfg_info.tx_fifo_size = TX_FIFO_SIZE_8822C;
	adapter->hw_cfg_info.rx_fifo_size = RX_FIFO_SIZE_8822C;
	adapter->hw_cfg_info.ac_oqt_size = OQT_ENTRY_AC_8822C;
	adapter->hw_cfg_info.non_ac_oqt_size = OQT_ENTRY_NOAC_8822C;
	adapter->hw_cfg_info.usb_txagg_num = BLK_DESC_NUM;
	adapter->txff_alloc.rsvd_drv_pg_num = RSVD_PG_DRV_NUM;

	api->halmac_pinmux_set_func = pinmux_set_func_8822c;
	api->halmac_pinmux_free_func = pinmux_free_func_8822c;
	api->halmac_get_hw_value = get_hw_value_8822c;
	api->halmac_set_hw_value = set_hw_value_8822c;
	api->halmac_cfg_drv_info = cfg_drv_info_8822c;
	api->halmac_fill_txdesc_checksum = fill_txdesc_check_sum_8822c;
	api->halmac_init_low_pwr = init_low_pwr_8822c;

	return HALMAC_RET_SUCCESS;
}

enum halmac_ret_status
set_trx_fifo_info_8822c(struct halmac_adapter *adapter)
{
	u16 cur_pg_addr;
	u32 txff_size = TX_FIFO_SIZE_8822C;
	u32 rxff_size = RX_FIFO_SIZE_8822C;
	struct halmac_txff_allocation *info = &adapter->txff_alloc;

	if (info->rx_fifo_exp_mode == HALMAC_RX_FIFO_EXPANDING_MODE_1_BLOCK) {
		txff_size = TX_FIFO_SIZE_RX_EXPAND_1BLK_8822C;
		rxff_size = RX_FIFO_SIZE_RX_EXPAND_1BLK_8822C;
	} else if (info->rx_fifo_exp_mode ==
		   HALMAC_RX_FIFO_EXPANDING_MODE_2_BLOCK) {
		txff_size = TX_FIFO_SIZE_RX_EXPAND_2BLK_8822C;
		rxff_size = RX_FIFO_SIZE_RX_EXPAND_2BLK_8822C;
	} else if (info->rx_fifo_exp_mode ==
		   HALMAC_RX_FIFO_EXPANDING_MODE_3_BLOCK) {
		txff_size = TX_FIFO_SIZE_RX_EXPAND_3BLK_8822C;
		rxff_size = RX_FIFO_SIZE_RX_EXPAND_3BLK_8822C;
	} else if (info->rx_fifo_exp_mode ==
		   HALMAC_RX_FIFO_EXPANDING_MODE_4_BLOCK) {
		txff_size = TX_FIFO_SIZE_RX_EXPAND_4BLK_8822C;
		rxff_size = RX_FIFO_SIZE_RX_EXPAND_4BLK_8822C;
	}

	if (info->la_mode != HALMAC_LA_MODE_DISABLE) {
		txff_size = TX_FIFO_SIZE_LA_8822C;
		rxff_size = RX_FIFO_SIZE_8822C;
	}

	adapter->hw_cfg_info.tx_fifo_size = txff_size;
	adapter->hw_cfg_info.rx_fifo_size = rxff_size;
	info->tx_fifo_pg_num = (u16)(txff_size >> TX_PAGE_SIZE_SHIFT_88XX);

	info->rsvd_pg_num = info->rsvd_drv_pg_num +
					RSVD_PG_H2C_EXTRAINFO_NUM +
					RSVD_PG_H2C_STATICINFO_NUM +
					RSVD_PG_H2CQ_NUM +
					RSVD_PG_CPU_INSTRUCTION_NUM +
					RSVD_PG_FW_TXBUF_NUM +
					RSVD_PG_CSIBUF_NUM;

	if (info->rsvd_pg_num > info->tx_fifo_pg_num)
		return HALMAC_RET_CFG_TXFIFO_PAGE_FAIL;

	info->acq_pg_num = info->tx_fifo_pg_num - info->rsvd_pg_num;
	info->rsvd_boundary = info->tx_fifo_pg_num - info->rsvd_pg_num;

	cur_pg_addr = info->tx_fifo_pg_num;
	cur_pg_addr -= RSVD_PG_CSIBUF_NUM;
	info->rsvd_csibuf_addr = cur_pg_addr;
	cur_pg_addr -= RSVD_PG_FW_TXBUF_NUM;
	info->rsvd_fw_txbuf_addr = cur_pg_addr;
	cur_pg_addr -= RSVD_PG_CPU_INSTRUCTION_NUM;
	info->rsvd_cpu_instr_addr = cur_pg_addr;
	cur_pg_addr -= RSVD_PG_H2CQ_NUM;
	info->rsvd_h2cq_addr = cur_pg_addr;
	cur_pg_addr -= RSVD_PG_H2C_STATICINFO_NUM;
	info->rsvd_h2c_sta_info_addr = cur_pg_addr;
	cur_pg_addr -= RSVD_PG_H2C_EXTRAINFO_NUM;
	info->rsvd_h2c_info_addr = cur_pg_addr;
	cur_pg_addr -= info->rsvd_drv_pg_num;
	info->rsvd_drv_addr = cur_pg_addr;

	if (info->rsvd_boundary != info->rsvd_drv_addr)
		return HALMAC_RET_CFG_TXFIFO_PAGE_FAIL;

	return HALMAC_RET_SUCCESS;
}

#endif /* HALMAC_8822C_SUPPORT */
