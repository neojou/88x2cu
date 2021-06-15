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
#include "_usb.h"
#include "../mac.h"

#if MAC_USB_SUPPORT

u8 reg_read8_usb(struct mac_adapter *adapter, u32 addr)
{
	return PLTFM_REG_R8(addr);
}

void reg_write8_usb(struct mac_adapter *adapter, u32 addr, u8 val)
{
	PLTFM_REG_W8(addr, val);
}

u16 reg_read16_usb(struct mac_adapter *adapter, u32 addr)
{
	return PLTFM_REG_R16(addr);
}

void reg_write16_usb(struct mac_adapter *adapter, u32 addr, u16 val)
{
	PLTFM_REG_W16(addr, val);
}

u32 reg_read32_usb(struct mac_adapter *adapter, u32 addr)
{
	return PLTFM_REG_R32(addr);
}

void reg_write32_usb(struct mac_adapter *adapter, u32 addr, u32 val)
{
	PLTFM_REG_W32(addr, val);
}

enum rtw_tx_queue_type {
	/* the order of AC queues matters */
	RTW_TX_QUEUE_BK = 0x0,
	RTW_TX_QUEUE_BE = 0x1,
	RTW_TX_QUEUE_VI = 0x2,
	RTW_TX_QUEUE_VO = 0x3,

	RTW_TX_QUEUE_BCN = 0x4,
	RTW_TX_QUEUE_MGMT = 0x5,
	RTW_TX_QUEUE_HI0 = 0x6,
	RTW_TX_QUEUE_H2C = 0x7,
	/* keep it last */
	RTK_MAX_TX_QUEUE_NUM
};

u8 get_bulkout_id(struct mac_adapter *adapter, u8 qsel, u8 mode)
{
	u8 bulkout_id = 0;

	switch (qsel) {
	case TX_DESC_QSEL_BEACON:
		bulkout_id = RTW_TX_QUEUE_BCN;
		break;
	case TX_DESC_QSEL_H2C:
		bulkout_id = RTW_TX_QUEUE_H2C;
		break;
	case TX_DESC_QSEL_MGMT:
		bulkout_id = RTW_TX_QUEUE_MGMT;
		break;
	case TX_DESC_QSEL_HIGH:
		bulkout_id = RTW_TX_QUEUE_HI0;
		break;
	case TX_DESC_QSEL_TID6:
	case TX_DESC_QSEL_TID7:
		bulkout_id = RTW_TX_QUEUE_VO;
		break;
	case TX_DESC_QSEL_TID4:
	case TX_DESC_QSEL_TID5:
		bulkout_id = RTW_TX_QUEUE_VI;
		break;
	case TX_DESC_QSEL_TID0:
	case TX_DESC_QSEL_TID3:
		bulkout_id = RTW_TX_QUEUE_BE;
		break;
	case TX_DESC_QSEL_TID1:
	case TX_DESC_QSEL_TID2:
		bulkout_id = RTW_TX_QUEUE_BK;
		break;
	default:
		RTW_ERR("%s NEO qsel=%d unknown\n", __func__, qsel);
		bulkout_id = RTW_TX_QUEUE_BCN;
		break;
	}

	return bulkout_id;
}

u32 usb_pre_init(struct mac_adapter *adapter, void *param)
{
	u32 val32 = 0;

	RTW_INFO("%s TODO NEO\n", __func__);
#if 0 // NEO
	val32 = PLTFM_REG_R32(R_AX_USB_HOST_REQUEST_2) | B_AX_R_USBIO_MODE;
	PLTFM_REG_W32(R_AX_USB_HOST_REQUEST_2, val32);
	// fix USB IO hang suggest by chihhanli@realtek.com

	val32 = PLTFM_REG_R32(R_AX_HCI_FUNC_EN);
	val32 &= ~B_AX_HCI_RXDMA_EN;
	val32 &= ~B_AX_HCI_TXDMA_EN;
	PLTFM_REG_W32(R_AX_HCI_FUNC_EN, val32);
	val32 |= B_AX_HCI_RXDMA_EN;
	val32 |= B_AX_HCI_TXDMA_EN;
	PLTFM_REG_W32(R_AX_HCI_FUNC_EN, val32);
	// fix USB TRX hang suggest by chihhanli@realtek.com

	val32 = PLTFM_REG_R32(R_AX_USB_ENDPOINT_3);
	if ((val32 & B_AX_BULKOUT0) == B_AX_BULKOUT0)
		adapter->usb_info.ep5 = 1;
	if ((val32 & B_AX_BULKOUT1) == B_AX_BULKOUT1)
		adapter->usb_info.ep6 = 1;
	if (((PLTFM_REG_R32(R_AX_USB_ENDPOINT_3) >> B_AX_AC_BULKOUT_SH) &
		B_AX_AC_BULKOUT_MSK) == 1)
		adapter->usb_info.ep10 = 1;
	if (((PLTFM_REG_R32(R_AX_USB_ENDPOINT_3) >> B_AX_AC_BULKOUT_SH) &
		B_AX_AC_BULKOUT_MSK) == 2) {
		adapter->usb_info.ep10 = 1;
		adapter->usb_info.ep11 = 1;
	}
	if (((PLTFM_REG_R32(R_AX_USB_ENDPOINT_3) >> B_AX_AC_BULKOUT_SH) &
		B_AX_AC_BULKOUT_MSK) == 3) {
		adapter->usb_info.ep10 = 1;
		adapter->usb_info.ep11 = 1;
		adapter->usb_info.ep12 = 1;
	}
#endif // if 0 NEO
	return MACSUCCESS;
}

u32 usb_init(struct mac_adapter *adapter, void *param)
{
	u32 val32;
	u8 val8;

	RTW_INFO("%s TODO NEO\n", __func__);
#if 0 // NEO
	adapter->usb_info.max_bulkout_wd_num = GET_FIELD
		(PLTFM_REG_R32(R_AX_CH_PAGE_CTRL), B_AX_PREC_PAGE_CH011);

	val32 = PLTFM_REG_R32(R_AX_USB3_MAC_NPI_CONFIG_INTF_0);
	val32 &= ~B_AX_SSPHY_LFPS_FILTER;
	PLTFM_REG_W32(R_AX_USB3_MAC_NPI_CONFIG_INTF_0, val32);

	if (is_chip_cut(adapter, CHIP_CUT_A) &&
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852A)) {
		val32 = PLTFM_REG_R32(R_AX_USB3_MAC_LINK_0);
		val32 &= ~B_AX_R_DIS_USB3_U1_EN;
		val32 &= ~B_AX_R_DIS_USB3_U2_EN;
		PLTFM_REG_W32(R_AX_USB3_MAC_LINK_0, val32);
	}
	/* else if (is_chip_cut(adapter, CHIP_CUT_B) &&
	 *    is_chip_id(adapter, MAC_AX_CHIP_ID_8852A)) {
	 *	val32 = PLTFM_REG_R32(R_AX_USB3_MAC_LINK_0);
	 *	val32 &= ~B_AX_R_DIS_USB3_U1_EN;
	 *	val32 &= ~B_AX_R_DIS_USB3_U2_EN;
	 *	PLTFM_REG_W32(R_AX_USB3_MAC_LINK_0, val32);
	 *} else {
	 *	val32 = PLTFM_REG_R32(R_AX_USB3_MAC_LINK_0);
	 *	val32 |= B_AX_R_DIS_USB3_U1_EN;
	 *	val32 |= B_AX_R_DIS_USB3_U2_EN;
	 *	PLTFM_REG_W32(R_AX_USB3_MAC_LINK_0, val32);
	 *}
	 */
	val32 = get_usb_mode(adapter);
	if (val32 == USB3)
		PLTFM_REG_W8(R_AX_RXDMA_SETTING, USB3_BULKSIZE);
	else if (val32 == USB2)
		PLTFM_REG_W8(R_AX_RXDMA_SETTING, USB2_BULKSIZE);
	else if (val32 == USB11)
		PLTFM_REG_W8(R_AX_RXDMA_SETTING, USB11_BULKSIZE);
	else
		return MACHWNOSUP;

	val8 = PLTFM_REG_R8(R_AX_USB_ENDPOINT_0);
	val8 = SET_CLR_WORD(val8, EP5, B_AX_EP_IDX);
	PLTFM_REG_W8(R_AX_USB_ENDPOINT_0, val8);
	PLTFM_REG_W8(R_AX_USB_ENDPOINT_2 + 1, NUMP);
	val8 = PLTFM_REG_R8(R_AX_USB_ENDPOINT_0);
	val8 = SET_CLR_WORD(val8, EP6, B_AX_EP_IDX);
	PLTFM_REG_W8(R_AX_USB_ENDPOINT_0, val8);
	PLTFM_REG_W8(R_AX_USB_ENDPOINT_2 + 1, NUMP);
	val8 = PLTFM_REG_R8(R_AX_USB_ENDPOINT_0);
	val8 = SET_CLR_WORD(val8, EP7, B_AX_EP_IDX);
	PLTFM_REG_W8(R_AX_USB_ENDPOINT_0, val8);
	PLTFM_REG_W8(R_AX_USB_ENDPOINT_2 + 1, NUMP);
	val8 = PLTFM_REG_R8(R_AX_USB_ENDPOINT_0);
	val8 = SET_CLR_WORD(val8, EP9, B_AX_EP_IDX);
	PLTFM_REG_W8(R_AX_USB_ENDPOINT_0, val8);
	PLTFM_REG_W8(R_AX_USB_ENDPOINT_2 + 1, NUMP);
	val8 = PLTFM_REG_R8(R_AX_USB_ENDPOINT_0);
	val8 = SET_CLR_WORD(val8, EP10, B_AX_EP_IDX);
	PLTFM_REG_W8(R_AX_USB_ENDPOINT_0, val8);
	PLTFM_REG_W8(R_AX_USB_ENDPOINT_2 + 1, NUMP);
	val8 = PLTFM_REG_R8(R_AX_USB_ENDPOINT_0);
	val8 = SET_CLR_WORD(val8, EP11, B_AX_EP_IDX);
	PLTFM_REG_W8(R_AX_USB_ENDPOINT_0, val8);
	PLTFM_REG_W8(R_AX_USB_ENDPOINT_2 + 1, NUMP);
	val8 = PLTFM_REG_R8(R_AX_USB_ENDPOINT_0);
	val8 = SET_CLR_WORD(val8, EP12, B_AX_EP_IDX);
	PLTFM_REG_W8(R_AX_USB_ENDPOINT_0, val8);
	PLTFM_REG_W8(R_AX_USB_ENDPOINT_2 + 1, NUMP);
#endif // if 0 NEO
	return MACSUCCESS;
}

u32 usb_deinit(struct mac_adapter *adapter, void *param)
{
	return MACSUCCESS;
}

u32 usb_flush_mode(struct mac_adapter *adapter, u8 mode)
{
	u32 cnt, val32;

	RTW_INFO("%s TODO NEO\n", __func__);
#if 0 // NEO
	if (mode == MAC_AX_FUNC_DIS) {
		val32 = PLTFM_REG_R32(R_AX_USB_WLAN0_1) & ~B_AX_USBRX_RST
			& ~B_AX_USBTX_RST;
		PLTFM_REG_W32(R_AX_USB_WLAN0_1, val32);

		val32 = PLTFM_REG_R32(R_AX_RX_FUNCTION_STOP);
		val32 &= ~B_AX_HDR_RX_STOP;
		PLTFM_REG_W32(R_AX_RX_FUNCTION_STOP, val32);
		return MACSUCCESS;
	} else if (mode == MAC_AX_FUNC_EN) {
		val32 = PLTFM_REG_R32(R_AX_RX_FUNCTION_STOP);
		val32 |= B_AX_HDR_RX_STOP;
		PLTFM_REG_W32(R_AX_RX_FUNCTION_STOP, val32);
		cnt = 2000;
		while (cnt--) {
			val32 = PLTFM_REG_R32(R_AX_HDP_DBG_INFO_10);
			if (GET_FIELD(val32, B_AX_DMA_ST_HDR_HDP) |
				GET_FIELD(val32, B_AX_RX_ST_HDR_HDP))
				break;
			PLTFM_DELAY_US(1);
		}
		if (cnt == 0)
			return MACLV1STEPERR;
		val32 = PLTFM_REG_R32(R_AX_USB_WLAN0_1) | B_AX_USBRX_RST
			| B_AX_USBTX_RST;
		PLTFM_REG_W32(R_AX_USB_WLAN0_1, val32);
		return MACSUCCESS;
	} else {
		return MACLV1STEPERR;
	}
#endif // if 0 NEO
	return MACSUCCESS;
}

u32 read_usb2phy_para(struct mac_adapter *adapter, u16 offset)
{
	u32 value32 = 0;
	u8 rdata = 0;

	RTW_INFO("%s TODO NEO\n", __func__);
#if 0 // NEO
	value32 = SET_CLR_WORD(value32, offset - phyoffset,
			       B_AX_USB_SIE_INTF_ADDR);
	value32 |= B_AX_USB_REG_SEL;
	value32 |= B_AX_USB_REG_EN;
	value32 |= B_AX_USB_REG_STATUS;
	PLTFM_REG_W32(R_AX_USB_SIE_INTF, value32);

	while (PLTFM_REG_R32(R_AX_USB_SIE_INTF) & B_AX_USB_REG_EN)
		;

	rdata = GET_FIELD(PLTFM_REG_R32(R_AX_USB_SIE_INTF),
			  B_AX_USB_SIE_INTF_RD);

	//DD-Yingli suggest that shall clear it if read operation is done.
	PLTFM_REG_W32(R_AX_USB_SIE_INTF, 0);
#endif // if 0 NEO
	return rdata;
}

u32 write_usb2phy_para(struct mac_adapter *adapter, u16 offset, u8 val)
{
	u32 value32 = 0;

	RTW_INFO("%s TODO NEO\n", __func__);
#if 0 // NEO
	value32 = SET_CLR_WORD(value32, val, B_AX_USB_SIE_INTF_WD);
	value32 = SET_CLR_WORD(value32, offset, B_AX_USB_SIE_INTF_ADDR);
	value32 |= B_AX_USB_REG_SEL;
	value32 |= B_AX_USB_WRITE_EN;
	value32 |= B_AX_USB_REG_EN;
	value32 |= B_AX_USB_REG_STATUS;

	PLTFM_REG_W32(R_AX_USB_SIE_INTF, value32);
	while (PLTFM_REG_R32(R_AX_USB_SIE_INTF) & B_AX_USB_REG_EN)
		;

	//DD-Yingli suggest that shall clear it if write operation is done.
	PLTFM_REG_W32(R_AX_USB_SIE_INTF, 0);

#endif // if 0 NEO
	return MACSUCCESS;
}

u32 read_usb3phy_para(struct mac_adapter *adapter, u16 offset, u8 b_sel)
{
	u32 value32 = 0;
	u16 rdata = 0;

	RTW_INFO("%s TODO NEO\n", __func__);
#if 0 // NEO
	if (is_chip_cut(adapter, CHIP_CUT_A)) {
		value32 = (u32)offset;
		value32 |= B_AX_USB3_PHY_REG_RDFLAG;
		PLTFM_REG_W32(R_AX_USB3_PHY, value32);
		while (PLTFM_REG_R32(R_AX_USB3_PHY) & B_AX_USB3_PHY_REG_RDFLAG)
			;
		rdata = GET_FIELD(PLTFM_REG_R32(R_AX_USB3_PHY),
				  B_AX_USB3_PHY_RWDATA);
	} else {
		value32 = SET_CLR_WORD(value32, offset + 0x100,
				       B_AX_USB_SIE_INTF_ADDR);
		value32 |= B_AX_USB_REG_SEL;
		value32 |= B_AX_USB_REG_SEL;
		value32 |= B_AX_USB_REG_EN;
		if (b_sel)
			value32 |= B_AX_USB_PHY_BYTE_SEL;

		PLTFM_REG_W32(R_AX_USB_SIE_INTF, value32);
		while (PLTFM_REG_R32(R_AX_USB_SIE_INTF) & B_AX_USB_REG_EN)
			;

		rdata = GET_FIELD(PLTFM_REG_R32(R_AX_USB_SIE_INTF),
				  B_AX_USB_SIE_INTF_RD);
	}
#endif // if 0 NEO
	return rdata;
}

u32 write_usb3phy_para(struct mac_adapter *adapter, u16 offset, u8 b_sel,
		       u8 val)
{
	u32 value32 = 0;

	RTW_INFO("%s TODO NEO\n", __func__);
#if 0 // NEO
	if (is_chip_cut(adapter, CHIP_CUT_A)) {
		value32 = SET_CLR_WORD(value32, val, B_AX_USB3_PHY_RWDATA);
		value32 |= (u32)offset;
		value32 |= B_AX_USB3_PHY_REG_WRFLAG;
		PLTFM_REG_W32(R_AX_USB3_PHY, value32);
		while (PLTFM_REG_R32(R_AX_USB3_PHY) & B_AX_USB3_PHY_REG_WRFLAG)
			;
	} else {
		value32 = SET_CLR_WORD(value32, val, B_AX_USB_SIE_INTF_WD);
		value32 = SET_CLR_WORD(value32, offset + 0x100,
				       B_AX_USB_SIE_INTF_ADDR);
		value32 |= B_AX_USB_REG_SEL;
		value32 |= B_AX_USB_REG_SEL;
		value32 |= B_AX_USB_WRITE_EN;
		value32 |= B_AX_USB_REG_EN;
		if (b_sel)
			value32 |= B_AX_USB_PHY_BYTE_SEL;
		PLTFM_REG_W32(R_AX_USB_SIE_INTF, value32);
		while (PLTFM_REG_R32(R_AX_USB_SIE_INTF) & B_AX_USB_REG_EN)
			;
	}
#endif // if 0 NEO
	return MACSUCCESS;
}

u32 u2u3_switch(struct mac_adapter *adapter)
{
	u8 val8 = 0;

	RTW_INFO("%s TODO NEO\n", __func__);
#if 0 // NEO
	val8  = PLTFM_REG_R8(R_AX_USB_STATUS) & B_AX_R_USB2_SEL;
	if (val8  != B_AX_R_USB2_SEL)
		val8  = 0;
	else
		val8  = 1;

	if (is_chip_cut(adapter, CHIP_CUT_A) &&
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852A))
		PLTFM_REG_W8(R_AX_PAD_CTRL2 + 2, val8  ? U3SWITCHU2 : U2SWITCHU3_ACUT);
	else if (is_chip_cut(adapter, CHIP_CUT_B) &&
		 is_chip_id(adapter, MAC_AX_CHIP_ID_8852A))
		PLTFM_REG_W8(R_AX_PAD_CTRL2 + 2, val8  ? U3SWITCHU2 : U2SWITCHU3_BCUT);
	else
		PLTFM_REG_W8(R_AX_PAD_CTRL2 + 2, val8  ? U3SWITCHU2 : U2SWITCHU3_BCUT);
#endif // if 0 NEO
	return MACSUCCESS;
}

u32 get_usb_mode(struct mac_adapter *adapter)
{
	u32 val32 = 0;
	u32 hs = 0;

	RTW_INFO("%s TODO NEO\n", __func__);
#if 0 // NEO
	val32 = PLTFM_REG_R32(R_AX_USB_STATUS) & B_AX_R_USB2_SEL;
	hs = PLTFM_REG_R32(R_AX_USB_STATUS) & B_AX_MODE_HS;
	if (val32 == B_AX_R_USB2_SEL)
		val32 = USB3;
	else if ((val32 != B_AX_R_USB2_SEL) && (hs == B_AX_MODE_HS))
		val32 = USB2;
	else
		val32 = USB11;
#endif // if 0 NEO
	return val32;
}

u32 get_usb_support_ability(struct mac_adapter *adapter)
{
	u32 u2force = 0;
	u32 u3force = 0;

	RTW_INFO("%s TODO NEO\n", __func__);
#if 0 // NEO
	u2force = PLTFM_REG_R32(R_AX_USB_HOST_REQUEST_2) &
		  B_AX_R_FORCE_U3MAC_HS_MODE;
	u3force = PLTFM_REG_R32(R_AX_PAD_CTRL2) &
		  B_AX_USB3_USB2_TRANSITION;

	if (u2force == B_AX_R_FORCE_U3MAC_HS_MODE)
		return FORCEUSB2MODE;
	else if (u3force == B_AX_USB3_USB2_TRANSITION)
		return SWITCHMODE;
	else
#endif // if 0 NEO
		return FORCEUSB3MODE;
}

#if 0 //NEO

u32 usb_tx_agg_cfg(struct mac_adapter *adapter,
		   struct mac_ax_usb_tx_agg_cfg *agg)
{
	u32 dw1 = ((struct wd_body_usb *)agg->pkt)->dword1;

	RTW_INFO("%s TODO NEO\n", __func__);
#if 0 // NEO
	((struct wd_body_usb *)agg->pkt)->dword1 =
		SET_CLR_WORD(dw1, agg->agg_num, AX_TXD_DMA_TXAGG_NUM);
#endif // if 0 NEO
	return MACSUCCESS;
}

u32 usb_rx_agg_cfg(struct mac_adapter *adapter,
		   struct mac_ax_rx_agg_cfg *cfg)
{
	u8 size;
	u8 timeout;
	u8 agg_en;
	u8 agg_mode;
	u8 pkt_num;
	u32 val32;
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);

	RTW_INFO("%s TODO NEO\n", __func__);
#if 0 // NEO
	if (cfg->mode == MAC_AX_RX_AGG_MODE_DMA) {
		agg_en = 1;
		agg_mode = 1;
	} else if (cfg->mode == MAC_AX_RX_AGG_MODE_USB) {
		agg_en = 1;
		agg_mode = 0;
	} else {
		agg_en = 0;
		agg_mode = 0;
	}

	if (cfg->thold.drv_define == 0) {
		size = 0x5;
		timeout = 0x20;
		pkt_num = 0;
	} else {
		size = cfg->thold.size;
		timeout = cfg->thold.timeout;
		pkt_num = cfg->thold.pkt_num;
	}

	val32 = MAC_REG_R32(R_AX_RXAGG_0);
	MAC_REG_W32(R_AX_RXAGG_0, (agg_en ? B_AX_RXAGG_EN : 0) |
		    (agg_mode ? B_AX_RXAGG_DMA_STORE : 0) |
		    (val32 & B_AX_RXAGG_SW_EN) |
		    SET_WORD(pkt_num, B_AX_RXAGG_PKTNUM_TH) |
		    SET_WORD(timeout, B_AX_RXAGG_TIMEOUT_TH) |
		    SET_WORD(size, B_AX_RXAGG_LEN_TH));
#endif // if 0 NEO
	return MACSUCCESS;
}

u32 usb_pwr_switch(void *vadapter,
		   u8 pre_switch, u8 on)
{
	return MACSUCCESS;
}

u32 set_usb_wowlan(struct mac_adapter *adapter,
		   enum mac_ax_wow_ctrl w_c)
{
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);

	RTW_INFO("%s TODO NEO\n", __func__);
#if 0 // NEO
	if (w_c == MAC_AX_WOW_ENTER) {
		MAC_REG_W32(R_AX_RSV_CTRL, MAC_REG_R32(R_AX_RSV_CTRL) |
			    B_AX_WLOCK_1C_B6);
		MAC_REG_W32(R_AX_RSV_CTRL, MAC_REG_R32(R_AX_RSV_CTRL) |
			    B_AX_R_DIS_PRST);
		MAC_REG_W32(R_AX_RSV_CTRL, MAC_REG_R32(R_AX_RSV_CTRL) &
			    ~B_AX_WLOCK_1C_B6);
	} else if (w_c == MAC_AX_WOW_LEAVE) {
		MAC_REG_W32(R_AX_RSV_CTRL, MAC_REG_R32(R_AX_RSV_CTRL) |
			    B_AX_WLOCK_1C_B6);
		MAC_REG_W32(R_AX_RSV_CTRL, MAC_REG_R32(R_AX_RSV_CTRL) &
			    ~B_AX_R_DIS_PRST);
		MAC_REG_W32(R_AX_RSV_CTRL, MAC_REG_R32(R_AX_RSV_CTRL) &
			    ~B_AX_WLOCK_1C_B6);
	} else {
		PLTFM_MSG_ERR("[ERR] Invalid WoWLAN input.\n");
		return MACFUNCINPUT;
	}
#endif // if 0 NEO

	return MACSUCCESS;
}

#endif // if 0 NEO

#endif /* #if MAC_AX_USB_SUPPORT */
