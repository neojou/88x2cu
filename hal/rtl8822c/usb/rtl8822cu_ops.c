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
#define _RTL8822CU_OPS_C_

#include <drv_types.h>			/* PADAPTER, basic_types.h and etc. */
#include <hal_intf.h>			/* struct hal_ops */
#include <hal_data.h>			/* HAL_DATA_TYPE */
#include "../../hal_halmac.h"		/* register define */
#include "../rtl8822c.h"		/* 8822c hal common define. rtl8822cu_init_default_value ...*/
#include "rtl8822cu.h"			/* 8822cu functions */

#ifdef CONFIG_SUPPORT_USB_INT
static void rtl8822cu_interrupt_handler(PADAPTER padapter, u16 pkt_len, u8 *pbuf)
{	
	}
#endif /* CONFIG_SUPPORT_USB_INT */

void rtl8822cu_set_hw_type(struct dvobj_priv *pdvobj)
{
	pdvobj->HardwareType = HARDWARE_TYPE_RTL8822CU;
	RTW_INFO("CHIP TYPE: RTL8822C\n");
}

static u8 sethwreg(PADAPTER padapter, u8 variable, u8 *val)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(padapter);
	struct dvobj_priv	*pdvobjpriv = adapter_to_dvobj(padapter);
	struct pwrctrl_priv *pwrctl = adapter_to_pwrctl(padapter);
	struct registry_priv *registry_par = &padapter->registrypriv;
	int status = 0;
	u8 ret = _SUCCESS;

	switch (variable) {
	case HW_VAR_RXDMA_AGG_PG_TH:
#ifdef CONFIG_USB_RX_AGGREGATION
		if (pdvobjpriv->traffic_stat.cur_tx_tp < 1 && pdvobjpriv->traffic_stat.cur_rx_tp < 1) {
			/* for low traffic, do not usb AGGREGATION */
			pHalData->rxagg_usb_timeout = 0x01;
			pHalData->rxagg_usb_size = 0x01;

		} else {
#ifdef CONFIG_PLATFORM_NOVATEK_NT72668
			pHalData->rxagg_usb_timeout = 0x20;
			pHalData->rxagg_usb_size = 0x03;
#elif defined(CONFIG_PLATFORM_HISILICON)
			/* use 16k to workaround for HISILICON platform */
			pHalData->rxagg_usb_timeout = 8;
			pHalData->rxagg_usb_size = 3;
#elif defined(CONFIG_PLATFORM_RTK119X_AM)
			/* reduce rxagg_usb_timeout to avoid rx fifo full at high throughput case */
			pHalData->rxagg_usb_timeout = 0x10;
			pHalData->rxagg_usb_size = 0x05;
#else
			/* default setting */
			pHalData->rxagg_usb_timeout = 0x20;
			pHalData->rxagg_usb_size = 0x05;
#endif
		}
//NEO
//		rtw_halmac_rx_agg_switch(pdvobjpriv, _TRUE);

#if 0
		RTW_INFO("\n==========RAFFIC_STATISTIC==============\n");
		RTW_INFO("cur_tx_bytes:%lld\n", pdvobjpriv->traffic_stat.cur_tx_bytes);
		RTW_INFO("cur_rx_bytes:%lld\n", pdvobjpriv->traffic_stat.cur_rx_bytes);

		RTW_INFO("last_tx_bytes:%lld\n", pdvobjpriv->traffic_stat.last_tx_bytes);
		RTW_INFO("last_rx_bytes:%lld\n", pdvobjpriv->traffic_stat.last_rx_bytes);

		RTW_INFO("cur_tx_tp:%d\n", pdvobjpriv->traffic_stat.cur_tx_tp);
		RTW_INFO("cur_rx_tp:%d\n", pdvobjpriv->traffic_stat.cur_rx_tp);
		RTW_INFO("\n========================\n");
#endif
#endif
		break;
	case HW_VAR_SET_RPWM:
		break;
	case HW_VAR_AMPDU_MAX_TIME:
		rtw_write8(padapter, REG_AMPDU_MAX_TIME_V1_8822C, 0x70);
		break;
	case HW_VAR_SET_DRV_ERLY_INT:
		switch (*val) {
		#ifdef CONFIG_TDLS
		#ifdef CONFIG_TDLS_CH_SW
			case TDLS_BCN_ERLY_ON:
				padapter->tdlsinfo.chsw_info.bcn_early_reg_bkp = rtw_read8(padapter, REG_DRVERLYINT);
				rtw_write8(padapter, REG_DRVERLYINT, 20);
				break;
			case TDLS_BCN_ERLY_OFF:
				rtw_write8(padapter, REG_DRVERLYINT, padapter->tdlsinfo.chsw_info.bcn_early_reg_bkp);
				break;
		#endif
		#endif
		}
		break;
	default:
		ret = rtl8822c_sethwreg(padapter, variable, val);
		break;
	}

	return ret;
}

static void gethwreg(PADAPTER padapter, u8 variable, u8 *val)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(padapter);

	rtl8822c_gethwreg(padapter, variable, val);
}

/*
	Description:
		Change default setting of specified variable.
*/
static u8 sethaldefvar(PADAPTER padapter, HAL_DEF_VARIABLE eVariable, void *pValue)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(padapter);
	u8 bResult = _SUCCESS;

	switch (eVariable) {
	default:
		rtl8822c_sethaldefvar(padapter, eVariable, pValue);
		break;
	}

	return bResult;
}

/*
	Description:
		Query setting of specified variable.
*/
static u8 gethaldefvar(PADAPTER	padapter, HAL_DEF_VARIABLE eVariable, void *pValue)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(padapter);
	u8 bResult = _SUCCESS;

	switch (eVariable) {
	case HW_VAR_MAX_RX_AMPDU_FACTOR:
		*(HT_CAP_AMPDU_FACTOR *)pValue = MAX_AMPDU_FACTOR_64K;
		break;
	default:
		bResult = rtl8822c_gethaldefvar(padapter, eVariable, pValue);
		break;
	}

	return bResult;
}

static void rtl8822cu_init_default_value(PADAPTER	padapter)
{
	rtl8822c_init_default_value(padapter);
}

static u8 rtl8822cu_ps_func(PADAPTER padapter, HAL_INTF_PS_FUNC efunc_id, u8 *val)
{
	u8 bResult = _TRUE;

	switch (efunc_id) {

	default:
		break;
	}
	return bResult;
}

#ifdef CONFIG_RTW_LED
static void read_ledsetting(PADAPTER adapter)
{
	struct led_priv *ledpriv = adapter_to_led(adapter);

#ifdef CONFIG_RTW_SW_LED
	PHAL_DATA_TYPE hal;
	
	hal = GET_HAL_DATA(adapter);
	ledpriv->bRegUseLed = _TRUE;

	switch (hal->EEPROMCustomerID) {
	default:
		hal->CustomerID = RT_CID_DEFAULT;
		break;
	}

	switch (hal->CustomerID) {
	case RT_CID_DEFAULT:
	default:
		ledpriv->LedStrategy = SW_LED_MODE9;
		break;
	}
#else /* HW LED */
	ledpriv->LedStrategy = HW_LED;
#endif /* CONFIG_RTW_SW_LED */
}
#endif /* CONFIG_RTW_LED */
 

void rtl8822cu_set_hal_ops(PADAPTER padapter)
{
	struct hal_ops *ops;
	int err;


	err = rtl8822cu_halmac_init_adapter(padapter);
	if (err) {
		RTW_INFO("%s: [ERROR]HALMAC initialize FAIL!\n", __func__);
		return;
	}

	rtl8822c_set_hal_ops(padapter);

	ops = &padapter->hal_func;

	ops->hal_init = rtl8822cu_init;
	ops->hal_deinit = rtl8822cu_deinit;

	ops->inirp_init = rtl8822cu_inirp_init;
	ops->inirp_deinit = rtl8822cu_inirp_deinit;

	ops->init_xmit_priv = rtl8822cu_init_xmit_priv;
	ops->free_xmit_priv = rtl8822cu_free_xmit_priv;

#ifdef CONFIG_RTW_SW_LED
	ops->InitSwLeds = rtl8822cu_initswleds;
	ops->DeInitSwLeds = rtl8822cu_deinitswleds;
#endif

	ops->init_default_value = rtl8822cu_init_default_value;
	ops->intf_chip_configure = rtl8822cu_interface_configure;

	ops->set_hw_reg_handler = sethwreg;
	ops->GetHwRegHandler = gethwreg;
	ops->get_hal_def_var_handler = gethaldefvar;
	ops->SetHalDefVarHandler = sethaldefvar;


	ops->mgnt_xmit = rtl8822cu_mgnt_xmit;

#ifdef CONFIG_HOSTAPD_MLME
	ops->hostap_mgnt_xmit_entry = rtl8822cu_hostap_mgnt_xmit_entry;
#endif
	ops->interface_ps_func = rtl8822cu_ps_func;
#ifdef CONFIG_SUPPORT_USB_INT
	ops->interrupt_handler = rtl8822cu_interrupt_handler;
#endif


}
