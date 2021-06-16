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

#include <linux/bitfield.h>
#include "init.h"
//#include "security_cam.h"
//#include "hw.h"


#define RSVD_PG_DRV_NUM			16
#define RSVD_PG_H2C_EXTRAINFO_NUM	24
#define RSVD_PG_H2C_STATICINFO_NUM	8
#define RSVD_PG_H2CQ_NUM		8
#define RSVD_PG_CPU_INSTRUCTION_NUM	0
#define RSVD_PG_FW_TXBUF_NUM		4

#define TX_PAGE_SIZE_SHIFT	7

enum rtw_dma_mapping {
	RTW_DMA_MAPPING_EXTRA	= 0,
	RTW_DMA_MAPPING_LOW	= 1,
	RTW_DMA_MAPPING_NORMAL	= 2,
	RTW_DMA_MAPPING_HIGH	= 3,

	RTW_DMA_MAPPING_MAX,
	RTW_DMA_MAPPING_UNDEF,
};

struct rtw_rqpn {
	enum rtw_dma_mapping dma_map_vo;
	enum rtw_dma_mapping dma_map_vi;
	enum rtw_dma_mapping dma_map_be;
	enum rtw_dma_mapping dma_map_bk;
	enum rtw_dma_mapping dma_map_mg;
	enum rtw_dma_mapping dma_map_hi;
};

static struct rtw_rqpn rqpn_table[] = {
	{RTW_DMA_MAPPING_NORMAL, RTW_DMA_MAPPING_NORMAL,
	 RTW_DMA_MAPPING_LOW, RTW_DMA_MAPPING_LOW,
	 RTW_DMA_MAPPING_EXTRA, RTW_DMA_MAPPING_HIGH},
	{RTW_DMA_MAPPING_NORMAL, RTW_DMA_MAPPING_NORMAL,
	 RTW_DMA_MAPPING_LOW, RTW_DMA_MAPPING_LOW,
	 RTW_DMA_MAPPING_EXTRA, RTW_DMA_MAPPING_HIGH},
	{RTW_DMA_MAPPING_NORMAL, RTW_DMA_MAPPING_NORMAL,
	 RTW_DMA_MAPPING_NORMAL, RTW_DMA_MAPPING_HIGH,
	 RTW_DMA_MAPPING_HIGH, RTW_DMA_MAPPING_HIGH},
	{RTW_DMA_MAPPING_NORMAL, RTW_DMA_MAPPING_NORMAL,
	 RTW_DMA_MAPPING_LOW, RTW_DMA_MAPPING_LOW,
	 RTW_DMA_MAPPING_HIGH, RTW_DMA_MAPPING_HIGH},
	{RTW_DMA_MAPPING_NORMAL, RTW_DMA_MAPPING_NORMAL,
	 RTW_DMA_MAPPING_LOW, RTW_DMA_MAPPING_LOW,
	 RTW_DMA_MAPPING_EXTRA, RTW_DMA_MAPPING_HIGH},
};

struct rtw_page_table {
	u16 hq_num;
	u16 nq_num;
	u16 lq_num;
	u16 exq_num;
	u16 gapq_num;
};

static struct rtw_page_table page_table_8822c[] = {
	{64, 64, 64, 64, 1},
	{64, 64, 64, 64, 1},
	{64, 64, 0, 0, 1},
	{64, 64, 64, 0, 1},
	{64, 64, 64, 64, 1},
};

#ifdef CONFIG_NEW_HALMAC_INTERFACE
struct mac_ax_adapter *get_mac_ax_adapter(enum mac_ax_intf intf,
					  u8 chip_id, u8 chip_cut,
					  void *phl_adapter, void *drv_adapter,
					  struct mac_ax_pltfm_cb *pltfm_cb)
{
	struct mac_ax_adapter *adapter = NULL;

	switch (chip_id) {
#if MAC_AX_8852A_SUPPORT
	case MAC_AX_CHIP_ID_8852A:
		adapter = get_mac_8852a_adapter(intf, chip_cut, phl_adapter,
						drv_adapter, pltfm_cb);
		break;
#endif
	default:
		return NULL;
	}

	return adapter;
}
#else
struct mac_adapter *get_mac_adapter(enum mac_intf intf,
					  u8 chip_id, u8 cv,
					  void *drv_adapter,
					  struct mac_pltfm_cb *pltfm_cb)
{
	struct mac_adapter *adapter = NULL;

	switch (chip_id) {
#if MAC_AX_8852A_SUPPORT
	case MAC_CHIP_ID_8852A:
		adapter = get_mac_8852a_adapter(intf, cv, drv_adapter,
						pltfm_cb);
		break;
#endif
#if MAC_AX_8852B_SUPPORT
	case MAC_CHIP_ID_8852B:
		adapter = get_mac_8852b_adapter(intf, cv, drv_adapter,
						pltfm_cb);
		break;
#endif
	case MAC_CHIP_ID_8822C:
		adapter = get_mac_8822c_adapter(intf, cv, drv_adapter,
						pltfm_cb);
		break;

	default:
		return NULL;
	}

	return adapter;
}
#endif

static int txdma_queue_mapping(struct mac_adapter *adapter)
{
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct rtw_phl_com_t *phl_com = (struct rtw_phl_com_t *)(adapter->phl_adapter);
	struct hal_spec_t *hal_spec = phl_get_ic_spec(phl_com);
	struct rtw_rqpn *rqpn;
	u8 max_bulkout_num = hal_spec->max_bulkout_num;
	u16 txdma_pq_map = 0;

	if (max_bulkout_num < 2 || max_bulkout_num > 4) {
		PLTFM_MSG_ERR("max_bulkout_num %d invalid\n", max_bulkout_num);
		return MACNOITEM;
	}
	rqpn = &rqpn_table[max_bulkout_num];

	txdma_pq_map |= BIT_TXDMA_HIQ_MAP(rqpn->dma_map_hi);
	txdma_pq_map |= BIT_TXDMA_MGQ_MAP(rqpn->dma_map_mg);
	txdma_pq_map |= BIT_TXDMA_BKQ_MAP(rqpn->dma_map_bk);
	txdma_pq_map |= BIT_TXDMA_BEQ_MAP(rqpn->dma_map_be);
	txdma_pq_map |= BIT_TXDMA_VIQ_MAP(rqpn->dma_map_vi);
	txdma_pq_map |= BIT_TXDMA_VOQ_MAP(rqpn->dma_map_vo);
	MAC_REG_W16(REG_TXDMA_PQ_MAP, txdma_pq_map);

	return MACSUCCESS;

}

static u32
set_trx_fifo_info(struct mac_adapter *adapter)
{
	struct mac_fifo_info *fifo = &adapter->fifo_info;
	u8 csi_buf_pg_num = adapter->hw_info->csi_buf_pg_num;
	u16 cur_pg_addr;
	u32 txff_size = adapter->hw_info->txff_size;

	/* config rsvd page num */
	fifo->rsvd_drv_pg_num = 8;
	fifo->txff_pg_num = txff_size >> 7;
	fifo->rsvd_pg_num = fifo->rsvd_drv_pg_num +
			   RSVD_PG_H2C_EXTRAINFO_NUM +
			   RSVD_PG_H2C_STATICINFO_NUM +
			   RSVD_PG_H2CQ_NUM +
			   RSVD_PG_CPU_INSTRUCTION_NUM +
			   RSVD_PG_FW_TXBUF_NUM +
			   csi_buf_pg_num;

	if (fifo->rsvd_pg_num > fifo->txff_pg_num) {
		PLTFM_MSG_ERR("[ERR] %s rsvd_pg_num(%d) > txff_pg_num(%d)\n",
			      __func__, fifo->rsvd_pg_num, fifo->txff_pg_num);
		return MACBUFSZ;
	}

	fifo->acq_pg_num = fifo->txff_pg_num - fifo->rsvd_pg_num;
	fifo->rsvd_boundary = fifo->txff_pg_num - fifo->rsvd_pg_num;

	cur_pg_addr = fifo->txff_pg_num;
	cur_pg_addr -= csi_buf_pg_num;
	fifo->rsvd_csibuf_addr = cur_pg_addr;
	cur_pg_addr -= RSVD_PG_FW_TXBUF_NUM;
	fifo->rsvd_fw_txbuf_addr = cur_pg_addr;
	cur_pg_addr -= RSVD_PG_CPU_INSTRUCTION_NUM;
	fifo->rsvd_cpu_instr_addr = cur_pg_addr;
	cur_pg_addr -= RSVD_PG_H2CQ_NUM;
	fifo->rsvd_h2cq_addr = cur_pg_addr;
	cur_pg_addr -= RSVD_PG_H2C_STATICINFO_NUM;
	fifo->rsvd_h2c_sta_info_addr = cur_pg_addr;
	cur_pg_addr -= RSVD_PG_H2C_EXTRAINFO_NUM;
	fifo->rsvd_h2c_info_addr = cur_pg_addr;
	cur_pg_addr -= fifo->rsvd_drv_pg_num;
	fifo->rsvd_drv_addr = cur_pg_addr;

	if (fifo->rsvd_boundary != fifo->rsvd_drv_addr) {
		PLTFM_MSG_ERR("[ERR] %s rsvd_boundary(%d) != rsvd_drv_addr(%d)\n",
			      __func__, fifo->rsvd_boundary, fifo->rsvd_drv_addr);
		return MACBUFSZ;
	}
	return MACSUCCESS;
}

static u32
priority_queue_cfg(struct mac_adapter *adapter)
{
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct rtw_hal_com_t *hal_com = (struct rtw_hal_com_t *)adapter->drv_adapter;
	struct mac_fifo_info *fifo = &adapter->fifo_info;
	struct rtw_page_table *pg_tbl = NULL;
	int cnt;
	u16 pubq_num;
	u32 rxff_size = adapter->hw_info->rxff_size;
	u32 ret;

	ret = set_trx_fifo_info(adapter);	
	if (ret) {
		PLTFM_MSG_ERR("[ERR] set_trx_fifo_info, ret=%d\n", ret);
		return ret;
	}

	if (hal_com->bulkout_num >= 2 && hal_com->bulkout_num <=4) {
		pg_tbl = &page_table_8822c[hal_com->bulkout_num];
	}
	if (pg_tbl == NULL) {
		PLTFM_MSG_ERR("[ERR] %s usb bulkout num(%d) is not expected \n",
			      __func__, hal_com->bulkout_num);
		return MACCMP;
	}

	pubq_num = fifo->acq_pg_num - pg_tbl->hq_num - pg_tbl->lq_num -
		   pg_tbl->nq_num - pg_tbl->exq_num - pg_tbl->gapq_num;
	MAC_REG_W16(REG_FIFOPAGE_INFO_1, pg_tbl->hq_num);
	MAC_REG_W16(REG_FIFOPAGE_INFO_2, pg_tbl->lq_num);
	MAC_REG_W16(REG_FIFOPAGE_INFO_3, pg_tbl->nq_num);
	MAC_REG_W16(REG_FIFOPAGE_INFO_4, pg_tbl->exq_num);
	MAC_REG_W16(REG_FIFOPAGE_INFO_5, pubq_num);
	MAC_REG_W32_SET(REG_RQPN_CTRL_2, BIT_LD_RQPN);

	MAC_REG_W16(REG_FIFOPAGE_CTRL_2, fifo->rsvd_boundary);
	MAC_REG_W8_SET(REG_FWHW_TXQ_CTRL + 2, BIT_EN_WR_FREE_TAIL >> 16);

	MAC_REG_W16(REG_BCNQ_BDNY_V1, fifo->rsvd_boundary);
	MAC_REG_W16(REG_FIFOPAGE_CTRL_2 + 2, fifo->rsvd_boundary);
	MAC_REG_W16(REG_BCNQ1_BDNY_V1, fifo->rsvd_boundary);
	MAC_REG_W32(REG_RXFF_BNDY, rxff_size - C2H_PKT_BUF - 1);
	MAC_REG_W8_SET(REG_AUTO_LLT_V1, BIT_AUTO_INIT_LLT_V1);

#if MAC_USB_SUPPORT
	MAC_REG_W8_CLR(REG_AUTO_LLT_V1, BIT(4) | BIT(5) | BIT(6) | BIT(7));
	MAC_REG_W8_SET(REG_AUTO_LLT_V1, BIT(4) | BIT(5));

	MAC_REG_W8(REG_AUTO_LLT_V1 + 3, 0x3);
	MAC_REG_W8_SET(REG_TXDMA_OFFSET_CHK + 1, BIT(1));
#endif

	MAC_REG_W8_SET(REG_AUTO_LLT_V1, BIT_AUTO_INIT_LLT_V1);
	cnt = 1000;
	while (MAC_REG_R8(REG_AUTO_LLT_V1) & BIT_AUTO_INIT_LLT_V1) {
		cnt--;
		if (cnt == 0)
			return MACHWERR;
	}

	MAC_REG_W8(REG_CR + 3, 0);
	return MACSUCCESS;
}

static int
init_h2c(struct mac_adapter *adapter)
{
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct rtw_hal_com_t *hal_com = (struct rtw_hal_com_t *)adapter->drv_adapter;
	struct mac_fifo_info *fifo = &adapter->fifo_info;
	u8 value8;
	u32 value32;
	u32 h2cq_addr;
	u32 h2cq_size;
	u32 h2cq_free;
	u32 wp, rp;

	h2cq_addr = fifo->rsvd_h2cq_addr << TX_PAGE_SIZE_SHIFT;
	h2cq_size = RSVD_PG_H2CQ_NUM << TX_PAGE_SIZE_SHIFT;

	value32 = MAC_REG_R32(REG_H2C_HEAD);
	value32 = (value32 & 0xFFFC0000) | h2cq_addr;
	MAC_REG_W32(REG_H2C_HEAD, value32);

	value32 = MAC_REG_R32(REG_H2C_READ_ADDR);
	value32 = (value32 & 0xFFFC0000) | h2cq_addr;
	MAC_REG_W32(REG_H2C_READ_ADDR, value32);

	value32 = MAC_REG_R32(REG_H2C_TAIL);
	value32 &= 0xFFFC0000;
	value32 |= (h2cq_addr + h2cq_size);
	MAC_REG_W32(REG_H2C_TAIL, value32);

	value8 = MAC_REG_R8(REG_H2C_INFO);
	value8 = (u8)((value8 & 0xFC) | 0x01);
	MAC_REG_W8(REG_H2C_INFO, value8);

	value8 = MAC_REG_R8(REG_H2C_INFO);
	value8 = (u8)((value8 & 0xFB) | 0x04);
	MAC_REG_W8(REG_H2C_INFO, value8);

	value8 = MAC_REG_R8(REG_TXDMA_OFFSET_CHK + 1);
	value8 = (u8)((value8 & 0x7f) | 0x80);
	MAC_REG_W8(REG_TXDMA_OFFSET_CHK + 1, value8);

	wp = MAC_REG_R32(REG_H2C_PKT_WRITEADDR) & 0x3FFFF;
	rp = MAC_REG_R32(REG_H2C_PKT_READADDR) & 0x3FFFF;
	h2cq_free = wp >= rp ? h2cq_size - (wp - rp) : rp - wp;

	if (h2cq_size != h2cq_free) {
		PLTFM_MSG_ERR("[ERR] H2C queue mismatch\n");
		PLTFM_MSG_ERR("[ERR] wp:0x%x, rp:0x%x\n", wp, rp);
		PLTFM_MSG_ERR("[ERR] h2cq_size:0x%x, h2cq_free:0x%x\n", h2cq_size, h2cq_free);
		return MACBUFSZ;
	}

	adapter->last_hmebox_num = 0;
	return MACSUCCESS;
}

static u32
fwff_is_empty(struct mac_adapter *adapter)
{
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 cnt;

	cnt = 5000;
	while (MAC_REG_R16(REG_FWFF_CTRL) !=
		MAC_REG_R16(REG_FWFF_PKT_INFO)) {
		if (cnt == 0) {
			PLTFM_MSG_ERR("[ERR]polling fwff empty fail\n");
			return MACFFCFG;
		}
		cnt--;
		PLTFM_DELAY_US(50);
	}
	return MACSUCCESS;
}

u32 mac_sys_init(struct mac_adapter *adapter)
{
	u32 ret = MACSUCCESS;

	pr_info("%s NEO TODO\n", __func__);
	return ret;
}

static void init_txq_ctrl(struct mac_adapter *adapter)
{
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);
	u8 value8;

	value8 = MAC_REG_R8(REG_FWHW_TXQ_CTRL);
	value8 |= (BIT(7) & ~BIT(1) & ~BIT(2));
	MAC_REG_W8(REG_FWHW_TXQ_CTRL, value8);

	MAC_REG_W8(REG_FWHW_TXQ_CTRL + 1, 0x1F);
}


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

#define WLAN_SIFS_CFG	(WLAN_SIFS_CCK_CONT_TX | \
			(WLAN_SIFS_OFDM_CONT_TX << BIT_SHIFT_SIFS_OFDM_CTX) | \
			(WLAN_SIFS_CCK_TRX << BIT_SHIFT_SIFS_CCK_TRX) | \
			(WLAN_SIFS_OFDM_TRX << BIT_SHIFT_SIFS_OFDM_TRX))

#define WLAN_SIFS_DUR_TUNE	(WLAN_SIFS_CCK_DUR_TUNE | \
				(WLAN_SIFS_OFDM_DUR_TUNE << 8))

static void init_sifs_ctrl(struct mac_adapter *adapter)
{
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);

	MAC_REG_W16(REG_SPEC_SIFS, WLAN_SIFS_DUR_TUNE);
	MAC_REG_W32(REG_SIFS, WLAN_SIFS_CFG);
	MAC_REG_W16(REG_RESP_SIFS_CCK,
		       WLAN_SIFS_CCK_CTX | WLAN_SIFS_CCK_IRX << 8);
	MAC_REG_W16(REG_RESP_SIFS_OFDM,
		       WLAN_SIFS_OFDM_CTX | WLAN_SIFS_OFDM_IRX << 8);
}


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

static void init_rate_fallback_ctrl(struct mac_adapter *adapter)
{
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);

	MAC_REG_W32(REG_DARFRC, WLAN_DATA_RATE_FB_CNT_1_4);
	MAC_REG_W32(REG_DARFRCH, WLAN_DATA_RATE_FB_CNT_5_8);
	MAC_REG_W32(REG_RARFRCH, WLAN_RTS_RATE_FB_CNT_5_8);

	MAC_REG_W32(REG_ARFR0, WLAN_DATA_RATE_FB_RATE0);
	MAC_REG_W32(REG_ARFRH0, WLAN_DATA_RATE_FB_RATE0_H);
	MAC_REG_W32(REG_ARFR1_V1, WLAN_RTS_RATE_FB_RATE1);
	MAC_REG_W32(REG_ARFRH1_V1, WLAN_RTS_RATE_FB_RATE1_H);
	MAC_REG_W32(REG_ARFR4, WLAN_RTS_RATE_FB_RATE4);
	MAC_REG_W32(REG_ARFRH4, WLAN_RTS_RATE_FB_RATE4_H);
	MAC_REG_W32(REG_ARFR5, WLAN_RTS_RATE_FB_RATE5);
	MAC_REG_W32(REG_ARFRH5, WLAN_RTS_RATE_FB_RATE5_H);
}


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

/* 2 REG_RRSR_8822C */
#define BIT_SHIFT_RRSR_RSC_8822C 21
#define BIT_MASK_RRSR_RSC_8822C 0x3
#define BIT_RRSR_RSC_8822C(x)                                                  \
	(((x) & BIT_MASK_RRSR_RSC_8822C) << BIT_SHIFT_RRSR_RSC_8822C)
#define BITS_RRSR_RSC_8822C                                                    \
	(BIT_MASK_RRSR_RSC_8822C << BIT_SHIFT_RRSR_RSC_8822C)
#define BIT_CLEAR_RRSR_RSC_8822C(x) ((x) & (~BITS_RRSR_RSC_8822C))
#define BIT_GET_RRSR_RSC_8822C(x)                                              \
	(((x) >> BIT_SHIFT_RRSR_RSC_8822C) & BIT_MASK_RRSR_RSC_8822C)
#define BIT_SET_RRSR_RSC_8822C(x, v)                                           \
	(BIT_CLEAR_RRSR_RSC_8822C(x) | BIT_RRSR_RSC_8822C(v))
#define BIT_SHIFT_RRSC_BITMAP_8822C 0
#define BIT_MASK_RRSC_BITMAP_8822C 0xfffff
#define BIT_RRSC_BITMAP_8822C(x)                                               \
	(((x) & BIT_MASK_RRSC_BITMAP_8822C) << BIT_SHIFT_RRSC_BITMAP_8822C)
#define BITS_RRSC_BITMAP_8822C                                                 \
	(BIT_MASK_RRSC_BITMAP_8822C << BIT_SHIFT_RRSC_BITMAP_8822C)
#define BIT_CLEAR_RRSC_BITMAP_8822C(x) ((x) & (~BITS_RRSC_BITMAP_8822C))
#define BIT_GET_RRSC_BITMAP_8822C(x)                                           \
	(((x) >> BIT_SHIFT_RRSC_BITMAP_8822C) & BIT_MASK_RRSC_BITMAP_8822C)
#define BIT_SET_RRSC_BITMAP_8822C(x, v)                                        \
	(BIT_CLEAR_RRSC_BITMAP_8822C(x) | BIT_RRSC_BITMAP_8822C(v))
static void init_protocol_cfg(struct mac_adapter *adapter)

{
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);
	u8 value8;
	u16 pre_txcnt;
	u32 max_agg_num;
	u32 max_rts_agg_num;
	u32 value32;

	init_txq_ctrl(adapter);
	init_sifs_ctrl(adapter);
	init_rate_fallback_ctrl(adapter);


	MAC_REG_W8(REG_AMPDU_MAX_TIME_V1, WLAN_AMPDU_MAX_TIME);
	MAC_REG_W8_SET(REG_TX_HANG_CTRL, BIT_EN_EOF_V1);

	pre_txcnt = WLAN_PRE_TXCNT_TIME_TH | BIT_EN_PRECNT;
	MAC_REG_W8(REG_PRECNT_CTRL, (u8)(pre_txcnt & 0xFF));
	MAC_REG_W8(REG_PRECNT_CTRL + 1, (u8)(pre_txcnt >> 8));

	max_agg_num = WLAN_MAX_AGG_PKT_LIMIT;
	max_rts_agg_num = WLAN_RTS_MAX_AGG_PKT_LIMIT;
	value32 = WLAN_RTS_LEN_TH | (WLAN_RTS_TX_TIME_TH << 8) |
				(max_agg_num << 16) | (max_rts_agg_num << 24);
	MAC_REG_W32(REG_PROT_MODE_CTRL, value32);

	MAC_REG_W16(REG_BAR_MODE_CTRL + 2,
		       WLAN_BAR_RETRY_LIMIT | WLAN_RA_TRY_RATE_AGG_LIMIT << 8);

	MAC_REG_W8(REG_FAST_EDCA_VOVI_SETTING, WALN_FAST_EDCA_VO_TH);
	MAC_REG_W8(REG_FAST_EDCA_VOVI_SETTING + 2, WLAN_FAST_EDCA_VI_TH);
	MAC_REG_W8(REG_FAST_EDCA_BEBK_SETTING, WLAN_FAST_EDCA_BE_TH);
	MAC_REG_W8(REG_FAST_EDCA_BEBK_SETTING + 2, WLAN_FAST_EDCA_BK_TH);

	/*close A/B/C/D-cut BA parser*/
	MAC_REG_W8_CLR(REG_LIFETIME_EN, BIT(5));

	/*Bypass TXBF error protection due to sounding failure*/
	value32 = MAC_REG_R32(REG_BF0_TIME_SETTING) & (~BIT_BF0_UPDATE_EN);
	MAC_REG_W32(REG_BF0_TIME_SETTING, value32 | BIT_BF0_TIMER_EN);
	value32 = MAC_REG_R32(REG_BF1_TIME_SETTING) & (~BIT_BF1_UPDATE_EN);
	MAC_REG_W32(REG_BF1_TIME_SETTING, value32 | BIT_BF1_TIMER_EN);
	value32 = MAC_REG_R32(REG_BF_TIMEOUT_EN) & (~BIT_BF0_TIMEOUT_EN) &
		 (~BIT_BF1_TIMEOUT_EN);
	MAC_REG_W32(REG_BF_TIMEOUT_EN, value32);

	/*Fix incorrect HW default value of RSC*/
	value32 = BIT_CLEAR_RRSR_RSC_8822C(MAC_REG_R32(REG_RRSR));
	MAC_REG_W32(REG_RRSR, value32);

	value8 = MAC_REG_R8(REG_INIRTS_RATE_SEL);
	MAC_REG_W8(REG_INIRTS_RATE_SEL, value8 | BIT(5));
}


static void cfg_mac_clk(struct mac_adapter *adapter)
{
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 value32;

	value32 = MAC_REG_R32(REG_AFE_CTRL1) & ~(BIT(20) | BIT(21));
	value32 |= (MAC_CLK_HW_DEF_80M << BIT_SHIFT_MAC_CLK_SEL);
	MAC_REG_W32(REG_AFE_CTRL1, value32);

	MAC_REG_W8(REG_USTIME_TSF, MAC_CLK_SPEED);
	MAC_REG_W8(REG_USTIME_EDCA, MAC_CLK_SPEED);
}

#define WLAN_EDCA_VO_PARAM	0x002FA226
#define WLAN_EDCA_VI_PARAM	0x005EA328
#define WLAN_EDCA_BE_PARAM	0x005EA42B
#define WLAN_EDCA_BK_PARAM	0x0000A44F

#define WLAN_TBTT_TIME	(WLAN_TBTT_PROHIBIT |\
			(WLAN_TBTT_HOLD_TIME << BIT_SHIFT_TBTT_HOLD_TIME_AP))

#define WLAN_NAV_CFG		(WLAN_RDG_NAV | (WLAN_TXOP_NAV << 16))
#define WLAN_RX_TSF_CFG		(WLAN_CCK_RX_TSF | (WLAN_OFDM_RX_TSF) << 8)

static  void init_edca_cfg(struct mac_adapter *adapter)
{
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);
	u8 value8;

	MAC_REG_W32(REG_EDCA_VO_PARAM, WLAN_EDCA_VO_PARAM);
	MAC_REG_W32(REG_EDCA_VI_PARAM, WLAN_EDCA_VI_PARAM);
	MAC_REG_W32(REG_EDCA_BE_PARAM, WLAN_EDCA_BE_PARAM);
	MAC_REG_W32(REG_EDCA_BK_PARAM, WLAN_EDCA_BK_PARAM);

	MAC_REG_W8(REG_PIFS, WLAN_PIFS_TIME);

	MAC_REG_W8_CLR(REG_TX_PTCL_CTRL + 1, BIT(4));

	value8 = MAC_REG_R8(REG_RD_CTRL + 1);
	value8 = (value8 | BIT(0) | BIT(1) | BIT(2));
	MAC_REG_W8(REG_RD_CTRL + 1, value8);

	cfg_mac_clk(adapter);

	value8 = MAC_REG_R8(REG_MISC_CTRL);
	value8 = (value8 | BIT(3) | BIT(1) | BIT(0));
	MAC_REG_W8(REG_MISC_CTRL, value8);

	/* Init SYNC_CLI_SEL : reg 0x5B4[6:4] = 0 */
	MAC_REG_W8_CLR(REG_TIMER0_SRC_SEL, BIT(4) | BIT(5) | BIT(6));

	/* Clear TX pause */
	MAC_REG_W16(REG_TXPAUSE, 0x0000);

	MAC_REG_W8(REG_SLOT, WLAN_SLOT_TIME);

	MAC_REG_W32(REG_RD_NAV_NXT, WLAN_NAV_CFG);
	MAC_REG_W16(REG_RXTSF_OFFSET_CCK, WLAN_RX_TSF_CFG);

	/* Set beacon cotnrol - enable TSF and other related functions */
	MAC_REG_W8(REG_BCN_CTRL, (u8)(MAC_REG_R8(REG_BCN_CTRL) |
					  BIT_EN_BCN_FUNCTION));

	/* Set send beacon related registers */
	MAC_REG_W32(REG_TBTT_PROHIBIT, WLAN_TBTT_TIME);
	MAC_REG_W8(REG_DRVERLYINT, WLAN_DRV_EARLY_INT);
	MAC_REG_W8(REG_BCN_CTRL_CLINT0, WLAN_BCN_CTRL_CLT0);
	MAC_REG_W8(REG_BCNDMATIM, WLAN_BCN_DMA_TIME);
	MAC_REG_W8(REG_BCN_MAX_ERR, WLAN_BCN_MAX_ERR);

	/* MU primary packet fail, BAR packet will not issue */
	MAC_REG_W8_SET(REG_BAR_TX_CTRL, BIT(0));
}

#define BIT_SHIFT_RXPSF_PKTLENTHR 13
#define BIT_MASK_RXPSF_PKTLENTHR 0x7
#define BIT_RXPSF_PKTLENTHR(x)                                                 \
	(((x) & BIT_MASK_RXPSF_PKTLENTHR) << BIT_SHIFT_RXPSF_PKTLENTHR)
#define BITS_RXPSF_PKTLENTHR                                                   \
	(BIT_MASK_RXPSF_PKTLENTHR << BIT_SHIFT_RXPSF_PKTLENTHR)
#define BIT_CLEAR_RXPSF_PKTLENTHR(x) ((x) & (~BITS_RXPSF_PKTLENTHR))
#define BIT_GET_RXPSF_PKTLENTHR(x)                                             \
	(((x) >> BIT_SHIFT_RXPSF_PKTLENTHR) & BIT_MASK_RXPSF_PKTLENTHR)
#define BIT_SET_RXPSF_PKTLENTHR(x, v)                                          \
	(BIT_CLEAR_RXPSF_PKTLENTHR(x) | BIT_RXPSF_PKTLENTHR(v))

#define BIT_RXPSF_CTRLEN BIT(12)
#define BIT_RXPSF_VHTCHKEN BIT(11)
#define BIT_RXPSF_HTCHKEN BIT(10)
#define BIT_RXPSF_OFDMCHKEN BIT(9)
#define BIT_RXPSF_CCKCHKEN BIT(8)
#define BIT_RXPSF_OFDMRST BIT(7)
#define BIT_RXPSF_CCKRST BIT(6)
#define BIT_RXPSF_MHCHKEN BIT(5)
#define BIT_RXPSF_CONT_ERRCHKEN BIT(4)
#define BIT_RXPSF_ALL_ERRCHKEN BIT(3)

static void init_low_pwr(struct mac_adapter *adapter)
{
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);
	u16 value16;

	/*RXGCK FIFO threshold CFG*/
	value16 = (MAC_REG_R16(REG_RXPSF_CTRL + 2) & 0xF00F);
	value16 |= BIT(10) | BIT(8) | BIT(6) | BIT(4);
	MAC_REG_W16(REG_RXPSF_CTRL + 2, value16);

	/*invalid_pkt CFG*/
	value16 = 0;
	value16 = BIT_SET_RXPSF_PKTLENTHR(value16, 1);
	value16 |= BIT_RXPSF_CTRLEN | BIT_RXPSF_VHTCHKEN | BIT_RXPSF_HTCHKEN
		| BIT_RXPSF_OFDMCHKEN | BIT_RXPSF_CCKCHKEN
		| BIT_RXPSF_OFDMRST;

	MAC_REG_W16(REG_RXPSF_CTRL, value16);
	MAC_REG_W32(REG_RXPSF_TYPE_CTRL, 0xFFFFFFFF);

}

#define WLAN_EIFS_DUR_TUNE 0x40

#define WLAN_RX_FILTER0		0xFFFFFFFF
#define WLAN_RX_FILTER2		0xFFFF
#define WLAN_RCR_CFG		0xE410220E
#define WLAN_RXPKT_MAX_SZ	12288
#define WLAN_RXPKT_MAX_SZ_512	(WLAN_RXPKT_MAX_SZ >> 9)

#define WLAN_TX_FUNC_CFG1		0x30
#define WLAN_TX_FUNC_CFG2		0x30
#define WLAN_MAC_OPT_NORM_FUNC1		0x98
#define WLAN_MAC_OPT_LB_FUNC1		0x80
#define WLAN_MAC_OPT_FUNC2		0xB1810041

static void init_wmac_cfg(struct mac_adapter *adapter)
{
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);
	u8 value8;

	MAC_REG_W32(REG_MAR, 0xFFFFFFFF);
	MAC_REG_W32(REG_MAR + 4, 0xFFFFFFFF);

	MAC_REG_W8(REG_BBPSF_CTRL + 2, WLAN_RESP_TXRATE);
	MAC_REG_W8(REG_ACKTO, WLAN_ACK_TO);
	MAC_REG_W8(REG_ACKTO_CCK, WLAN_ACK_TO_CCK);
	MAC_REG_W16(REG_EIFS, WLAN_EIFS_DUR_TUNE);

	MAC_REG_W8(REG_NAV_CTRL + 2, WLAN_NAV_MAX);

	MAC_REG_W8_SET(REG_WMAC_TRXPTCL_CTL_H, BIT_EN_TXCTS_IN_RXNAV_V1);
	MAC_REG_W8(REG_WMAC_TRXPTCL_CTL_H  + 2, WLAN_BAR_ACK_TYPE);

	MAC_REG_W32(REG_RXFLTMAP0, WLAN_RX_FILTER0);
	MAC_REG_W16(REG_RXFLTMAP2, WLAN_RX_FILTER2);

	MAC_REG_W32(REG_RCR, WLAN_RCR_CFG);
	value8 = MAC_REG_R8(REG_RXPSF_CTRL + 2);
	value8 = value8 | 0xe;
	MAC_REG_W8(REG_RXPSF_CTRL + 2, value8);

	MAC_REG_W8(REG_RX_PKT_LIMIT, WLAN_RXPKT_MAX_SZ_512);

	MAC_REG_W8(REG_TCR + 2, WLAN_TX_FUNC_CFG2);
	MAC_REG_W8(REG_TCR + 1, WLAN_TX_FUNC_CFG1);

	MAC_REG_W16_SET(REG_GENERAL_OPTION,
			   BIT_DUMMY_FCS_READY_MASK_EN | BIT_RXFIFO_GNT_CUT);

	MAC_REG_W8_SET(REG_SND_PTCL_CTRL, BIT_R_DISABLE_CHECK_VHTSIGB_CRC);

	MAC_REG_W32(REG_WMAC_OPTION_FUNCTION_2, WLAN_MAC_OPT_FUNC2);

	value8 = WLAN_MAC_OPT_NORM_FUNC1;

	MAC_REG_W8(REG_WMAC_OPTION_FUNCTION_1, value8);

	init_low_pwr(adapter);	
}

u32 mac_trx_init(struct mac_adapter *adapter)
{
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);
	u8 en_fwff, value8;
	u16 value16;
	u32 ret = MACSUCCESS;

	ret = txdma_queue_mapping(adapter);
	if (ret) {
		PLTFM_MSG_ERR("[ERR] txdma_queue_mapping, ret=%d\n", ret);
		goto out;
	}

	en_fwff = MAC_REG_R8(REG_WMAC_FWPKT_CR) & BIT_FWEN;
	if (en_fwff) {
		MAC_REG_W8_CLR(REG_WMAC_FWPKT_CR, BIT_FWEN);
		if (fwff_is_empty(adapter) != MACSUCCESS)
			PLTFM_MSG_ERR("[ERR]fwff is not empty\n");
	}

	value8 = 0;
	MAC_REG_W8(REG_CR, value8);
	value16 = MAC_REG_R16(REG_FWFF_PKT_INFO);
	MAC_REG_W16(REG_FWFF_CTRL, value16);

	value8 = MAC_TRX_ENABLE;
	MAC_REG_W8(REG_CR, value8);
	if (en_fwff)
		MAC_REG_W8_SET(REG_WMAC_FWPKT_CR, BIT_FWEN);
	MAC_REG_W32(REG_H2CQ_CSR, BIT(31));

	ret = priority_queue_cfg(adapter);
	if (ret) {
		PLTFM_MSG_ERR("[ERR] priority_queue_cfg, ret=%d\n", ret);
		return ret;
	}

	ret = init_h2c(adapter);
	if (ret) {
		PLTFM_MSG_ERR("[ERR] int_h2c, ret=%d\n", ret);
		return ret;
	}

	init_protocol_cfg(adapter);
	init_edca_cfg(adapter);
	init_wmac_cfg(adapter);

out:
	return ret;
}


static u32
mac_init(struct mac_adapter *adapter)
{
	u32 ret;

	ret = mac_trx_init(adapter);
	if (ret) {
		PLTFM_MSG_ERR("[ERR] mac trx init, ret=%d\n", ret);
		return ret;
	}

	return ret;
}

u32 mac_set_rts_full(struct mac_adapter *adapter, bool enable)
{
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);
	u8 value8;

	pr_info("%s NEO enable=%d\n", __func__, enable);

	value8 = MAC_REG_R8(REG_INIRTS_RATE_SEL);

	if (enable)
		MAC_REG_W8(REG_INIRTS_RATE_SEL, value8 | BIT(5));
	else
		MAC_REG_W8(REG_INIRTS_RATE_SEL, value8 & ~(BIT(5)));

	return MACSUCCESS;
}

#if 0 // NEO
u32 dmac_func_en(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
#if MAC_AX_PCIE_SUPPORT
	//enum mac_ax_intf intf = adapter->hw_info->intf;
#endif
	u32 val32;
	u32 ret = 0;

	val32 = (B_AX_MAC_FUNC_EN | B_AX_DMAC_FUNC_EN | B_AX_MAC_SEC_EN |
		 B_AX_DISPATCHER_EN | B_AX_DLE_CPUIO_EN | B_AX_PKT_IN_EN |
		 B_AX_DMAC_TBL_EN | B_AX_PKT_BUF_EN | B_AX_STA_SCH_EN |
		 B_AX_TXPKT_CTRL_EN | B_AX_WD_RLS_EN | B_AX_MPDU_PROC_EN);
	MAC_REG_W32(R_AX_DMAC_FUNC_EN, val32);

	val32 = (B_AX_MAC_SEC_CLK_EN | B_AX_DISPATCHER_CLK_EN |
		 B_AX_DLE_CPUIO_CLK_EN | B_AX_PKT_IN_CLK_EN |
		 B_AX_STA_SCH_CLK_EN | B_AX_TXPKT_CTRL_CLK_EN |
		 B_AX_WD_RLS_CLK_EN);
	MAC_REG_W32(R_AX_DMAC_CLK_EN, val32);

	return ret;
}

u32 cmac_func_en(struct mac_ax_adapter *adapter, u8 band, u8 en)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 val32_func_en = 0;
	u32 val32_ck_en = 0;
	u32 val32_c1pc_en = 0;
	u32 addrl_func_en[] = {R_AX_CMAC_FUNC_EN, R_AX_CMAC_FUNC_EN_C1};
	u32 addrl_ck_en[] = {R_AX_CK_EN, R_AX_CK_EN_C1};

	val32_func_en = B_AX_CMAC_EN | B_AX_CMAC_TXEN | B_AX_CMAC_RXEN |
			B_AX_PHYINTF_EN | B_AX_CMAC_DMA_EN | B_AX_PTCLTOP_EN |
			B_AX_SCHEDULER_EN | B_AX_TMAC_EN | B_AX_RMAC_EN;
	val32_ck_en = B_AX_CMAC_CKEN | B_AX_PHYINTF_CKEN | B_AX_CMAC_DMA_CKEN |
		      B_AX_PTCLTOP_CKEN | B_AX_SCHEDULER_CKEN | B_AX_TMAC_CKEN |
		      B_AX_RMAC_CKEN;
	val32_c1pc_en = B_AX_R_SYM_WLCMAC1_PC_EN |
			B_AX_R_SYM_WLCMAC1_P1_PC_EN |
			B_AX_R_SYM_WLCMAC1_P2_PC_EN |
			B_AX_R_SYM_WLCMAC1_P3_PC_EN |
			B_AX_R_SYM_WLCMAC1_P4_PC_EN;

	if (band >= MAC_AX_BAND_NUM) {
		PLTFM_MSG_ERR("band %d invalid\n", band);
		return MACFUNCINPUT;
	}

	if (en) {
		if (band == MAC_AX_BAND_1) {
			MAC_REG_W32(R_AX_AFE_CTRL1,
				    MAC_REG_R32(R_AX_AFE_CTRL1) |
				    val32_c1pc_en);
			MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND,
				    MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND) &
				    ~B_AX_R_SYM_ISO_CMAC12PP);
			MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND,
				    MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND) |
				    B_AX_CMAC1_FEN);
		}
		MAC_REG_W32(addrl_ck_en[band],
			    MAC_REG_R32(addrl_ck_en[band]) | val32_ck_en);
		MAC_REG_W32(addrl_func_en[band],
			    MAC_REG_R32(addrl_func_en[band]) | val32_func_en);
	} else {
		MAC_REG_W32(addrl_func_en[band],
			    MAC_REG_R32(addrl_func_en[band]) & ~val32_func_en);
		MAC_REG_W32(addrl_ck_en[band],
			    MAC_REG_R32(addrl_ck_en[band]) & ~val32_ck_en);
		if (band == MAC_AX_BAND_1) {
			MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND,
				    MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND) &
				    ~B_AX_CMAC1_FEN);
			MAC_REG_W32(R_AX_SYS_ISO_CTRL_EXTEND,
				    MAC_REG_R32(R_AX_SYS_ISO_CTRL_EXTEND) |
				    B_AX_R_SYM_ISO_CMAC12PP);
			MAC_REG_W32(R_AX_AFE_CTRL1,
				    MAC_REG_R32(R_AX_AFE_CTRL1) &
				    ~val32_c1pc_en);
		}
	}
//Reset BACAM chunchu
//RMAC reg0x3C[1:0] = 2'b10
	return MACSUCCESS;
}

u32 chip_func_en(struct mac_ax_adapter *adapter)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	u32 val32;
	u32 ret = MACSUCCESS;

	if (is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) ||
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852B)) {
		/* patch for OCP */
		val32 = MAC_REG_R32(R_AX_SPSLDO_ON_CTRL0);
		val32 |= SET_WOR2(B_AX_OCP_L1_MSK, B_AX_OCP_L1_SH,
				  B_AX_OCP_L1_MSK);
		MAC_REG_W32(R_AX_SPSLDO_ON_CTRL0, val32);
	}

	return ret;
}

u32 mac_sys_init(struct mac_ax_adapter *adapter)
{
	u32 ret;

	ret = dmac_func_en(adapter);
	if (ret)
		return ret;

	ret = cmac_func_en(adapter, MAC_AX_BAND_0, MAC_AX_FUNC_EN);
	if (ret)
		return ret;

	ret = chip_func_en(adapter);
	if (ret)
		return ret;

	return ret;
}

#endif //NEO

static u32
wait_txfifo_empty(struct mac_adapter *adapter)
{
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 cnt = 1000;
	u32 ret = MACSUCCESS;

	do {
		if (MAC_REG_R8(REG_TXPKT_EMPTY) != 0xFF)
			goto chk_failed;

		if ((MAC_REG_R8(REG_TXPKT_EMPTY + 1) & 0x06) != 0x06)
			goto chk_failed;

		ret = MACSUCCESS;
		break;
chk_failed:
		ret = MACFFCFG;
		PLTFM_DELAY_MS(2);
		cnt--;
	} while (cnt != 0);

	return ret;
}

u32 mac_enable_fw(struct mac_adapter *adapter)
{
	struct mac_ops *mac_ops = adapter_to_mac_ops(adapter);
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ret;

	pr_info("%s NEO mac_enable_fw\n", __func__);

	ret = wait_txfifo_empty(adapter);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: wait_txfifo_empty fail\n", __func__);
		return ret;
	}

	ret = mac_ops->fwdl(adapter);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: mac_fwdl fail\n", __func__);
		return ret;
	}

	return ret;
}


u32 mac_check_h2cq_fifo(struct mac_adapter *adapter)
{
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_fifo_info *fifo = &adapter->fifo_info;
	u8 cnt;
	u8 value8;
	u16 value16;
	u32 i;
	u32 start_pg;
	u32 residue;
	u32 value32;
	u32 h2cq_addr;
	u32 h2cq_ele;
	u32 ret = MACSUCCESS;

	h2cq_addr = fifo->rsvd_h2cq_addr << TX_PAGE_SIZE_SHIFT;

	cnt = 100;
	do {
		// disable clk gate
		value8 = MAC_REG_R8(REG_RCR + 2);
		MAC_REG_W8(REG_RCR + 2, value8 | BIT(3));

		start_pg = h2cq_addr >> 12;
		start_pg += 0x780;
		residue = h2cq_addr & (4096 - 1);

		value16 = MAC_REG_R16(REG_PKTBUF_DBG_CTRL) & 0xF000;

		MAC_REG_W16(REG_PKTBUF_DBG_CTRL, (u16)(start_pg) | value16);
		h2cq_ele = MAC_REG_R32(0x8000 + residue);
		MAC_REG_W16(REG_PKTBUF_DBG_CTRL, value16);

		// restore for clk gate setting
		MAC_REG_W8(REG_RCR + 2, value8);

		if ((h2cq_ele & 0xFFFF) == 0xFF01)
			goto out;

		PLTFM_DELAY_US(5);
	} while (cnt--);

	PLTFM_MSG_ERR("[ERR]%s: h2cq compare!!\n", __func__);
	ret = MACFFCFG;

out:
	return ret;

}

static bool _is_fw_read_cmd_down(struct mac_adapter *adapter, u8 msgbox_num)
{
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);
	int retry_cnts = 100;
	u8 valid;

	do {
		valid = MAC_REG_R8(REG_HMETFR);
		valid &= BIT(msgbox_num);
		if (!valid)
			return true;

		PLTFM_DELAY_MS(1);
	} while (retry_cnts--);

	return false;
}

u32 mac_send_h2c_reg(struct mac_adapter *adapter, u8 *h2c)
{
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);
	u8 h2c_box_num = adapter->last_hmebox_num;
	u32 msgbox_addr = 0;
	u32 msgbox_ex_addr = 0;
	u32 h2c_cmd = 0;
	u32 h2c_cmd_ex = 0;

	if (!_is_fw_read_cmd_down(adapter, h2c_box_num)) {
		return MACPOLLTO;
	}

	/* Write Ext command (byte 4~7) */
	msgbox_ex_addr = 0x0088 + (h2c_box_num * 4);
	memcpy((u8 *)(&h2c_cmd_ex), h2c + 4, 4);
	MAC_REG_W32(msgbox_ex_addr, h2c_cmd_ex);

	/* Write command (byte 0~3) */
	msgbox_addr = 0x01D0 + (h2c_box_num * 4);
	memcpy((u8 *)(&h2c_cmd), h2c, 4);
	MAC_REG_W32(msgbox_addr, h2c_cmd);

	/* update last msg box number */
	adapter->last_hmebox_num = (h2c_box_num + 1) % 4;

	return MACSUCCESS;
}

u32 mac_send_general_info_reg(struct mac_adapter *adapter)
{
	u8 h2c[8] = {0};

#define CLASS_GENERAL_INFO_REG				0x02
#define CMD_ID_GENERAL_INFO_REG				0x0C
#define GENERAL_INFO_REG_SET_CMD_ID(buf, v)		le32p_replace_bits((__le32 *)(buf), v, GENMASK(4, 0))
#define GENERAL_INFO_REG_SET_CLASS(buf, v)		le32p_replace_bits((__le32 *)(buf), v, GENMASK(7, 5))
#define GENERAL_INFO_REG_SET_RFE_TYPE(buf, v)		le32p_replace_bits((__le32 *)(buf), v, GENMASK(15, 8))
#define GENERAL_INFO_REG_SET_RF_TYPE(buf, v)		le32p_replace_bits((__le32 *)(buf), v, GENMASK(23, 16))
#define GENERAL_INFO_REG_SET_CUT_VERSION(buf, v)	le32p_replace_bits((__le32 *)(buf), v, GENMASK(31, 24))
#define GENERAL_INFO_REG_SET_RX_ANT_STATUS(buf, v)	le32p_replace_bits((__le32 *)(buf)+1, v, GENMASK(3, 0))
#define GENERAL_INFO_REG_SET_TX_ANT_STATUS(buf, v)	le32p_replace_bits((__le32 *)(buf)+1, v, GENMASK(7,4))

	GENERAL_INFO_REG_SET_CMD_ID(h2c, CMD_ID_GENERAL_INFO_REG);
	GENERAL_INFO_REG_SET_CLASS(h2c, CLASS_GENERAL_INFO_REG);
	GENERAL_INFO_REG_SET_RFE_TYPE(h2c, 0); //info->rfe_type
	GENERAL_INFO_REG_SET_RF_TYPE(h2c, 2); //rftype
	GENERAL_INFO_REG_SET_RX_ANT_STATUS(h2c, 3); //info->rx_ant_status
	GENERAL_INFO_REG_SET_TX_ANT_STATUS(h2c, 3); //info->tx_ant_status

	return mac_send_h2c_reg(adapter, h2c);
}

u32 mac_cfg_drv_info(struct mac_adapter *adapter)
{
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);
	u16 value16;

	value16 = MAC_REG_R16(REG_RXPSF_CTRL);

	// disable hdr check
	value16 &= ~(BIT_RXPSF_MHCHKEN);

	// disable fcs error counter
	value16 &= ~(BIT_RXPSF_CONT_ERRCHKEN);

	// disable ccx reset
	value16 &= ~(BIT_RXPSF_CCKRST);

	MAC_REG_W16(REG_RXPSF_CTRL, value16);

	return MACSUCCESS;
}

u32 mac_hal_init(struct mac_adapter *adapter,
		 struct mac_trx_info *trx_info,
		 struct mac_fwdl_info *fwdl_info,
		 struct mac_intf_info *intf_info)
{
	struct mac_ops *mac_ops = adapter_to_mac_ops(adapter);
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_hw_info *hw_info = adapter->hw_info;
	u32 ret;
	u32 rom_addr;

	PLTFM_MUTEX_INIT(&adapter->fw_info.seq_lock);
	PLTFM_MUTEX_INIT(&adapter->hw_info->ind_access_lock);

	ret = mac_ops->pwr_switch(adapter, 1);
	if (ret == MACALRDYON) {
		ret = mac_ops->pwr_switch(adapter, 0);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]pwr_switch 0 fail %d\n", ret);
			return ret;
		}
		ret = mac_ops->pwr_switch(adapter, 1);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]pwr_switch 0->1 fail %d\n", ret);
			return ret;
		}
	}
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]pwr_switch 1 fail %d\n", ret);
		return ret;
	}

	ret = ops->intf_pre_init(adapter, intf_info);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]intf_pre_init %d\n", ret);
		return ret;
	}

	ret = mac_enable_fw(adapter); // RTW_FW_NIC
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]enable_fw %d\n", ret);
		return ret;
	}


	ret = mac_init(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]mac_init %d\n", ret);
		return ret;
	}

	ret = mac_send_general_info_h2c(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]send_general_info failed: %d\n", ret);
		return ret;
	}

	ret = mac_send_phydm_info_h2c(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]send_phydm_info failed: %d\n", ret);
		return ret;
	}

	ret = mac_check_h2cq_fifo(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] check h2cq fifo failed: %d\n", ret);
		return ret;
	}

	ret = mac_send_general_info_reg(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] send general info reg failed: %d\n", ret);
		return ret;
	}

	ret = mac_cfg_drv_info(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR] cfg drv info failed: %d\n", ret);
		return ret;
	}

#if 0 //NEO
	ret = set_enable_bb_rf(adapter, MAC_AX_FUNC_EN);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]set_enable_bb_rf %d\n", ret);
		return ret;
	}

	ret = ops->sys_init(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]sys_init %d\n", ret);
		return ret;
	}

	ret = ops->trx_init(adapter, trx_info);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]trx_init %d\n", ret);
		return ret;
	}

	ret = intf_ops->intf_init(adapter, intf_info);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]intf_init %d\n", ret);
		return ret;
	}
#endif //NEO
	return ret;
}

#if 0 //NEO

u32 mac_hal_deinit(struct mac_ax_adapter *adapter)
{
	struct mac_ax_ops *ops = adapter_to_mac_ops(adapter);
	struct mac_ax_intf_ops *intf_ops = adapter_to_intf_ops(adapter);
	u32 ret;

	ret = free_sec_info_tbl(adapter);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]remove security info tbl\n");
		return ret;
	}

	ret = mac_remove_role_by_band(adapter, MAC_AX_BAND_0, 1);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]remove band0 role fail\n");
		return ret;
	}

	ret = mac_remove_role_by_band(adapter, MAC_AX_BAND_1, 1);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]remove band0 role fail\n");
		return ret;
	}

	ret = intf_ops->intf_deinit(adapter, NULL);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]intf deinit\n");
		return ret;
	}

	ret = ops->pwr_switch(adapter, 0);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]pwr switch off\n");
		return ret;
	}

	PLTFM_MUTEX_DEINIT(&adapter->fw_info.seq_lock);
	PLTFM_MUTEX_DEINIT(&adapter->hw_info->ind_access_lock);

	return ret;
}

#endif //NEO

u32 mac_hal_fast_init(struct mac_adapter *adapter,
		      struct mac_trx_info *trx_info,
		      struct mac_fwdl_info *fwdl_info,
		      struct mac_intf_info *intf_info)
{
	struct mac_ops *mac_ops = adapter_to_mac_ops(adapter);
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_hw_info *hw_info = adapter->hw_info;
	u32 rom_addr;
	u32 ret;

	ret = mac_ops->pwr_switch(adapter, 1);
	if (ret == MACALRDYON) {
		ret = mac_ops->pwr_switch(adapter, 0);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]pwr_switch 0 fail %d\n", ret);
			return ret;
		}
		ret = mac_ops->pwr_switch(adapter, 1);
		if (ret != MACSUCCESS) {
			PLTFM_MSG_ERR("[ERR]pwr_switch 0->1 fail %d\n", ret);
			return ret;
		}
	}
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]pwr_switch 1 fail %d\n", ret);
		return ret;
	}

	ret = ops->intf_pre_init(adapter, intf_info);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]intf_pre_init %d\n", ret);
		return ret;
	}

	/* for efuse hidden rpt */
	MAC_REG_W8(REG_C2HEVT, C2H_DEFEATURE_RSVD);

	ret = mac_enable_fw(adapter); // RTW_FW_NIC
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]enable_fw %d\n", ret);
		return ret;
	}

	ret = mac_init(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]mac_init %d\n", ret);
		return ret;
	}

	return ret;
}

u32 mac_init_state(struct mac_adapter *adapter)
{
	struct mac_state_mach sm = MAC_DFLT_SM;

	adapter->sm = sm;
	adapter->fw_info.h2c_seq = 0;
	adapter->fw_info.rec_seq = 0;

	return MACSUCCESS;
}

