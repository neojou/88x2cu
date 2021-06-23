/******************************************************************************
 *
 * Copyright(c) 2017 - 2019 Realtek Corporation. All rights reserved.
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

#include "halmac_usb_8822c.h"
#include "halmac_pwr_seq_8822c.h"
#include "../halmac_init_88xx.h"
#include "../halmac_common_88xx.h"

#if (HALMAC_8822C_SUPPORT && HALMAC_USB_SUPPORT)

/**
 * halmac_pcie_switch_8822c() - pcie gen1/gen2 switch
 * @adapter : the adapter of halmac
 * @pcie_cfg : gen1/gen2 selection
 * Author : KaiYuan Chang
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
pcie_switch_usb_8822c(struct halmac_adapter *adapter, enum halmac_pcie_cfg cfg)
{
	return HALMAC_RET_NOT_SUPPORT;
}

/**
 * intf_tun_usb_8822c() - usb interface fine tuning
 * @adapter : the adapter of halmac
 * Author : Ivan
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
intf_tun_usb_8822c(struct halmac_adapter *adapter)
{
	return HALMAC_RET_SUCCESS;
}

#endif /* HALMAC_8822C_SUPPORT*/
