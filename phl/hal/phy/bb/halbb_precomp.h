/******************************************************************************
 *
 * Copyright(c) 2007 - 2020  Realtek Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of version 2 of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * The full GNU General Public License is included in this distribution in the
 * file called LICENSE.
 *
 * Contact Information:
 * wlanfae <wlanfae@realtek.com>
 * Realtek Corporation, No. 2, Innovation Road II, Hsinchu Science Park,
 * Hsinchu 300, Taiwan.
 *
 * Larry Finger <Larry.Finger@lwfinger.net>
 *
 *****************************************************************************/
#ifndef __HALBB_PRECOMP_H__
#define __HALBB_PRECOMP_H__

#include "halbb_cfg_ic.h"

/*---[Define Only] ----------------------------------------------------------*/
#include "../../hal_headers_le.h"

#include "halbb_types.h"
#include "halbb_features.h"
#include "halbb_ic_hw_info.h"
#include "halbb_ic_sw_info.h"
#include "halbb_interface.h"

#if 0 //NEO
#if (PLATFOM_IS_LITTLE_ENDIAN)
	#include "halbb_physts_ie_l_endian.h"
	#include "halbb_ra_l_endian.h"
	#include "halbb_rua_tbl_l_endian.h"
#else
	#include "halbb_physts_ie_b_endian.h"
	#include "halbb_ra_b_endian.h"
	#include "halbb_rua_tbl_b_endian.h"
#endif

#endif // NEO

#include "halbb_ra.h"

#if 0 //NEO
#include "halbb_ra_ex.h"
#include "halbb_hw_cfg.h"
#include "halbb_hw_cfg_ex.h"
#include "halbb_api.h"
#include "halbb_api_ex.h"
#endif // NEO

#include "halbb_interface_ex.h"
#include "halbb_math_lib_ex.h"
#include "halbb_math_lib.h"
#include "halbb_dbg.h"

#if 0 //NEO
#include "halbb_dbg_cmd.h"
#include "halbb_dbg_cmd_ex.h"
#include "halbb_physts_ex.h"
#include "halbb_physts.h"
#include "halbb_cmn_rpt_ex.h"
#include "halbb_cmn_rpt.h"
#endif // NEO

#include "halbb_init.h"
#include "halbb_init_ex.h"

#if 0 //NEO
#include "halbb_pmac_setting.h"
#include "halbb_pmac_setting_ex.h"
#include "halbb_plcp_tx_ex.h"
#include "halbb_plcp_tx.h"
#include "halbb_plcp_gen.h"
#endif // NEO
#include "halbb_la_mode_ex.h"
#include "halbb_la_mode.h"
#if 0 //NEO
#include "halbb_psd.h"
#include "halbb_pwr_ctrl_ex.h"
#include "halbb_pwr_ctrl.h"
#include "halbb_mp_ex.h"
#include "halbb_dfs.h"
#include "halbb_dfs_ex.h"
#include "halbb_cfo_trk.h"
#include "halbb_edcca.h"
#include "halbb_edcca_ex.h"
#endif // NEO
#include "halbb_env_mntr_ex.h"
#include "halbb_env_mntr.h"
#include "halbb_dig_ex.h"
#include "halbb_dig.h"

#if 0 //NEO
#include "halbb_dyn_csi_rsp.h"
#include "halbb_dyn_csi_rsp_ex.h"
#include "halbb_ant_div.h"
#include "halbb_statistics.h"
#include "halbb_ch_info_ex.h"
#include "halbb_ch_info.h"
#include "halbb_auto_dbg.h"
#include "halbb_dbcc_ex.h"
#include "halbb_dbcc.h"
#include "halbb_rua_tbl.h"
#include "halbb_rua_tbl_ex.h"
#include "halbb_fwofld.h"
#endif // NEO

#include "halbb_ex.h"
#include "halbb.h"

#endif
