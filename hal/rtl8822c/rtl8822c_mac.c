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
#define _RTL8822C_MAC_C_

#include <drv_types.h>		/* PADAPTER, basic_types.h and etc. */
#include <hal_data.h>		/* HAL_DATA_TYPE */
#include "../hal_halmac.h"	/* Register Definition and etc. */
#include "rtl8822c.h"		/* FW array */


inline u8 rtl8822c_rcr_config(PADAPTER p, u32 rcr)
{
	u32 v32;
	int err;

	v32 = GET_HAL_DATA(p)->ReceiveConfig;
	v32 ^= rcr;
	v32 &= BIT_APP_PHYSTS_8822C;
	if (v32) {
		v32 = rcr & BIT_APP_PHYSTS_8822C;
		RTW_INFO("%s: runtime %s rx phy status!\n",
			 __FUNCTION__, v32 ? "ENABLE" : "DISABLE");
		if (v32) {
			err = rtw_halmac_config_rx_info(adapter_to_dvobj(p), HALMAC_DRV_INFO_PHY_STATUS);
			if (err) {
				RTW_INFO("%s: Enable rx phy status FAIL!!", __FUNCTION__);
				rcr &= ~BIT_APP_PHYSTS_8822C;
			}
		} else {
			err = rtw_halmac_config_rx_info(adapter_to_dvobj(p), HALMAC_DRV_INFO_NONE);
			if (err) {
				RTW_INFO("%s: Disable rx phy status FAIL!!", __FUNCTION__);
				rcr |= BIT_APP_PHYSTS_8822C;
			}
		}
	}

	rtw_write32(p, REG_RCR_8822C, rcr);

	GET_HAL_DATA(p)->ReceiveConfig = rcr;
	return _TRUE;
}

inline u8 rtl8822c_rx_ba_ssn_appended(PADAPTER p)
{
	return rtw_hal_rcr_check(p, BIT_APP_BASSN_8822C);
}

inline u8 rtl8822c_rx_fcs_append_switch(PADAPTER p, u8 enable)
{
	u32 rcr_bit;
	u8 ret = _TRUE;

	rcr_bit = BIT_APP_FCS_8822C;
	if (_TRUE == enable)
		ret = rtw_hal_rcr_add(p, rcr_bit);
	else
		ret = rtw_hal_rcr_clear(p, rcr_bit);

	return ret;
}

inline u8 rtl8822c_rx_fcs_appended(PADAPTER p)
{
	return rtw_hal_rcr_check(p, BIT_APP_FCS_8822C);
}

inline u8 rtl8822c_rx_tsf_addr_filter_config(PADAPTER p, u8 config)
{
	u8 v8;

	v8 = GET_HAL_DATA(p)->rx_tsf_addr_filter_config;

	if (v8 != config)
		rtw_write8(p, REG_NAN_RX_TSF_FILTER_8822C, config);

	GET_HAL_DATA(p)->rx_tsf_addr_filter_config = config;
	return _TRUE;
}

u32 rtl8822c_get_tx_desc_size(struct _ADAPTER *a)
{
	struct dvobj_priv *d;
	u32 size = 48;	/* HALMAC_TX_DESC_SIZE_8822C */
	int err = 0;


	d = adapter_to_dvobj(a);

	err = rtw_halmac_get_tx_desc_size(d, &size);
	if (err) {
		RTW_WARN(FUNC_ADPT_FMT ": Fail to get TX Descriptor size!!(err=%d)\n",
			 FUNC_ADPT_ARG(a), err);
		size = 48;
	}

	return size;
}

u32 rtl8822c_get_rx_desc_size(struct _ADAPTER *a)
{
	struct dvobj_priv *d;
	u32 size = 24;	/* HALMAC_RX_DESC_SIZE_8822C */
	int err = 0;


	d = adapter_to_dvobj(a);

	err = rtw_halmac_get_rx_desc_size(d, &size);
	if (err) {
		RTW_WARN(FUNC_ADPT_FMT ": Fail to get RX Descriptor size!!(err=%d)\n",
			 FUNC_ADPT_ARG(a), err);
		size = 24;
	}

	return size;
}

