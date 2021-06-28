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

/*************************************************************
 * include files
 ************************************************************/

#include "mp_precomp.h"
#include "phydm_precomp.h"

#ifdef PHYDM_AUTO_DEGBUG

#if (ODM_IC_11N_SERIES_SUPPORT == 1)

void phydm_dbg_port_dump_n(void *dm_void, u32 *_used, char *output,
			   u32 *_out_len)
{
	u32 value32 = 0;
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 used = *_used;
	u32 out_len = *_out_len;

	PDM_SNPF(out_len, used, output + used, out_len - used,
		 "not support now\n");

	*_used = used;
	*_out_len = out_len;
}

#endif

#if (ODM_IC_11AC_SERIES_SUPPORT == 1)
void phydm_dbg_port_dump_ac(void *dm_void, u32 *_used, char *output,
			    u32 *_out_len)
{
	u32 value32 = 0;
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 used = *_used;
	u32 out_len = *_out_len;

	value32 = odm_get_bb_reg(dm, R_0xf80, MASKDWORD);
	PDM_SNPF(out_len, used, output + used, out_len - used,
		 "\r\n %-35s = 0x%x", "rptreg of sc/bw/ht/...", value32);

	/* dbg_port = basic state machine */
	{
		odm_set_bb_reg(dm, ODM_REG_DBG_RPT_11AC, MASKDWORD, 0x000);
		value32 = odm_get_bb_reg(dm, ODM_REG_DBG_RPT_11AC, MASKDWORD);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\r\n %-35s = 0x%x", "0x8fc", value32);

		value32 = odm_get_bb_reg(dm, ODM_REG_RPT_11AC, MASKDWORD);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\r\n %-35s = 0x%x", "basic state machine", value32);
	}

	/* dbg_port = state machine */
	{
		odm_set_bb_reg(dm, ODM_REG_DBG_RPT_11AC, MASKDWORD, 0x007);
		value32 = odm_get_bb_reg(dm, ODM_REG_DBG_RPT_11AC, MASKDWORD);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\r\n %-35s = 0x%x", "0x8fc", value32);

		value32 = odm_get_bb_reg(dm, ODM_REG_RPT_11AC, MASKDWORD);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\r\n %-35s = 0x%x", "state machine", value32);
	}

	/* dbg_port = CCA-related*/
	{
		odm_set_bb_reg(dm, ODM_REG_DBG_RPT_11AC, MASKDWORD, 0x204);
		value32 = odm_get_bb_reg(dm, ODM_REG_DBG_RPT_11AC, MASKDWORD);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\r\n %-35s = 0x%x", "0x8fc", value32);

		value32 = odm_get_bb_reg(dm, ODM_REG_RPT_11AC, MASKDWORD);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\r\n %-35s = 0x%x", "CCA-related", value32);
	}

	/* dbg_port = edcca/rxd*/
	{
		odm_set_bb_reg(dm, ODM_REG_DBG_RPT_11AC, MASKDWORD, 0x278);
		value32 = odm_get_bb_reg(dm, ODM_REG_DBG_RPT_11AC, MASKDWORD);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\r\n %-35s = 0x%x", "0x8fc", value32);

		value32 = odm_get_bb_reg(dm, ODM_REG_RPT_11AC, MASKDWORD);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\r\n %-35s = 0x%x", "edcca/rxd", value32);
	}

	/* dbg_port = rx_state/mux_state/ADC_MASK_OFDM*/
	{
		odm_set_bb_reg(dm, ODM_REG_DBG_RPT_11AC, MASKDWORD, 0x290);
		value32 = odm_get_bb_reg(dm, ODM_REG_DBG_RPT_11AC, MASKDWORD);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\r\n %-35s = 0x%x", "0x8fc", value32);

		value32 = odm_get_bb_reg(dm, ODM_REG_RPT_11AC, MASKDWORD);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\r\n %-35s = 0x%x",
			 "rx_state/mux_state/ADC_MASK_OFDM", value32);
	}

	/* dbg_port = bf-related*/
	{
		odm_set_bb_reg(dm, ODM_REG_DBG_RPT_11AC, MASKDWORD, 0x2B2);
		value32 = odm_get_bb_reg(dm, ODM_REG_DBG_RPT_11AC, MASKDWORD);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\r\n %-35s = 0x%x", "0x8fc", value32);

		value32 = odm_get_bb_reg(dm, ODM_REG_RPT_11AC, MASKDWORD);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\r\n %-35s = 0x%x", "bf-related", value32);
	}

	/* dbg_port = bf-related*/
	{
		odm_set_bb_reg(dm, ODM_REG_DBG_RPT_11AC, MASKDWORD, 0x2B8);
		value32 = odm_get_bb_reg(dm, ODM_REG_DBG_RPT_11AC, MASKDWORD);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\r\n %-35s = 0x%x", "0x8fc", value32);

		value32 = odm_get_bb_reg(dm, ODM_REG_RPT_11AC, MASKDWORD);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\r\n %-35s = 0x%x", "bf-related", value32);
	}

	/* dbg_port = txon/rxd*/
	{
		odm_set_bb_reg(dm, ODM_REG_DBG_RPT_11AC, MASKDWORD, 0xA03);
		value32 = odm_get_bb_reg(dm, ODM_REG_DBG_RPT_11AC, MASKDWORD);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\r\n %-35s = 0x%x", "0x8fc", value32);

		value32 = odm_get_bb_reg(dm, ODM_REG_RPT_11AC, MASKDWORD);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\r\n %-35s = 0x%x", "txon/rxd", value32);
	}

	/* dbg_port = l_rate/l_length*/
	{
		odm_set_bb_reg(dm, ODM_REG_DBG_RPT_11AC, MASKDWORD, 0xA0B);
		value32 = odm_get_bb_reg(dm, ODM_REG_DBG_RPT_11AC, MASKDWORD);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\r\n %-35s = 0x%x", "0x8fc", value32);

		value32 = odm_get_bb_reg(dm, ODM_REG_RPT_11AC, MASKDWORD);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\r\n %-35s = 0x%x", "l_rate/l_length", value32);
	}

	/* dbg_port = rxd/rxd_hit*/
	{
		odm_set_bb_reg(dm, ODM_REG_DBG_RPT_11AC, MASKDWORD, 0xA0D);
		value32 = odm_get_bb_reg(dm, ODM_REG_DBG_RPT_11AC, MASKDWORD);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\r\n %-35s = 0x%x", "0x8fc", value32);

		value32 = odm_get_bb_reg(dm, ODM_REG_RPT_11AC, MASKDWORD);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\r\n %-35s = 0x%x", "rxd/rxd_hit", value32);
	}

	/* dbg_port = dis_cca*/
	{
		odm_set_bb_reg(dm, ODM_REG_DBG_RPT_11AC, MASKDWORD, 0xAA0);
		value32 = odm_get_bb_reg(dm, ODM_REG_DBG_RPT_11AC, MASKDWORD);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\r\n %-35s = 0x%x", "0x8fc", value32);

		value32 = odm_get_bb_reg(dm, ODM_REG_RPT_11AC, MASKDWORD);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\r\n %-35s = 0x%x", "dis_cca", value32);
	}

	/* dbg_port = tx*/
	{
		odm_set_bb_reg(dm, ODM_REG_DBG_RPT_11AC, MASKDWORD, 0xAB0);
		value32 = odm_get_bb_reg(dm, ODM_REG_DBG_RPT_11AC, MASKDWORD);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\r\n %-35s = 0x%x", "0x8fc", value32);

		value32 = odm_get_bb_reg(dm, ODM_REG_RPT_11AC, MASKDWORD);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\r\n %-35s = 0x%x", "tx", value32);
	}

	/* dbg_port = rx plcp*/
	{
		odm_set_bb_reg(dm, ODM_REG_DBG_RPT_11AC, MASKDWORD, 0xAD0);
		value32 = odm_get_bb_reg(dm, ODM_REG_DBG_RPT_11AC, MASKDWORD);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\r\n %-35s = 0x%x", "0x8fc", value32);

		value32 = odm_get_bb_reg(dm, ODM_REG_RPT_11AC, MASKDWORD);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\r\n %-35s = 0x%x", "rx plcp", value32);

		odm_set_bb_reg(dm, ODM_REG_DBG_RPT_11AC, MASKDWORD, 0xAD1);
		value32 = odm_get_bb_reg(dm, ODM_REG_DBG_RPT_11AC, MASKDWORD);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\r\n %-35s = 0x%x", "0x8fc", value32);

		value32 = odm_get_bb_reg(dm, ODM_REG_RPT_11AC, MASKDWORD);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\r\n %-35s = 0x%x", "rx plcp", value32);

		odm_set_bb_reg(dm, ODM_REG_DBG_RPT_11AC, MASKDWORD, 0xAD2);
		value32 = odm_get_bb_reg(dm, ODM_REG_DBG_RPT_11AC, MASKDWORD);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\r\n %-35s = 0x%x", "0x8fc", value32);

		value32 = odm_get_bb_reg(dm, ODM_REG_RPT_11AC, MASKDWORD);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\r\n %-35s = 0x%x", "rx plcp", value32);

		odm_set_bb_reg(dm, ODM_REG_DBG_RPT_11AC, MASKDWORD, 0xAD3);
		value32 = odm_get_bb_reg(dm, ODM_REG_DBG_RPT_11AC, MASKDWORD);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\r\n %-35s = 0x%x", "0x8fc", value32);

		value32 = odm_get_bb_reg(dm, ODM_REG_RPT_11AC, MASKDWORD);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "\r\n %-35s = 0x%x", "rx plcp", value32);
	}
	*_used = used;
	*_out_len = out_len;
}
#endif

#ifdef PHYDM_IC_JGR3_SERIES_SUPPORT
void phydm_dbg_port_dump_jgr3(void *dm_void, u32 *_used, char *output,
			      u32 *_out_len)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 used = *_used;
	u32 out_len = *_out_len;
	/*u32 dbg_port_idx_all[3] = {0x000, 0x001, 0x002};*/
	u32 val = 0;
	u32 dbg_port_idx = 0;
	u32 i = 0;

	PDM_VAST_SNPF(out_len, used, output + used, out_len - used,
		      "%-17s = %s\n", "DbgPort index", "Value");

	for (dbg_port_idx = 0x0; dbg_port_idx <= 0xfff; dbg_port_idx++) {
		if (phydm_set_bb_dbg_port(dm, DBGPORT_PRI_3, dbg_port_idx)) {
			val = phydm_get_bb_dbg_port_val(dm);
			PDM_VAST_SNPF(out_len, used, output + used,
				      out_len - used,
				      "0x%-15x = 0x%x\n", dbg_port_idx, val);
			phydm_release_bb_dbg_port(dm);
		}
	}
	*_used = used;
	*_out_len = out_len;
}
#endif

void phydm_dbg_port_dump(void *dm_void, u32 *_used, char *output, u32 *_out_len)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 used = *_used;
	u32 out_len = *_out_len;

	PDM_VAST_SNPF(out_len, used, output + used, out_len - used,
		      "------ BB debug port start ------\n");

	switch (dm->ic_ip_series) {
	#ifdef PHYDM_IC_JGR3_SERIES_SUPPORT
	case PHYDM_IC_JGR3:
		phydm_dbg_port_dump_jgr3(dm, &used, output, &out_len);
		break;
	#endif

	#if (ODM_IC_11AC_SERIES_SUPPORT == 1)
	case PHYDM_IC_AC:
		phydm_dbg_port_dump_ac(dm, &used, output, &out_len);
		break;
	#endif

	#if (ODM_IC_11N_SERIES_SUPPORT == 1)
	case PHYDM_IC_N:
		phydm_dbg_port_dump_n(dm, &used, output, &out_len);
		break;
	#endif

	default:
		break;
	}
	*_used = used;
	*_out_len = out_len;
}

void phydm_auto_dbg_console(
	void *dm_void,
	char input[][16],
	u32 *_used,
	char *output,
	u32 *_out_len)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	char help[] = "-h";
	u32 var1[10] = {0};
	u32 used = *_used;
	u32 out_len = *_out_len;

	PHYDM_SSCANF(input[1], DCMD_DECIMAL, &var1[0]);

	if ((strcmp(input[1], help) == 0)) {
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "hang: {1} {1:Show DbgPort, 2:Auto check hang}\n");
		return;
	} else if (var1[0] == 1) {
		PHYDM_SSCANF(input[2], DCMD_DECIMAL, &var1[1]);
		if (var1[1] == 1) {
			phydm_dbg_port_dump(dm, &used, output, &out_len);
		} else if (var1[1] == 2) {
			PDM_SNPF(out_len, used, output + used,
				 out_len - used, "Not support\n");
		}
	}

	*_used = used;
	*_out_len = out_len;
}

#endif
