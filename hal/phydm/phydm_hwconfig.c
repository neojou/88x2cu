/******************************************************************************
 *
 * Copyright(c) 2007 - 2017  Realtek Corporation.
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

/*@************************************************************
 * include files
 ************************************************************/

#include "mp_precomp.h"
#include "phydm_precomp.h"

#define READ_AND_CONFIG_MP(ic, txt) (odm_read_and_config_mp_##ic##txt(dm))
#define READ_AND_CONFIG_TC(ic, txt) (odm_read_and_config_tc_##ic##txt(dm))

#if (PHYDM_TESTCHIP_SUPPORT == 1)
#define READ_AND_CONFIG(ic, txt)                     \
	do {                                         \
		if (dm->is_mp_chip)                  \
			READ_AND_CONFIG_MP(ic, txt); \
		else                                 \
			READ_AND_CONFIG_TC(ic, txt); \
	} while (0)
#else
#define READ_AND_CONFIG READ_AND_CONFIG_MP
#endif

#define GET_VERSION_MP(ic, txt) (odm_get_version_mp_##ic##txt())
#define GET_VERSION_TC(ic, txt) (odm_get_version_tc_##ic##txt())

#if (PHYDM_TESTCHIP_SUPPORT == 1)
#define GET_VERSION(ic, txt) (dm->is_mp_chip ? GET_VERSION_MP(ic, txt) : GET_VERSION_TC(ic, txt))
#else
#define GET_VERSION(ic, txt) GET_VERSION_MP(ic, txt)
#endif

u32 query_phydm_trx_capability(struct dm_struct *dm)
{
	u32 value32 = 0xFFFFFFFF;

	return value32;
}

u32 query_phydm_stbc_capability(struct dm_struct *dm)
{
	u32 value32 = 0xFFFFFFFF;


	return value32;
}

u32 query_phydm_ldpc_capability(struct dm_struct *dm)
{
	u32 value32 = 0xFFFFFFFF;

	return value32;
}

u32 query_phydm_txbf_parameters(struct dm_struct *dm)
{
	u32 value32 = 0xFFFFFFFF;

	return value32;
}

u32 query_phydm_txbf_capability(struct dm_struct *dm)
{
	u32 value32 = 0xFFFFFFFF;

	return value32;
}
