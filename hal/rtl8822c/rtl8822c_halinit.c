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
#define _RTL8822C_HALINIT_C_

#include <drv_types.h>		/* PADAPTER, basic_types.h and etc. */
#include <hal_data.h>		/* GET_HAL_SPEC(), HAL_DATA_TYPE */
#include "../hal_halmac.h"	/* HALMAC API */
#include "rtl8822c.h"

#define CALLED_FROM_HAL
#include "../phl/phl_headers.h"
#include "../phl/hal/hal_headers.h"

void rtl8822c_init_hal_spec(PADAPTER adapter)
{
	struct hal_spec_t *hal_spec;


	hal_spec = GET_HAL_SPEC(adapter);
	rtw_halmac_fill_hal_spec(adapter_to_dvobj(adapter), hal_spec);

	hal_spec->ic_name = "rtl8822c";
	hal_spec->macid_num = 128;
	/* hal_spec->sec_cam_ent_num follow halmac setting */
	hal_spec->sec_cap = SEC_CAP_CHK_BMC | SEC_CAP_CHK_EXTRA_SEC;
	hal_spec->wow_cap = WOW_CAP_TKIP_OL;
	hal_spec->macid_cap = MACID_DROP;

	hal_spec->rfpath_num_2g = 2;
	hal_spec->rfpath_num_5g = 2;
	hal_spec->rf_reg_path_num = hal_spec->rf_reg_path_avail_num = 2;
	hal_spec->rf_reg_trx_path_bmp = 0x33;
	hal_spec->max_tx_cnt = 2;

	hal_spec->tx_nss_num = 2;
	hal_spec->rx_nss_num = 2;
	hal_spec->band_cap = BAND_CAP_2G | BAND_CAP_5G;
	hal_spec->bw_cap = BW_CAP_20M | BW_CAP_40M | BW_CAP_80M;
	hal_spec->port_num = 5;
	hal_spec->proto_cap = PROTO_CAP_11B | PROTO_CAP_11G | PROTO_CAP_11N | PROTO_CAP_11AC;

	hal_spec->txgi_max = 127;
	hal_spec->txgi_pdbm = 4;

	hal_spec->wl_func = 0
			    | WL_FUNC_P2P
			    | WL_FUNC_MIRACAST
			    | WL_FUNC_TDLS
			    ;

	hal_spec->tx_aclt_unit_factor = 8;

	hal_spec->rx_tsf_filter = 1;

	hal_spec->pg_txpwr_saddr = 0x10;
	hal_spec->pg_txgi_diff_factor = 2;

	hal_spec->hci_type = 0;

	rtw_macid_ctl_init_sleep_reg(adapter_to_macidctl(adapter)
		, REG_MACID_SLEEP_8822C
		, REG_MACID_SLEEP1_8822C
		, REG_MACID_SLEEP2_8822C
		, REG_MACID_SLEEP3_8822C);
	
	rtw_macid_ctl_init_drop_reg(adapter_to_macidctl(adapter)
		, REG_MACID_DROP0_8822C
		, REG_MACID_DROP1_8822C
		, REG_MACID_DROP2_8822C
		, REG_MACID_DROP3_8822C);

}


void _rtw_hal_set_fw_rsvd_page(_adapter *adapter, bool finished, u8 *page_num);

enum halmac_ret_status 
set_trx_fifo_info_8822c(struct halmac_adapter *halmac);

static int init_mac_flow(struct dvobj_priv *d)
{
	PADAPTER p;
	struct hal_com_data *hal;
	struct halmac_adapter *halmac;
	struct halmac_api *api;
	struct phl_info_t *phl_info = d->phl;
	struct hal_info_t *hal_info = phl_info->hal;
	enum rtw_hal_status hal_status;
	union halmac_wlan_addr hwa;
	enum halmac_trx_mode trx_mode;
	enum halmac_ret_status status;
	u8 drv_rsvd_num;
	u8 nettype;
	int err, err_ret = -1;

	p = dvobj_get_primary_adapter(d);
	hal = GET_HAL_DATA(p);
	halmac = dvobj_to_halmac(d);
	api = HALMAC_GET_API(halmac);

	_rtw_hal_set_fw_rsvd_page(p, _FALSE, &drv_rsvd_num);
	halmac->txff_alloc.rsvd_drv_pg_num = 8;
	hal->drv_rsvd_page_number = 8;

	hal_status = rtw_hal_mac_trx_init(hal_info);
	if (hal_status != RTW_HAL_STATUS_SUCCESS) {
		RTW_ERR("%s: drtw_hal_mac_trx_init FAIL! status=0x%02x\n", __func__, hal_status);
		return -1;
	}


	status = set_trx_fifo_info_8822c(halmac);
	if (status != HALMAC_RET_SUCCESS) {
		RTW_ERR("[ERR]set trx fifo!\n");
		goto out;
	}

	halmac->pq_map[HALMAC_PQ_MAP_VO] = HALMAC_MAP2_NQ;
	halmac->pq_map[HALMAC_PQ_MAP_VI] = HALMAC_MAP2_NQ;
	halmac->pq_map[HALMAC_PQ_MAP_BE] = HALMAC_MAP2_LQ;
	halmac->pq_map[HALMAC_PQ_MAP_BK] = HALMAC_MAP2_LQ;
	halmac->pq_map[HALMAC_PQ_MAP_MG] = HALMAC_MAP2_HQ;
	halmac->pq_map[HALMAC_PQ_MAP_HI] = HALMAC_MAP2_HQ;

	halmac->txff_alloc.high_queue_pg_num = 64;
	halmac->txff_alloc.low_queue_pg_num = 64;
	halmac->txff_alloc.normal_queue_pg_num = 64;
	halmac->txff_alloc.extra_queue_pg_num = 0;
	halmac->txff_alloc.pub_queue_pg_num = 1;

	halmac->h2c_info.buf_size = 1024;

	/* Driver insert flow: Sync driver setting with register */
	/* Sync driver RCR cache with register setting */
	rtw_hal_get_hwreg(dvobj_get_primary_adapter(d), HW_VAR_RCR, NULL);

	err_ret = 0;
out:
	return err_ret;
}

u8 rtl8822c_hal_init(PADAPTER adapter)
{
	PHAL_DATA_TYPE hal;
	struct dvobj_priv *d;
	struct halmac_adapter *halmac;
	struct halmac_api *api;
	struct phl_info_t *phl_info = d->phl;
	struct hal_info_t *hal_info = phl_info->hal;
	struct halmac_fw_version *info;
	enum halmac_ret_status status;
	enum rtw_hal_status hal_status;
	u8 fw_bin = _TRUE;
	u32 ok;
	int err = 0, err_ret = -1;

	d = adapter_to_dvobj(adapter);
	hal = GET_HAL_DATA(adapter);
	halmac = dvobj_to_halmac(d);
	info = &halmac->fw_ver;
	api = HALMAC_GET_API(halmac);

	hal->bMacPwrCtrlOn = _TRUE;
	{
		/* 5.1. (Driver) Reset driver variables if needed */
		hal->LastHMEBoxNum = 0;

		/* 5.2. (Driver) Get FW version */
		hal->firmware_version = info->version;
		hal->firmware_sub_version = info->sub_version;
		hal->firmware_size = array_length_mp_8822c_fw_nic;
	}

	/* InitMACFlow */
	err = init_mac_flow(d);
	if (err) {
		RTW_ERR("%s init_mac_flow err=%d\n", __func__, err);
		return _FALSE;
	}

	#ifdef CONFIG_CORE_CMD_THREAD 
	if (is_primary_adapter(adapter) && !adapter->cmdThread) {
		RTW_INFO(FUNC_ADPT_FMT " start RTW_CMD_THREAD\n", FUNC_ADPT_ARG(adapter));
		adapter->cmdThread = kthread_run(rtw_cmd_thread, adapter, "RTW_CMD_THREAD");
		if (IS_ERR(adapter->cmdThread)) {
			adapter->cmdThread = NULL;
			return _FALSE;
		}
		else
			_rtw_down_sema(&adapter_to_dvobj(adapter)->cmdpriv.start_cmdthread_sema); /* wait for cmd_thread to run */
	}
	#endif

	halmac->halmac_state.dlfw_state = HALMAC_GEN_INFO_SENT;

	/* Init BB, RF */
	ok = rtw_hal_init_phy(adapter);
	if (_FALSE == ok) {
		RTW_ERR("%s rtw_hal_init_phy=%d\n", __func__, err);
		return _FALSE;
	}

	/* Sync driver status with hardware setting */
	rtw_hal_get_hwreg(adapter, HW_VAR_RCR, NULL);
	hal->bFWReady = _TRUE;
	hal->fw_ractrl = _TRUE;

	return _TRUE;
}

void rtl8822c_init_misc(PADAPTER adapter)
{
	PHAL_DATA_TYPE hal;
	u8 v8 = 0;
	u32 v32 = 0;


	hal = GET_HAL_DATA(adapter);


	/* initial security setting */
	invalidate_cam_all(adapter);

	/* check RCR/ICV bit */
	rtw_hal_rcr_clear(adapter, BIT_ACRC32_8822C | BIT_AICV_8822C);

	/* clear rx ctrl frame */
	rtw_write16(adapter, REG_RXFLTMAP1_8822C, 0);

	/*Enable MAC security engine*/
	rtw_write16(adapter, REG_CR, (rtw_read16(adapter, REG_CR) | BIT_MAC_SEC_EN));

#ifdef CONFIG_XMIT_ACK
	/* ack for xmit mgmt frames. */
	rtw_write32(adapter, REG_FWHW_TXQ_CTRL_8822C,
		rtw_read32(adapter, REG_FWHW_TXQ_CTRL_8822C) | BIT_EN_QUEUE_RPT_8822C(BIT(4)));
#endif /* CONFIG_XMIT_ACK */

#ifdef CONFIG_TCP_CSUM_OFFLOAD_RX
	rtw_hal_rcr_add(adapter, BIT_TCPOFLD_EN_8822C);
#endif /* CONFIG_TCP_CSUM_OFFLOAD_RX*/
}

u32 rtl8822c_init(PADAPTER adapter)
{
	u8 ok = _TRUE;
	PHAL_DATA_TYPE hal;

	hal = GET_HAL_DATA(adapter);

	ok = rtl8822c_hal_init(adapter);
	if (_FALSE == ok)
		return _FAIL;

	rtl8822c_phy_init_haldm(adapter);
#ifdef CONFIG_BEAMFORMING
	rtl8822c_phy_bf_init(adapter);
#endif

#ifdef CONFIG_FW_MULTI_PORT_SUPPORT
	/*HW / FW init*/
	rtw_hal_set_default_port_id_cmd(adapter, 0);
#endif

#ifdef CONFIG_BT_COEXIST
	/* Init BT hw config. */
	if (_TRUE == hal->EEPROMBluetoothCoexist) {
		rtw_btcoex_HAL_Initialize(adapter, _FALSE);
		#ifdef CONFIG_FW_MULTI_PORT_SUPPORT
		rtw_hal_set_wifi_btc_port_id_cmd(adapter);
		#endif
	} else
#endif /* CONFIG_BT_COEXIST */
		rtw_btcoex_wifionly_hw_config(adapter);

	rtl8822c_init_misc(adapter);

	return _SUCCESS;
}

u32 rtl8822c_deinit(PADAPTER adapter)
{
	struct dvobj_priv *d;
	PHAL_DATA_TYPE hal;
	int err;


	d = adapter_to_dvobj(adapter);
	hal = GET_HAL_DATA(adapter);

	hal->bFWReady = _FALSE;
	hal->fw_ractrl = _FALSE;

	err = rtw_halmac_deinit_hal(d);
	if (err)
		return _FAIL;

	return _SUCCESS;
}

void rtl8822c_init_default_value(PADAPTER adapter)
{
	PHAL_DATA_TYPE hal;
	u8 i;


	hal = GET_HAL_DATA(adapter);

	/* init default value */
	hal->fw_ractrl = _FALSE;

	/* init phydm default value */
	hal->bIQKInitialized = _FALSE;

	/* init Efuse variables */
	hal->EfuseUsedBytes = 0;
	hal->EfuseUsedPercentage = 0;

	hal->EfuseHal.fakeEfuseBank = 0;
	hal->EfuseHal.fakeEfuseUsedBytes = 0;
	_rtw_memset(hal->EfuseHal.fakeEfuseContent, 0xFF, EFUSE_MAX_HW_SIZE);
	_rtw_memset(hal->EfuseHal.fakeEfuseInitMap, 0xFF, EFUSE_MAX_MAP_LEN);
	_rtw_memset(hal->EfuseHal.fakeEfuseModifiedMap, 0xFF, EFUSE_MAX_MAP_LEN);
	hal->EfuseHal.BTEfuseUsedBytes = 0;
	hal->EfuseHal.BTEfuseUsedPercentage = 0;
	_rtw_memset(hal->EfuseHal.BTEfuseContent, 0xFF, EFUSE_MAX_BT_BANK * EFUSE_MAX_HW_SIZE);
	_rtw_memset(hal->EfuseHal.BTEfuseInitMap, 0xFF, EFUSE_BT_MAX_MAP_LEN);
	_rtw_memset(hal->EfuseHal.BTEfuseModifiedMap, 0xFF, EFUSE_BT_MAX_MAP_LEN);
	hal->EfuseHal.fakeBTEfuseUsedBytes = 0;
	_rtw_memset(hal->EfuseHal.fakeBTEfuseContent, 0xFF, EFUSE_MAX_BT_BANK * EFUSE_MAX_HW_SIZE);
	_rtw_memset(hal->EfuseHal.fakeBTEfuseInitMap, 0xFF, EFUSE_BT_MAX_MAP_LEN);
	_rtw_memset(hal->EfuseHal.fakeBTEfuseModifiedMap, 0xFF, EFUSE_BT_MAX_MAP_LEN);

}
