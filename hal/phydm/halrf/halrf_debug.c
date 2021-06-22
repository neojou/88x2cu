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
 * ************************************************************
 */

#include "mp_precomp.h"
#include "phydm_precomp.h"

void halrf_basic_profile(void *dm_void, u32 *_used, char *output, u32 *_out_len)
{
}

void halrf_debug_trace(void *dm_void, char input[][16], u32 *_used,
		       char *output, u32 *_out_len)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _hal_rf_ *rf = &dm->rf_table;
	u32 one = 1;
	u32 used = *_used;
	u32 out_len = *_out_len;
	u32 rf_var[10] = {0};
	u8 i;

	for (i = 0; i < 5; i++)
		if (input[i + 1])
			PHYDM_SSCANF(input[i + 2], DCMD_DECIMAL, &rf_var[i]);

	if (rf_var[0] == 100) {
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\n[DBG MSG] RF Selection\n");
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "00. (( %s ))TX_PWR_TRACK\n",
			 ((rf->rf_dbg_comp & DBG_RF_TX_PWR_TRACK) ? ("V") :
			 (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "01. (( %s ))IQK\n",
			 ((rf->rf_dbg_comp & DBG_RF_IQK) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "02. (( %s ))LCK\n",
			 ((rf->rf_dbg_comp & DBG_RF_LCK) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "03. (( %s ))DPK\n",
			 ((rf->rf_dbg_comp & DBG_RF_DPK) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "04. (( %s ))TXGAPK\n",
			 ((rf->rf_dbg_comp & DBG_RF_TXGAPK) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "06. (( %s ))DPK_TRACK\n",
			 ((rf->rf_dbg_comp & DBG_RF_DPK_TRACK) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "29. (( %s ))MP\n",
			 ((rf->rf_dbg_comp & DBG_RF_MP) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "30. (( %s ))TMP\n",
			 ((rf->rf_dbg_comp & DBG_RF_TMP) ? ("V") : (".")));
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "31. (( %s ))INIT\n",
			 ((rf->rf_dbg_comp & DBG_RF_INIT) ? ("V") : (".")));

	} else if (rf_var[0] == 101) {
		rf->rf_dbg_comp = 0;
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "Disable all DBG COMP\n");
	} else {
		if (rf_var[1] == 1) /*enable*/
			rf->rf_dbg_comp |= (one << rf_var[0]);
		else if (rf_var[1] == 2) /*disable*/
			rf->rf_dbg_comp &= ~(one << rf_var[0]);
	}
	PDM_SNPF(out_len, used, output + used, out_len - used,
		 "\nCurr-RF_Dbg_Comp = 0x%x\n", rf->rf_dbg_comp);

	*_used = used;
	*_out_len = out_len;
}

void halrf_dack_debug_cmd(void *dm_void, char input[][16])
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _hal_rf_ *rf = &dm->rf_table;
	u32 dm_value[10] = {0};
	u8 i;

	for (i = 0; i < 7; i++)
		if (input[i + 1])
			PHYDM_SSCANF(input[i + 2], DCMD_DECIMAL, &dm_value[i]);

	if (dm_value[0] == 1)
		halrf_dack_trigger(dm, true);
	else			
		halrf_dack_trigger(dm, false);	
}

struct halrf_command {
	char name[16];
	u8 id;
};

enum halrf_CMD_ID {
	HALRF_HELP,
	HALRF_SUPPORTABILITY,
	HALRF_DBG_COMP,
	HALRF_PROFILE,
	HALRF_IQK_INFO,
	HALRF_IQK,
	HALRF_IQK_DEBUG,
	HALRF_DPK,
	HALRF_DACK,
	HALRF_DACK_DEBUG,
	HALRF_DUMP_RFK_REG,
#ifdef CONFIG_2G_BAND_SHIFT
	HAL_BAND_SHIFT,
#endif
};

struct halrf_command halrf_cmd_ary[] = {
	{"-h", HALRF_HELP},
	{"ability", HALRF_SUPPORTABILITY},
	{"dbg", HALRF_DBG_COMP},
	{"profile", HALRF_PROFILE},
	{"iqk_info", HALRF_IQK_INFO},
	{"iqk", HALRF_IQK},
	{"iqk_dbg", HALRF_IQK_DEBUG},
	{"dpk", HALRF_DPK},
	{"dack", HALRF_DACK},
	{"dack_dbg", HALRF_DACK_DEBUG},
	{"dump_rfk_reg", HALRF_DUMP_RFK_REG},
#ifdef CONFIG_2G_BAND_SHIFT
	{"band_shift", HAL_BAND_SHIFT},
#endif
};

void halrf_cmd_parser(void *dm_void, char input[][16], u32 *_used, char *output,
		      u32 *_out_len, u32 input_num)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
}

void halrf_init_debug_setting(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _hal_rf_ *rf = &dm->rf_table;

	rf->rf_dbg_comp = DBG_RF_RFK;
}
