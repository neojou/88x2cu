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
#define _RTL8822CU_HALINIT_C_

#include <hal_data.h>			/* HAL DATA */
#include "../../hal_halmac.h"		/* HALMAC API */
#include "../rtl8822c.h"		/* rtl8822c hal common define */
#include "rtl8822cu.h"

#ifndef CONFIG_USB_HCI

	#error "CONFIG_USB_HCI shall be on!\n"

#endif



#ifdef CONFIG_RTW_LED
static void init_hwled(PADAPTER adapter, u8 enable)
{
	u8 mode = 0;
	struct led_priv *ledpriv = adapter_to_led(adapter);

	if (ledpriv->LedStrategy != HW_LED)
		return;

	rtw_halmac_led_cfg(adapter_to_dvobj(adapter), enable, mode);
}
#endif

static void hal_init_misc(PADAPTER adapter)
{
#ifdef CONFIG_RTW_LED
	struct led_priv *ledpriv = adapter_to_led(adapter);

	init_hwled(adapter, 1);
#ifdef CONFIG_RTW_SW_LED
	if (ledpriv->bRegUseLed == _TRUE)
		rtw_halmac_led_cfg(adapter_to_dvobj(adapter), _TRUE, 3);
#endif
#endif /* CONFIG_RTW_LED */

}

u32 rtl8822cu_init(PADAPTER padapter)
{
	u8 status = _SUCCESS;
	systime init_start_time = rtw_get_current_time();

	if (rtl8822c_init(padapter) == _FAIL)
		status = _FAIL;
	else
		hal_init_misc(padapter);

	RTW_INFO("%s in %dms, status=%d\n", __func__, rtw_get_passing_time_ms(init_start_time), status);
	return status;
}

static void hal_deinit_misc(PADAPTER adapter)
{
#ifdef CONFIG_RTW_LED
	struct led_priv *ledpriv = adapter_to_led(adapter);

	init_hwled(adapter, 0);
#ifdef CONFIG_RTW_SW_LED
	if (ledpriv->bRegUseLed == _TRUE)
		rtw_halmac_led_cfg(adapter_to_dvobj(adapter), _FALSE, 3);
#endif
#endif /* CONFIG_RTW_LED */
}

u32 rtl8822cu_deinit(PADAPTER padapter)
{
	struct pwrctrl_priv *pwrctl = adapter_to_pwrctl(padapter);
	HAL_DATA_TYPE	*pHalData = GET_HAL_DATA(padapter);
	struct dvobj_priv *pobj_priv = adapter_to_dvobj(padapter);
	u8 status = _TRUE;

	RTW_INFO("==> %s\n", __func__);

	hal_deinit_misc(padapter);
	status = rtl8822c_deinit(padapter);
	if (status == _FALSE) {
		RTW_INFO("%s: rtl8822c_hal_deinit fail\n", __func__);
		return _FAIL;
	}

	RTW_INFO("%s <==\n", __func__);
	return _SUCCESS;
}


u32 rtl8822cu_inirp_init(PADAPTER padapter)
{
	struct registry_priv *regsty = adapter_to_regsty(padapter);
	u8 i, status;
	struct recv_buf *precvbuf;
	struct dvobj_priv *pdev = adapter_to_dvobj(padapter);
	struct recv_priv *precvpriv = &adapter_to_dvobj(padapter)->recvpriv;

	status = _SUCCESS;

	precvpriv->ff_hwaddr = RECV_BULK_IN_ADDR;

exit:
	return status;

}

u32 rtl8822cu_inirp_deinit(PADAPTER padapter)
{

	//rtw_read_port_cancel(padapter);


	return _SUCCESS;
}

void rtl8822cu_update_interrupt_mask(PADAPTER padapter, u8 bHIMR0 , u32 AddMSR, u32 RemoveMSR)
{
	HAL_DATA_TYPE *pHalData;
	u32 *himr;

	pHalData = GET_HAL_DATA(padapter);

	if (bHIMR0)
		himr = &(pHalData->IntrMask[0]);
	else
		himr = &(pHalData->IntrMask[1]);

	if (AddMSR)
		*himr |= AddMSR;

	if (RemoveMSR)
		*himr &= (~RemoveMSR);

	if (bHIMR0)
		rtw_write32(padapter, REG_HIMR0_8822C, *himr);
	else
		rtw_write32(padapter, REG_HIMR1_8822C, *himr);

}

static void config_chip_out_EP(PADAPTER padapter, u8 NumOutPipe)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(padapter);


	pHalData->OutEpQueueSel = 0;
	pHalData->OutEpNumber = 0;

	switch (NumOutPipe) {
	case 4:
		pHalData->OutEpQueueSel = TX_SELE_HQ | TX_SELE_LQ | TX_SELE_NQ;
		pHalData->OutEpNumber = 4;
		break;
	case 3:
		pHalData->OutEpQueueSel = TX_SELE_HQ | TX_SELE_LQ | TX_SELE_NQ;
		pHalData->OutEpNumber = 3;
		break;
	case 2:
		pHalData->OutEpQueueSel = TX_SELE_HQ | TX_SELE_NQ;
		pHalData->OutEpNumber = 2;
		break;
	case  1:
		pHalData->OutEpQueueSel = TX_SELE_HQ;
		pHalData->OutEpNumber = 1;
		break;
	default:
		break;
	}

	RTW_INFO("%s OutEpQueueSel(0x%02x), OutEpNumber(%d)\n", __func__, pHalData->OutEpQueueSel, pHalData->OutEpNumber);

}

static u8 usb_set_queue_pipe_mapping(PADAPTER padapter, u8 NumInPipe, u8 NumOutPipe)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(padapter);
	u8 result = _FALSE;

	config_chip_out_EP(padapter, NumOutPipe);

	/* Normal chip with one IN and one OUT doesn't have interrupt IN EP. */
	if (1 == pHalData->OutEpNumber) {
		if (1 != NumInPipe)
			return result;
	}

	result = Hal_MappingOutPipe(padapter, NumOutPipe);

	return result;

}

void rtl8822cu_interface_configure(PADAPTER padapter)
{
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(padapter);
	struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
	PUSB_DATA pusb_data = dvobj_to_usb(dvobj);

	if (IS_SUPER_SPEED_USB(padapter))
		pHalData->UsbBulkOutSize = USB_SUPER_SPEED_BULK_SIZE;
	else if (IS_HIGH_SPEED_USB(padapter))
		pHalData->UsbBulkOutSize = USB_HIGH_SPEED_BULK_SIZE;
	else
		pHalData->UsbBulkOutSize = USB_FULL_SPEED_BULK_SIZE;

#ifdef CONFIG_USB_TX_AGGREGATION
	/* according to value defined by halmac */
	pHalData->UsbTxAggMode		= 1;
	rtw_halmac_usb_get_txagg_desc_num(dvobj, &pHalData->UsbTxAggDescNum);
#endif /* CONFIG_USB_TX_AGGREGATION */

#ifdef CONFIG_USB_RX_AGGREGATION
	/* according to value defined by halmac */
	pHalData->rxagg_mode = RX_AGG_USB;
#ifdef CONFIG_PLATFORM_NOVATEK_NT72668
	pHalData->rxagg_usb_size = 0x03;
	pHalData->rxagg_usb_timeout = 0x20;
#elif defined(CONFIG_PLATFORM_HISILICON)
	 /* use 16k to workaround for HISILICON platform */
	pHalData->rxagg_usb_size = 3;
	pHalData->rxagg_usb_timeout = 8;
#endif /* CONFIG_PLATFORM_NOVATEK_NT72668 */
#endif /* CONFIG_USB_RX_AGGREGATION */

	usb_set_queue_pipe_mapping(padapter,
			   pusb_data->RtNumInPipes, pusb_data->RtNumOutPipes);
}
