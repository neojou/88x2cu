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
#define _RTL8822C_PHY_C_

#include <hal_data.h>		/* HAL_DATA_TYPE */
#include "../hal_halmac.h"
#include "rtl8822c.h"

/*
 * ============================================================
 * functions
 * ============================================================
 */
static void init_phydm_cominfo(PADAPTER adapter)
{
	PHAL_DATA_TYPE hal;
	struct dm_struct *p_dm_odm;
	u32 support_ability = 0;

	hal = GET_HAL_DATA(adapter);
	p_dm_odm = &hal->odmpriv;

	Init_ODM_ComInfo(adapter);

	odm_cmn_info_init(p_dm_odm, ODM_CMNINFO_PACKAGE_TYPE, hal->PackageType);

	odm_cmn_info_init(p_dm_odm, ODM_CMNINFO_DIS_DPD
		, hal->txpwr_pg_mode == TXPWR_PG_WITH_PWR_IDX ? _TRUE : _FALSE);
	odm_cmn_info_init(p_dm_odm, ODM_CMNINFO_TSSI_ENABLE
		, hal->txpwr_pg_mode == TXPWR_PG_WITH_TSSI_OFFSET ? _TRUE : _FALSE);
}

void rtl8822c_phy_init_dm_priv(PADAPTER adapter)
{
	struct dm_struct *podmpriv = adapter_to_phydm(adapter);

	init_phydm_cominfo(adapter);
	odm_init_all_timers(podmpriv);
}

void rtl8822c_phy_deinit_dm_priv(PADAPTER adapter)
{
	struct dm_struct *podmpriv = adapter_to_phydm(adapter);

	odm_cancel_all_timers(podmpriv);
}

void rtl8822c_phy_init_haldm(PADAPTER adapter)
{
	rtw_phydm_init(adapter);
}

static u32 phy_calculatebitshift(u32 mask)
{
	u32 i;


	for (i = 0; i <= 31; i++)
		if (mask & BIT(i))
			break;

	return i;
}

u32 rtl8822c_read_bb_reg(PADAPTER adapter, u32 addr, u32 mask)
{
	u32 val = 0, val_org, shift;


#if (DISABLE_BB_RF == 1)
	return 0;
#endif

	val_org = rtw_read32(adapter, addr);
	shift = phy_calculatebitshift(mask);
	val = (val_org & mask) >> shift;

	return val;
}

void rtl8822c_write_bb_reg(PADAPTER adapter, u32 addr, u32 mask, u32 val)
{
	u32 val_org, shift;


#if (DISABLE_BB_RF == 1)
	return;
#endif

	if (mask != 0xFFFFFFFF) {
		/* not "double word" write */
		val_org = rtw_read32(adapter, addr);
		shift = phy_calculatebitshift(mask);
		val = ((val_org & (~mask)) | ((val << shift) & mask));
	}

	rtw_write32(adapter, addr, val);
}

u32 rtl8822c_read_rf_reg(PADAPTER adapter, enum rf_path path, u32 addr, u32 mask)
{
	struct dm_struct *phydm = adapter_to_phydm(adapter);
	u32 val;

	val = config_phydm_read_rf_reg_8822c(phydm, path, addr, mask);
	if (!config_phydm_read_rf_check_8822c(val))
		RTW_INFO(FUNC_ADPT_FMT ": read RF reg path=%d addr=0x%x mask=0x%x FAIL!\n",
			 FUNC_ADPT_ARG(adapter), path, addr, mask);

	return val;
}

void rtl8822c_write_rf_reg(PADAPTER adapter, enum rf_path path, u32 addr, u32 mask, u32 val)
{
	struct dm_struct *phydm = adapter_to_phydm(adapter);
	u8 ret;

	ret = config_phydm_write_rf_reg_8822c(phydm, path, addr, mask, val);
	if (_FALSE == ret)
		RTW_INFO(FUNC_ADPT_FMT ": write RF reg path=%d addr=0x%x mask=0x%x val=0x%x FAIL!\n",
			 FUNC_ADPT_ARG(adapter), path, addr, mask, val);
}

static void set_tx_power_level_by_path(PADAPTER adapter, u8 channel, u8 path)
{
	PHAL_DATA_TYPE hal = GET_HAL_DATA(adapter);
	u8 under_survey_ch = phy_check_under_survey_ch(adapter);
	u8 under_24g = (hal->current_band_type == BAND_ON_24G);

	if (under_24g)
		phy_set_tx_power_index_by_rate_section(adapter, path, channel, CCK);

	phy_set_tx_power_index_by_rate_section(adapter, path, channel, OFDM);

	if (!under_survey_ch) {
		phy_set_tx_power_index_by_rate_section(adapter, path, channel, HT_MCS0_MCS7);
		phy_set_tx_power_index_by_rate_section(adapter, path, channel, HT_MCS8_MCS15);
		phy_set_tx_power_index_by_rate_section(adapter, path, channel, VHT_1SSMCS0_1SSMCS9);
		phy_set_tx_power_index_by_rate_section(adapter, path, channel, VHT_2SSMCS0_2SSMCS9);
	}
}

void rtl8822c_set_tx_power_level(PADAPTER adapter, u8 channel)
{
	struct hal_spec_t *hal_spec = GET_HAL_SPEC(adapter);
	u8 path;

	for (path = RF_PATH_A; path < hal_spec->rf_reg_path_num; ++path) {
		/*
		* can't bypass unused path
		* because phydm need all path values to calculate min diff
		*/
		set_tx_power_level_by_path(adapter, channel, path);
	}
}

void rtl8822c_set_txpwr_done(_adapter *adapter)
{
	HAL_DATA_TYPE *hal_data = GET_HAL_DATA(adapter);
	struct dm_struct *phydm = adapter_to_phydm(adapter);

	config_phydm_set_txagc_to_hw_8822c(phydm);

#ifdef CONFIG_TXPWR_PG_WITH_TSSI_OFFSET
	if (hal_data->txpwr_pg_mode == TXPWR_PG_WITH_TSSI_OFFSET
	) {
		halrf_calculate_tssi_codeword(phydm);
		halrf_set_tssi_codeword(phydm);
	}
#endif
}

u8 rtl8822c_get_dis_dpd_by_rate_diff(PADAPTER adapter, u8 rate)
{
	struct dm_struct *phydm = adapter_to_phydm(adapter);
	u16 dis_dpd_rate;
	u8 dis_dpd_rate_diff = 0;

	dis_dpd_rate = phydm_get_dis_dpd_by_rate_8822c(phydm);
	switch (rate) {
		case MGN_6M:
				((dis_dpd_rate & BIT(0)) == BIT(0))?(dis_dpd_rate_diff = 3):(dis_dpd_rate_diff = 0);
			break;
		case MGN_9M:
				((dis_dpd_rate & BIT(1)) == BIT(1))?(dis_dpd_rate_diff = 3):(dis_dpd_rate_diff = 0);
			break;
		case MGN_MCS0:
				((dis_dpd_rate & BIT(2)) == BIT(2))?(dis_dpd_rate_diff = 3):(dis_dpd_rate_diff = 0);			
			break;
		case MGN_MCS1:
				((dis_dpd_rate & BIT(3)) == BIT(3))?(dis_dpd_rate_diff = 3):(dis_dpd_rate_diff = 0);			
			break;
		case MGN_MCS8:
				((dis_dpd_rate & BIT(4)) == BIT(4))?(dis_dpd_rate_diff = 3):(dis_dpd_rate_diff = 0);				
			break;
		case MGN_MCS9:
				((dis_dpd_rate & BIT(5)) == BIT(5))?(dis_dpd_rate_diff = 3):(dis_dpd_rate_diff = 0);			
			break;
		case MGN_VHT1SS_MCS0:
				((dis_dpd_rate & BIT(6)) == BIT(6))?(dis_dpd_rate_diff = 3):(dis_dpd_rate_diff = 0);				
			break;
		case MGN_VHT1SS_MCS1:
				((dis_dpd_rate & BIT(7)) == BIT(7))?(dis_dpd_rate_diff = 3):(dis_dpd_rate_diff = 0);				
			break;
		case MGN_VHT2SS_MCS0:
				((dis_dpd_rate & BIT(8)) == BIT(8))?(dis_dpd_rate_diff = 3):(dis_dpd_rate_diff = 0);			
			break;
		case MGN_VHT2SS_MCS1:
				((dis_dpd_rate & BIT(9)) == BIT(9))?(dis_dpd_rate_diff = 3):(dis_dpd_rate_diff = 0);			
			break;
		default:
			dis_dpd_rate_diff = 0;
			break;
	}

	return dis_dpd_rate_diff;
}

/*
 * Parameters:
 *	padatper
 *	powerindex	power index for rate
 *	rfpath		Antenna(RF) path, type "enum rf_path"
 *	rate		data rate, type "enum MGN_RATE"
 */
void rtl8822c_set_tx_power_index(PADAPTER adapter, u32 powerindex, enum rf_path rfpath, u8 rate)
{
	HAL_DATA_TYPE *hal = GET_HAL_DATA(adapter);
	struct dm_struct *phydm = adapter_to_phydm(adapter);
	u8 shift = 0;
	boolean write_ret;

	if (!IS_1T_RATE(rate) && !IS_2T_RATE(rate)) {
		RTW_ERR(FUNC_ADPT_FMT" invalid rate(%s)\n", FUNC_ADPT_ARG(adapter), MGN_RATE_STR(rate));
		rtw_warn_on(1);
		goto exit;
	}

	rate = MRateToHwRate(rate);

	/*
	* For 8822C, phydm api use 4 bytes txagc value
	* driver must combine every four 1 byte to one 4 byte and send to phydm api
	*/
	shift = rate % 4;
	hal->txagc_set_buf |= ((powerindex & 0xff) << (shift * 8));

	if (shift != 3)
		goto exit;

	rate = rate & 0xFC;
	write_ret = config_phydm_write_txagc_8822c(phydm, hal->txagc_set_buf, rfpath, rate);

	if (write_ret == true)
		goto clear_buf;

	rtw_warn_on(write_ret != true);

clear_buf:
	hal->txagc_set_buf = 0;

exit:
	return;
}

/*
 * Description:
 *	Check need to switch band or not
 * Parameters:
 *	channelToSW	channel wiii be switch to
 * Return:
 *	_TRUE		need to switch band
 *	_FALSE		not need to switch band
 */
static u8 need_switch_band(PADAPTER adapter, u8 channelToSW)
{
	u8 u1tmp = 0;
	u8 ret_value = _TRUE;
	u8 Band = BAND_ON_5G, BandToSW = BAND_ON_5G;
	PHAL_DATA_TYPE hal = GET_HAL_DATA(adapter);

	Band = hal->current_band_type;

	/* Use current swich channel to judge Band Type and switch Band if need */
	if (channelToSW > 14)
		BandToSW = BAND_ON_5G;
	else
		BandToSW = BAND_ON_24G;

	if (BandToSW != Band) {
		/* record current band type for other hal use */
		hal->current_band_type = (BAND_TYPE)BandToSW;
		ret_value = _TRUE;
	} else
		ret_value = _FALSE;

	return ret_value;
}

static u8 get_pri_ch_id(PADAPTER adapter)
{
	u8 pri_ch_idx = 0;
	PHAL_DATA_TYPE hal = GET_HAL_DATA(adapter);

	if (hal->current_channel_bw == CHANNEL_WIDTH_80) {
		/* primary channel is at lower subband of 80MHz & 40MHz */
		if ((hal->nCur40MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_LOWER) && (hal->nCur80MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_LOWER))
			pri_ch_idx = VHT_DATA_SC_20_LOWEST_OF_80MHZ;
		/* primary channel is at lower subband of 80MHz & upper subband of 40MHz */
		else if ((hal->nCur40MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_UPPER) && (hal->nCur80MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_LOWER))
			pri_ch_idx = VHT_DATA_SC_20_LOWER_OF_80MHZ;
		/* primary channel is at upper subband of 80MHz & lower subband of 40MHz */
		else if ((hal->nCur40MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_LOWER) && (hal->nCur80MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_UPPER))
			pri_ch_idx = VHT_DATA_SC_20_UPPER_OF_80MHZ;
		/* primary channel is at upper subband of 80MHz & upper subband of 40MHz */
		else if ((hal->nCur40MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_UPPER) && (hal->nCur80MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_UPPER))
			pri_ch_idx = VHT_DATA_SC_20_UPPERST_OF_80MHZ;
		else {
			if (hal->nCur80MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_LOWER)
				pri_ch_idx = VHT_DATA_SC_40_LOWER_OF_80MHZ;
			else if (hal->nCur80MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_UPPER)
				pri_ch_idx = VHT_DATA_SC_40_UPPER_OF_80MHZ;
			else
				RTW_INFO("SCMapping: DONOT CARE Mode Setting\n");
		}
	} else if (hal->current_channel_bw == CHANNEL_WIDTH_40) {
		/* primary channel is at upper subband of 40MHz */
		if (hal->nCur40MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_UPPER)
			pri_ch_idx = VHT_DATA_SC_20_UPPER_OF_80MHZ;
		/* primary channel is at lower subband of 40MHz */
		else if (hal->nCur40MhzPrimeSC == HAL_PRIME_CHNL_OFFSET_LOWER)
			pri_ch_idx = VHT_DATA_SC_20_LOWER_OF_80MHZ;
		else
			RTW_INFO("SCMapping: DONOT CARE Mode Setting\n");
	}

	return  pri_ch_idx;
}

static void mac_switch_bandwidth(PADAPTER adapter, u8 pri_ch_idx)
{
	u8 channel = 0, bw = 0;
	PHAL_DATA_TYPE hal = GET_HAL_DATA(adapter);
	int err;

	channel = hal->current_channel;
	bw = hal->current_channel_bw;
	err = rtw_halmac_set_bandwidth(adapter_to_dvobj(adapter), channel, pri_ch_idx, bw);
	if (err) {
		RTW_INFO(FUNC_ADPT_FMT ": (channel=%d, pri_ch_idx=%d, bw=%d) fail\n",
			 FUNC_ADPT_ARG(adapter), channel, pri_ch_idx, bw);
	}
}

static void switch_chnl_and_set_bw_by_drv(PADAPTER adapter, u8 switch_band)
{
	PHAL_DATA_TYPE hal = GET_HAL_DATA(adapter);
	struct dm_struct *p_dm_odm = &hal->odmpriv;
	u8 center_ch = 0, ret = 0;

	/* set channel & Bandwidth register */
	/* 1. set switch band register if need to switch band */
	if (switch_band) {
		/* hal->current_channel is center channel of pmlmeext->cur_channel(primary channel) */
		ret = config_phydm_switch_band_8822c(p_dm_odm, hal->current_channel);

		if (!ret) {
			RTW_INFO("%s: config_phydm_switch_band_8822c fail\n", __FUNCTION__);
			rtw_warn_on(1);
			return;
		}
	}

	/* 2. set channel register */
	if (hal->bSwChnl) {
		ret = config_phydm_switch_channel_8822c(p_dm_odm, hal->current_channel);
		hal->bSwChnl = _FALSE;

		if (!ret) {
			RTW_INFO("%s: config_phydm_switch_channel_8822c fail\n", __FUNCTION__);
			rtw_warn_on(1);
			return;
		}
	}

	/* 3. set Bandwidth register */
	if (hal->bSetChnlBW) {
		/* get primary channel index */
		u8 pri_ch_idx = get_pri_ch_id(adapter);

		/* 3.1 set MAC register */
		mac_switch_bandwidth(adapter, pri_ch_idx);

		/* 3.2 set BB/RF registet */
		ret = config_phydm_switch_bandwidth_8822c(p_dm_odm, pri_ch_idx, hal->current_channel_bw);
		hal->bSetChnlBW = _FALSE;

		if (!ret) {
			RTW_INFO("%s: config_phydm_switch_bandwidth_8822c fail\n", __FUNCTION__);
			rtw_warn_on(1);
			return;
		}
	}
}

#ifdef RTW_CHANNEL_SWITCH_OFFLOAD
static void switch_chnl_and_set_bw_by_fw(PADAPTER adapter, u8 switch_band)
{
	PHAL_DATA_TYPE hal = GET_HAL_DATA(adapter);

	if (switch_band ||hal->bSwChnl || hal->bSetChnlBW) {
		rtw_hal_switch_chnl_and_set_bw_offload(adapter,
			hal->current_channel, get_pri_ch_id(adapter), hal->current_channel_bw);

		hal->bSwChnl = _FALSE;
		hal->bSetChnlBW = _FALSE;
	}
}
#endif

/*
 * Description:
 *	Set channel & bandwidth & offset
 */
void rtl8822c_switch_chnl_and_set_bw(PADAPTER adapter)
{
	PHAL_DATA_TYPE hal = GET_HAL_DATA(adapter);
	struct dm_struct *p_dm_odm = &hal->odmpriv;
	u8 center_ch = 0, ret = 0, switch_band = _FALSE;

	if (adapter->bNotifyChannelChange) {
		RTW_INFO("[%s] bSwChnl=%d, ch=%d, bSetChnlBW=%d, bw=%d\n",
			 __FUNCTION__,
			 hal->bSwChnl,
			 hal->current_channel,
			 hal->bSetChnlBW,
			 hal->current_channel_bw);
	}

	if (RTW_CANNOT_RUN(adapter_to_dvobj(adapter))) {
		hal->bSwChnlAndSetBWInProgress = _FALSE;
		return;
	}

	switch_band = need_switch_band(adapter, hal->current_channel);

	/* config channel, bw, offset setting */
#ifdef RTW_CHANNEL_SWITCH_OFFLOAD
	if (hal->ch_switch_offload) {

	#ifdef RTW_REDUCE_SCAN_SWITCH_CH_TIME
		struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
		_adapter *iface;
		struct mlme_ext_priv *mlmeext;
		u8 drv_switch = _TRUE;
		int i;

		for (i = 0; i < dvobj->iface_nums; i++) {
			iface = dvobj->padapters[i];
			mlmeext = &iface->mlmeextpriv;

			/* check scan state */
			if (mlmeext_scan_state(mlmeext) != SCAN_DISABLE
				&& mlmeext_scan_state(mlmeext) != SCAN_COMPLETE
					&& mlmeext_scan_state(mlmeext) != SCAN_BACKING_OP)
				drv_switch = _FALSE;
		}
	#else
		u8 drv_switch = _FALSE;
	#endif

		if (drv_switch == _TRUE)
			switch_chnl_and_set_bw_by_drv(adapter, switch_band);
		else
		switch_chnl_and_set_bw_by_fw(adapter, switch_band);

	} else {
		switch_chnl_and_set_bw_by_drv(adapter, switch_band);
	}
#else
	switch_chnl_and_set_bw_by_drv(adapter, switch_band);
#endif /* RTW_CHANNEL_SWITCH_OFFLOAD */


	/* config coex setting */
	if (switch_band) {
#ifdef CONFIG_BT_COEXIST
		if (hal->EEPROMBluetoothCoexist) {
			struct mlme_ext_priv *mlmeext;

			/* switch band under site survey or not, must notify to BT COEX */
			mlmeext = &adapter->mlmeextpriv;
			if (mlmeext_scan_state(mlmeext) != SCAN_DISABLE)
				rtw_btcoex_switchband_notify(_TRUE, hal->current_band_type);
			else
				rtw_btcoex_switchband_notify(_FALSE, hal->current_band_type);
		} else
			rtw_btcoex_wifionly_switchband_notify(adapter);
#else /* !CONFIG_BT_COEXIST */
		rtw_btcoex_wifionly_switchband_notify(adapter);
#endif /* CONFIG_BT_COEXIST */
	}

	/* TX Power Setting */
	odm_clear_txpowertracking_state(p_dm_odm);
	rtw_hal_set_tx_power_level(adapter, hal->current_channel);

	/* IQK */
	if ((hal->bNeedIQK == _TRUE)
	    || (adapter->registrypriv.mp_mode == 1)) {
		rtw_phydm_iqk_trigger(adapter);
		hal->bNeedIQK = _FALSE;
	}
}

/*
 * Description:
 *	Store channel setting to hal date
 * Parameters:
 *	bSwitchChannel		swith channel or not
 *	bSetBandWidth		set band or not
 *	ChannelNum		center channel
 *	ChnlWidth		bandwidth
 *	ChnlOffsetOf40MHz	channel offset for 40MHz Bandwidth
 *	ChnlOffsetOf80MHz	channel offset for 80MHz Bandwidth
 *	CenterFrequencyIndex1	center channel index
 */

void rtl8822c_handle_sw_chnl_and_set_bw(
	PADAPTER Adapter, u8 bSwitchChannel, u8 bSetBandWidth,
	u8 ChannelNum, enum channel_width ChnlWidth, u8 ChnlOffsetOf40MHz,
	u8 ChnlOffsetOf80MHz, u8 CenterFrequencyIndex1)
{
	PADAPTER pDefAdapter = GetDefaultAdapter(Adapter);
	PHAL_DATA_TYPE hal = GET_HAL_DATA(pDefAdapter);
	u8 tmpChannel = hal->current_channel;
	enum channel_width tmpBW = hal->current_channel_bw;
	u8 tmpnCur40MhzPrimeSC = hal->nCur40MhzPrimeSC;
	u8 tmpnCur80MhzPrimeSC = hal->nCur80MhzPrimeSC;
	u8 tmpCenterFrequencyIndex1 = hal->CurrentCenterFrequencyIndex1;
	struct mlme_ext_priv *pmlmeext = &Adapter->mlmeextpriv;


	/* check swchnl or setbw */
	if (!bSwitchChannel && !bSetBandWidth) {
		RTW_INFO("%s: not switch channel and not set bandwidth\n", __FUNCTION__);
		return;
	}

	/* skip switch channel operation for current channel & ChannelNum(will be switch) are the same */
	if (bSwitchChannel) {
		if (hal->current_channel != ChannelNum) {
			if (HAL_IsLegalChannel(Adapter, ChannelNum))
				hal->bSwChnl = _TRUE;
			else
				return;
		}
	}

	/* check set BandWidth */
	if (bSetBandWidth) {
		/* initial channel bw setting */
		if (hal->bChnlBWInitialized == _FALSE) {
			hal->bChnlBWInitialized = _TRUE;
			hal->bSetChnlBW = _TRUE;
		} else if ((hal->current_channel_bw != ChnlWidth) || /* check whether need set band or not */
			   (hal->nCur40MhzPrimeSC != ChnlOffsetOf40MHz) ||
			   (hal->nCur80MhzPrimeSC != ChnlOffsetOf80MHz) ||
			(hal->CurrentCenterFrequencyIndex1 != CenterFrequencyIndex1))
			hal->bSetChnlBW = _TRUE;
	}

	/* return if not need set bandwidth nor channel after check*/
	if (!hal->bSetChnlBW && !hal->bSwChnl && hal->bNeedIQK != _TRUE)
		return;

	/* set channel number to hal data */
	if (hal->bSwChnl) {
		hal->current_channel = ChannelNum;
		hal->CurrentCenterFrequencyIndex1 = ChannelNum;
	}

	/* set bandwidth info to hal data */
	if (hal->bSetChnlBW) {
		hal->current_channel_bw = ChnlWidth;
		hal->nCur40MhzPrimeSC = ChnlOffsetOf40MHz;
		hal->nCur80MhzPrimeSC = ChnlOffsetOf80MHz;
		hal->CurrentCenterFrequencyIndex1 = CenterFrequencyIndex1;
	}

	/* switch channel & bandwidth */
	if (!RTW_CANNOT_RUN(adapter_to_dvobj(Adapter)))
		rtl8822c_switch_chnl_and_set_bw(Adapter);
	else {
		if (hal->bSwChnl) {
			hal->current_channel = tmpChannel;
			hal->CurrentCenterFrequencyIndex1 = tmpChannel;
		}

		if (hal->bSetChnlBW) {
			hal->current_channel_bw = tmpBW;
			hal->nCur40MhzPrimeSC = tmpnCur40MhzPrimeSC;
			hal->nCur80MhzPrimeSC = tmpnCur80MhzPrimeSC;
			hal->CurrentCenterFrequencyIndex1 = tmpCenterFrequencyIndex1;
		}
	}
}

/*
 * Description:
 *	Change channel, bandwidth & offset
 * Parameters:
 *	center_ch	center channel
 *	bw		bandwidth
 *	offset40	channel offset for 40MHz Bandwidth
 *	offset80	channel offset for 80MHz Bandwidth
 */
void rtl8822c_set_channel_bw(PADAPTER adapter, u8 center_ch, enum channel_width bw, u8 offset40, u8 offset80)
{
	rtl8822c_handle_sw_chnl_and_set_bw(adapter, _TRUE, _TRUE, center_ch, bw, offset40, offset80, center_ch);
}

void rtl8822c_notch_filter_switch(PADAPTER adapter, bool enable)
{
	if (enable)
		RTW_INFO("%s: Enable notch filter\n", __FUNCTION__);
	else
		RTW_INFO("%s: Disable notch filter\n", __FUNCTION__);
}


#ifdef CONFIG_BEAMFORMING
/* REG_TXBF_CTRL		(Offset 0x42C) */
#define BITS_R_TXBF1_AID_8822C			(BIT_MASK_R_TXBF1_AID_8822C << BIT_SHIFT_R_TXBF1_AID_8822C)
#define BIT_CLEAR_R_TXBF1_AID_8822C(x)		((x) & (~BITS_R_TXBF1_AID_8822C))
#define BIT_SET_R_TXBF1_AID_8822C(x, v)		(BIT_CLEAR_R_TXBF1_AID_8822C(x) | BIT_R_TXBF1_AID_8822C(v))

#define BITS_R_TXBF0_AID_8822C			(BIT_MASK_R_TXBF0_AID_8822C << BIT_SHIFT_R_TXBF0_AID_8822C)
#define BIT_CLEAR_R_TXBF0_AID_8822C(x)		((x) & (~BITS_R_TXBF0_AID_8822C))
#define BIT_SET_R_TXBF0_AID_8822C(x, v)		(BIT_CLEAR_R_TXBF0_AID_8822C(x) | BIT_R_TXBF0_AID_8822C(v))

/* REG_NDPA_OPT_CTRL		(Offset 0x45F) */
#define BITS_R_NDPA_BW_8822C			(BIT_MASK_R_NDPA_BW_8822C << BIT_SHIFT_R_NDPA_BW_8822C)
#define BIT_CLEAR_R_NDPA_BW_8822C(x)		((x) & (~BITS_R_NDPA_BW_8822C))
#define BIT_SET_R_NDPA_BW_8822C(x, v)		(BIT_CLEAR_R_NDPA_BW_8822C(x) | BIT_R_NDPA_BW_8822C(v))

/* REG_ASSOCIATED_BFMEE_SEL	(Offset 0x714) */
#define BITS_AID1_8822C				(BIT_MASK_AID1_8822C << BIT_SHIFT_AID1_8822C)
#define BIT_CLEAR_AID1_8822C(x)			((x) & (~BITS_AID1_8822C))
#define BIT_SET_AID1_8822C(x, v)		(BIT_CLEAR_AID1_8822C(x) | BIT_AID1_8822C(v))

#define BITS_AID0_8822C				(BIT_MASK_AID0_8822C << BIT_SHIFT_AID0_8822C)
#define BIT_CLEAR_AID0_8822C(x)			((x) & (~BITS_AID0_8822C))
#define BIT_SET_AID0_8822C(x, v)		(BIT_CLEAR_AID0_8822C(x) | BIT_AID0_8822C(v))

/* REG_SND_PTCL_CTRL		(Offset 0x718) */
#define BIT_VHTNDP_RPTPOLL_CSI_STR_OFFSET_SEL_8822C	BIT(15)

/* REG_MU_TX_CTL		(Offset 0x14C0) */
#define BIT_R_MU_P1_WAIT_STATE_EN_8822C		BIT(16)

#define BIT_SHIFT_R_MU_RL_8822C			12
/* #define BIT_MASK_R_MU_RL_8822C			0xF */
#define BITS_R_MU_RL_8822C			(BIT_MASK_R_MU_RL_8822C << BIT_SHIFT_R_MU_RL_8822C)
#define BIT_R_MU_RL_8822C(x)			(((x) & BIT_MASK_R_MU_RL_8822C) << BIT_SHIFT_R_MU_RL_8822C)
#define BIT_CLEAR_R_MU_RL_8822C(x)		((x) & (~BITS_R_MU_RL_8822C))
#define BIT_SET_R_MU_RL_8822C(x, v)		(BIT_CLEAR_R_MU_RL_8822C(x) | BIT_R_MU_RL_8822C(v))

#define BIT_SHIFT_R_MU_TAB_SEL_8822C		8
#define BIT_MASK_R_MU_TAB_SEL_8822C		0x7
#define BITS_R_MU_TAB_SEL_8822C			(BIT_MASK_R_MU_TAB_SEL_8822C << BIT_SHIFT_R_MU_TAB_SEL_8822C)
#define BIT_R_MU_TAB_SEL_8822C(x)		(((x) & BIT_MASK_R_MU_TAB_SEL_8822C) << BIT_SHIFT_R_MU_TAB_SEL_8822C)
#define BIT_CLEAR_R_MU_TAB_SEL_8822C(x)		((x) & (~BITS_R_MU_TAB_SEL_8822C))
#define BIT_SET_R_MU_TAB_SEL_8822C(x, v)	(BIT_CLEAR_R_MU_TAB_SEL_8822C(x) | BIT_R_MU_TAB_SEL_8822C(v))

#define BIT_R_EN_MU_MIMO_8822C			BIT(7)

#define BITS_R_MU_TABLE_VALID_8822C		(BIT_MASK_R_MU_TABLE_VALID_8822C << BIT_SHIFT_R_MU_TABLE_VALID_8822C)
#define BIT_CLEAR_R_MU_TABLE_VALID_8822C(x)	((x) & (~BITS_R_MU_TABLE_VALID_8822C))
#define BIT_SET_R_MU_TABLE_VALID_8822C(x, v)	(BIT_CLEAR_R_MU_TABLE_VALID_8822C(x) | BIT_R_MU_TABLE_VALID_8822C(v))

/* REG_WMAC_MU_BF_CTL		(Offset 0x1680) */
#define BITS_WMAC_MU_BFRPTSEG_SEL_8822C			(BIT_MASK_WMAC_MU_BFRPTSEG_SEL_8822C << BIT_SHIFT_WMAC_MU_BFRPTSEG_SEL_8822C)
#define BIT_CLEAR_WMAC_MU_BFRPTSEG_SEL_8822C(x)		((x) & (~BITS_WMAC_MU_BFRPTSEG_SEL_8822C))
#define BIT_SET_WMAC_MU_BFRPTSEG_SEL_8822C(x, v)	(BIT_CLEAR_WMAC_MU_BFRPTSEG_SEL_8822C(x) | BIT_WMAC_MU_BFRPTSEG_SEL_8822C(v))

#define BITS_WMAC_MU_BF_MYAID_8822C		(BIT_MASK_WMAC_MU_BF_MYAID_8822C << BIT_SHIFT_WMAC_MU_BF_MYAID_8822C)
#define BIT_CLEAR_WMAC_MU_BF_MYAID_8822C(x)	((x) & (~BITS_WMAC_MU_BF_MYAID_8822C))
#define BIT_SET_WMAC_MU_BF_MYAID_8822C(x, v)	(BIT_CLEAR_WMAC_MU_BF_MYAID_8822C(x) | BIT_WMAC_MU_BF_MYAID_8822C(v))

/* REG_WMAC_ASSOCIATED_MU_BFMEE7	(Offset 0x168E) */
#define BIT_STATUS_BFEE7_8822C			BIT(10)

enum _HW_CFG_SOUNDING_TYPE {
	HW_CFG_SOUNDING_TYPE_SOUNDDOWN,
	HW_CFG_SOUNDING_TYPE_LEAVE,
	HW_CFG_SOUNDING_TYPE_RESET,
	HW_CFG_SOUNDING_TYPE_MAX
};

static u8 _bf_get_nrx(PADAPTER adapter)
{
	u8 nrx = 0;

	nrx = GET_HAL_RX_NSS(adapter);
	return (nrx - 1);
}

static void _sounding_reset_all(PADAPTER adapter)
{
	struct beamforming_info *info;
	struct beamformee_entry *bfee;
	u8 i;
	u32 mu_tx_ctl;


	info = GET_BEAMFORM_INFO(adapter);

	rtw_write8(adapter, REG_TXBF_CTRL_8822C+3, 0);

	/* Clear all MU entry table */
	for (i = 0; i < MAX_BEAMFORMEE_ENTRY_NUM; i++) {
		bfee = &info->bfee_entry[i];
		for (i = 0; i < 8; i++)
			bfee->gid_valid[i] = 0;
	}

	mu_tx_ctl = rtw_read32(adapter, REG_MU_TX_CTL_8822C);
	for (i = 0; i < 6; i++) {
		mu_tx_ctl = BIT_SET_R_MU_TAB_SEL_8822C(mu_tx_ctl, i);
		rtw_write32(adapter, REG_MU_TX_CTL_8822C, mu_tx_ctl);
		/* set MU STA gid valid table */
		rtw_write32(adapter, REG_MU_STA_GID_VLD_8822C, 0);
	}

	/* Disable TxMU PPDU */
	mu_tx_ctl &= ~BIT_R_EN_MU_MIMO_8822C;
	rtw_write32(adapter, REG_MU_TX_CTL_8822C, mu_tx_ctl);
}

static void _sounding_config_su(PADAPTER adapter, struct beamformee_entry *bfee, enum _HW_CFG_SOUNDING_TYPE cfg_type)
{
	u32 txbf_ctrl, new_ctrl;


	txbf_ctrl = rtw_read32(adapter, REG_TXBF_CTRL_8822C);
	new_ctrl = txbf_ctrl;

	/* Clear TxBF status at 20M/40/80M first */
	switch (bfee->su_reg_index) {
	case 0:
		new_ctrl &= ~(BIT_R_TXBF0_20M_8822C|BIT_R_TXBF0_40M_8822C|BIT_R_TXBF0_80M_8822C);
		break;
	case 1:
		new_ctrl &= ~(BIT_R_TXBF1_20M_8822C|BIT_R_TXBF1_40M_8822C|BIT_R_TXBF1_80M_8822C);
		break;
	}

	switch (cfg_type) {
	case HW_CFG_SOUNDING_TYPE_SOUNDDOWN:
		switch (bfee->sound_bw) {
		default:
		case CHANNEL_WIDTH_80:
			if (0 == bfee->su_reg_index)
				new_ctrl |= BIT_R_TXBF0_80M_8822C;
			else if (1 == bfee->su_reg_index)
				new_ctrl |= BIT_R_TXBF1_80M_8822C;
			/* fall through */
		case CHANNEL_WIDTH_40:
			if (0 == bfee->su_reg_index)
				new_ctrl |= BIT_R_TXBF0_40M_8822C;
			else if (1 == bfee->su_reg_index)
				new_ctrl |= BIT_R_TXBF1_40M_8822C;
			/* fall through */
		case CHANNEL_WIDTH_20:
			if (0 == bfee->su_reg_index)
				new_ctrl |= BIT_R_TXBF0_20M_8822C;
			else if (1 == bfee->su_reg_index)
				new_ctrl |= BIT_R_TXBF1_20M_8822C;
			break;
		}
		break;

	default:
		RTW_INFO("%s: SU cfg_type=%d, don't apply Vmatrix!\n", __FUNCTION__, cfg_type);
		break;
	}

	if (new_ctrl != txbf_ctrl)
		rtw_write32(adapter, REG_TXBF_CTRL_8822C, new_ctrl);
}

static void _sounding_config_mu(PADAPTER adapter, struct beamformee_entry *bfee, enum _HW_CFG_SOUNDING_TYPE cfg_type)
{
	struct beamforming_info *info;
	u8 is_bitmap_ready = _FALSE;
	u32 mu_tx_ctl;
	u16 bitmap;
	u8 id1, id0, gid;
	u32 gid_valid[6] = {0};
	u8 i, j;
	u32 val32;


	info = GET_BEAMFORM_INFO(adapter);

	switch (cfg_type) {
	case HW_CFG_SOUNDING_TYPE_LEAVE:
		RTW_INFO("%s: MU HW_CFG_SOUNDING_TYPE_LEAVE\n", __FUNCTION__);

		/* Clear the entry table */
		mu_tx_ctl = rtw_read32(adapter, REG_MU_TX_CTL_8822C);
		if (TEST_FLAG(bfee->cap, BEAMFORMEE_CAP_VHT_MU)) {
			for (i = 0; i < 8; i++)
				bfee->gid_valid[i] = 0;

			mu_tx_ctl = BIT_SET_R_MU_TAB_SEL_8822C(mu_tx_ctl, bfee->mu_reg_index);
			rtw_write32(adapter, REG_MU_TX_CTL_8822C, mu_tx_ctl);
			/* Set MU STA gid valid table */
			rtw_write32(adapter, REG_MU_STA_GID_VLD_8822C, 0);
		} else {
			RTW_ERR("%s: ERROR! It is not an MU BFee entry!!\n",  __FUNCTION__);
		}

		if (info->beamformee_su_cnt == 0) {
			/* Disable TxMU PPDU */
			mu_tx_ctl &= ~BIT_R_EN_MU_MIMO_8822C;
			rtw_write32(adapter, REG_MU_TX_CTL_8822C, mu_tx_ctl);
		}

		break;

	case HW_CFG_SOUNDING_TYPE_SOUNDDOWN:
		RTW_INFO("%s: MU HW_CFG_SOUNDING_TYPE_SOUNDDOWN\n",  __FUNCTION__);

		/* Update all MU entry table */
		i = 0;
		do {
			/* Check BB GID bitmap ready */
			val32 = phy_query_bb_reg(adapter, 0xF4C, 0xFFFF0000);

			is_bitmap_ready = (val32 & BIT(15)) ? _TRUE : _FALSE;
			i++;
			rtw_udelay_os(5);
		} while ((_FALSE == is_bitmap_ready) && (i < 100));

		bitmap = (u16)(val32 & 0x3FFF);

		for (i = 0; i < 15; i++) {
			if (i < 5) {
				/* bit0~4 */
				id0 = 0;
				id1 = i + 1;
			} else if (i < 9) {
				/* bit5~8 */
				id0 = 1;
				id1 = i - 3;
			} else if (i < 12) {
				/* bit9~11 */
				id0 = 2;
				id1 = i - 6;
			} else if (i < 14) {
				/* bit12~13 */
				id0 = 3;
				id1 = i - 8;
			} else {
				/* bit14 */
				id0 = 4;
				id1 = i - 9;
			}
			if (bitmap & BIT(i)) {
				/* Pair 1 */
				gid = (i << 1) + 1;
				gid_valid[id0] |= (BIT(gid));
				gid_valid[id1] |= (BIT(gid));
				/* Pair 2 */
				gid += 1;
				gid_valid[id0] |= (BIT(gid));
				gid_valid[id1] |= (BIT(gid));
			} else {
				/* Pair 1 */
				gid = (i << 1) + 1;
				gid_valid[id0] &= ~(BIT(gid));
				gid_valid[id1] &= ~(BIT(gid));
				/* Pair 2 */
				gid += 1;
				gid_valid[id0] &= ~(BIT(gid));
				gid_valid[id1] &= ~(BIT(gid));
			}
		}

		for (i = 0; i < MAX_BEAMFORMEE_ENTRY_NUM; i++) {
			bfee = &info->bfee_entry[i];
			if (_FALSE == bfee->used)
				continue;
			if (TEST_FLAG(bfee->cap, BEAMFORMEE_CAP_VHT_MU)
			    && (bfee->mu_reg_index < 6)) {
				val32 = gid_valid[bfee->mu_reg_index];
				for (j = 0; j < 4; j++) {
					bfee->gid_valid[j] = (u8)(val32 & 0xFF);
					val32 >>= 8;
				}
			}
		}

		mu_tx_ctl = rtw_read32(adapter, REG_MU_TX_CTL_8822C);
		for (i = 0; i < 6; i++) {
			mu_tx_ctl = BIT_SET_R_MU_TAB_SEL_8822C(mu_tx_ctl, i);
			rtw_write32(adapter, REG_MU_TX_CTL_8822C, mu_tx_ctl);
			/* Set MU STA gid valid table */
			rtw_write32(adapter, REG_MU_STA_GID_VLD_8822C, gid_valid[i]);
		}

		/* Enable TxMU PPDU */
		mu_tx_ctl |= BIT_R_EN_MU_MIMO_8822C;
		rtw_write32(adapter, REG_MU_TX_CTL_8822C, mu_tx_ctl);

		break;

	default:
		break;
	}
}

static void _config_sounding(PADAPTER adapter, struct beamformee_entry *bfee, u8 mu_sounding, enum _HW_CFG_SOUNDING_TYPE cfg_type)
{
	if (cfg_type == HW_CFG_SOUNDING_TYPE_RESET) {
		RTW_INFO("%s: HW_CFG_SOUNDING_TYPE_RESET\n", __FUNCTION__);
		_sounding_reset_all(adapter);
		return;
	}

	if (_FALSE == mu_sounding)
		_sounding_config_su(adapter, bfee, cfg_type);
	else
		_sounding_config_mu(adapter, bfee, cfg_type);
}

static void _config_beamformer_su(PADAPTER adapter, struct beamformer_entry *bfer)
{
	/* Beamforming */
	u8 nc_index = 0, nr_index = 0;
	u8 grouping = 0, codebookinfo = 0, coefficientsize = 0;
	u32 addr_bfer_info, addr_csi_rpt;
	u32 csi_param;
	/* Misc */
	u8 i;


	RTW_INFO("%s: Config SU BFer entry HW setting\n", __FUNCTION__);

	if (bfer->su_reg_index == 0) {
		addr_bfer_info = REG_ASSOCIATED_BFMER0_INFO_8822C;
		addr_csi_rpt = REG_TX_CSI_RPT_PARAM_BW20_8822C;
	} else {
		addr_bfer_info = REG_ASSOCIATED_BFMER1_INFO_8822C;
		addr_csi_rpt = REG_TX_CSI_RPT_PARAM_BW20_8822C + 2;
	}

	/* Sounding protocol control */
	rtw_write8(adapter, REG_SND_PTCL_CTRL_8822C, 0xDB);

	/* MAC address/Partial AID of Beamformer */
	for (i = 0; i < ETH_ALEN; i++)
		rtw_write8(adapter, addr_bfer_info+i, bfer->mac_addr[i]);

	/* CSI report parameters of Beamformer */
	nc_index = _bf_get_nrx(adapter);
	/*
	 * 0x718[7] = 1 use Nsts
	 * 0x718[7] = 0 use reg setting
	 * As Bfee, we use Nsts, so nr_index don't care
	 */
	nr_index = bfer->NumofSoundingDim;
	grouping = 0;
	/* for ac = 1, for n = 3 */
	if (TEST_FLAG(bfer->cap, BEAMFORMER_CAP_VHT_SU))
		codebookinfo = 1;
	else if (TEST_FLAG(bfer->cap, BEAMFORMER_CAP_HT_EXPLICIT))
		codebookinfo = 3;
	coefficientsize = 3;
	csi_param = (u16)((coefficientsize<<10)|(codebookinfo<<8)|(grouping<<6)|(nr_index<<3)|(nc_index));
	rtw_write16(adapter, addr_csi_rpt, csi_param);
	RTW_INFO("%s: nc=%d nr=%d group=%d codebookinfo=%d coefficientsize=%d\n",
		 __FUNCTION__, nc_index, nr_index, grouping, codebookinfo, coefficientsize);
	RTW_INFO("%s: csi=0x%04x\n", __FUNCTION__, csi_param);

	/* ndp_rx_standby_timer */
	rtw_write8(adapter, REG_SND_PTCL_CTRL_8822C+3, 0x70);

	/* partial merge from halmac api cfg_sounding_88xx(), may need to refine to apply the api immediately */
	{
		u32 tmp6dc = 0;
		u8 csi_rsc = 0x0;

		tmp6dc = (rtw_read32(adapter, REG_BBPSF_CTRL_8822C) | BIT(30) | (csi_rsc << 13));
		if (check_fwstate(&adapter->mlmepriv, WIFI_AP_STATE) == _TRUE)
			tmp6dc |= BIT(12);
		else
			tmp6dc &= ~BIT(12);
		rtw_write32(adapter, REG_BBPSF_CTRL_8822C, tmp6dc);

		rtw_write32(adapter, REG_CSI_RRSR_8822C, 0x550);
	}
}

static void _config_beamformer_mu(PADAPTER adapter, struct beamformer_entry *bfer)
{
	/* General */
	PHAL_DATA_TYPE hal;
	/* Beamforming */
	struct beamforming_info *bf_info;
	u8 nc_index = 0, nr_index = 0;
	u8 grouping = 0, codebookinfo = 0, coefficientsize = 0;
	u32 csi_param;
	/* Misc */
	u8 i, val8;
	u16 val16;

	RTW_INFO("%s: Config MU BFer entry HW setting\n", __FUNCTION__);

	hal = GET_HAL_DATA(adapter);
	bf_info = GET_BEAMFORM_INFO(adapter);

	/* Reset GID table */
	for (i = 0; i < 8; i++)
		bfer->gid_valid[i] = 0;
	for (i = 0; i < 16; i++)
		bfer->user_position[i] = 0;

	/* CSI report parameters of Beamformer */
	nc_index = _bf_get_nrx(adapter);
	nr_index = 1; /* 0x718[7] = 1 use Nsts, 0x718[7] = 0 use reg setting. as Bfee, we use Nsts, so Nr_index don't care */
	grouping = 0; /* no grouping */
	codebookinfo = 1; /* 7 bit for psi, 9 bit for phi */
	coefficientsize = 0; /* This is nothing really matter */
	csi_param = (u16)((coefficientsize<<10)|(codebookinfo<<8)|
			(grouping<<6)|(nr_index<<3)|(nc_index));

	RTW_INFO("%s: nc=%d nr=%d group=%d codebookinfo=%d coefficientsize=%d\n",
		__func__, nc_index, nr_index, grouping, codebookinfo,
		coefficientsize);
	RTW_INFO("%s: csi=0x%04x\n", __func__, csi_param);

	rtw_halmac_bf_add_mu_bfer(adapter_to_dvobj(adapter), bfer->p_aid,
			csi_param, bfer->aid & 0xfff, HAL_CSI_SEG_4K,
			bfer->mac_addr);

	bf_info->cur_csi_rpt_rate = HALMAC_OFDM6;
	rtw_halmac_bf_cfg_sounding(adapter_to_dvobj(adapter), HAL_BFEE,
			bf_info->cur_csi_rpt_rate);

	/* Set 0x6A0[14] = 1 to accept action_no_ack */
	val8 = rtw_read8(adapter, REG_RXFLTMAP0_8822C+1);
	val8 |= (BIT_MGTFLT14EN_8822C >> 8);
	rtw_write8(adapter, REG_RXFLTMAP0_8822C+1, val8);

	/* Set 0x6A2[5:4] = 1 to NDPA and BF report poll */
	val8 = rtw_read8(adapter, REG_RXFLTMAP1_8822C);
	val8 |= BIT_CTRLFLT4EN_8822C | BIT_CTRLFLT5EN_8822C;
	rtw_write8(adapter, REG_RXFLTMAP1_8822C, val8);

}

static void _config_beamformee_su(PADAPTER adapter, struct beamformee_entry *bfee)
{
	/* General */
	struct mlme_priv *mlme;
	/* Beamforming */
	struct beamforming_info *info;
	u8 idx;
	u16 p_aid = 0;
	/* Misc */
	u8 val8;
	u16 val16;
	u32 val32;


	RTW_INFO("%s: Config SU BFee entry HW setting\n", __FUNCTION__);

	mlme = &adapter->mlmepriv;
	info = GET_BEAMFORM_INFO(adapter);
	idx = bfee->su_reg_index;

	if ((check_fwstate(mlme, WIFI_ADHOC_STATE) == _TRUE)
	    || (check_fwstate(mlme, WIFI_ADHOC_MASTER_STATE) == _TRUE))
		p_aid = bfee->mac_id;
	else
		p_aid = bfee->p_aid;

	phydm_txbf_rfmode(GET_PDM_ODM(adapter), info->beamformee_su_cnt, info->beamformee_mu_cnt);

	/* P_AID of Beamformee & enable NDPA transmission & enable NDPA interrupt */
	val32 = rtw_read32(adapter, REG_TXBF_CTRL_8822C);
	if (idx == 0) {
		val32 = BIT_SET_R_TXBF0_AID_8822C(val32, p_aid);
		val32 &= ~(BIT_R_TXBF0_20M_8822C | BIT_R_TXBF0_40M_8822C | BIT_R_TXBF0_80M_8822C);
	} else {
		val32 = BIT_SET_R_TXBF1_AID_8822C(val32, p_aid);
		val32 &= ~(BIT_R_TXBF1_20M_8822C | BIT_R_TXBF1_40M_8822C | BIT_R_TXBF1_80M_8822C);
	}
	val32 |= BIT_R_EN_NDPA_INT_8822C | BIT_USE_NDPA_PARAMETER_8822C | BIT_R_ENABLE_NDPA_8822C;
	rtw_write32(adapter, REG_TXBF_CTRL_8822C, val32);

	/* CSI report parameters of Beamformee */
	val32 = rtw_read32(adapter, REG_ASSOCIATED_BFMEE_SEL_8822C);
	if (idx == 0) {
		val32 = BIT_SET_AID0_8822C(val32, p_aid);
		val32 |= BIT_TXUSER_ID0_8822C;

		/* unknown? */
		val32 &= 0x03FFFFFF;
		val32 |= 0x60000000;
	} else {
		val32 = BIT_SET_AID1_8822C(val32, p_aid);
		val32 |= BIT_TXUSER_ID1_8822C;

		/* unknown? */
		val32 &= 0x03FFFFFF;
		val32 |= 0xE0000000;
	}
	rtw_write32(adapter, REG_ASSOCIATED_BFMEE_SEL_8822C, val32);
}

static void _config_beamformee_mu(PADAPTER adapter, struct beamformee_entry *bfee)
{
	/* General */
	PHAL_DATA_TYPE hal;
	/* Beamforming */
	struct beamforming_info *info;
	u8 idx;
	u32 gid_valid = 0, user_position_l = 0, user_position_h = 0;
	u32 mu_reg[6] = {REG_WMAC_ASSOCIATED_MU_BFMEE2_8822C,
			 REG_WMAC_ASSOCIATED_MU_BFMEE3_8822C,
			 REG_WMAC_ASSOCIATED_MU_BFMEE4_8822C,
			 REG_WMAC_ASSOCIATED_MU_BFMEE5_8822C,
			 REG_WMAC_ASSOCIATED_MU_BFMEE6_8822C,
			 REG_WMAC_ASSOCIATED_MU_BFMEE7_8822C};
	/* Misc */
	u8 i, val8;
	u16 val16;
	u32 val32;


	RTW_INFO("%s: Config MU BFee entry HW setting\n", __FUNCTION__);

	hal = GET_HAL_DATA(adapter);
	info = GET_BEAMFORM_INFO(adapter);
	idx = bfee->mu_reg_index;

	/* User position table */
	switch (idx) {
	case 0:
		gid_valid = 0x7fe;
		user_position_l = 0x111110;
		user_position_h = 0x0;
		break;
	case 1:
		gid_valid = 0x7f806;
		user_position_l = 0x11000004;
		user_position_h = 0x11;
		break;
	case 2:
		gid_valid = 0x1f81818;
		user_position_l = 0x400040;
		user_position_h = 0x11100;
		break;
	case 3:
		gid_valid = 0x1e186060;
		user_position_l = 0x4000400;
		user_position_h = 0x1100040;
		break;
	case 4:
		gid_valid = 0x66618180;
		user_position_l = 0x40004000;
		user_position_h = 0x10040400;
		break;
	case 5:
		gid_valid = 0x79860600;
		user_position_l = 0x40000;
		user_position_h = 0x4404004;
		break;
	}

	for (i = 0; i < 8; i++) {
		if (i < 4) {
			bfee->gid_valid[i] = (u8)(gid_valid & 0xFF);
			gid_valid >>= 8;
		} else {
			bfee->gid_valid[i] = 0;
		}
	}
	for (i = 0; i < 16; i++) {
		if (i < 4)
			bfee->user_position[i] = (u8)((user_position_l >> (i*8)) & 0xFF);
		else if (i < 8)
			bfee->user_position[i] = (u8)((user_position_h >> ((i-4)*8)) & 0xFF);
		else
			bfee->user_position[i] = 0;
	}

	/* Sounding protocol control */
	rtw_write8(adapter, REG_SND_PTCL_CTRL_8822C, 0xDB);

	/* select MU STA table */
	val32 = rtw_read32(adapter, REG_MU_TX_CTL_8822C);
	val32 = BIT_SET_R_MU_TAB_SEL_8822C(val32, idx);
	rtw_write32(adapter, REG_MU_TX_CTL_8822C, val32);

	/* Reset gid_valid table */
	rtw_write32(adapter, REG_MU_STA_GID_VLD_8822C, 0);
	rtw_write32(adapter, REG_MU_STA_USER_POS_INFO_8822C , user_position_l);
	rtw_write32(adapter, REG_MU_STA_USER_POS_INFO_8822C+4 , user_position_h);

	/* set validity of MU STAs */
	val32 = BIT_SET_R_MU_TABLE_VALID_8822C(val32, info->beamformee_mu_reg_maping);
	rtw_write32(adapter, REG_MU_TX_CTL_8822C, val32);

	RTW_INFO("%s: RegMUTxCtrl=0x%x, user_position_l=0x%x, user_position_h=0x%x\n",
		 __FUNCTION__, val32, user_position_l, user_position_h);

	val16 = rtw_read16(adapter, mu_reg[idx]);
	val16 &= 0xFE00; /* Clear PAID */
	val16 |= BIT(9); /* Enable MU BFee */
	val16 |= bfee->p_aid;
	rtw_write16(adapter, mu_reg[idx], val16);
	RTW_INFO("%s: Write mu_reg 0x%x = 0x%x\n",
		 __FUNCTION__, mu_reg[idx], val16);

	/* 0x42C[30] = 1 (0: from Tx desc, 1: from 0x45F) */
	val8 = rtw_read8(adapter, REG_TXBF_CTRL_8822C+3);
	val8 |= 0xD0; /* Set bit 28, 30, 31 to 3b'111 */
	rtw_write8(adapter, REG_TXBF_CTRL_8822C+3, val8);

	/* Set NDPA rate*/
	val8 = phydm_get_ndpa_rate(GET_PDM_ODM(adapter));
	rtw_write8(adapter, REG_NDPA_RATE_8822C, val8);

	val8 = rtw_read8(adapter, REG_NDPA_OPT_CTRL_8822C);
	val8 = BIT_SET_R_NDPA_BW_8822C(val8, 0); /* Clear bit 0, 1 */
	rtw_write8(adapter, REG_NDPA_OPT_CTRL_8822C, val8);

	val32 = rtw_read32(adapter, REG_SND_PTCL_CTRL_8822C);
	val32 = (val32 & 0xFF0000FF) | 0x020200; /* Set [23:8] to 0x0202 */
	rtw_write32(adapter, REG_SND_PTCL_CTRL_8822C, val32);

	/* Set 0x6A0[14] = 1 to accept action_no_ack */
	val8 = rtw_read8(adapter, REG_RXFLTMAP0_8822C+1);
	val8 |= (BIT_MGTFLT14EN_8822C >> 8);
	rtw_write8(adapter, REG_RXFLTMAP0_8822C+1, val8);

	/* 0x718[15] = 1. Patch for STA2 CSI report start offset error issue */
	val8 = rtw_read8(adapter, REG_SND_PTCL_CTRL_8822C+1);
	val8 |= (BIT_VHTNDP_RPTPOLL_CSI_STR_OFFSET_SEL_8822C >> 8);
	rtw_write8(adapter, REG_SND_PTCL_CTRL_8822C+1, val8);

	/* End of MAC registers setting */

	phydm_txbf_rfmode(GET_PDM_ODM(adapter), info->beamformee_su_cnt, info->beamformee_mu_cnt);

	/* <tynli_mark> <TODO> Need to set timer 2015.12.23 */
	/* Special for plugfest */
	rtw_mdelay_os(50); /* wait for 4-way handshake ending */
	rtw_bf_send_vht_gid_mgnt_packet(adapter, bfee->mac_addr, bfee->gid_valid, bfee->user_position);
}

static void _reset_beamformer_su(PADAPTER adapter, struct beamformer_entry *bfer)
{
	/* Beamforming */
	struct beamforming_info *info;
	u8 idx;


	info = GET_BEAMFORM_INFO(adapter);
	/* SU BFer */
	idx = bfer->su_reg_index;

	if (idx == 0) {
		rtw_write32(adapter, REG_ASSOCIATED_BFMER0_INFO_8822C, 0);
		rtw_write16(adapter, REG_ASSOCIATED_BFMER0_INFO_8822C+4, 0);
		rtw_write16(adapter, REG_TX_CSI_RPT_PARAM_BW20_8822C, 0);
	} else {
		rtw_write32(adapter, REG_ASSOCIATED_BFMER1_INFO_8822C, 0);
		rtw_write16(adapter, REG_ASSOCIATED_BFMER1_INFO_8822C+4, 0);
		rtw_write16(adapter, REG_TX_CSI_RPT_PARAM_BW20_8822C+2, 0);
	}

	info->beamformer_su_reg_maping &= ~BIT(idx);
	bfer->su_reg_index = 0xFF;

	RTW_INFO("%s: Clear SU BFer entry(%d) HW setting\n", __FUNCTION__, idx);
}

static void _reset_beamformer_mu(PADAPTER adapter, struct beamformer_entry *bfer)
{
	struct beamforming_info *bf_info;

	bf_info = GET_BEAMFORM_INFO(adapter);

	rtw_halmac_bf_del_mu_bfer(adapter_to_dvobj(adapter));

	if (bf_info->beamformer_su_cnt == 0 &&
			bf_info->beamformer_mu_cnt == 0)
		rtw_halmac_bf_del_sounding(adapter_to_dvobj(adapter), HAL_BFEE);

	RTW_INFO("%s: Clear MU BFer entry HW setting\n", __FUNCTION__);
}

static void _reset_beamformee_su(PADAPTER adapter, struct beamformee_entry *bfee)
{
	/* Beamforming */
	struct beamforming_info *info;
	u8 idx;
	/* Misc */
	u32 txbf_ctrl, bfmee_sel;


	info = GET_BEAMFORM_INFO(adapter);
	/* SU BFee */
	idx = bfee->su_reg_index;

	/* Force disable sounding config */
	_config_sounding(adapter, bfee, _FALSE, HW_CFG_SOUNDING_TYPE_LEAVE);

	/* clear P_AID */
	txbf_ctrl = rtw_read32(adapter, REG_TXBF_CTRL_8822C);
	bfmee_sel = rtw_read32(adapter, REG_ASSOCIATED_BFMEE_SEL_8822C);
	if (idx == 0) {
		txbf_ctrl = BIT_SET_R_TXBF0_AID_8822C(txbf_ctrl, 0);
		txbf_ctrl &= ~(BIT_R_TXBF0_20M_8822C | BIT_R_TXBF0_40M_8822C | BIT_R_TXBF0_80M_8822C);

		bfmee_sel = BIT_SET_AID0_8822C(bfmee_sel, 0);
		bfmee_sel &= ~BIT_TXUSER_ID0_8822C;
	} else {
		txbf_ctrl = BIT_SET_R_TXBF1_AID_8822C(txbf_ctrl, 0);
		txbf_ctrl &= ~(BIT_R_TXBF1_20M_8822C | BIT_R_TXBF1_40M_8822C | BIT_R_TXBF1_80M_8822C);

		bfmee_sel = BIT_SET_AID1_8822C(bfmee_sel, 0);
		bfmee_sel &= ~BIT_TXUSER_ID1_8822C;
	}
	txbf_ctrl |= BIT_R_EN_NDPA_INT_8822C | BIT_USE_NDPA_PARAMETER_8822C | BIT_R_ENABLE_NDPA_8822C;
	rtw_write32(adapter, REG_TXBF_CTRL_8822C, txbf_ctrl);
	rtw_write32(adapter, REG_ASSOCIATED_BFMEE_SEL_8822C, bfmee_sel);

	info->beamformee_su_reg_maping &= ~BIT(idx);
	bfee->su_reg_index = 0xFF;

	RTW_INFO("%s: Clear SU BFee entry(%d) HW setting\n", __FUNCTION__, idx);
}

static void _reset_beamformee_mu(PADAPTER adapter, struct beamformee_entry *bfee)
{
	/* Beamforming */
	struct beamforming_info *info;
	u8 idx;
	u32 mu_reg[6] = {REG_WMAC_ASSOCIATED_MU_BFMEE2_8822C,
			 REG_WMAC_ASSOCIATED_MU_BFMEE3_8822C,
			 REG_WMAC_ASSOCIATED_MU_BFMEE4_8822C,
			 REG_WMAC_ASSOCIATED_MU_BFMEE5_8822C,
			 REG_WMAC_ASSOCIATED_MU_BFMEE6_8822C,
			 REG_WMAC_ASSOCIATED_MU_BFMEE7_8822C};
	/* Misc */
	u32 val32;


	info = GET_BEAMFORM_INFO(adapter);
	/* MU BFee */
	idx = bfee->mu_reg_index;

	/* Disable sending NDPA & BF-rpt-poll to this BFee */
	rtw_write16(adapter, mu_reg[idx] , 0);
	/* Set validity of MU STA */
	val32 = rtw_read32(adapter, REG_MU_TX_CTL_8822C);
	val32 &= ~BIT(idx);
	rtw_write32(adapter, REG_MU_TX_CTL_8822C, val32);

	/* Force disable sounding config */
	_config_sounding(adapter, bfee, _TRUE, HW_CFG_SOUNDING_TYPE_LEAVE);

	info->beamformee_mu_reg_maping &= ~BIT(idx);
	bfee->mu_reg_index = 0xFF;

	RTW_INFO("%s: Clear MU BFee entry(%d) HW setting\n", __FUNCTION__, idx);
}

void rtl8822c_phy_bf_reset_all(PADAPTER adapter)
{
	struct beamforming_info *info;
	u8 i, val8;
	u32 val32;


	RTW_INFO("+%s\n", __FUNCTION__);
	info = GET_BEAMFORM_INFO(adapter);

	info->bSetBFHwConfigInProgess = _TRUE;

	/* Reset MU BFer entry setting */
	/* Clear validity of MU STA0 and MU STA1 */
	val32 = rtw_read32(adapter, REG_MU_TX_CTL_8822C);
	val32 = BIT_SET_R_MU_TABLE_VALID_8822C(val32, 0);
	rtw_write32(adapter, REG_MU_TX_CTL_8822C, val32);

	/* Reset SU BFer entry setting */
	rtw_write32(adapter, REG_ASSOCIATED_BFMER0_INFO_8822C, 0);
	rtw_write16(adapter, REG_ASSOCIATED_BFMER0_INFO_8822C+4, 0);
	rtw_write16(adapter, REG_TX_CSI_RPT_PARAM_BW20_8822C, 0);

	rtw_write32(adapter, REG_ASSOCIATED_BFMER1_INFO_8822C, 0);
	rtw_write16(adapter, REG_ASSOCIATED_BFMER1_INFO_8822C+4, 0);
	rtw_write16(adapter, REG_TX_CSI_RPT_PARAM_BW20_8822C+2, 0);

	/* Force disable sounding */
	_config_sounding(adapter, NULL, _FALSE, HW_CFG_SOUNDING_TYPE_RESET);

	/* Config RF mode */
	phydm_txbf_rfmode(GET_PDM_ODM(adapter), info->beamformee_su_cnt, info->beamformee_mu_cnt);

	/* Reset MU BFee entry setting */

	/* Disable sending NDPA & BF-rpt-poll to all BFee */
	for (i=0; i < MAX_NUM_BEAMFORMEE_MU; i++)
		rtw_write16(adapter, REG_WMAC_ASSOCIATED_MU_BFMEE2_8822C+(i*2), 0);

	/* set validity of MU STA */
	rtw_write32(adapter, REG_MU_TX_CTL_8822C, 0);

	/* Reset SU BFee entry setting */
	/* SU BF0 and BF1 */
	val32 = BIT_R_EN_NDPA_INT_8822C | BIT_USE_NDPA_PARAMETER_8822C | BIT_R_ENABLE_NDPA_8822C;
	rtw_write32(adapter, REG_TXBF_CTRL_8822C, val32);
	rtw_write32(adapter, REG_ASSOCIATED_BFMEE_SEL_8822C, 0);

	info->bSetBFHwConfigInProgess = _FALSE;

	RTW_INFO("-%s\n", __FUNCTION__);
}

void rtl8822c_phy_bf_init(PADAPTER adapter)
{
	u8 v8;
	u32 v32;

	v32 = rtw_read32(adapter, REG_MU_TX_CTL_8822C);
	/* Enable P1 aggr new packet according to P0 transfer time */
	v32 |= BIT_R_MU_P1_WAIT_STATE_EN_8822C;
	/* MU Retry Limit */
	v32 = BIT_SET_R_MU_RL_8822C(v32, 0xA);
	/* Disable Tx MU-MIMO until sounding done */
	v32 &= ~BIT_R_EN_MU_MIMO_8822C;
	/* Clear validity of MU STAs */
	v32 = BIT_SET_R_MU_TABLE_VALID_8822C(v32, 0);
	rtw_write32(adapter, REG_MU_TX_CTL_8822C, v32);

	/* MU-MIMO Option as default value */
	v8 = BIT_WMAC_TXMU_ACKPOLICY_8822C(3);
	v8 |= BIT_WMAC_TXMU_ACKPOLICY_EN_8822C;
	rtw_write8(adapter, REG_MU_BF_OPTION_8822C, v8);
	/* MU-MIMO Control as default value */
	rtw_write16(adapter, REG_WMAC_MU_BF_CTL_8822C, 0);

	/* Set MU NDPA rate & BW source */
	/* 0x42C[30] = 1 (0: from Tx desc, 1: from 0x45F) */
	v8 = rtw_read8(adapter, REG_TXBF_CTRL_8822C+3);
	v8 |= (BIT_USE_NDPA_PARAMETER_8822C >> 24);
	rtw_write8(adapter, REG_TXBF_CTRL_8822C+3, v8);
	/* 0x45F[7:0] = 0x10 (Rate=OFDM_6M, BW20) */
	rtw_write8(adapter, REG_NDPA_OPT_CTRL_8822C, 0x10);

	/* Temp Settings */
	/* STA2's CSI rate is fixed at 6M */
	v8 = rtw_read8(adapter, 0x6DF);
	v8 = (v8 & 0xC0) | 0x4;
	rtw_write8(adapter, 0x6DF, v8);
}

void rtl8822c_phy_bf_enter(PADAPTER adapter, struct sta_info *sta)
{
	struct beamforming_info *info;
	struct beamformer_entry *bfer;
	struct beamformee_entry *bfee;


	RTW_INFO("+%s: " MAC_FMT "\n", __FUNCTION__, MAC_ARG(sta->cmn.mac_addr));

	info = GET_BEAMFORM_INFO(adapter);
	bfer = rtw_bf_bfer_get_entry_by_addr(adapter, sta->cmn.mac_addr);
	bfee = rtw_bf_bfee_get_entry_by_addr(adapter, sta->cmn.mac_addr);

	info->bSetBFHwConfigInProgess = _TRUE;

	if (bfer) {
		bfer->state = BEAMFORM_ENTRY_HW_STATE_ADDING;

		if (TEST_FLAG(bfer->cap, BEAMFORMER_CAP_VHT_MU))
			_config_beamformer_mu(adapter, bfer);
		else if (TEST_FLAG(bfer->cap, BEAMFORMER_CAP_VHT_SU|BEAMFORMER_CAP_HT_EXPLICIT))
			_config_beamformer_su(adapter, bfer);

		bfer->state = BEAMFORM_ENTRY_HW_STATE_ADDED;
	}

	if (bfee) {
		bfee->state = BEAMFORM_ENTRY_HW_STATE_ADDING;

		if (TEST_FLAG(bfee->cap, BEAMFORMEE_CAP_VHT_MU))
			_config_beamformee_mu(adapter, bfee);
		else if (TEST_FLAG(bfee->cap, BEAMFORMEE_CAP_VHT_SU|BEAMFORMEE_CAP_HT_EXPLICIT))
			_config_beamformee_su(adapter, bfee);

		bfee->state = BEAMFORM_ENTRY_HW_STATE_ADDED;
	}

	info->bSetBFHwConfigInProgess = _FALSE;

	RTW_INFO("-%s\n", __FUNCTION__);
}

void rtl8822c_phy_bf_leave(PADAPTER adapter, u8 *addr)
{
	struct beamforming_info *info;
	struct beamformer_entry *bfer;
	struct beamformee_entry *bfee;


	RTW_INFO("+%s: " MAC_FMT "\n", __FUNCTION__, MAC_ARG(addr));

	info = GET_BEAMFORM_INFO(adapter);

	bfer = rtw_bf_bfer_get_entry_by_addr(adapter, addr);
	bfee = rtw_bf_bfee_get_entry_by_addr(adapter, addr);

	/* Clear P_AID of Beamformee */
	/* Clear MAC address of Beamformer */
	/* Clear Associated Bfmee Sel */
	if (bfer) {
		bfer->state = BEAMFORM_ENTRY_HW_STATE_DELETING;

		rtw_write8(adapter, REG_SND_PTCL_CTRL_8822C, 0xD8);

		if (TEST_FLAG(bfer->cap, BEAMFORMER_CAP_VHT_MU))
			_reset_beamformer_mu(adapter, bfer);
		else if (TEST_FLAG(bfer->cap, BEAMFORMER_CAP_VHT_SU|BEAMFORMER_CAP_HT_EXPLICIT))
			_reset_beamformer_su(adapter, bfer);

		bfer->state = BEAMFORM_ENTRY_HW_STATE_NONE;
		bfer->cap = BEAMFORMING_CAP_NONE;
		bfer->used = _FALSE;
	}

	if (bfee) {
		bfee->state = BEAMFORM_ENTRY_HW_STATE_DELETING;

		phydm_txbf_rfmode(GET_PDM_ODM(adapter), info->beamformee_su_cnt, info->beamformee_mu_cnt);

		if (TEST_FLAG(bfee->cap, BEAMFORMEE_CAP_VHT_MU))
			_reset_beamformee_mu(adapter, bfee);
		else if (TEST_FLAG(bfee->cap, BEAMFORMEE_CAP_VHT_SU|BEAMFORMEE_CAP_HT_EXPLICIT))
			_reset_beamformee_su(adapter, bfee);

		bfee->state = BEAMFORM_ENTRY_HW_STATE_NONE;
		bfee->cap = BEAMFORMING_CAP_NONE;
		bfee->used = _FALSE;
	}

	RTW_INFO("-%s\n", __FUNCTION__);
}

void rtl8822c_phy_bf_set_gid_table(PADAPTER adapter,
		struct beamformer_entry	*bfer_info)
{
	struct beamformer_entry *bfer;
	struct beamforming_info *info;
	u32 gid_valid[2] = {0};
	u32 user_position[4] = {0};
	int i;

	/* update bfer info */
	bfer = rtw_bf_bfer_get_entry_by_addr(adapter, bfer_info->mac_addr);
	if (!bfer) {
		RTW_INFO("%s: Cannot find BFer entry!!\n", __func__);
		return;
	}
	_rtw_memcpy(bfer->gid_valid, bfer_info->gid_valid, 8);
	_rtw_memcpy(bfer->user_position, bfer_info->user_position, 16);

	info = GET_BEAMFORM_INFO(adapter);
	info->bSetBFHwConfigInProgess = _TRUE;

	/* For GID 0~31 */
	for (i = 0; i < 4; i++)
		gid_valid[0] |= (bfer->gid_valid[i] << (i << 3));

	for (i = 0; i < 8; i++) {
		if (i < 4)
			user_position[0] |= (bfer->user_position[i] << (i << 3));
		else
			user_position[1] |= (bfer->user_position[i] << ((i - 4) << 3));
	}

	RTW_INFO("%s: STA0: gid_valid=0x%x, user_position_l=0x%x, user_position_h=0x%x\n",
		__func__, gid_valid[0], user_position[0], user_position[1]);

	/* For GID 32~64 */
	for (i = 4; i < 8; i++)
		gid_valid[1] |= (bfer->gid_valid[i] << ((i - 4) << 3));

	for (i = 8; i < 16; i++) {
		if (i < 12)
			user_position[2] |= (bfer->user_position[i] << ((i - 8) << 3));
		else
			user_position[3] |= (bfer->user_position[i] << ((i - 12) << 3));
	}

	RTW_INFO("%s: STA1: gid_valid=0x%x, user_position_l=0x%x, user_position_h=0x%x\n",
		__func__, gid_valid[1], user_position[2], user_position[3]);

	rtw_halmac_bf_cfg_mu_bfee(adapter_to_dvobj(adapter), gid_valid, user_position);

	info->bSetBFHwConfigInProgess = _FALSE;
}

void rtl8822c_phy_bf_sounding_status(PADAPTER adapter, u8 status)
{
	struct beamforming_info	*info;
	struct sounding_info *sounding;
	struct beamformee_entry *bfee;
	enum _HW_CFG_SOUNDING_TYPE sounding_type;
	u16 val16;
	u32 val32;
	u8 is_sounding_success[6] = {0};


	RTW_INFO("+%s\n", __FUNCTION__);

	info = GET_BEAMFORM_INFO(adapter);
	sounding = &info->sounding_info;

	info->bSetBFHwConfigInProgess = _TRUE;

	if (sounding->state == SOUNDING_STATE_SU_SOUNDDOWN) {
		/* SU sounding done */
		RTW_INFO("%s: SUBFeeCurIdx=%d\n", __FUNCTION__, sounding->su_bfee_curidx);

		bfee = &info->bfee_entry[sounding->su_bfee_curidx];
		if (bfee->bSoundingTimeout) {
			RTW_INFO("%s: Return because SUBFeeCurIdx(%d) is sounding timeout!!!\n", __FUNCTION__, sounding->su_bfee_curidx);
			info->bSetBFHwConfigInProgess = _FALSE;
			return;
		}

		RTW_INFO("%s: Config SU sound down HW settings\n", __FUNCTION__);
		/* Config SU sounding */
		if (_TRUE == status)
			sounding_type = HW_CFG_SOUNDING_TYPE_SOUNDDOWN;
		else
			sounding_type = HW_CFG_SOUNDING_TYPE_LEAVE;
		_config_sounding(adapter, bfee, _FALSE, sounding_type);

		/* <tynli_note> Why set here? */
		/* disable NDP packet use beamforming */
		val16 = rtw_read16(adapter, REG_TXBF_CTRL_8822C);
		val16 |= BIT_DIS_NDP_BFEN_8822C;
		rtw_write16(adapter, REG_TXBF_CTRL_8822C, val16);
	} else if (sounding->state == SOUNDING_STATE_MU_SOUNDDOWN) {
		/* MU sounding done */
		RTW_INFO("%s: Config MU sound down HW settings\n", __FUNCTION__);

		val32 = rtw_read32(adapter, REG_WMAC_ASSOCIATED_MU_BFMEE2_8822C);
		is_sounding_success[0] = (val32 & BIT_STATUS_BFEE2_8822C) ? 1:0;
		is_sounding_success[1] = ((val32 >> 16) & BIT_STATUS_BFEE3_8822C) ? 1:0;
		val32 = rtw_read32(adapter, REG_WMAC_ASSOCIATED_MU_BFMEE4_8822C);
		is_sounding_success[2] = (val32 & BIT_STATUS_BFEE4_8822C) ? 1:0;
		is_sounding_success[3] = ((val32 >> 16) & BIT_BIT_STATUS_BFEE5_8822C) ? 1:0;
		val32 = rtw_read32(adapter, REG_WMAC_ASSOCIATED_MU_BFMEE6_8822C);
		is_sounding_success[4] = (val32 & BIT_STATUS_BFEE6_8822C) ? 1:0;
		is_sounding_success[5] = ((val32 >> 16) & BIT_STATUS_BFEE7_8822C) ? 1:0;

		RTW_INFO("%s: is_sounding_success STA1:%d, STA2:%d, STA3:%d, STA4:%d, STA5:%d, STA6:%d\n",
			 __FUNCTION__, is_sounding_success[0], is_sounding_success[1] , is_sounding_success[2],
			 is_sounding_success[3], is_sounding_success[4], is_sounding_success[5]);

		/* Config MU sounding */
		_config_sounding(adapter, NULL, _TRUE, HW_CFG_SOUNDING_TYPE_SOUNDDOWN);
	} else {
		RTW_INFO("%s: Invalid sounding state(%d). Do nothing!\n", __FUNCTION__, sounding->state);
	}

	info->bSetBFHwConfigInProgess = _FALSE;

	RTW_INFO("-%s\n", __FUNCTION__);
}
#endif /* CONFIG_BEAMFORMING */

