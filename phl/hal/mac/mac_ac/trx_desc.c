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

#include "trx_desc.h"

#if 0 // NEO 

#define RXD_RPKT_TYPE_INVALID	0xFF
#define TXD_AC_TYPE_MSK		0x3
#define TXD_TID_IND_SH		2
#define TID_MAX_NUM		8

#define TID_0_QSEL 0
#define TID_1_QSEL 1
#define TID_2_QSEL 1
#define TID_3_QSEL 0
#define TID_4_QSEL 2
#define TID_5_QSEL 2
#define TID_6_QSEL 3
#define TID_7_QSEL 3
#define TID_0_IND 0
#define TID_1_IND 0
#define TID_2_IND 1
#define TID_3_IND 1
#define TID_4_IND 0
#define TID_5_IND 1
#define TID_6_IND 0
#define TID_7_IND 1

enum wd_info_pkt_type {
	WD_INFO_PKT_NORMAL,

	/* keep last */
	WD_INFO_PKT_LAST,
	WD_INFO_PKT_MAX = WD_INFO_PKT_LAST,
};

static u8 wd_info_tmpl[WD_INFO_PKT_MAX][24] = {
	/* normal packet */
	{0x00, 0x06, 0x8b, 0x50, 0x00, 0x00, 0x0b, 0x0c,
	 0x00, 0x01, 0x4c, 0x00, 0x00, 0x00, 0x00, 0x08,
	 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
};

static u8 qsel_l[TID_MAX_NUM] = {
	TID_0_QSEL, TID_1_QSEL, TID_2_QSEL, TID_3_QSEL,
	TID_4_QSEL, TID_5_QSEL, TID_6_QSEL, TID_7_QSEL
};

static u8 tid_ind[TID_MAX_NUM] = {
	TID_0_IND, TID_1_IND, TID_2_IND, TID_3_IND,
	TID_4_IND, TID_5_IND, TID_6_IND, TID_7_IND
};

#endif // if 0 NEO

u32 mac_txdesc_len(struct mac_adapter *adapter,
		   struct mac_txpkt_info *info)
{
	u32 len = 48;
	struct mac_pkt_data *data = &info->u.data;

	switch (info->type) {
	case MAC_PKT_H2C:
		break;
	default:
		data->offset = len;
		break;
	}

	return len;
}

#if 0 // NEO

static u32 txdes_proc_h2c_fwdl(struct mac_ax_adapter *adapter,
			       struct mac_ax_txpkt_info *info, u8 *buf, u32 len)
{
	struct wd_body_t *wdb;
	struct mac_ax_pkt_data *datai = &info->u.data;

	if (len != mac_txdesc_len(adapter, info))
		return MACBUFSZ;

	wdb = (struct wd_body_t *)buf;
	wdb->dword0 = cpu_to_le32(SET_WORD(MAC_AX_DMA_H2C, AX_TXD_CH_DMA) |
			(info->type == MAC_AX_PKT_FWDL ? AX_TXD_FWDL_EN : 0 |
			(datai->pkt_offset == 0x1 ? AX_TXD_PKT_OFFSET : 0)));
	wdb->dword1 = 0;
	wdb->dword2 = cpu_to_le32(SET_WORD(info->pktsize, AX_TXD_TXPKTSIZE));
	wdb->dword3 = 0;
	wdb->dword4 = 0;
	wdb->dword5 = 0;

	return MACSUCCESS;
}

static u32 txdes_proc_data(struct mac_ax_adapter *adapter,
			   struct mac_ax_txpkt_info *info, u8 *buf, u32 len)
{
	struct wd_body_t *wdb;
	struct wd_info_t *wdi;
	struct mac_ax_pkt_data *datai = &info->u.data;
	struct mac_role_tbl *role;
	u8 ch, qsel;

	if (len != mac_txdesc_len(adapter, info))
		return MACBUFSZ;

	if (datai->ch > MAC_AX_DATA_HIQ) {
		PLTFM_MSG_ERR("[ERR]txd ch: %d\n", datai->ch);
		return MACNOITEM;
	}

	role = mac_role_srch(adapter, datai->macid);
	if (!role) {
		PLTFM_MSG_ERR("[ERR]cannot find macid: %d\n", datai->macid);
		return MACNOITEM;
	}

	wdb = (struct wd_body_t *)buf;
	if (adapter->hw_info->intf == MAC_AX_INTF_SDIO)
		wdb->dword0 =
			cpu_to_le32(AX_TXD_STF_MODE);
	else if (adapter->hw_info->intf == MAC_AX_INTF_USB)
		wdb->dword0 =
			cpu_to_le32(AX_TXD_STF_MODE |
				    (datai->pkt_offset ?
				     AX_TXD_PKT_OFFSET : 0));
	else
		wdb->dword0 =
			cpu_to_le32((datai->wd_page ? AX_TXD_WD_PAGE : 0) |
				    (adapter->dle_info.qta_mode ==
				     MAC_AX_QTA_SCC_STF ||
				     adapter->dle_info.qta_mode ==
				     MAC_AX_QTA_DBCC_STF ?
				     AX_TXD_STF_MODE : 0));

	if (datai->ch == MAC_AX_DATA_HIQ &&
	    adapter->hw_info->intf == MAC_AX_INTF_USB)
		ch = role->info.band ? MAC_AX_DMA_B1MG : MAC_AX_DMA_B0MG;
	else if (datai->ch == MAC_AX_DATA_HIQ)
		ch = role->info.band ? MAC_AX_DMA_B1HI : MAC_AX_DMA_B0HI;
	else
		ch = datai->ch;

	wdb->dword0 |=
		cpu_to_le32(SET_WORD(datai->hw_seq_mode,
				     AX_TXD_EN_HWSEQ_MODE) |
			    SET_WORD(datai->hw_ssn_sel,
				     AX_TXD_HW_SSN_SEL) |
			    SET_WORD(datai->headerwllc_len,
				     AX_TXD_HDR_LLC_LEN) |
			    SET_WORD(ch, AX_TXD_CH_DMA) |
			    (datai->hw_amsdu ? AX_TXD_HWAMSDU : 0) |
			    (datai->smh_en ? AX_TXD_SMH_EN : 0) |
			    (datai->hw_aes_iv ? AX_TXD_HW_AES_IV : 0) |
			    (datai->chk_en ? AX_TXD_CHK_EN : 0) |
			    (datai->wdinfo_en ? AX_TXD_WDINFO_EN : 0) |
			    SET_WORD(datai->wp_offset,
				     AX_TXD_WP_OFFSET));
	wdb->dword1 =
		cpu_to_le32(SET_WORD(datai->shcut_camid, AX_TXD_SHCUT_CAMID));
	/* Get bb and qsel from qsel by according MAC ID */
	if (datai->ch == MAC_AX_DATA_HIQ)
		qsel = role->info.band ? MAC_AX_HI1_SEL : MAC_AX_HI0_SEL;
	else
		qsel = (role->wmm << 2) | qsel_l[datai->tid];
	wdb->dword2 =
		cpu_to_le32(SET_WORD(info->pktsize, AX_TXD_TXPKTSIZE) |
			    SET_WORD(qsel, AX_TXD_QSEL) |
			    (tid_ind[datai->tid] ? AX_TXD_TID_IND : 0) |
			    SET_WORD(role->macid, AX_TXD_MACID));

	wdb->dword3 = cpu_to_le32(SET_WORD(datai->wifi_seq,
					   AX_TXD_WIFI_SEQ) |
				  (datai->agg_en ? AX_TXD_AGG_EN : 0) |
				  ((datai->bk || datai->ack_ch_info) ?
				    AX_TXD_BK : 0));
	wdb->dword4 = 0;
	wdb->dword5 = 0;

	wdi = (struct wd_info_t *)wd_info_tmpl[WD_INFO_PKT_NORMAL];
	wdi->dword0 =
		cpu_to_le32((datai->userate ? AX_TXD_USERATE_SEL : 0) |
			     SET_WORD(datai->data_rate, AX_TXD_DATARATE) |
			     SET_WORD(datai->data_bw, AX_TXD_DATA_BW) |
			     (datai->er_bw ? AX_TXD_DATA_BW_ER : 0) |
			     SET_WORD(datai->data_gi_ltf, AX_TXD_GI_LTF) |
			     (datai->data_er ? AX_TXD_DATA_ER : 0) |
			     (datai->data_dcm ? AX_TXD_DATA_DCM : 0) |
			     SET_WORD(datai->data_stbc, AX_TXD_DATA_STBC) |
			     (datai->data_ldpc ? AX_TXD_DATA_LDPC : 0) |
			     (datai->dis_data_fb ? AX_TXD_DISDATAFB : 0) |
			     (datai->dis_rts_fb ? AX_TXD_DISRTSFB : 0) |
			     SET_WORD(datai->multiport_id,
				      AX_TXD_MULTIPORT_ID) |
			     SET_WORD(datai->mbssid, AX_TXD_MBSSID) |
			     (datai->ack_ch_info ? AX_TXD_ACK_CH_INFO : 0));
	wdi->dword1 =
		cpu_to_le32(SET_WORD(datai->max_agg_num, AX_TXD_MAX_AGG_NUM) |
			    SET_WORD(datai->tx_cnt_lmt, AX_TXD_DATA_TXCNT_LMT) |
			    (datai->tx_cnt_lmt_sel ?
			     AX_TXD_DATA_TXCNT_LMT_SEL : 0) |
			    (datai->nav_use_hdr ? AX_TXD_NAVUSEHDR : 0) |
			    (datai->bmc ? AX_TXD_BMC : 0) |
			    (datai->a_ctrl_uph ? AX_TXD_A_CTRL_UPH : 0) |
			    (datai->a_ctrl_bsr ? AX_TXD_A_CTRL_BSR : 0) |
			    (datai->a_ctrl_cas ? AX_TXD_A_CTRL_CAS : 0));
	wdi->dword2 =
		cpu_to_le32(SET_WORD(datai->lifetime_sel, AX_TXD_LIFETIME_SEL) |
				SET_WORD(datai->sec_type, AX_TXD_SECTYPE) |
			    (datai->hw_sec_en ? AX_TXD_SEC_HW_ENC : 0) |
			    (datai->no_ack ? AX_TXD_NO_ACK : 0) |
			    SET_WORD(datai->sec_cam_idx, AX_TXD_SEC_CAM_IDX) |
			    SET_WORD(datai->ampdu_density,
				     AX_TXD_AMPDU_DENSITY));

	wdi->dword3 =
		cpu_to_le32((datai->null_0 ? AX_TXD_NULL_0 : 0) |
			    (datai->null_1 ? AX_TXD_NULL_1 : 0) |
			    (datai->sifs_tx ? AX_TXD_SIFS_TX : 0) |
			    SET_WORD(datai->ndpa, AX_TXD_NDPA) |
			    SET_WORD(datai->snd_pkt_sel, AX_TXD_SND_PKT_SEL) |
			    (datai->tri_frame ? AX_TXD_TRI_FRAME : 0) |
			    (datai->rtt ? AX_TXD_RTT_EN : 0) |
			    (datai->ht_data_snd ? AX_TXD_HT_DATA_SND : 0));

	wdi->dword4 =
		cpu_to_le32((datai->rts_en ? AX_TXD_RTS_EN : 0) |
			    (datai->cts2self ? AX_TXD_CTS2SELF : 0) |
			    SET_WORD(datai->cca_rts, AX_TXD_CCA_RTS) |
			    (datai->hw_rts_en ? AX_TXD_HW_RTS_EN : 0) |
			    SET_WORD(datai->sw_define, AX_TXD_SW_DEFINE));

	wdi->dword5 =
		cpu_to_le32(SET_WORD(datai->ndpa_dur, AX_TXD_NDPA_DURATION));

	if (datai->wdinfo_en != 0)
		PLTFM_MEMCPY(buf + WD_BODY_LEN, (u8 *)wdi, WD_INFO_LEN);

	if (adapter->hw_info->wd_checksum_en)
		mac_wd_checksum(adapter, info, buf);

	return MACSUCCESS;
}

static u32 txdes_proc_mgnt(struct mac_ax_adapter *adapter,
			   struct mac_ax_txpkt_info *info, u8 *buf, u32 len)
{
	struct wd_body_t *wdb;
	struct wd_info_t *wdi;
	struct mac_ax_pkt_mgnt *datai = &info->u.mgnt;
	struct mac_role_tbl *role;

	if (len != mac_txdesc_len(adapter, info))
		return MACBUFSZ;

	role = mac_role_srch(adapter, datai->macid);
	if (!role) {
		PLTFM_MSG_ERR("[ERR]cannot find macid: %d\n", datai->macid);
		return MACNOITEM;
	}

	/* only use ch0 in initial development phase, */
	/* and modify it for normal using later.*/
	/* wd_info is always appended in initial development phase */
	wdb = (struct wd_body_t *)buf;
	if (adapter->hw_info->intf == MAC_AX_INTF_SDIO)
		wdb->dword0 =
			cpu_to_le32(AX_TXD_STF_MODE);
	else if (adapter->hw_info->intf == MAC_AX_INTF_USB)
		wdb->dword0 =
			cpu_to_le32(AX_TXD_STF_MODE |
				    (datai->pkt_offset ?
				     AX_TXD_PKT_OFFSET : 0));
	else
		wdb->dword0 =
			cpu_to_le32((datai->wd_page ? AX_TXD_WD_PAGE : 0) |
				    (adapter->dle_info.qta_mode ==
				     MAC_AX_QTA_SCC_STF ||
				     adapter->dle_info.qta_mode ==
				     MAC_AX_QTA_DBCC_STF ?
				     AX_TXD_STF_MODE : 0));

	wdb->dword0 |=
		cpu_to_le32(SET_WORD(datai->hw_seq_mode,
				     AX_TXD_EN_HWSEQ_MODE) |
			    SET_WORD(datai->hw_ssn_sel,
				     AX_TXD_HW_SSN_SEL) |
			    SET_WORD(datai->hdr_len,
				     AX_TXD_HDR_LLC_LEN) |
			    (datai->wdinfo_en ? AX_TXD_WDINFO_EN : 0));

	if (adapter->hw_info->intf == MAC_AX_INTF_USB &&
	    is_chip_id(adapter, MAC_AX_CHIP_ID_8852A) &&
	    is_chip_cut(adapter, CHIP_CUT_A))
		wdb->dword0 |=
			cpu_to_le32(SET_WORD(MAC_AX_DMA_B1MG, AX_TXD_CH_DMA));
	else
		wdb->dword0 |=
			cpu_to_le32(SET_WORD((role->info.band ?
					       MAC_AX_DMA_B1MG :
					       MAC_AX_DMA_B0MG),
					       AX_TXD_CH_DMA));

	wdb->dword1 = 0;
	/* Get bb and qsel from qsel by according MAC ID */
	wdb->dword2 =
		cpu_to_le32(SET_WORD(info->pktsize, AX_TXD_TXPKTSIZE) |
			    SET_WORD((role->info.band ?
				      MAC_AX_MG1_SEL : MAC_AX_MG0_SEL),
				     AX_TXD_QSEL) |
				     SET_WORD(role->macid, AX_TXD_MACID));
	wdb->dword3 = cpu_to_le32(SET_WORD(datai->wifi_seq,
					   AX_TXD_WIFI_SEQ) |
				 (datai->bk ? AX_TXD_BK : 0));
	wdb->dword4 = 0;
	wdb->dword5 = 0;

	wdi = (struct wd_info_t *)wd_info_tmpl[WD_INFO_PKT_NORMAL];
	wdi->dword0 =
		cpu_to_le32((datai->userate ? AX_TXD_USERATE_SEL : 0) |
			    SET_WORD(datai->data_rate, AX_TXD_DATARATE) |
			    SET_WORD(datai->data_bw, AX_TXD_DATA_BW) |
			    (datai->er_bw ? AX_TXD_DATA_BW_ER : 0) |
			    SET_WORD(datai->data_gi_ltf, AX_TXD_GI_LTF) |
			    (datai->data_er ? AX_TXD_DATA_ER : 0) |
			    (datai->data_dcm ? AX_TXD_DATA_DCM : 0) |
			    SET_WORD(datai->data_stbc, AX_TXD_DATA_STBC) |
			    (datai->data_ldpc ? AX_TXD_DATA_LDPC : 0) |
			    (datai->dis_data_fb ? AX_TXD_DISDATAFB : 0) |
			    (datai->dis_rts_fb ? AX_TXD_DISRTSFB : 0) |
			    SET_WORD(datai->multiport_id, AX_TXD_MULTIPORT_ID) |
			    SET_WORD(datai->mbssid, AX_TXD_MBSSID));
	wdi->dword1 =
		cpu_to_le32(SET_WORD(datai->max_agg_num, AX_TXD_MAX_AGG_NUM) |
			    SET_WORD(datai->tx_cnt_lmt, AX_TXD_DATA_TXCNT_LMT) |
			    (datai->tx_cnt_lmt_sel ?
			     AX_TXD_DATA_TXCNT_LMT_SEL : 0) |
			    (datai->nav_use_hdr ? AX_TXD_NAVUSEHDR : 0) |
			    (datai->bmc ? AX_TXD_BMC : 0));
	wdi->dword2 =
		cpu_to_le32(SET_WORD(datai->lifetime_sel, AX_TXD_LIFETIME_SEL) |
			    SET_WORD(datai->sec_type, AX_TXD_SECTYPE) |
			    (datai->hw_sec_en ? AX_TXD_SEC_HW_ENC : 0) |
			    (datai->no_ack ? AX_TXD_NO_ACK : 0) |
			    SET_WORD(datai->sec_cam_idx, AX_TXD_SEC_CAM_IDX) |
			    SET_WORD(datai->ampdu_density,
				     AX_TXD_AMPDU_DENSITY));

	wdi->dword3 =
		cpu_to_le32((datai->null_0 ? AX_TXD_NULL_0 : 0) |
			    (datai->null_1 ? AX_TXD_NULL_1 : 0) |
			    (datai->sifs_tx ? AX_TXD_SIFS_TX : 0) |
			    SET_WORD(datai->ndpa, AX_TXD_NDPA) |
			    SET_WORD(datai->snd_pkt_sel, AX_TXD_SND_PKT_SEL) |
			    (datai->rtt ? AX_TXD_RTT_EN : 0));

	wdi->dword4 =
		cpu_to_le32((datai->rts_en ? AX_TXD_RTS_EN : 0) |
			    (datai->cts2self ? AX_TXD_CTS2SELF : 0) |
			    SET_WORD(datai->cca_rts, AX_TXD_CCA_RTS) |
			    (datai->hw_rts_en ? AX_TXD_HW_RTS_EN : 0));

	wdi->dword5 =
		cpu_to_le32(SET_WORD(datai->ndpa_dur, AX_TXD_NDPA_DURATION));

	if (datai->wdinfo_en != 0)
		PLTFM_MEMCPY(buf + WD_BODY_LEN, (u8 *)wdi, WD_INFO_LEN);

	if (adapter->hw_info->wd_checksum_en)
		mac_wd_checksum(adapter, info, buf);

	return MACSUCCESS;
}

static struct txd_proc_type txdes_proc_mac[] = {
	{MAC_AX_PKT_H2C, txdes_proc_h2c_fwdl},
	{MAC_AX_PKT_FWDL, txdes_proc_h2c_fwdl},
	{MAC_AX_PKT_DATA, txdes_proc_data},
	{MAC_AX_PKT_MGNT, txdes_proc_mgnt},
	{MAC_AX_PKT_INVALID, NULL},
};

#endif // if 0 NEO


#include <linux/bitfield.h>

#define SET_TX_DESC_TXPKTSIZE(txdesc, value)                                   \
	le32p_replace_bits((__le32 *)(txdesc) + 0x00, value, GENMASK(15, 0))
#define SET_TX_DESC_OFFSET(txdesc, value)                                      \
	le32p_replace_bits((__le32 *)(txdesc) + 0x00, value, GENMASK(23, 16))
#define SET_TX_DESC_PKT_OFFSET(txdesc, value)                                  \
	le32p_replace_bits((__le32 *)(txdesc) + 0x01, value, GENMASK(28, 24))
#define SET_TX_DESC_QSEL(txdesc, value)                                        \
	le32p_replace_bits((__le32 *)(txdesc) + 0x01, value, GENMASK(12, 8))
#define SET_TX_DESC_BMC(txdesc, value)                                         \
	le32p_replace_bits((__le32 *)(txdesc) + 0x00, value, BIT(24))
#define SET_TX_DESC_RATE_ID(txdesc, value)                                     \
	le32p_replace_bits((__le32 *)(txdesc) + 0x01, value, GENMASK(20, 16))
#define SET_TX_DESC_DATARATE(txdesc, value)                                    \
	le32p_replace_bits((__le32 *)(txdesc) + 0x04, value, GENMASK(6, 0))
#define SET_TX_DESC_DISDATAFB(txdesc, value)                                   \
	le32p_replace_bits((__le32 *)(txdesc) + 0x03, value, BIT(10))
#define SET_TX_DESC_USE_RATE(txdesc, value)                                    \
	le32p_replace_bits((__le32 *)(txdesc) + 0x03, value, BIT(8))
#define SET_TX_DESC_SEC_TYPE(txdesc, value)                                    \
	le32p_replace_bits((__le32 *)(txdesc) + 0x01, value, GENMASK(23, 22))
#define SET_TX_DESC_DATA_BW(txdesc, value)                                     \
	le32p_replace_bits((__le32 *)(txdesc) + 0x05, value, GENMASK(6, 5))
#define SET_TX_DESC_SW_SEQ(txdesc, value)                                      \
	le32p_replace_bits((__le32 *)(txdesc) + 0x09, value, GENMASK(23, 12))
#define SET_TX_DESC_MAX_AGG_NUM(txdesc, value)                                 \
	le32p_replace_bits((__le32 *)(txdesc) + 0x03, value, GENMASK(21, 17))
#define SET_TX_DESC_USE_RTS(tx_desc, value)                                    \
	le32p_replace_bits((__le32 *)(txdesc) + 0x03, value, BIT(12))
#define SET_TX_DESC_AMPDU_DENSITY(txdesc, value)                               \
	le32p_replace_bits((__le32 *)(txdesc) + 0x02, value, GENMASK(22, 20))
#define SET_TX_DESC_DATA_STBC(txdesc, value)                                   \
	le32p_replace_bits((__le32 *)(txdesc) + 0x05, value, GENMASK(9, 8))
#define SET_TX_DESC_DATA_LDPC(txdesc, value)                                   \
	le32p_replace_bits((__le32 *)(txdesc) + 0x05, value, BIT(7))
#define SET_TX_DESC_AGG_EN(txdesc, value)                                      \
	le32p_replace_bits((__le32 *)(txdesc) + 0x02, value, BIT(12))
#define SET_TX_DESC_LS(txdesc, value)                                          \
	le32p_replace_bits((__le32 *)(txdesc) + 0x00, value, BIT(26))
#define SET_TX_DESC_DATA_SHORT(txdesc, value)				       \
	le32p_replace_bits((__le32 *)(txdesc) + 0x05, value, BIT(4))
#define SET_TX_DESC_SPE_RPT(tx_desc, value)                                    \
	le32p_replace_bits((__le32 *)(txdesc) + 0x02, value, BIT(19))
#define SET_TX_DESC_SW_DEFINE(tx_desc, value)                                  \
	le32p_replace_bits((__le32 *)(txdesc) + 0x06, value, GENMASK(11, 0))
#define SET_TX_DESC_DISQSELSEQ(txdesc, value)                                 \
	le32p_replace_bits((__le32 *)(txdesc) + 0x00, value, BIT(31))
#define SET_TX_DESC_EN_HWSEQ(txdesc, value)                                   \
	le32p_replace_bits((__le32 *)(txdesc) + 0x08, value, BIT(15))
#define SET_TX_DESC_HW_SSN_SEL(txdesc, value)                                 \
	le32p_replace_bits((__le32 *)(txdesc) + 0x03, value, GENMASK(7, 6))
#define SET_TX_DESC_NAVUSEHDR(txdesc, value)				       \
	le32p_replace_bits((__le32 *)(txdesc) + 0x03, value, BIT(15))
#define SET_TX_DESC_BT_NULL(txdesc, value)				       \
	le32p_replace_bits((__le32 *)(txdesc) + 0x02, value, BIT(23))
#define SET_TX_DESC_TXDESC_CHECKSUM(txdesc, value)                             \
	le32p_replace_bits((__le32 *)(txdesc) + 0x07, value, GENMASK(15, 0))
#define SET_TX_DESC_DMA_TXAGG_NUM(txdesc, value)                             \
	le32p_replace_bits((__le32 *)(txdesc) + 0x07, value, GENMASK(31, 24))
#define GET_TX_DESC_PKT_OFFSET(txdesc) \
	le32_get_bits(*((__le32 *)(txdesc) + 0x01), GENMASK(28, 24))
#define GET_TX_DESC_QSEL(txdesc) \
	le32_get_bits(*((__le32 *)(txdesc) + 0x01), GENMASK(12, 8))

static inline
void fill_txdesc_checksum_common(u8 *txdesc, size_t words)
{
	__le16 chksum = 0;
	__le16 *data = (__le16 *)(txdesc);

	SET_TX_DESC_TXDESC_CHECKSUM(txdesc, 0x0000);

	while (words--)
		chksum ^= *data++;

	SET_TX_DESC_TXDESC_CHECKSUM(txdesc, __le16_to_cpu(chksum));
}

u32 mac_wd_checksum(struct mac_adapter *adapter,
		    struct mac_txpkt_info *info, u8 *wddesc)
{
	struct mac_pkt_data *data = &info->u.data;
	size_t words;

	if (!wddesc) {
		PLTFM_MSG_ERR("[ERR]null pointer\n");
		return MACNPTR;
	}

	words = (data->pkt_offset * 8 + 48) / 2;
	fill_txdesc_checksum_common(wddesc, words);

	return MACSUCCESS;
}

u32 mac_build_txdesc(struct mac_adapter *adapter,
		     struct mac_txpkt_info *info, u8 *buf, u32 len)
{
	struct mac_pkt_data *pkt_info = &info->u.data;
	__le32 *txdesc = (__le32 *)buf;

	if (!info) {
		PLTFM_MSG_ERR("[ERR] info is NULL\n");
		return MACNPTR;
	}

	SET_TX_DESC_TXPKTSIZE(txdesc,  info->pktsize);
	SET_TX_DESC_OFFSET(txdesc, pkt_info->offset);
	SET_TX_DESC_PKT_OFFSET(txdesc, pkt_info->pkt_offset);
	SET_TX_DESC_RATE_ID(txdesc, pkt_info->rate_id);
	SET_TX_DESC_DATARATE(txdesc, pkt_info->data_rate);
	SET_TX_DESC_QSEL(txdesc, pkt_info->qsel);
	SET_TX_DESC_DATA_BW(txdesc, pkt_info->data_bw);
	SET_TX_DESC_SEC_TYPE(txdesc, pkt_info->sec_type);
	SET_TX_DESC_AGG_EN(txdesc, pkt_info->agg_en);
	SET_TX_DESC_MAX_AGG_NUM(txdesc, pkt_info->max_agg_num);
	SET_TX_DESC_AMPDU_DENSITY(txdesc, pkt_info->ampdu_density);
	SET_TX_DESC_SW_SEQ(txdesc, pkt_info->wifi_seq);
	SET_TX_DESC_DATA_STBC(txdesc, pkt_info->data_stbc);
	SET_TX_DESC_DATA_LDPC(txdesc, pkt_info->data_ldpc);
	SET_TX_DESC_DISDATAFB(txdesc, pkt_info->dis_data_fb);
	SET_TX_DESC_BMC(txdesc, pkt_info->bmc);
	SET_TX_DESC_USE_RATE(txdesc, pkt_info->userate);
	SET_TX_DESC_LS(txdesc, pkt_info->ls);
	SET_TX_DESC_DATA_SHORT(txdesc, pkt_info->short_gi);
	SET_TX_DESC_SPE_RPT(txdesc, pkt_info->report);
	SET_TX_DESC_DISQSELSEQ(txdesc, pkt_info->dis_qselseq);
	SET_TX_DESC_EN_HWSEQ(txdesc, pkt_info->en_hwseq);
	SET_TX_DESC_HW_SSN_SEL(txdesc, pkt_info->hw_ssn_sel);
	SET_TX_DESC_NAVUSEHDR(txdesc, pkt_info->nav_use_hdr);

	mac_wd_checksum(adapter, info, (u8 *)(txdesc));

	return MACSUCCESS;
}

#if 0 // NEO

u32 mac_refill_txdesc(struct mac_ax_adapter *adapter,
		      struct mac_ax_txpkt_info *txpkt_info,
		      struct mac_ax_refill_info *mask,
		      struct mac_ax_refill_info *info)
{
	u32 dw0 = ((struct wd_body_t *)info->pkt)->dword0;
	u32 dw1 = ((struct wd_body_t *)info->pkt)->dword1;

	if (mask->packet_offset)
		((struct wd_body_t *)info->pkt)->dword0 =
			dw0 | (info->packet_offset ? AX_TXD_PKT_OFFSET : 0);

	if (mask->agg_num == AX_TXD_DMA_TXAGG_NUM_MSK)
		((struct wd_body_t *)info->pkt)->dword1 =
			SET_CLR_WORD(dw1, info->agg_num, AX_TXD_DMA_TXAGG_NUM);

	if (adapter->hw_info->wd_checksum_en)
		mac_wd_checksum(adapter, txpkt_info, info->pkt);

	return MACSUCCESS;
}

static u32 rxdes_parse_comm(struct mac_ax_adapter *adapter,
			    struct mac_ax_rxpkt_info *info, u8 *buf)
{
	u32 hdr_val = le32_to_cpu(((struct rxd_short_t *)buf)->dword0);

	info->rxdlen = hdr_val & AX_RXD_LONG_RXD ? RXD_LONG_LEN : RXD_SHORT_LEN;
	info->pktsize = GET_FIELD(hdr_val, AX_RXD_RPKT_LEN);
	info->shift = (u8)GET_FIELD(hdr_val, AX_RXD_SHIFT);
	info->drvsize = (u8)GET_FIELD(hdr_val, AX_RXD_DRV_INFO_SIZE);

	return MACSUCCESS;
}

static u32 rxdes_parse_wifi(struct mac_ax_adapter *adapter,
			    struct mac_ax_rxpkt_info *info, u8 *buf, u32 len)
{
	u32 hdr_val;

	info->type = MAC_AX_PKT_DATA;

	hdr_val = le32_to_cpu(((struct rxd_short_t *)buf)->dword3);
	info->u.data.crc_err = !!(hdr_val & AX_RXD_CRC32_ERR);
	info->u.data.icv_err = !!(hdr_val & AX_RXD_ICV_ERR);

	return MACSUCCESS;
}

static u32 rxdes_parse_c2h(struct mac_ax_adapter *adapter,
			   struct mac_ax_rxpkt_info *info, u8 *buf, u32 len)
{
	info->type = MAC_AX_PKT_C2H;

	return MACSUCCESS;
}

static u32 rxdes_parse_ch_info(struct mac_ax_adapter *adapter,
			       struct mac_ax_rxpkt_info *info, u8 *buf, u32 len)
{
	info->type = MAC_AX_PKT_CH_INFO;

	return MACSUCCESS;
}

static u32 rxdes_parse_dfs(struct mac_ax_adapter *adapter,
			   struct mac_ax_rxpkt_info *info, u8 *buf, u32 len)
{
	info->type = MAC_AX_PKT_DFS;

	return MACSUCCESS;
}

static u32 rxdes_parse_ppdu(struct mac_ax_adapter *adapter,
			    struct mac_ax_rxpkt_info *info, u8 *buf, u32 len)
{
	u32 hdr_val = le32_to_cpu(((struct rxd_short_t *)buf)->dword0);

	info->type = MAC_AX_PKT_PPDU;
	info->u.ppdu.mac_info = !!(hdr_val & AX_RXD_MAC_INFO_VLD);

	return MACSUCCESS;
}

static struct rxd_parse_type rxdes_parse_mac[] = {
	{RXD_S_RPKT_TYPE_WIFI, rxdes_parse_wifi},
	{RXD_S_RPKT_TYPE_C2H, rxdes_parse_c2h},
	{RXD_S_RPKT_TYPE_PPDU, rxdes_parse_ppdu},
	{RXD_S_RPKT_TYPE_CH_INFO, rxdes_parse_ch_info},
	{RXD_S_RPKT_TYPE_DFS_RPT, rxdes_parse_dfs},
	{RXD_RPKT_TYPE_INVALID, NULL},
};

u32 mac_parse_rxdesc(struct mac_ax_adapter *adapter,
		     struct mac_ax_rxpkt_info *info, u8 *buf, u32 len)
{
	struct rxd_parse_type *parse = rxdes_parse_mac;
	u8 rpkt_type;
	u32 hdr_val;
	u32 (*handler)(struct mac_ax_adapter *adapter,
		       struct mac_ax_rxpkt_info *info, u8 *buf, u32 len) = NULL;

	hdr_val = le32_to_cpu(((struct rxd_short_t *)buf)->dword0);
	rpkt_type = (u8)GET_FIELD(hdr_val, AX_RXD_RPKT_TYPE);

	rxdes_parse_comm(adapter, info, buf);

	for (; parse->type != RXD_RPKT_TYPE_INVALID; parse++) {
		if (rpkt_type == parse->type) {
			handler = parse->handler;
			break;
		}
	}

	if (!handler) {
		PLTFM_MSG_ERR("[ERR]null type handler type: %X\n", parse->type);
		return MACNOITEM;
	}

	return handler(adapter, info, buf, len);
}
#endif // if 0 NEO

