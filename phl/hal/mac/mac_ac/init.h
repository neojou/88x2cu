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

#ifndef _MAC_INIT_H_
#define _MAC_INIT_H_

#include "../type.h"
#if MAC_AX_8852A_SUPPORT
#include "mac_8852a/init_8852a.h"
#endif
#if MAC_AX_8852B_SUPPORT
#include "mac_8852b/init_8852b.h"
#endif
#if MAC_AC_8822C_SUPPORT
#include "mac_8822c/init_8822c.h"
#endif

#include "role.h"
#if 0 // NEO
#include "fwdl.h"
#if MAC_PCIE_SUPPORT
#include "_pcie.h"
#endif

#endif // if 0 NEO

#ifdef CONFIG_NEW_HALMAC_INTERFACE
struct mac_ax_adapter *get_mac_ax_adapter(enum mac_ax_intf intf,
					  u8 chip_id, u8 chip_cut,
					  void *phl_adapter, void *drv_adapter,
					  struct mac_ax_pltfm_cb *pltfm_cb);
#else
struct mac_adapter *get_mac_adapter(enum mac_intf intf,
					  u8 chip_id, u8 chip_cut,
					  void *drv_adapter,
					  struct mac_pltfm_cb *pltfm_cb);
#endif

u32 cmac_func_en(struct mac_adapter *adapter, u8 band, u8 en);
u32 mac_sys_init(struct mac_adapter *adapter);
u32 mac_trx_init(struct mac_adapter *adapter);
u32 mac_hal_init(struct mac_adapter *adapter,
		 struct mac_trx_info *trx_info,
		 struct mac_fwdl_info *fwdl_info,
		 struct mac_intf_info *intf_info);
u32 mac_hal_fast_init(struct mac_adapter *adapter,
		      struct mac_trx_info *trx_info,
		      struct mac_fwdl_info *fwdl_info,
		      struct mac_intf_info *intf_info);
#if 0 // NEO
u32 mac_hal_deinit(struct mac_ax_adapter *adapter);
u32 mac_ax_init_state(struct mac_ax_adapter *adapter);
#endif // if 0 NEO

#endif
