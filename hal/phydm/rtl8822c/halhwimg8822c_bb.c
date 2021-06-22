/******************************************************************************
 *
 * Copyright(c) 2007 - 2017 Realtek Corporation.
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

/*Image2HeaderVersion: R3 1.5.12*/
#include "mp_precomp.h"
#include "../phydm_precomp.h"

#define CUT_DONT_CARE	0xf
#define RFE_DONT_CARE	0xff
#define PARA_IF 0x8
#define PARA_ELSE_IF	0x9
#define PARA_ELSE	0xa
#define PARA_END	0xb
#define PARA_CHK	0x4

/******************************************************************************
 *                           phy_reg_pg.TXT
 ******************************************************************************/

const u32 array_mp_8822c_phy_reg_pg[] = {
	0, 0, 0, 0x00000c20, 0xffffffff, 0x484c5054,
	0, 0, 0, 0x00000c24, 0xffffffff, 0x54585858,
	0, 0, 0, 0x00000c28, 0xffffffff, 0x44484c50,
	0, 0, 0, 0x00000c2c, 0xffffffff, 0x50545858,
	0, 0, 0, 0x00000c30, 0xffffffff, 0x4044484c,
	0, 0, 1, 0x00000c34, 0xffffffff, 0x50545858,
	0, 0, 1, 0x00000c38, 0xffffffff, 0x4044484c,
	0, 0, 0, 0x00000c3c, 0xffffffff, 0x50545858,
	0, 0, 0, 0x00000c40, 0xffffffff, 0x4044484c,
	0, 0, 0, 0x00000c44, 0xffffffff, 0x5858383c,
	0, 0, 1, 0x00000c48, 0xffffffff, 0x484c5054,
	0, 0, 1, 0x00000c4c, 0xffffffff, 0x383c4044,
	0, 1, 0, 0x00000e20, 0xffffffff, 0x484c5054,
	0, 1, 0, 0x00000e24, 0xffffffff, 0x54585858,
	0, 1, 0, 0x00000e28, 0xffffffff, 0x44484c50,
	0, 1, 0, 0x00000e2c, 0xffffffff, 0x50545858,
	0, 1, 0, 0x00000e30, 0xffffffff, 0x4044484c,
	0, 1, 1, 0x00000e34, 0xffffffff, 0x50545858,
	0, 1, 1, 0x00000e38, 0xffffffff, 0x4044484c,
	0, 1, 0, 0x00000e3c, 0xffffffff, 0x50545858,
	0, 1, 0, 0x00000e40, 0xffffffff, 0x4044484c,
	0, 1, 0, 0x00000e44, 0xffffffff, 0x5858383c,
	0, 1, 1, 0x00000e48, 0xffffffff, 0x484c5054,
	0, 1, 1, 0x00000e4c, 0xffffffff, 0x383c4044,
	1, 0, 0, 0x00000c24, 0xffffffff, 0x54585858,
	1, 0, 0, 0x00000c28, 0xffffffff, 0x44484c50,
	1, 0, 0, 0x00000c2c, 0xffffffff, 0x50545858,
	1, 0, 0, 0x00000c30, 0xffffffff, 0x4044484c,
	1, 0, 1, 0x00000c34, 0xffffffff, 0x50545858,
	1, 0, 1, 0x00000c38, 0xffffffff, 0x4044484c,
	1, 0, 0, 0x00000c3c, 0xffffffff, 0x50545858,
	1, 0, 0, 0x00000c40, 0xffffffff, 0x4044484c,
	1, 0, 0, 0x00000c44, 0xffffffff, 0x5858383c,
	1, 0, 1, 0x00000c48, 0xffffffff, 0x484c5054,
	1, 0, 1, 0x00000c4c, 0xffffffff, 0x383c4044,
	1, 1, 0, 0x00000e24, 0xffffffff, 0x54585858,
	1, 1, 0, 0x00000e28, 0xffffffff, 0x44484c50,
	1, 1, 0, 0x00000e2c, 0xffffffff, 0x50545858,
	1, 1, 0, 0x00000e30, 0xffffffff, 0x4044484c,
	1, 1, 1, 0x00000e34, 0xffffffff, 0x50545858,
	1, 1, 1, 0x00000e38, 0xffffffff, 0x4044484c,
	1, 1, 0, 0x00000e3c, 0xffffffff, 0x50545858,
	1, 1, 0, 0x00000e40, 0xffffffff, 0x4044484c,
	1, 1, 0, 0x00000e44, 0xffffffff, 0x5858383c,
	1, 1, 1, 0x00000e48, 0xffffffff, 0x484c5054,
	1, 1, 1, 0x00000e4c, 0xffffffff, 0x383c4044
};

void
odm_read_and_config_mp_8822c_phy_reg_pg(struct dm_struct *dm)
{
	u32 i = 0;
	u32 array_len =
		 sizeof(array_mp_8822c_phy_reg_pg) / sizeof(u32);
	u32 *array = (u32 *)array_mp_8822c_phy_reg_pg;

	PHYDM_DBG(dm, ODM_COMP_INIT, "===> %s\n", __func__);

	dm->phy_reg_pg_version = 2;
	dm->phy_reg_pg_value_type = PHY_REG_PG_EXACT_VALUE;

	for (i = 0; i < array_len; i += 6) {
		u32	v1 = array[i];
		u32	v2 = array[i + 1];
		u32	v3 = array[i + 2];
		u32	v4 = array[i + 3];
		u32	v5 = array[i + 4];
		u32	v6 = array[i + 5];

		odm_config_bb_phy_reg_pg_8822c(dm, v1, v2, v3, v4, v5, v6);
	}
}

