/******************************************************************************
 *
 * Copyright(c) 2015 - 2017 Realtek Corporation.
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
#define _RTL8822CU_XMIT_C_

#include <drv_types.h>			/* PADAPTER, rtw_xmit.h and etc. */
#include <hal_data.h>			/* HAL_DATA_TYPE */
#include "../../hal_halmac.h"		/* halmac api */
#include "../rtl8822c.h"		/* rtl8822c_update_txdesc() */
#include "rtl8822cu.h"			/* OFFSET_SZ MAX_TX_AGG_PACKET_NUMBER_8822C */

static void update_txdesc_h2c_pkt(struct xmit_frame *pxmitframe, u8 *pmem, s32 sz)
{
	u8 *ptxdesc =  pmem;
	_adapter *padapter = pxmitframe->padapter;

	_rtw_memset(ptxdesc, 0, TXDESC_SIZE);
	SET_TX_DESC_TXPKTSIZE_8822C(ptxdesc, sz);
	SET_TX_DESC_QSEL_8822C(ptxdesc, HALMAC_TXDESC_QSEL_H2C_CMD);
	rtl8822c_cal_txdesc_chksum(padapter, ptxdesc);
	rtl8822c_dbg_dump_tx_desc(padapter, pxmitframe->frame_tag, ptxdesc);
}

static s32 update_txdesc(struct xmit_frame *pxmitframe, u8 *pmem, s32 sz, u8 bagg_pkt)
{
	int pull = 0;
	uint qsel;
	u8 data_rate, pwr_status, offset;
	_adapter *padapter = pxmitframe->padapter;
	struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
	struct pkt_attrib *pattrib = &pxmitframe->attrib;
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(padapter);
	u8 *ptxdesc =  pmem;
	struct mlme_ext_priv *pmlmeext = &padapter->mlmeextpriv;
	struct mlme_ext_info *pmlmeinfo = &(pmlmeext->mlmext_info);
	sint	bmcst = IS_MCAST(pattrib->ra);
	u16	SWDefineContent = 0x0;
	u8	DriverFixedRate = 0x0;
	u8 hw_port = rtw_hal_get_port(padapter);

#ifndef CONFIG_USE_USB_BUFFER_ALLOC_TX
	if (padapter->registrypriv.mp_mode == 0) {
		if ((PACKET_OFFSET_SZ != 0) && (!bagg_pkt)
		    && (rtw_usb_bulk_size_boundary(padapter, (TXDESC_SIZE + sz)) == _FALSE)) {
			ptxdesc = (pmem + PACKET_OFFSET_SZ);
			/* RTW_INFO("==> non-agg-pkt,shift pointer...\n"); */
			pull = 1;
		}
	}
#endif	/* !CONFIG_USE_USB_BUFFER_ALLOC_TX */

	_rtw_memset(ptxdesc, 0, TXDESC_SIZE);

	/* offset 0 */
	SET_TX_DESC_LS_8822C(ptxdesc, 1);

	/* RTW_INFO("%s==> pkt_len=%d,bagg_pkt=%02x\n",__FUNCTION__,sz,bagg_pkt); */
	SET_TX_DESC_TXPKTSIZE_8822C(ptxdesc, sz);

	offset = TXDESC_SIZE + OFFSET_SZ;

#ifdef CONFIG_TX_EARLY_MODE
	if (bagg_pkt)
		offset += EARLY_MODE_INFO_SIZE;
#endif
	/* RTW_INFO("%s==>offset(0x%02x)\n",__FUNCTION__,offset); */
	SET_TX_DESC_OFFSET_8822C(ptxdesc, offset);

	if (bmcst)
		SET_TX_DESC_BMC_8822C(ptxdesc, 1);

#ifndef CONFIG_USE_USB_BUFFER_ALLOC_TX
	if (padapter->registrypriv.mp_mode == 0) {
		if ((PACKET_OFFSET_SZ != 0) && (!bagg_pkt)) {
			if ((pull) && (pxmitframe->pkt_offset > 0))
				pxmitframe->pkt_offset = pxmitframe->pkt_offset - 1;
		}
	}
#endif /* !CONFIG_USE_USB_BUFFER_ALLOC_TX */

	/* RTW_INFO("%s, pkt_offset=0x%02x\n",__FUNCTION__,pxmitframe->pkt_offset); */
	/* pkt_offset, unit:8 bytes padding */
	if (pxmitframe->pkt_offset > 0)
		SET_TX_DESC_PKT_OFFSET_8822C(ptxdesc, pxmitframe->pkt_offset);

	SET_TX_DESC_MACID_8822C(ptxdesc, pattrib->mac_id);
	SET_TX_DESC_RATE_ID_8822C(ptxdesc, pattrib->raid);

	SET_TX_DESC_QSEL_8822C(ptxdesc,  pattrib->qsel);

	/*offset 12 */
	if (!pattrib->qos_en) {
		/* HW sequence, to fix to use 0 queue. todo: 4AC packets to use auto queue select */
		SET_TX_DESC_DISQSELSEQ_8822C(ptxdesc, 1);
		SET_TX_DESC_EN_HWSEQ_8822C(ptxdesc, 1);/* Hw set sequence number */
		SET_TX_DESC_HW_SSN_SEL_8822C(ptxdesc, pattrib->hw_ssn_sel);
		SET_TX_DESC_EN_HWEXSEQ_8822C(ptxdesc, 0);
	} else
		SET_TX_DESC_SW_SEQ_8822C(ptxdesc, pattrib->seqnum);

	if ((pxmitframe->frame_tag & 0x0f) == DATA_FRAMETAG) {
		/* RTW_INFO("pxmitframe->frame_tag == DATA_FRAMETAG\n");	*/
		rtl8822c_fill_txdesc_sectype(pattrib, ptxdesc);
#ifdef CONFIG_TCP_CSUM_OFFLOAD_TX
	if (pattrib->hw_csum == 1) {
		int offset = 48 + pxmitframe->pkt_offset*8 + 24;

		SET_TX_DESC_OFFSET_8822C(ptxdesc, offset);
		SET_TX_DESC_CHK_EN_8822C(ptxdesc, 1);
		SET_TX_DESC_WHEADER_LEN_8822C(ptxdesc, (pattrib->hdrlen + pattrib->iv_len + XATTRIB_GET_MCTRL_LEN(pattrib))>>1);
	}
#endif

		/* offset 20 */
#ifdef CONFIG_USB_TX_AGGREGATION
		if (pxmitframe->agg_num > 1) {
			/* RTW_INFO("%s agg_num:%d\n",__FUNCTION__,pxmitframe->agg_num); */
			SET_TX_DESC_DMA_TXAGG_NUM_8822C(ptxdesc, pxmitframe->agg_num);
		}
#endif /* CONFIG_USB_TX_AGGREGATION */

		rtl8822c_fill_txdesc_vcs(padapter, pattrib, ptxdesc);

		if (bmcst)
			rtl8822c_fill_txdesc_force_bmc_camid(pattrib, ptxdesc);

		if ((pattrib->ether_type != 0x888e) &&
		    (pattrib->ether_type != 0x0806) &&
		    (pattrib->ether_type != 0x88b4) &&
		    (pattrib->dhcp_pkt != 1)
#ifdef CONFIG_AUTO_AP_MODE
		    && (pattrib->pctrl != _TRUE)
#endif
		   ) {
			/* Non EAP & ARP & DHCP type data packet */
			if (pattrib->ampdu_en == _TRUE) {
				SET_TX_DESC_AGG_EN_8822C(ptxdesc, 1);
				SET_TX_DESC_MAX_AGG_NUM_8822C(ptxdesc, 0x1f);
				/* Set A-MPDU aggregation */
				SET_TX_DESC_AMPDU_DENSITY_8822C(ptxdesc, pattrib->ampdu_spacing);
			} else
				SET_TX_DESC_BK_8822C(ptxdesc, 1);

			rtl8822c_fill_txdesc_phy(padapter, pattrib, ptxdesc);

			/* compatibility for MCC consideration, use pmlmeext->cur_channel */
			if (!bmcst) {
				if (pmlmeext->cur_channel > 14)
					/* for 5G. OFDM 6M */
					SET_TX_DESC_DATA_RTY_LOWEST_RATE_8822C(ptxdesc, 4);
				else
					/* for 2.4G. CCK 1M */
					SET_TX_DESC_DATA_RTY_LOWEST_RATE_8822C(ptxdesc, 0);
			}

			if (pHalData->fw_ractrl == _FALSE) {
				SET_TX_DESC_USE_RATE_8822C(ptxdesc, 1);
				DriverFixedRate = 0x01;

				if (pHalData->INIDATA_RATE[pattrib->mac_id] & BIT(7))
					SET_TX_DESC_DATA_SHORT_8822C(ptxdesc, 1);

				SET_TX_DESC_DATARATE_8822C(ptxdesc, (pHalData->INIDATA_RATE[pattrib->mac_id] & 0x7F));
			}
			if (bmcst) {
				DriverFixedRate = 0x01;
				rtl8822c_fill_txdesc_bmc_tx_rate(pattrib, ptxdesc);
			}

			/* modify data rate by iwpriv or proc */
			if (padapter->fix_rate != 0xFF) {
				SET_TX_DESC_USE_RATE_8822C(ptxdesc, 1);

				DriverFixedRate = 0x01;
				if (padapter->fix_rate & BIT(7))
					SET_TX_DESC_DATA_SHORT_8822C(ptxdesc, 1);

				SET_TX_DESC_DATARATE_8822C(ptxdesc, (padapter->fix_rate & 0x7F));

				if (!padapter->data_fb)
					SET_TX_DESC_DISDATAFB_8822C(ptxdesc, 1);
			}

			if (pattrib->ldpc)
				SET_TX_DESC_DATA_LDPC_8822C(ptxdesc, 1);

			if (pattrib->stbc)
				SET_TX_DESC_DATA_STBC_8822C(ptxdesc, 1);

#ifdef CONFIG_WMMPS_STA
			if (pattrib->trigger_frame)
				SET_TX_DESC_TRI_FRAME_8822C (ptxdesc, 1);
#endif /* CONFIG_WMMPS_STA */			

		} else {
			/*
				EAP data packet and ARP packet and DHCP.
				Use the 1M data rate to send the EAP/ARP packet.
				This will maybe make the handshake smooth.
			*/

			SET_TX_DESC_USE_RATE_8822C(ptxdesc, 1);
			DriverFixedRate = 0x01;
			SET_TX_DESC_BK_8822C(ptxdesc, 1);

			/* HW will ignore this setting if the transmission rate is legacy OFDM */
			if (pmlmeinfo->preamble_mode == PREAMBLE_SHORT)
				SET_TX_DESC_DATA_SHORT_8822C(ptxdesc, 1);
#ifdef CONFIG_IP_R_MONITOR
			if((pattrib->ether_type == ETH_P_ARP) &&
				(IsSupportedTxOFDM(padapter->registrypriv.wireless_mode))) {
				SET_TX_DESC_DATARATE_8822C(ptxdesc, MRateToHwRate(IEEE80211_OFDM_RATE_6MB));
				#ifdef DBG_IP_R_MONITOR
				RTW_INFO(FUNC_ADPT_FMT ": SP Packet(0x%04X) rate=0x%x SeqNum = %d\n",
					FUNC_ADPT_ARG(padapter), pattrib->ether_type, MRateToHwRate(pmlmeext->tx_rate), pattrib->seqnum);
				#endif/*DBG_IP_R_MONITOR*/
			 } else
#endif/*CONFIG_IP_R_MONITOR*/
			SET_TX_DESC_DATARATE_8822C(ptxdesc, MRateToHwRate(pmlmeext->tx_rate));
		}

#ifdef CONFIG_TDLS
#ifdef CONFIG_XMIT_ACK
		/* CCX-TXRPT ack for xmit data frames */
		if (pxmitframe->ack_report) {
			SET_TX_DESC_SPE_RPT_8822C(ptxdesc, 1);
#ifdef DBG_CCX
			RTW_INFO("%s set tx report\n", __func__);
#endif
		}
#endif /* CONFIG_XMIT_ACK */
#endif
	} else if ((pxmitframe->frame_tag & 0x0f) == MGNT_FRAMETAG) {
		/* RTW_INFO("pxmitframe->frame_tag == MGNT_FRAMETAG\n");	*/
		SET_TX_DESC_MBSSID_8822C(ptxdesc, pattrib->mbssid & 0xF);

		SET_TX_DESC_USE_RATE_8822C(ptxdesc, 1);
		DriverFixedRate = 0x01;

		SET_TX_DESC_DATARATE_8822C(ptxdesc, MRateToHwRate(pattrib->rate));

		SET_TX_DESC_RTY_LMT_EN_8822C(ptxdesc, 1);
		if (pattrib->retry_ctrl == _TRUE)
			SET_TX_DESC_RTS_DATA_RTY_LMT_8822C(ptxdesc, 6);
		else
			SET_TX_DESC_RTS_DATA_RTY_LMT_8822C(ptxdesc, 12);

		/* VHT NDPA or HT NDPA Packet for Beamformer. */
		rtl8822c_fill_txdesc_mgnt_bf(pxmitframe, ptxdesc);

#ifdef CONFIG_XMIT_ACK
		/* CCX-TXRPT ack for xmit mgmt frames */
		if (pxmitframe->ack_report) {
			SET_TX_DESC_SPE_RPT_8822C(ptxdesc, 1);
#ifdef DBG_CCX
			RTW_INFO("%s set tx report\n", __func__);
#endif
		}
#endif /* CONFIG_XMIT_ACK */
	} else if ((pxmitframe->frame_tag & 0x0f) == TXAGG_FRAMETAG)
		RTW_INFO("pxmitframe->frame_tag == TXAGG_FRAMETAG\n");

#ifdef CONFIG_MP_INCLUDED
	else if (((pxmitframe->frame_tag & 0x0f) == MP_FRAMETAG) &&
		 (padapter->registrypriv.mp_mode == 1))

		fill_txdesc_for_mp(padapter, ptxdesc);
#endif

	else {
		RTW_INFO("pxmitframe->frame_tag = %d\n", pxmitframe->frame_tag);

		SET_TX_DESC_USE_RATE_8822C(ptxdesc, 1);
		DriverFixedRate = 0x01;
		SET_TX_DESC_DATARATE_8822C(ptxdesc, MRateToHwRate(pmlmeext->tx_rate));
	}

	rtl8822c_fill_txdesc_bf(pxmitframe, ptxdesc);

	if (DriverFixedRate)
		SWDefineContent |= 0x01;

	SET_TX_DESC_SW_DEFINE_8822C(ptxdesc, SWDefineContent);

	SET_TX_DESC_PORT_ID_8822C(ptxdesc, hw_port);
	SET_TX_DESC_MULTIPLE_PORT_8822C(ptxdesc, hw_port);

	rtl8822c_cal_txdesc_chksum(padapter, ptxdesc);
	rtl8822c_dbg_dump_tx_desc(padapter, pxmitframe->frame_tag, ptxdesc);
	return pull;
}

#define rtw_halmac_usb_write_port_complete(purb, regs)\
	rtw_halmac_usb_write_port_complete(purb)


struct data_urb *rtw_alloc_dataurb(struct trx_urb_buf_q *urb_q);
s32 rtw_free_dataurb(struct trx_urb_buf_q *urb_q, struct data_urb *urb);

static void rtw_halmac_usb_write_port_complete(struct urb *purb, struct pt_regs *regs)
{
	struct lite_data_buf *litexmitbuf =
		(struct lite_data_buf *)purb->context;
	struct xmit_buf *pxmitbuf = litexmitbuf->pxmitbuf;
	struct data_urb *xmiturb = litexmitbuf->dataurb;
	struct dvobj_priv *pdvobj = litexmitbuf->dvobj;
	_adapter	*padapter = pxmitbuf->padapter;
	struct xmit_priv	*pxmitpriv = &padapter->xmitpriv;

	switch (pxmitbuf->flags) {
	case VO_QUEUE_INX:
		pxmitpriv->voq_cnt--;
		break;
	case VI_QUEUE_INX:
		pxmitpriv->viq_cnt--;
		break;
	case BE_QUEUE_INX:
		pxmitpriv->beq_cnt--;
		break;
	case BK_QUEUE_INX:
		pxmitpriv->bkq_cnt--;
		break;
	default:
		break;
	}


	if (RTW_CANNOT_TX(pdvobj)) {
		RTW_INFO(
			"%s(): TX Warning! bDriverStopped(%s) OR bSurpriseRemoved(%s) pxmitbuf->buf_tag(%x)\n"
			 , __func__
			 , dev_is_drv_stopped(pdvobj) ? "True" : "False"
			 , dev_is_surprise_removed(pdvobj) ? "True" : "False"
			 , pxmitbuf->buf_tag);

		goto check_completion;
	}


	if (purb->status == 0) {

	} else {
		RTW_INFO("###=> urb_write_port_complete status(%d)\n",
			 purb->status);
		if ((purb->status == -EPIPE) || (purb->status == -EPROTO)) {
			/* usb_clear_halt(pusbdev, purb->pipe);	 */
			/* msleep(10); */
			/*sreset_set_wifi_error_status(padapter, USB_WRITE_PORT_FAIL);*/
		} else if (purb->status == -EINPROGRESS) {
			goto check_completion;

		} else if (purb->status == -ENOENT) {
			RTW_INFO("%s: -ENOENT\n", __func__);
			goto check_completion;

		} else if (purb->status == -ECONNRESET) {
			RTW_INFO("%s: -ECONNRESET\n", __func__);
			goto check_completion;

		} else if (purb->status == -ESHUTDOWN) {
			dev_set_drv_stopped(pdvobj);

			goto check_completion;
		} else {
			dev_set_surprise_removed(pdvobj);
			RTW_INFO("bSurpriseRemoved=TRUE\n");

			goto check_completion;
		}
	}

check_completion:
	rtw_sctx_done_err(&pxmitbuf->sctx,
		purb->status ? RTW_SCTX_DONE_WRITE_PORT_ERR : RTW_SCTX_DONE_SUCCESS);

	rtw_free_litedatabuf(&pdvobj->litexmitbuf_q, litexmitbuf);
	rtw_free_dataurb(&pdvobj->xmit_urb_q, xmiturb);
	rtw_free_xmitbuf(pxmitpriv, pxmitbuf);

	{
		tasklet_hi_schedule(&pxmitpriv->xmit_tasklet);
	}
}

u32 rtw_halmac_usb_write_port(_adapter *padapter, u32 addr, u32 len, u8 *wmem)
{
	unsigned int pipe;
	int status;
	u32 ret = _FAIL;
	PURB	purb = NULL;
	struct dvobj_priv	*pdvobj = adapter_to_dvobj(padapter);
	struct xmit_priv	*pxmitpriv = &padapter->xmitpriv;
	struct xmit_buf *pxmitbuf = (struct xmit_buf *)wmem;
	struct xmit_frame *pxmitframe = (struct xmit_frame *)pxmitbuf->priv_data;
	PUSB_DATA pusb_data = dvobj_to_usb(pdvobj);
	struct usb_device *pusbd = pusb_data->pusbdev;

	struct lite_data_buf *litexmitbuf = NULL;
	struct data_urb *xmiturb = NULL;

	unsigned long sp_flags;


	//RTW_INFO("%s : NEO bulkout_id:%d\n", __func__, pxmitbuf->bulkout_id);

	litexmitbuf = rtw_alloc_litedatabuf(&pdvobj->litexmitbuf_q);
	if (litexmitbuf == NULL) {
		RTW_INFO("%s,%d Can't alloc lite xmit buf\n",
			__func__, __LINE__);
		goto exit;
	}

	xmiturb = rtw_alloc_dataurb(&pdvobj->xmit_urb_q);
	if (xmiturb == NULL) {
		RTW_INFO("%s,%d Can't alloc lite xmit urb\n",
			__func__, __LINE__);
		goto exit;
	}

	if (RTW_CANNOT_TX(pdvobj)) {
#ifdef DBG_TX
		RTW_INFO(" DBG_TX %s:%d bDriverStopped%s, bSurpriseRemoved:%s\n"
			 , __func__, __LINE__,
			 dev_is_drv_stopped(pdvobj) ? "True" : "False"
			 dev_is_surprise_removed(pdvobj) ? "True" : "False");
#endif
		rtw_sctx_done_err(&pxmitbuf->sctx, RTW_SCTX_DONE_TX_DENY);
		goto exit;
	}

	_rtw_spinlock_irq(&pxmitpriv->lock, &sp_flags);

	switch (addr) {
	case VO_QUEUE_INX:
		pxmitpriv->voq_cnt++;
		pxmitbuf->flags = VO_QUEUE_INX;
		break;
	case VI_QUEUE_INX:
		pxmitpriv->viq_cnt++;
		pxmitbuf->flags = VI_QUEUE_INX;
		break;
	case BE_QUEUE_INX:
		pxmitpriv->beq_cnt++;
		pxmitbuf->flags = BE_QUEUE_INX;
		break;
	case BK_QUEUE_INX:
		pxmitpriv->bkq_cnt++;
		pxmitbuf->flags = BK_QUEUE_INX;
		break;
	case HIGH_QUEUE_INX:
		pxmitbuf->flags = HIGH_QUEUE_INX;
		break;
	default:
		pxmitbuf->flags = MGT_QUEUE_INX;
		break;
	}

	_rtw_spinunlock_irq(&pxmitpriv->lock, &sp_flags);

	purb = xmiturb->urb;

	/* translate DMA FIFO addr to pipehandle */
	pipe = bulkid2pipe(pdvobj, pxmitbuf->bulkout_id, _TRUE);

#ifdef CONFIG_REDUCE_USB_TX_INT
	if ((pxmitpriv->free_xmitbuf_cnt % NR_XMITBUFF == 0)
	    || (pxmitbuf->buf_tag > XMITBUF_DATA))
		purb->transfer_flags  &= (~URB_NO_INTERRUPT);
	else {
		purb->transfer_flags  |=  URB_NO_INTERRUPT;
		/* RTW_INFO("URB_NO_INTERRUPT "); */
	}
#endif

	litexmitbuf->dvobj = pdvobj;
	litexmitbuf->pbuf = pxmitframe->buf_addr;
	litexmitbuf->dataurb = xmiturb;
	litexmitbuf->pxmitbuf = pxmitbuf;

	usb_fill_bulk_urb(purb, pusbd, pipe,
			  litexmitbuf->pbuf,
			  len,
			  rtw_halmac_usb_write_port_complete,
			  litexmitbuf);

#ifdef CONFIG_USE_USB_BUFFER_ALLOC_TX
	purb->transfer_dma = pxmitbuf->dma_transfer_addr;
	purb->transfer_flags |= URB_NO_TRANSFER_DMA_MAP;
	purb->transfer_flags |= URB_ZERO_PACKET;
#endif /* CONFIG_USE_USB_BUFFER_ALLOC_TX */

#ifdef USB_PACKET_OFFSET_SZ
#if (USB_PACKET_OFFSET_SZ == 0)
	purb->transfer_flags |= URB_ZERO_PACKET;
#endif
#endif

#if 0
	if (bwritezero)
		purb->transfer_flags |= URB_ZERO_PACKET;
#endif

	status = usb_submit_urb(purb, GFP_ATOMIC);
	if (!status) {
	} else {
		rtw_sctx_done_err(&pxmitbuf->sctx, RTW_SCTX_DONE_WRITE_PORT_ERR);
		RTW_INFO("%s, status=%d\n", __func__, status);

		switch (status) {
		case -ENODEV:
			dev_set_drv_stopped(pdvobj);
			break;
		default:
			break;
		}
		goto exit;
	}

	ret = _SUCCESS;

	/* Commented by Albert 2009/10/13
	 * We add the URB_ZERO_PACKET flag to urb so that the host will send the zero packet automatically. */
	/*
		if(bwritezero == _TRUE)
		{
			usb_bulkout_zero(pintfhdl, addr);
		}
	*/


exit:
	if (ret != _SUCCESS) {
		rtw_free_litedatabuf(&pdvobj->litexmitbuf_q, litexmitbuf);
		rtw_free_xmitbuf(pxmitpriv, pxmitbuf);
	}
	return ret;

}


/* for non-agg data frame or  management frame */
static s32 rtw_dump_xframe(PADAPTER padapter, struct xmit_frame *pxmitframe)
{
	s32 ret = _SUCCESS;
	s32 inner_ret = _SUCCESS;
	int t, sz, w_sz, pull = 0;
	u8 *mem_addr;
	u8 ff_hwaddr;
	struct xmit_buf *pxmitbuf = pxmitframe->pxmitbuf;
	struct pkt_attrib *pattrib = &pxmitframe->attrib;
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	struct security_priv *psecuritypriv = &padapter->securitypriv;
	struct dvobj_priv *pdvobj = adapter_to_dvobj(padapter);


#ifdef CONFIG_80211N_HT
	if ((pxmitframe->frame_tag == DATA_FRAMETAG) &&
	    (pxmitframe->attrib.ether_type != 0x0806) &&
	    (pxmitframe->attrib.ether_type != 0x888e) &&
	    (pxmitframe->attrib.ether_type != 0x88b4) &&
	    (pxmitframe->attrib.dhcp_pkt != 1))
		rtw_issue_addbareq_cmd(padapter, pxmitframe, _FALSE);
#endif /* CONFIG_80211N_HT */

	mem_addr = pxmitframe->buf_addr;

	for (t = 0; t < pattrib->nr_frags; t++) {
		if (inner_ret != _SUCCESS && ret == _SUCCESS)
			ret = _FAIL;

		if (t != (pattrib->nr_frags - 1)) {

			sz = pxmitpriv->frag_len;
			sz = sz - 4 - (psecuritypriv->sw_encrypt ? 0 : pattrib->icv_len);
		} else   /* no frag */
			sz = pattrib->last_txcmdsz;

		if (pattrib->qsel == HALMAC_TXDESC_QSEL_H2C_CMD) {
			update_txdesc_h2c_pkt(pxmitframe, mem_addr, sz);
			w_sz = sz + TXDESC_SIZE;
			print_hex_dump(KERN_INFO, "NEO h2c: ", DUMP_PREFIX_OFFSET, 16, 1, mem_addr, w_sz, 1);
		} else {

			pull = update_txdesc(pxmitframe, mem_addr, sz, _FALSE);
			if (pull) {
				/* pull txdesc head */
				mem_addr += PACKET_OFFSET_SZ;

				pxmitframe->buf_addr = mem_addr;

				w_sz = sz + TXDESC_SIZE;
			} else
				w_sz = sz + TXDESC_SIZE + PACKET_OFFSET_SZ;
		}

		pxmitbuf->bulkout_id = rtw_halmac_usb_get_bulkout_id(pdvobj, mem_addr, w_sz);
		ff_hwaddr = rtw_get_ff_hwaddr(pxmitframe);

		inner_ret = rtw_halmac_usb_write_port(padapter, ff_hwaddr, w_sz, (unsigned char *)pxmitbuf);
		rtw_count_tx_stats(padapter, pxmitframe, sz);

		mem_addr += w_sz;

		mem_addr = (u8 *)RND4(((SIZE_PTR)(mem_addr)));

	}

	rtw_free_xmitframe(pxmitpriv, pxmitframe);

	if (ret != _SUCCESS)
		rtw_sctx_done_err(&pxmitbuf->sctx, RTW_SCTX_DONE_UNKNOWN);

	return ret;
}

static s32 rtl8822cu_xmitframe_complete(PADAPTER padapter, struct xmit_priv *pxmitpriv, struct xmit_buf *pxmitbuf)
{

	struct hw_xmit *phwxmits;
	sint hwentry;
	struct xmit_frame *pxmitframe = NULL;
	int res = _SUCCESS, xcnt = 0;

	phwxmits = pxmitpriv->hwxmits;
	hwentry = pxmitpriv->hwxmit_entry;

	if (pxmitbuf == NULL) {
		pxmitbuf = rtw_alloc_xmitbuf(pxmitpriv);
		if (!pxmitbuf)
			return _FALSE;
	}


	do {
		pxmitframe =  rtw_dequeue_xframe(pxmitpriv, phwxmits, hwentry);

		if (pxmitframe) {
			pxmitframe->pxmitbuf = pxmitbuf;

			pxmitframe->buf_addr = pxmitbuf->pbuf;

			pxmitbuf->priv_data = pxmitframe;

			if ((pxmitframe->frame_tag & 0x0f) == DATA_FRAMETAG) {
				/* TID0~15 */
				if (pxmitframe->attrib.priority <= 15)
					res = rtw_xmitframe_coalesce(padapter, pxmitframe->pkt, pxmitframe);

				rtw_os_xmit_complete(padapter, pxmitframe);/* always return ndis_packet after rtw_xmitframe_coalesce */
			}




			if (res == _SUCCESS)
				rtw_dump_xframe(padapter, pxmitframe);
			else {
				rtw_free_xmitbuf(pxmitpriv, pxmitbuf);
				rtw_free_xmitframe(pxmitpriv, pxmitframe);
			}

			xcnt++;

		} else {
			rtw_free_xmitbuf(pxmitpriv, pxmitbuf);
			return _FALSE;
		}

		break;

	} while (0/*xcnt < (NR_XMITFRAME >> 3)*/);

	return _TRUE;

}

static void rtl8822cu_xmit_tasklet(void *priv)
{
	int ret = _FALSE;
	_adapter *padapter = (_adapter *)priv;
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;

	while (1) {
		if (RTW_CANNOT_TX(dvobj)) {
			RTW_INFO("xmit_tasklet => bDriverStopped or bSurpriseRemoved or bWritePortCancel\n");
			break;
		}

		if (rtw_xmit_ac_blocked(padapter) == _TRUE)
			break;

		ret = rtl8822cu_xmitframe_complete(padapter, pxmitpriv, NULL);

		if (ret == _FALSE)
			break;

	}

}

s32	rtl8822cu_init_xmit_priv(PADAPTER padapter)
{
	struct xmit_priv *pxmitpriv = &padapter->xmitpriv;
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(padapter);

#ifdef PLATFORM_LINUX
	tasklet_init(&pxmitpriv->xmit_tasklet,
		     (void(*)(unsigned long))rtl8822cu_xmit_tasklet,
		     (unsigned long)padapter);
#endif
#ifdef CONFIG_TX_EARLY_MODE
	pHalData->bEarlyModeEnable = padapter->registrypriv.early_mode;
#endif
	rtl8822c_init_xmit_priv(padapter);
	return _SUCCESS;
}

void	rtl8822cu_free_xmit_priv(PADAPTER padapter)
{
}

s32 rtl8822cu_mgnt_xmit(PADAPTER padapter, struct xmit_frame *pmgntframe)
{
	return rtw_dump_xframe(padapter, pmgntframe);
}

#ifdef CONFIG_HOSTAPD_MLME

static void rtl8822cu_hostap_mgnt_xmit_cb(struct urb *urb)
{
#ifdef PLATFORM_LINUX
	struct sk_buff *skb = (struct sk_buff *)urb->context;

	rtw_skb_free(skb);
#endif
}

s32 rtl8822cu_hostap_mgnt_xmit_entry(PADAPTER padapter, _pkt *pkt)
{
#ifdef PLATFORM_LINUX
	u16 fc;
	int rc, len, pipe;
	unsigned int bmcst, tid, qsel;
	struct sk_buff *skb, *pxmit_skb;
	struct urb *urb;
	unsigned char *pxmitbuf;
	struct tx_desc *ptxdesc;
	struct rtw_ieee80211_hdr *tx_hdr;
	struct hostapd_priv *phostapdpriv = padapter->phostapdpriv;
	struct net_device *pnetdev = padapter->pnetdev;
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(padapter);
	struct dvobj_priv *pdvobj = adapter_to_dvobj(padapter);

	skb = pkt;

	len = skb->len;
	tx_hdr = (struct rtw_ieee80211_hdr *)(skb->data);
	fc = le16_to_cpu(tx_hdr->frame_ctl);
	bmcst = IS_MCAST(tx_hdr->addr1);

	if ((fc & RTW_IEEE80211_FCTL_FTYPE) != RTW_IEEE80211_FTYPE_MGMT)
		goto _exit;

	pxmit_skb = rtw_skb_alloc(len + TXDESC_SIZE);

	if (!pxmit_skb)
		goto _exit;

	pxmitbuf = pxmit_skb->data;

	urb = usb_alloc_urb(0, GFP_ATOMIC);
	if (!urb)
		goto _exit;

	/* ----- fill tx desc ----- */
	ptxdesc = (struct tx_desc *)pxmitbuf;
	_rtw_memset(ptxdesc, 0, sizeof(*ptxdesc));

	/* offset 0 */
	ptxdesc->txdw0 |= cpu_to_le32(len & 0x0000ffff);
	ptxdesc->txdw0 |= cpu_to_le32(((TXDESC_SIZE + OFFSET_SZ) << OFFSET_SHT) & 0x00ff0000);/* default = 32 bytes for TX Desc */
	ptxdesc->txdw0 |= cpu_to_le32(OWN | FSG | LSG);

	if (bmcst)
		ptxdesc->txdw0 |= cpu_to_le32(BIT(24));

	/* offset 4 */
	ptxdesc->txdw1 |= cpu_to_le32(0x00); /* MAC_ID */

	ptxdesc->txdw1 |= cpu_to_le32((0x12 << QSEL_SHT) & 0x00001f00);

	ptxdesc->txdw1 |= cpu_to_le32((0x06 << 16) & 0x000f0000);/* b mode */

	/* offset 8 */

	/* offset 12 */
	ptxdesc->txdw3 |= cpu_to_le32((le16_to_cpu(tx_hdr->seq_ctl) << 16) & 0xffff0000);

	/* offset 16 */
	ptxdesc->txdw4 |= cpu_to_le32(BIT(8)); /* driver uses rate */

	/* offset 20 */


	/* HW append seq */
	ptxdesc->txdw4 |= cpu_to_le32(BIT(7)); /* Hw set sequence number */
	ptxdesc->txdw3 |= cpu_to_le32((8 << 28)); /* set bit3 to 1. Suugested by TimChen. 2009.12.29. */


	rtl8822c_cal_txdesc_chksum(padapter, ptxdesc);
	/* ----- end of fill tx desc -----*/


	skb_put(pxmit_skb, len + TXDESC_SIZE);
	pxmitbuf = pxmitbuf + TXDESC_SIZE;
	_rtw_memcpy(pxmitbuf, skb->data, len);

	/*RTW_INFO("mgnt_xmit, len=%x\n", pxmit_skb->len); */


	/* ----- prepare urb for submit -----*/

	/* translate DMA FIFO addr to pipehandle */
	pipe = usb_sndbulkpipe(pdvobj->pusbdev, pHalData->Queue2EPNum[(u8)MGT_QUEUE_INX] & 0x0f);

	usb_fill_bulk_urb(urb, pdvobj->pusbdev, pipe,
		pxmit_skb->data, pxmit_skb->len, rtl8192cu_hostap_mgnt_xmit_cb, pxmit_skb);

	urb->transfer_flags |= URB_ZERO_PACKET;
	usb_anchor_urb(urb, &phostapdpriv->anchored);
	rc = usb_submit_urb(urb, GFP_ATOMIC);
	if (rc < 0) {
		usb_unanchor_urb(urb);
		kfree_skb(skb);
	}
	usb_free_urb(urb);


_exit:

	rtw_skb_free(skb);

#endif

	return 0;

}
#endif
