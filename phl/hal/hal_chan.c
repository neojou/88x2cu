/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
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
#define _HAL_CHAN_C_
#include "hal_headers.h"

#if 0 // NEO TODO
#ifdef CONFIG_PHL_DFS
enum rtw_hal_status
rtw_hal_radar_detect_cfg(void *hal, bool dfs_enable)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	u8 rpt_num = 0;/*52A A,B cut - 29/61/93/125 pkt*/
	u8 rpt_to = 1;/*52A A cut - 0 , 80us ,2ms, 52A B cut - 0 / 20ms / 40ms / 80ms*/

	PHL_INFO("====>%s dfs_en:%d ============\n", __func__, dfs_enable);
	if (dfs_enable) {
		rtw_hal_mac_dfs_rpt_cfg(hal_info, true, rpt_num, rpt_to);
		rtw_hal_bb_dfs_rpt_cfg(hal_info, true);
	}
	else {
		rtw_hal_mac_dfs_rpt_cfg(hal_info, false, 0, 0);
		rtw_hal_bb_dfs_rpt_cfg(hal_info, true);
	}
	return RTW_HAL_STATUS_SUCCESS;
}
#endif /*CONFIG_PHL_DFS*/

#endif // if 0 NEO

enum rtw_hal_status rtw_hal_set_ch_bw(void *hal, u8 band_idx,
		struct rtw_chan_def *chdef, bool do_rfk)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct rtw_chan_def *cur_chdef = &(hal_com->band[band_idx].cur_chandef);
	enum rtw_hal_status status = RTW_HAL_STATUS_SUCCESS;
	u8 center_ch = 0;
	u8 central_ch_seg1 = 0;
	enum band_type change_band;
	enum phl_phy_idx phy_idx = HW_PHY_0;

#if 1 // NEO
	struct dvobj_priv *dvobj = hal_com->drv_priv;
	_adapter *padapter = dvobj_get_primary_adapter(dvobj);
	u8 channel_offset, chnl_offset80 = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
	u8 chan = chdef->chan;
	enum channel_width bw = chdef->bw;
	enum chan_offset offset = chdef->offset;


	switch (offset) {
	case CHAN_OFFSET_NO_EXT:
		channel_offset = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
		break;
	case CHAN_OFFSET_UPPER:
		channel_offset = HAL_PRIME_CHNL_OFFSET_UPPER;
		break;
	case CHAN_OFFSET_LOWER:
		channel_offset = HAL_PRIME_CHNL_OFFSET_LOWER;
		break;
	case CHAN_OFFSET_NO_DEF:
	default:
		RTW_ERR("%s offset(%d) is not as expected\n", offset);
		break;
	}

	RTW_INFO("%s NEO offset(%d), channel_offset(%d)\n", __func__, offset, channel_offset);

	if (bw == CHANNEL_WIDTH_80) {
		if (center_ch > chan)
			chnl_offset80 = HAL_PRIME_CHNL_OFFSET_LOWER;
		else if (center_ch < chan)
			chnl_offset80 = HAL_PRIME_CHNL_OFFSET_UPPER;
		else
			chnl_offset80 = HAL_PRIME_CHNL_OFFSET_DONT_CARE;
	}

	RTW_INFO("%s NEO bw(%d), center_ch(%d), channel(%d), chnl_offset80(%d)\n",
		 __func__, bw, center_ch, chan, chnl_offset80);


	if ((chdef->chan != cur_chdef->chan) || (chdef->bw != cur_chdef->bw)) {
		if (band_idx == 1) {
			RTW_ERR("%s: band_idx==1\n", __func__);
			goto err_ret;
		}
		center_ch = rtw_phl_get_center_ch(chan, bw, offset);
		PHL_INFO("Using central channel %u for primary channel %u BW %u\n",
		         center_ch, chan, bw);

		rtw_set_oper_ch(padapter, chan);
		rtw_set_oper_bw(padapter, bw);
		rtw_set_oper_choffset(padapter, channel_offset);

		rtw_hal_set_chnl_bw(padapter, center_ch, bw, channel_offset, chnl_offset80);
	}

err_ret:
	return status;
#else
	if ((chandef->chan != cur_chdef->chan) || (chandef->bw != cur_chdef->bw)) {
		if (band_idx == 1)
			phy_idx = HW_PHY_1;

		status = rtw_hal_reset(hal_com, phy_idx, band_idx, true);
		if(status != RTW_HAL_STATUS_SUCCESS) {
			PHL_ERR("%s rtw_hal_reset en - failed\n", __func__);
			_os_warn_on(1);
		}
		/* if central channel changed, reset BB & MAC */
		center_ch = rtw_phl_get_center_ch(chan, bw, offset);
		PHL_INFO("Using central channel %u for primary channel %u BW %u\n",
		         center_ch, chan, bw);
		status = rtw_hal_mac_set_bw(hal_info, band_idx,  chan,
					      center_ch, central_ch_seg1, bw);
		if(status != RTW_HAL_STATUS_SUCCESS) {
			PHL_ERR("%s rtw_hal_mac_set_bw - failed\n", __func__);
			return status;
		}


		if(bw == CHANNEL_WIDTH_80_80 && central_ch_seg1 == 0) {
			PHL_ERR("%s mising info for 80+80M configuration\n", __func__);
			return RTW_HAL_STATUS_FAILURE;
		}
		status = rtw_hal_bb_set_ch_bw(hal_info, phy_idx, chan,
					      center_ch, central_ch_seg1, bw);
		if(status != RTW_HAL_STATUS_SUCCESS) {
			PHL_ERR("%s rtw_hal_bb_set_ch_bw - failed\n", __func__);
			return status;
		}

		chandef->chan = chan;
		chandef->bw = bw;
		chandef->center_ch = center_ch;

		if(chan > 14)
			change_band = BAND_ON_5G;
		else
			change_band = BAND_ON_24G;

		if (chandef->band != change_band) {
			chandef->band = change_band;
			rtw_hal_notify_switch_band(hal, change_band, phy_idx);
		}

		status = rtw_hal_rf_set_power(hal_info, phy_idx, PWR_LIMIT);

		if(status != RTW_HAL_STATUS_SUCCESS) {
			PHL_ERR("%s rtw_hal_rf_set_power - failed\n", __func__);
			return status;
		}

		status = rtw_hal_rf_set_power(hal_info, phy_idx, PWR_LIMIT_RU);

		if(status != RTW_HAL_STATUS_SUCCESS) {
			PHL_ERR("%s rtw_hal_rf_set_power - failed\n", __func__);
			return status;
		}

		PHL_INFO("%s band_idx:%d, ch:%d, bw:%d, offset:%d\n",
			__func__, band_idx, chan, bw, offset);

		status = rtw_hal_reset(hal_com, phy_idx, band_idx, false);
		if(status != RTW_HAL_STATUS_SUCCESS) {
			PHL_ERR("%s rtw_hal_reset dis- failed\n", __func__);
			_os_warn_on(1);
		}

		/*PHL_DUMP_CHAN_DEF_EX(chandef);*/
	}

	PHL_INFO("%s : do_rfk:%s\n",
		 __func__, (do_rfk) ?"Y" : "N");

	if (do_rfk) {
		status = rtw_hal_rf_chl_rfk_trigger(hal_info, phy_idx, true);
		if (status != RTW_HAL_STATUS_SUCCESS)
			PHL_ERR("rtw_hal_rf_chl_rfk_trigger fail!\n");
	}
#endif
	return status;
}

u8 rtw_hal_get_cur_ch(void *hal, u8 band_idx)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct rtw_chan_def *chandef = &(hal_com->band[band_idx].cur_chandef);

	return chandef->chan;
}

void rtw_hal_sync_cur_ch(void *hal, u8 band_idx, struct rtw_chan_def chandef)
{
	struct hal_info_t *hal_info = (struct hal_info_t *)hal;
	struct rtw_hal_com_t *hal_com = hal_info->hal_com;
	struct rtw_chan_def *cur_chandef = &(hal_com->band[band_idx].cur_chandef);

	PHL_INFO("%s: Sync cur chan to ch:%d bw:%d offset:%d\n",
		 __FUNCTION__, chandef.chan, chandef.bw, chandef.offset);
	cur_chandef->chan = chandef.chan;
	cur_chandef->bw = chandef.bw;
	cur_chandef->offset = chandef.offset;
}


