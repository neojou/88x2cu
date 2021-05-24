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
#ifndef __HALRF_PRECOMP_H__
#define __HALRF_PRECOMP_H__


/*@--------------------------[Define] ---------------------------------------*/

#ifdef CONFIG_RTL8822C
	#define RF_8822C_SUPPORT
#endif

#define	MASKBYTE0		0xff
#define	MASKBYTE1		0xff00
#define	MASKBYTE2		0xff0000
#define	MASKBYTE3		0xff000000
#define	MASKHWORD		0xffff0000
#define	MASKLWORD		0x0000ffff
#define	MASKDWORD		0xffffffff
#define	MASKRF		0xfffff
#define	MASKRFMODE		0xf0000
#define	MASKRFRXBB		0x003e0
#define	MASKTXPWR		0x0003f
#define	INVALID_RF_DATA 0xffffffff


/*---[Define Only] ----------------------------------------------------------*/
#include "../../hal_headers_le.h"

#include "halrf_ic_hw_info.h"
#include "halrf_ic_sw_info.h"

#if 0 //NEO
#include "halrf_hw_cfg.h"
#include "halrf_hw_cfg_ex.h"
#include "halrf_interface.h"
#include "halrf_dbg_cmd.h"
#include "halrf_dbg.h"
#endif //NEO

#include "halrf_txgapk.h"
#include "halrf_pwr_track.h"
#include "halrf_iqk.h"
#include "halrf_dpk.h"

#include "halrf_dack.h"

#if 0 //NEO
#include "halrf_pmac.h"
//#include "halrf_dbg_cmd.h"
#include "halrf_dbg_cmd_ex.h"
#include "halrf_init.h"
#include "halrf_init_ex.h"
#endif //NEO

#include "halrf_pwr_table.h"

#if 0 //NEO
#include "halrf_api.h"
#include "halrf_psd.h"
#include "halrf_kfree.h"
#include "halrf_hwimg.h"

#endif //NEO

#include "halrf.h"
#include "halrf_ex.h"

#ifdef RF_8822C_SUPPORT
#include "halrf_8822c/halrf_efuse_8822c.h"
#include "halrf_8822c/halrf_8822c_api.h"
#endif

#endif  /* __HALRF_PRECOMP_H__ */
