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

#ifndef __HALHWOUTSRC_H__
#define __HALHWOUTSRC_H__

/*@--------------------------Define -------------------------------------------*/
#define AGC_DIFF_CONFIG_MP(ic, band)				\
	(odm_read_and_config_mp_##ic##_agc_tab_diff(dm,		\
	array_mp_##ic##_agc_tab_diff_##band,			\
	sizeof(array_mp_##ic##_agc_tab_diff_##band) / sizeof(u32)))
#define AGC_DIFF_CONFIG_TC(ic, band)				\
	(odm_read_and_config_tc_##ic##_agc_tab_diff(dm,		\
	array_tc_##ic##_agc_tab_diff_##band,			\
	sizeof(array_tc_##ic##_agc_tab_diff_##band) / sizeof(u32)))
#if defined(DM_ODM_CE_MAC80211)
#else
#define AGC_DIFF_CONFIG(ic, band)                     \
	do {                                          \
		if (dm->is_mp_chip)                   \
			AGC_DIFF_CONFIG_MP(ic, band); \
		else                                  \
			AGC_DIFF_CONFIG_TC(ic, band); \
	} while (0)
#endif
/*@************************************************************
 * structure and define
 ************************************************************/

u32 query_phydm_trx_capability(struct dm_struct *dm);

u32 query_phydm_stbc_capability(struct dm_struct *dm);

u32 query_phydm_ldpc_capability(struct dm_struct *dm);

u32 query_phydm_txbf_parameters(struct dm_struct *dm);

u32 query_phydm_txbf_capability(struct dm_struct *dm);

#endif /*@#ifndef	__HALHWOUTSRC_H__*/
