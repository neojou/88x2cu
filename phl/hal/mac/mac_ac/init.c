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

// H2C, C2H : need to move to G6's H2C, fwcmd.c/fwcmd.h, and rewrite to common style
#define C2H_PKT_BUF		256
#define H2C_PKT_SIZE		32
#define H2C_PKT_HDR_SIZE	8

#define H2C_PKT_GENERAL_INFO	0x0D

#if 0 //NEO
#define SET_PKT_H2C_CATEGORY(h2c_pkt, value)                                   \
	le32p_replace_bits((__le32 *)(h2c_pkt) + 0x00, value, GENMASK(6, 0))
#define SET_PKT_H2C_CMD_ID(h2c_pkt, value)                                     \
	le32p_replace_bits((__le32 *)(h2c_pkt) + 0x00, value, GENMASK(15, 8))
#define SET_PKT_H2C_SUB_CMD_ID(h2c_pkt, value)                                 \
	le32p_replace_bits((__le32 *)(h2c_pkt) + 0x00, value, GENMASK(31, 16))
#define SET_PKT_H2C_TOTAL_LEN(h2c_pkt, value)                                  \
	le32p_replace_bits((__le32 *)(h2c_pkt) + 0x01, value, GENMASK(15, 0))

#define GENERAL_INFO_SET_FW_TX_BOUNDARY(h2c_pkt, value)                        \
	le32p_replace_bits((__le32 *)(h2c_pkt) + 0x02, value, GENMASK(23, 16))

static inline void rtw_h2c_pkt_set_header(u8 *h2c_pkt, u8 sub_id)
{
	SET_PKT_H2C_CATEGORY(h2c_pkt, H2C_PKT_CATEGORY);
	SET_PKT_H2C_CMD_ID(h2c_pkt, H2C_PKT_CMD_ID);
	SET_PKT_H2C_SUB_CMD_ID(h2c_pkt, sub_id);
}
#endif //NEO


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

#if 0 //NEO : rtw88 - need to check
	MAC_REG_W8(REG_CR, 0);
	MAC_REG_W8(REG_CR, MAC_TRX_ENABLE);
	MAC_REG_W32(REG_H2CQ_CSR, BIT_H2CQ_FULL);

	MAC_REG_W8_SET(REG_TXDMA_PQ_MAP, BIT_RXDMA_ARBBW_EN);
#endif //NEO

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

	return MACSUCCESS;
}

static u32
mac_init(struct mac_adapter *adapter)
{
	u32 ret;

	ret = txdma_queue_mapping(adapter);
	if (ret) {
		PLTFM_MSG_ERR("[ERR] txdma_queue_mapping, ret=%d\n", ret);
		return ret;
	}

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

	return ret;
}

static u32
send_general_info_fifo(struct mac_adapter *adapter)
{
	pr_info("%s NEO TODO\n", __func__);
	return MACSUCCESS;
#if 0 //NEO
	struct mac_fifo_info *fifo = &adapter->fifo_info;
	u8 h2c_pkt[H2C_PKT_SIZE] = {0};
	u16 total_size = H2C_PKT_HDR_SIZE + 4;

	rtw_h2c_pkt_set_header(h2c_pkt, H2C_PKT_GENERAL_INFO);

	SET_PKT_H2C_TOTAL_LEN(h2c_pkt, total_size);

	GENERAL_INFO_SET_FW_TX_BOUNDARY(h2c_pkt,
					fifo->rsvd_fw_txbuf_addr -
					fifo->rsvd_boundary);

	rtw_fw_send_h2c_packet(rtwdev, h2c_pkt);
#endif
}

static u32
send_general_info(struct mac_adapter *adapter)
{
	u32 ret;


	ret = send_general_info_fifo(adapter);
	if (ret) {
		PLTFM_MSG_ERR("[ERR] send_general_info_fifo, ret=%d\n", ret);
		return ret;
	}

	return ret;
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

	/* for efuse hidden rpt */
	MAC_REG_W8(REG_C2HEVT, C2H_DEFEATURE_RSVD);

	ret = mac_ops->enable_fw(adapter); // RTW_FW_NIC
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]enable_fw %d\n", ret);
		return ret;
	}

	ret = mac_init(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]mac_init %d\n", ret);
		return ret;
	}

	ret = send_general_info(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]mac_init %d\n", ret);
		return ret;
	}


#if 0 //NEO
	if (fwdl_info->fw_en) {
		if (fwdl_info->dlrom_en || fwdl_info->dlram_en) {
			ret = fwdl_pre_init(adapter, trx_info->qta_mode);
			if (ret != MACSUCCESS) {
				PLTFM_MSG_ERR("[ERR]fwdl_pre_init %d\n", ret);
				return ret;
			}
		}
		if (fwdl_info->dlrom_en) {
			switch (hw_info->chip_id) {
			case MAC_AX_CHIP_ID_8852A:
				rom_addr = RTL8852A_ROM_ADDR;
				break;
			case MAC_AX_CHIP_ID_8852B:
				rom_addr = RTL8852B_ROM_ADDR;
				break;
			default:
				PLTFM_MSG_ERR("[ERR]chip id\n");
				return MACNOITEM;
			}
			ret = ops->romdl(adapter,
					 fwdl_info->rom_buff,
					 rom_addr,
					 fwdl_info->rom_size);
			if (ret != MACSUCCESS) {
				PLTFM_MSG_ERR("[ERR]romdl %d\n", ret);
				return ret;
			}
		}

		if (fwdl_info->dlram_en) {
			if (fwdl_info->fw_from_hdr) {
				ret = ops->enable_fw(adapter,
						     fwdl_info->fw_cat);
				if (ret != MACSUCCESS) {
					PLTFM_MSG_ERR("[ERR]enable_fw %d\n",
						      ret);
					return ret;
				}
			} else {
				ret = ops->enable_cpu(adapter, 0,
						      fwdl_info->dlram_en);
				if (ret != MACSUCCESS) {
					PLTFM_MSG_ERR("[ERR]enable_cpu %d\n",
						      ret);
					return ret;
				}

				ret = ops->fwdl(adapter,
						fwdl_info->ram_buff,
						fwdl_info->ram_size);
				if (ret != MACSUCCESS) {
					PLTFM_MSG_ERR("[ERR]fwdl %d\n", ret);
					return ret;
				}
			}
		}
	}

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

	ret = mac_ops->enable_fw(adapter); // RTW_FW_NIC
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]enable_fw %d\n", ret);
		return ret;
	}

	ret = mac_init(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]mac_init %d\n", ret);
		return ret;
	}

	ret = send_general_info(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]mac_init %d\n", ret);
		return ret;
	}

	return ret;
}

#if 0 //NEO

u32 mac_ax_init_state(struct mac_ax_adapter *adapter)
{
	struct mac_ax_state_mach sm = MAC_AX_DFLT_SM;

	adapter->sm = sm;
	adapter->fw_info.h2c_seq = 0;
	adapter->fw_info.rec_seq = 0;

	return MACSUCCESS;
}

#endif // if 0
