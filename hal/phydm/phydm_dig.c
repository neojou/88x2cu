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

void phydm_fa_threshold_check(void *dm_void, boolean is_dfs_band)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_dig_struct *dig_t = &dm->dm_dig_table;
	u8 i = 0;

	dig_t->dm_dig_fa_th1 = DM_DIG_FA_TH1;

	if (dig_t->is_dbg_fa_th) {
		PHYDM_DBG(dm, DBG_DIG, "Manual Fix FA_th\n");
	} else if (dm->is_linked) {
		if (dm->rssi_min < 20) { /*@[PHYDM-252]*/
			dig_t->fa_th[0] = 500;
			dig_t->fa_th[1] = 750;
			dig_t->fa_th[2] = 1000;
		} else if (((dm->rx_tp >> 2) > dm->tx_tp) && /*Test RX TP*/
			   (dm->rx_tp < 10) && (dm->rx_tp > 1)) { /*TP=1~10Mb*/
			dig_t->fa_th[0] = 125;
			dig_t->fa_th[1] = 250;
			dig_t->fa_th[2] = 500;
		} else {
			dig_t->fa_th[0] = 250;
			dig_t->fa_th[1] = 500;
			dig_t->fa_th[2] = 750;
		}
	} else {
		if (is_dfs_band) { /* @For DFS band and no link */

			dig_t->fa_th[0] = 250;
			dig_t->fa_th[1] = 1000;
			dig_t->fa_th[2] = 2000;
		} else {
			dig_t->fa_th[0] = 2000;
			dig_t->fa_th[1] = 4000;
			dig_t->fa_th[2] = 5000;
		}
	}

	if ((dig_t->fa_source >= 1) && (dig_t->fa_source <= 3)) {
		for (i = 0; i < 3; i++)
			dig_t->fa_th[i] *= OFDM_FA_EXP_DURATION;

		dig_t->dm_dig_fa_th1 *= OFDM_FA_EXP_DURATION;
	}

	PHYDM_DBG(dm, DBG_DIG, "FA_th={%d,%d,%d}\n", dig_t->fa_th[0],
		  dig_t->fa_th[1], dig_t->fa_th[2]);
}

void phydm_set_big_jump_step(void *dm_void, u8 curr_igi)
{
}

#ifdef PHYDM_IC_JGR3_SERIES_SUPPORT
void phydm_write_dig_reg_jgr3(void *dm_void, u8 igi)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_dig_struct *dig_t = &dm->dm_dig_table;

	PHYDM_DBG(dm, DBG_DIG, "%s===>\n", __func__);

	odm_set_bb_reg(dm, R_0x1d70, ODM_BIT_IGI_11AC, igi);

	odm_set_bb_reg(dm, R_0x1d70, ODM_BIT_IGI_B_11AC3, igi);
}

u8 phydm_get_igi_reg_val_jgr3(void *dm_void, enum bb_path path)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 val = 0;

	PHYDM_DBG(dm, DBG_DIG, "%s===>\n", __func__);

	return (u8)val;

	if (path == BB_PATH_A)
		val = odm_get_bb_reg(dm, R_0x1d70, ODM_BIT_IGI_11AC);
	else if (path == BB_PATH_B)
		val = odm_get_bb_reg(dm, R_0x1d70, ODM_BIT_IGI_B_11AC3);

	return (u8)val;
}

void phydm_fa_cnt_statistics_jgr3(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_fa_struct *fa_t = &dm->false_alm_cnt;
	u32 ret_value = 0;
	u32 cck_enable = 0;

	ret_value = odm_get_bb_reg(dm, R_0x2de4, MASKDWORD);
	fa_t->cnt_cck_txen = (ret_value & 0xffff);
	fa_t->cnt_cck_txon = ((ret_value & 0xffff0000) >> 16);
	ret_value = odm_get_bb_reg(dm, R_0x2de0, MASKDWORD);
	fa_t->cnt_ofdm_txen = (ret_value & 0xffff);
	fa_t->cnt_ofdm_txon = ((ret_value & 0xffff0000) >> 16);

	ret_value = odm_get_bb_reg(dm, R_0x2d20, MASKDWORD);
	fa_t->cnt_fast_fsync = ret_value & 0xffff;
	fa_t->cnt_sb_search_fail = (ret_value & 0xffff0000) >> 16;

	ret_value = odm_get_bb_reg(dm, R_0x2d04, MASKDWORD);
	fa_t->cnt_parity_fail = (ret_value & 0xffff0000) >> 16;

	ret_value = odm_get_bb_reg(dm, R_0x2d08, MASKDWORD);
	fa_t->cnt_rate_illegal = ret_value & 0xffff;
	fa_t->cnt_crc8_fail = (ret_value & 0xffff0000) >> 16;

	ret_value = odm_get_bb_reg(dm, R_0x2d10, MASKDWORD);
	fa_t->cnt_mcs_fail = ret_value & 0xffff;

	/* read CCK CRC32 counter */
	ret_value = odm_get_bb_reg(dm, R_0x2c04, MASKDWORD);
	fa_t->cnt_cck_crc32_ok = ret_value & 0xffff;
	fa_t->cnt_cck_crc32_error = (ret_value & 0xffff0000) >> 16;

	/* read OFDM CRC32 counter */
	ret_value = odm_get_bb_reg(dm, R_0x2c14, MASKDWORD);
	fa_t->cnt_ofdm_crc32_ok = ret_value & 0xffff;
	fa_t->cnt_ofdm_crc32_error = (ret_value & 0xffff0000) >> 16;

	/* read OFDM2 CRC32 counter */
	ret_value = odm_get_bb_reg(dm, R_0x2c1c, MASKDWORD);
	fa_t->cnt_ofdm2_crc32_ok = ret_value & 0xffff;
	fa_t->cnt_ofdm2_crc32_error = (ret_value & 0xffff0000) >> 16;

	/* read HT CRC32 counter */
	ret_value = odm_get_bb_reg(dm, R_0x2c10, MASKDWORD);
	fa_t->cnt_ht_crc32_ok = ret_value & 0xffff;
	fa_t->cnt_ht_crc32_error = (ret_value & 0xffff0000) >> 16;

	/* read HT2 CRC32 counter */
	ret_value = odm_get_bb_reg(dm, R_0x2c18, MASKDWORD);
	fa_t->cnt_ht2_crc32_ok = ret_value & 0xffff;
	fa_t->cnt_ht2_crc32_error = (ret_value & 0xffff0000) >> 16;

	/*for VHT part */
	/*read VHT CRC32 counter */
	ret_value = odm_get_bb_reg(dm, R_0x2c0c, MASKDWORD);
	fa_t->cnt_vht_crc32_ok = ret_value & 0xffff;
	fa_t->cnt_vht_crc32_error = (ret_value & 0xffff0000) >> 16;

	/*read VHT2 CRC32 counter */
	ret_value = odm_get_bb_reg(dm, R_0x2c54, MASKDWORD);
	fa_t->cnt_vht2_crc32_ok = ret_value & 0xffff;
	fa_t->cnt_vht2_crc32_error = (ret_value & 0xffff0000) >> 16;

	ret_value = odm_get_bb_reg(dm, R_0x2d10, MASKDWORD);
	fa_t->cnt_mcs_fail_vht = (ret_value & 0xffff0000) >> 16;

	ret_value = odm_get_bb_reg(dm, R_0x2d0c, MASKDWORD);
	fa_t->cnt_crc8_fail_vhta = ret_value & 0xffff;
	fa_t->cnt_crc8_fail_vhtb = (ret_value & 0xffff0000) >> 16;

	/* @calculate OFDM FA counter instead of reading brk_cnt*/
	fa_t->cnt_ofdm_fail = fa_t->cnt_parity_fail + fa_t->cnt_rate_illegal +
			      fa_t->cnt_crc8_fail + fa_t->cnt_mcs_fail +
			      fa_t->cnt_fast_fsync + fa_t->cnt_sb_search_fail +
			      fa_t->cnt_mcs_fail_vht + fa_t->cnt_crc8_fail_vhta;

	/* Read CCK FA counter */
	fa_t->cnt_cck_fail = odm_get_bb_reg(dm, R_0x1a5c, MASKLWORD);

	/* read CCK/OFDM CCA counter */
	ret_value = odm_get_bb_reg(dm, R_0x2c08, MASKDWORD);
	fa_t->cnt_ofdm_cca = ((ret_value & 0xffff0000) >> 16);
	fa_t->cnt_cck_cca = ret_value & 0xffff;

	/* @CCK RxIQ weighting = 1 => 0x1a14[9:8]=0x0 */
	cck_enable = odm_get_bb_reg(dm, R_0x1a14, 0x300);
	
	if (cck_enable == 0x0) { /* @if(*dm->band_type == ODM_BAND_2_4G) */
		fa_t->cnt_all = fa_t->cnt_ofdm_fail + fa_t->cnt_cck_fail;
		fa_t->cnt_cca_all = fa_t->cnt_cck_cca + fa_t->cnt_ofdm_cca;
	} else {
		fa_t->cnt_all = fa_t->cnt_ofdm_fail;
		fa_t->cnt_cca_all = fa_t->cnt_ofdm_cca;
	}
}

#endif

void phydm_write_dig_reg_c50(void *dm_void, u8 igi)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	PHYDM_DBG(dm, DBG_DIG, "%s===>\n", __func__);

	odm_set_bb_reg(dm, ODM_REG(IGI_A, dm), ODM_BIT(IGI, dm), igi);
	odm_set_bb_reg(dm, ODM_REG(IGI_B, dm), ODM_BIT(IGI, dm), igi);
}

void phydm_write_dig_reg(void *dm_void, u8 igi)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_dig_struct *dig_t = &dm->dm_dig_table;
	u8 rf_gain = 0;

	PHYDM_DBG(dm, DBG_DIG, "%s===>\n", __func__);

	phydm_write_dig_reg_jgr3(dm, igi);

	if (igi == dig_t->cur_ig_value)
		dig_t->igi_trend = DIG_STABLE;
	else if (igi > dig_t->cur_ig_value)
		dig_t->igi_trend = DIG_INCREASING;
	else
		dig_t->igi_trend = DIG_DECREASING;

	PHYDM_DBG(dm, DBG_DIG, "Update IGI:0x%x -> 0x%x\n",
		  dig_t->cur_ig_value, igi);

	dig_t->cur_ig_value = igi;
}

void odm_write_dig(void *dm_void, u8 new_igi)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_dig_struct *dig_t = &dm->dm_dig_table;
	struct phydm_adaptivity_struct *adaptivity = &dm->adaptivity;

	PHYDM_DBG(dm, DBG_DIG, "%s===>\n", __func__);

	/* @1 Check IGI by upper bound */
	if (adaptivity->igi_lmt_en &&
	    new_igi > adaptivity->adapt_igi_up && dm->is_linked) {
		new_igi = adaptivity->adapt_igi_up;

		PHYDM_DBG(dm, DBG_DIG, "Force Adaptivity Up-bound=((0x%x))\n",
			  new_igi);
	}

	if (dig_t->cur_ig_value != new_igi) {
		/*@Add by YuChen for USB IO too slow issue*/
		if (*dm->edcca_mode == PHYDM_EDCCA_ADAPT_MODE) {
			if (new_igi < dig_t->cur_ig_value ||
			    dm->is_pause_dig) {
				dig_t->cur_ig_value = new_igi;
				phydm_adaptivity(dm);
			}
		}
		phydm_write_dig_reg(dm, new_igi);
	} else {
		dig_t->igi_trend = DIG_STABLE;
	}

	PHYDM_DBG(dm, DBG_DIG, "[%s]New_igi=((0x%x))\n\n",
		  ((dig_t->igi_trend == DIG_STABLE) ? "=" :
		  ((dig_t->igi_trend == DIG_INCREASING) ? "+" : "-")),
		  new_igi);
}

u8 phydm_get_igi_reg_val(void *dm_void, enum bb_path path)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 val = 0;
	u32 bit_map = ODM_BIT(IGI, dm);

	switch (path) {
	case BB_PATH_A:
		val = odm_get_bb_reg(dm, ODM_REG(IGI_A, dm), bit_map);
		break;
	#if (defined(PHYDM_COMPILE_ABOVE_2SS))
	case BB_PATH_B:
		val = odm_get_bb_reg(dm, ODM_REG(IGI_B, dm), bit_map);
		break;
	#endif

	#if (defined(PHYDM_COMPILE_ABOVE_3SS))
	case BB_PATH_C:
		val = odm_get_bb_reg(dm, ODM_REG(IGI_C, dm), bit_map);
		break;
	#endif

	#if (defined(PHYDM_COMPILE_ABOVE_4SS))
	case BB_PATH_D:
		val = odm_get_bb_reg(dm, ODM_REG(IGI_D, dm), bit_map);
		break;
	#endif

	default:
		break;
	}

	return (u8)val;
}

u8 phydm_get_igi(void *dm_void, enum bb_path path)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u8 val = 0;

	val = phydm_get_igi_reg_val_jgr3(dm, path);

	return val;
}

boolean
phydm_dig_abort(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	/* support_ability */
	if ((!(dm->support_ability & ODM_BB_FA_CNT)) ||
	    (!(dm->support_ability & ODM_BB_DIG))) {
		PHYDM_DBG(dm, DBG_DIG, "[DIG] Not Support\n");
		return true;
	}

	if (dm->pause_ability & ODM_BB_DIG) {
		return true;
	}

	if (*dm->is_scan_in_process) {
		PHYDM_DBG(dm, DBG_DIG, "Return: Scan in process\n");
		return true;
	}

	if (dm->dm_dig_table.fw_dig_enable) {
		PHYDM_DBG(dm, DBG_DIG, "Return: FW DIG enable\n");
		return true;
	}

	return false;
}

#ifdef PHYDM_HW_IGI
#ifdef BB_RAM_SUPPORT
void phydm_rd_hwigi_pre_setting(void *dm_void, u32 *_used, char *output,
				u32 *_out_len)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 used = *_used;
	u32 out_len = *_out_len;
	u8 igi_ofst = 0x0;
	u32 t1, t2, t3 = 0x0;

	igi_ofst = (u8)odm_get_bb_reg(dm, R_0x1e80, MASKBYTE0);
	t1 = odm_get_bb_reg(dm, R_0x1e80, MASKBYTE1) * 400;
	t2 = odm_get_bb_reg(dm, R_0x1e80, MASKBYTE2) * 400;
	t3 = odm_get_bb_reg(dm, R_0x1e80, MASKBYTE3) * 400;

	PDM_SNPF(out_len, used, output + used, out_len - used,
		 "igi_offset:0x%x, t1:%d(ns), t2:%d(ns), t3:%d(ns)\n",
		 igi_ofst, t1, t2, t3);
}

void phydm_set_hwigi_pre_setting(void *dm_void, u8 igi_ofst, u8 t1, u8 t2,
				 u8 t3)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 reg_0x1e80 = 0;

	reg_0x1e80 = igi_ofst + (t1 << 8) + (t2 << 16) + (t3 << 24);
	odm_set_bb_reg(dm, R_0x1e80, MASKDWORD, reg_0x1e80);
}

void phydm_rd_hwigi_table(void *dm_void, u8 macid, u32 *_used, char *output,
			  u32 *_out_len)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 used = *_used;
	u32 out_len = *_out_len;
	boolean hwigi_en = false;
	u8 hwigi = 0x0;
	u8 hwigi_rx_offset = 0x0;
	u32 reg_0x1e84 = 0x0;

	reg_0x1e84 |= (macid & 0x3f) << 24; /*macid*/
	reg_0x1e84 |= BIT(31); /*read_en*/
	odm_set_bb_reg(dm, R_0x1e84, MASKDWORD, reg_0x1e84);

	hwigi_en = (boolean)odm_get_bb_reg(dm, R_0x2de8, BIT(15));
	hwigi = (u8)odm_get_bb_reg(dm, R_0x2de8, 0x7f00);
	odm_set_bb_reg(dm, R_0x1e84, MASKDWORD, 0x0); /* disable rd/wt*/

	PDM_SNPF(out_len, used, output + used, out_len - used,
		 "(macid:%d) hwigi_en:%d, hwigi:0x%x\n", macid, hwigi_en,
		 hwigi);

	*_used = used;
	*_out_len = out_len;
}

void phydm_wt_hwigi_table(void *dm_void, u8 macid, boolean hwigi_en, u8 hwigi)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_bb_ram_per_sta *dm_ram_per_sta = NULL;
	u32 reg_0x1e84 = 0;

	if (macid > 63)
		macid = 63;

	dm_ram_per_sta = &dm->p_bb_ram_ctrl.pram_sta_ctrl[macid];
	dm_ram_per_sta->hw_igi_en = hwigi_en;
	dm_ram_per_sta->hw_igi = hwigi;

	reg_0x1e84 = (dm_ram_per_sta->tx_pwr_offset0_en << 15) +
		     ((dm_ram_per_sta->tx_pwr_offset0 & 0x7f) << 8) +
		     (dm_ram_per_sta->tx_pwr_offset1_en << 23) +
		     ((dm_ram_per_sta->tx_pwr_offset1 & 0x7f) << 16);

	reg_0x1e84 |= (hwigi_en << 7) + (hwigi & 0x7f);
	reg_0x1e84 |= (macid & 0x3f) << 24;/*macid*/
	reg_0x1e84 |= BIT(30); /*write_en*/
	odm_set_bb_reg(dm, R_0x1e84, MASKDWORD, reg_0x1e84);
	odm_set_bb_reg(dm, R_0x1e84, MASKDWORD, 0x80000000); /*read_en*/
	odm_set_bb_reg(dm, R_0x1e84, MASKDWORD, 0x0); /*disable rd/wt*/
}

void phydm_rst_hwigi(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_bb_ram_per_sta *dm_ram_per_sta = NULL;
	u32 reg_0x1e84 = 0;
	u8 i = 0;

	PHYDM_DBG(dm, DBG_DIG, "reset hwigi!\n");

	for (i = 0; i < 64; i++) {
		dm_ram_per_sta = &dm->p_bb_ram_ctrl.pram_sta_ctrl[i];
		dm_ram_per_sta->hw_igi_en = false;
		dm_ram_per_sta->hw_igi = 0x0;

		reg_0x1e84 = (dm_ram_per_sta->tx_pwr_offset0_en << 15) +
			     ((dm_ram_per_sta->tx_pwr_offset0 & 0x7f) << 8) +
			     (dm_ram_per_sta->tx_pwr_offset1_en << 23) +
			     ((dm_ram_per_sta->tx_pwr_offset1 & 0x7f) << 16);

		reg_0x1e84 |= (i & 0x3f) << 24;
		reg_0x1e84 |= BIT(30);
		odm_set_bb_reg(dm, R_0x1e84, MASKDWORD, reg_0x1e84);
	}

	odm_set_bb_reg(dm, R_0x1e84, MASKDWORD, 0x80000000);
	odm_set_bb_reg(dm, R_0x1e84, MASKDWORD, 0x0);
}

void phydm_hwigi(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct cmn_sta_info *sta = NULL;
	struct phydm_bb_ram_per_sta *dm_ram_per_sta = NULL;
	struct rssi_info *rssi = NULL;
	struct phydm_dig_struct *dig_t = &dm->dm_dig_table;
	struct phydm_bb_ram_ctrl *bb_ctrl = &dm->p_bb_ram_ctrl;
	u8 sta_cnt = 0;
	u8 i = 0;
	u8 hwigi = 0x0;
	u8 macid = 0;
	u8 macid_cnt = 0;
	u64 macid_cur = 0;
	u64 macid_diff = 0;
	u64 macid_mask = 0;

	if (!(bb_ctrl->hwigi_watchdog_en)) {
		return;
	}

	for (i = 0; i < ODM_ASSOCIATE_ENTRY_NUM; i++) {
		sta = dm->phydm_sta_info[i];
		if (is_sta_active(sta)) {
			sta_cnt++;

			if (sta->mac_id > 63)
				macid = 63;
			else
				macid = sta->mac_id;

			dm_ram_per_sta = &bb_ctrl->pram_sta_ctrl[macid];
			rssi = &sta->rssi_stat;
			macid_mask = (u64)BIT(sta->mac_id);
			bb_ctrl->hwigi_macid_is_linked |= macid_mask;
			macid_cur |= macid_mask;
			PHYDM_DBG(dm, DBG_DIG,
				  "STA_id=%d, MACID=%d, RSSI=%d, hwigi_en=%d, hwigi=0x%x\n",
				  i, sta->mac_id, rssi->rssi,
				  dm_ram_per_sta->hw_igi_en,
				  dm_ram_per_sta->hw_igi);

			hwigi = MAX_2((u8)(rssi->rssi + 10),
				      dig_t->cur_ig_value);

			if (hwigi > DIG_MAX_PERFORMANCE_MODE)
				hwigi = DIG_MAX_PERFORMANCE_MODE;
			else if (hwigi < DIG_MIN_PERFORMANCE)
				hwigi = DIG_MIN_PERFORMANCE;

			if (dm_ram_per_sta->hw_igi == hwigi) {
				PHYDM_DBG(dm, DBG_DIG,
					  "hwigi not change!\n");
			} else {

				PHYDM_DBG(dm, DBG_DIG,
					  "hwigi update: ((0x%x)) -> ((0x%x))\n",
					  dm_ram_per_sta->hw_igi, hwigi);

				phydm_wt_hwigi_table(dm, sta->mac_id, true, hwigi);
			}

			if (sta_cnt == dm->number_linked_client)
				break;
		}
	}
	macid_diff = bb_ctrl->hwigi_macid_is_linked ^ macid_cur;
	if (macid_diff)
		bb_ctrl->hwigi_macid_is_linked &= ~macid_diff;
	while (macid_diff) {
		if (macid_diff & 0x1)
			phydm_wt_hwigi_table(dm, macid_cnt, false, 0x0);
		macid_cnt++;
		macid_diff >>= 1;
	}
}

void phydm_hwigi_dbg(void *dm_void, char input[][16], u32 *_used,
		     char *output, u32 *_out_len)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_bb_ram_ctrl *bb_ctrl = &dm->p_bb_ram_ctrl;
	char help[] = "-h";
	u32 used = *_used;
	u32 out_len = *_out_len;
	u32 var1[7] = {0};
	u8 i = 0;

	if ((strcmp(input[1], help) == 0)) {
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "Disable/Enable watchdog : {0/1}\n");
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "Set hwigi pre-setting: {2} {IGI offset} {T1(after data tx)} {T2(after Rx)} {T3(after rsp tx)}\n");
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "Set hwigi table: {3} {en} {value} {macid}\n");
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "Read hwigi : {4} {macid(0~63), 255:all}\n");
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "Reset all hwigi : {5}\n");
	} else {
		for (i = 0; i < 7; i++) {
			if (input[i + 1])
				PHYDM_SSCANF(input[i + 1], DCMD_DECIMAL,
					     &var1[i]);
		}
		switch (var1[0]) {
		case 0:
		case 1:
			bb_ctrl->hwigi_watchdog_en = (var1[0]) ? true : false;
			break;
		case 2:
			phydm_set_hwigi_pre_setting(dm, (u8)var1[1],
						    (u8)var1[2], (u8)var1[3],
						    (u8)var1[4]);
			break;
		case 3:
			phydm_wt_hwigi_table(dm, (u8)var1[3], (boolean)var1[1],
					     (boolean)var1[2]);
			break;
		case 4:
			phydm_rd_hwigi_pre_setting(dm, &used, output, &out_len);
			if ((u8)var1[1] == 0xff)
				for (i = 0; i < 64; i++)
					phydm_rd_hwigi_table(dm, i, &used,
							     output, &out_len);
			else
				phydm_rd_hwigi_table(dm, (u8)var1[1], &used,
						     output, &out_len);
			break;
		case 5:
			phydm_rst_hwigi(dm);
			break;
		}
	}
	*_used = used;
	*_out_len = out_len;
}
#endif
#endif

void phydm_dig_init(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_dig_struct *dig_t = &dm->dm_dig_table;
#if (DM_ODM_SUPPORT_TYPE & (ODM_AP))
	struct phydm_fa_struct *false_alm_cnt = &dm->false_alm_cnt;
#endif
	struct phydm_bb_ram_ctrl *bb_ctrl = &dm->p_bb_ram_ctrl;
	u32 ret_value = 0;
	u8 i;

	dig_t->dm_dig_max = DIG_MAX_BALANCE_MODE;
	dig_t->dm_dig_min = DIG_MIN_PERFORMANCE;
	dig_t->dig_max_of_min = DIG_MAX_OF_MIN_BALANCE_MODE;

	dig_t->cur_ig_value = phydm_get_igi(dm, BB_PATH_A);

	dig_t->fa_th[0] = 250;
	dig_t->fa_th[1] = 500;
	dig_t->fa_th[2] = 750;
	dig_t->dm_dig_fa_th1 = DM_DIG_FA_TH1;
	dig_t->is_dbg_fa_th = false;
	dig_t->igi_dyn_up_hit = false;
	dig_t->fw_dig_enable = false;
	dig_t->fa_source = 0;

#if (DM_ODM_SUPPORT_TYPE & (ODM_AP))
	/* @For RTL8881A */
	false_alm_cnt->cnt_ofdm_fail_pre = 0;
#endif

	dig_t->rx_gain_range_max = DIG_MAX_BALANCE_MODE;
	dig_t->rx_gain_range_min = dig_t->cur_ig_value;

#ifdef PHYDM_TDMA_DIG_SUPPORT
	#if (DM_ODM_SUPPORT_TYPE & (ODM_WIN | ODM_CE))
		dm->original_dig_restore = true;
		dm->tdma_dig_state_number = DIG_NUM_OF_TDMA_STATES;
		dm->tdma_dig_timer_ms = DIG_TIMER_MS;
	#endif
	dig_t->tdma_force_l_igi = 0xff;
	dig_t->tdma_force_h_igi = 0xff;
#endif
#ifdef CFG_DIG_DAMPING_CHK
	phydm_dig_recorder_reset(dm);
	dig_t->dig_dl_en = 1;
#endif

#ifdef PHYDM_HW_IGI
	bb_ctrl->hwigi_watchdog_en = false;
#endif
}
void phydm_dig_abs_boundary_decision(struct dm_struct *dm, boolean is_dfs_band)
{
	struct phydm_dig_struct *dig_t = &dm->dm_dig_table;
	struct phydm_adaptivity_struct *adapt = &dm->adaptivity;

	if (is_dfs_band) {
		if (*dm->band_width == CHANNEL_WIDTH_20){
			dig_t->dm_dig_min = DIG_MIN_DFS;
		}
		else
			dig_t->dm_dig_min = DIG_MIN_DFS;

		dig_t->dig_max_of_min = DIG_MIN_DFS;
		dig_t->dm_dig_max = DIG_MAX_BALANCE_MODE;
	} else if (!dm->is_linked) {
		dig_t->dm_dig_max = DIG_MAX_COVERAGR;
		dig_t->dm_dig_min = DIG_MIN_COVERAGE;
	} else {
		if (*dm->bb_op_mode == PHYDM_BALANCE_MODE) {
		/*service > 2 devices*/
			dig_t->dm_dig_max = DIG_MAX_BALANCE_MODE;
			#if (DIG_HW == 1)
			dig_t->dig_max_of_min = DIG_MIN_COVERAGE;
			#else
			dig_t->dig_max_of_min = DIG_MAX_OF_MIN_BALANCE_MODE;
			#endif
		} else if (*dm->bb_op_mode == PHYDM_PERFORMANCE_MODE) {
		/*service 1 devices*/
			dig_t->dm_dig_max = DIG_MAX_PERFORMANCE_MODE;
			dig_t->dig_max_of_min = DIG_MAX_OF_MIN_PERFORMANCE_MODE;
		}

		dig_t->dm_dig_min = DIG_MIN_PERFORMANCE;
	}

	PHYDM_DBG(dm, DBG_DIG, "Abs{Max, Min}={0x%x, 0x%x}, Max_of_min=0x%x\n",
		  dig_t->dm_dig_max, dig_t->dm_dig_min, dig_t->dig_max_of_min);
}

void phydm_dig_dym_boundary_decision(struct dm_struct *dm, boolean is_dfs_band)
{
	struct phydm_dig_struct *dig_t = &dm->dm_dig_table;
#ifdef CFG_DIG_DAMPING_CHK
	struct phydm_dig_recorder_strcut *dig_rc = &dig_t->dig_recorder_t;
#endif
	u8 offset = 15, tmp_max = 0;
	u8 max_of_rssi_min = 0;

	PHYDM_DBG(dm, DBG_DIG, "%s ======>\n", __func__);

	if (!dm->is_linked) {
		/*@if no link, always stay at lower bound*/
		dig_t->rx_gain_range_max = dig_t->dig_max_of_min;
		dig_t->rx_gain_range_min = dig_t->dm_dig_min;

		PHYDM_DBG(dm, DBG_DIG, "No-Link, Dyn{Max, Min}={0x%x, 0x%x}\n",
			  dig_t->rx_gain_range_max, dig_t->rx_gain_range_min);
		return;
	}

	PHYDM_DBG(dm, DBG_DIG, "rssi_min=%d, ofst=%d\n", dm->rssi_min, offset);

	/* @DIG lower bound */
	if (is_dfs_band)
		dig_t->rx_gain_range_min = dig_t->dm_dig_min;
	else if (dm->rssi_min > dig_t->dig_max_of_min)
		dig_t->rx_gain_range_min = dig_t->dig_max_of_min;
	else if (dm->rssi_min < dig_t->dm_dig_min)
		dig_t->rx_gain_range_min = dig_t->dm_dig_min;
	else
		dig_t->rx_gain_range_min = dm->rssi_min;

#ifdef CFG_DIG_DAMPING_CHK
	/*@Limit Dyn min by damping*/
	if (dig_t->dig_dl_en &&
	    dig_rc->damping_limit_en &&
	    dig_t->rx_gain_range_min < dig_rc->damping_limit_val) {
		PHYDM_DBG(dm, DBG_DIG,
			  "[Limit by Damping] Dig_dyn_min=0x%x -> 0x%x\n",
			  dig_t->rx_gain_range_min, dig_rc->damping_limit_val);

		dig_t->rx_gain_range_min = dig_rc->damping_limit_val;
	}
#endif

	/* @DIG upper bound */
	tmp_max = dig_t->rx_gain_range_min + offset;
	if (dig_t->rx_gain_range_min != dm->rssi_min) {
		max_of_rssi_min = dm->rssi_min + offset;
		if (tmp_max > max_of_rssi_min)
			tmp_max = max_of_rssi_min;
	}

	if (tmp_max > dig_t->dm_dig_max)
		dig_t->rx_gain_range_max = dig_t->dm_dig_max;
	else if (tmp_max < dig_t->dm_dig_min)
		dig_t->rx_gain_range_max = dig_t->dm_dig_min;
	else
		dig_t->rx_gain_range_max = tmp_max;

	#ifdef CONFIG_PHYDM_ANTENNA_DIVERSITY
	/* @1 Force Lower Bound for AntDiv */
	if (!dm->is_one_entry_only &&
	    (dm->support_ability & ODM_BB_ANT_DIV) &&
	    (dm->ant_div_type == CG_TRX_HW_ANTDIV ||
	     dm->ant_div_type == CG_TRX_SMART_ANTDIV)) {
		if (dig_t->ant_div_rssi_max > dig_t->dig_max_of_min)
			dig_t->rx_gain_range_min = dig_t->dig_max_of_min;
		else
			dig_t->rx_gain_range_min = (u8)dig_t->ant_div_rssi_max;

		PHYDM_DBG(dm, DBG_DIG, "Force Dyn-Min=0x%x, RSSI_max=0x%x\n",
			  dig_t->rx_gain_range_min, dig_t->ant_div_rssi_max);
	}
	#endif

	PHYDM_DBG(dm, DBG_DIG, "Dyn{Max, Min}={0x%x, 0x%x}\n",
		  dig_t->rx_gain_range_max, dig_t->rx_gain_range_min);
}

void phydm_dig_abnormal_case(struct dm_struct *dm)
{
	struct phydm_dig_struct *dig_t = &dm->dm_dig_table;

	/* @Abnormal lower bound case */
	if (dig_t->rx_gain_range_min > dig_t->rx_gain_range_max)
		dig_t->rx_gain_range_min = dig_t->rx_gain_range_max;

	PHYDM_DBG(dm, DBG_DIG, "Abnoraml checked {Max, Min}={0x%x, 0x%x}\n",
		  dig_t->rx_gain_range_max, dig_t->rx_gain_range_min);
}

u8 phydm_new_igi_by_fa(struct dm_struct *dm, u8 igi, u32 fa_metrics,
		       u8 *step_size)
{
	struct phydm_dig_struct *dig_t = &dm->dm_dig_table;

	if (fa_metrics > dig_t->fa_th[2])
		igi = igi + step_size[0];
	else if (fa_metrics > dig_t->fa_th[1])
		igi = igi + step_size[1];
	else if (fa_metrics < dig_t->fa_th[0])
		igi = igi - step_size[2];

	return igi;
}

u8 phydm_get_new_igi(struct dm_struct *dm, u8 igi, u32 fa_metrics,
		     boolean is_dfs_band)
{
	struct phydm_dig_struct *dig_t = &dm->dm_dig_table;
	u8 step[3] = {0};

	if (dm->is_linked) {
		if (dm->pre_rssi_min <= dm->rssi_min) {
			PHYDM_DBG(dm, DBG_DIG, "pre_rssi_min <= rssi_min\n");
			step[0] = 2;
			step[1] = 1;
			step[2] = 2;
		} else {
			step[0] = 4;
			step[1] = 2;
			step[2] = 2;
		}
	} else {
		step[0] = 2;
		step[1] = 1;
		step[2] = 2;
	}

	PHYDM_DBG(dm, DBG_DIG, "step = {-%d, +%d, +%d}\n", step[2], step[1],
		  step[0]);

	if (dm->first_connect) {
		if (is_dfs_band) {
			if (dm->rssi_min > DIG_MAX_DFS)
				igi = DIG_MAX_DFS;
			else
				igi = dm->rssi_min;
			PHYDM_DBG(dm, DBG_DIG, "DFS band:IgiMax=0x%x\n",
				  dig_t->rx_gain_range_max);
		} else {
			igi = dig_t->rx_gain_range_min;
		}

		PHYDM_DBG(dm, DBG_DIG, "First connect: foce IGI=0x%x\n", igi);
	} else if (dm->is_linked) {
		PHYDM_DBG(dm, DBG_DIG, "Adjust IGI @ linked\n");
		/* @4 Abnormal # beacon case */
		igi = phydm_new_igi_by_fa(dm, igi, fa_metrics, step);
	} else {
		/* @2 Before link */
		PHYDM_DBG(dm, DBG_DIG, "Adjust IGI before link\n");

		if (dm->first_disconnect) {
			igi = dig_t->dm_dig_min;
			PHYDM_DBG(dm, DBG_DIG,
				  "First disconnect:foce IGI to lower bound\n");
		} else {
			PHYDM_DBG(dm, DBG_DIG, "Pre_IGI=((0x%x)), FA=((%d))\n",
				  igi, fa_metrics);

			igi = phydm_new_igi_by_fa(dm, igi, fa_metrics, step);
		}
	}

	/*@Check IGI by dyn-upper/lower bound */
	if (igi < dig_t->rx_gain_range_min)
		igi = dig_t->rx_gain_range_min;

	if (igi >= dig_t->rx_gain_range_max) {
		igi = dig_t->rx_gain_range_max;
		dig_t->igi_dyn_up_hit = true;
	} else {
		dig_t->igi_dyn_up_hit = false;
	}
	PHYDM_DBG(dm, DBG_DIG, "igi_dyn_up_hit=%d\n",
		  dig_t->igi_dyn_up_hit);

	PHYDM_DBG(dm, DBG_DIG, "fa_metrics = %d, IGI: 0x%x -> 0x%x\n",
		  fa_metrics, dig_t->cur_ig_value, igi);

	return igi;
}

boolean phydm_dig_dfs_mode_en(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	boolean dfs_mode_en = false;

	/* @Modify lower bound for DFS band */
	if (dm->is_dfs_band) {
		#if (DM_ODM_SUPPORT_TYPE & (ODM_AP))
		dfs_mode_en = true;
		#else
		if (phydm_dfs_master_enabled(dm))
			dfs_mode_en = true;
		#endif
		PHYDM_DBG(dm, DBG_DIG, "In DFS band\n");
	}
	return dfs_mode_en;
}

void phydm_dig_fa_source(void *dm_void, u8 fa_source, u32 *fa_metrics)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_fa_struct *fa = &dm->false_alm_cnt;

	switch (fa_source) {
		case 1:
			*fa_metrics = fa->time_fa_exp;
			break;
		#ifdef IFS_CLM_SUPPORT
		case 2:
			if (fa->time_fa_ifs_clm) {
				*fa_metrics = fa->time_fa_ifs_clm;
			} else {
				fa_source = 1;
				*fa_metrics = fa->time_fa_exp;
			}
			break;
		#endif
		#ifdef FAHM_SUPPORT
		case 3:
			if (fa->time_fa_fahm) {
				*fa_metrics = fa->time_fa_fahm;
			} else {
				fa_source = 1;
				*fa_metrics = fa->time_fa_exp;
			}
			break;
		#endif
		default:
			break;
	}

	PHYDM_DBG(dm, DBG_DIG,
		  "fa_source:%d, fa_cnt=%d ,time_fa_exp=%d, fa_metrics=%d\n",
		  fa_source, fa->cnt_all, fa->time_fa_exp, *fa_metrics);
}

void phydm_get_dig_coverage(void *dm_void, u8 *max, u8 *min)
{
	*min = DIG_MIN_COVERAGE;
	*max = DIG_MAX_PERFORMANCE_MODE;
}

u8 phydm_get_igi_for_target_pin_scan(void *dm_void, u8 rssi)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u8 igi = 0;
	u8 max = 0;
	u8 min = 0;

	igi = rssi + 10;

	phydm_get_dig_coverage(dm, &max, &min);

	if (igi > max)
		igi = max;
	else if (igi < min)
		igi = min;

	return igi;
}

/* @3============================================================
 * 3 FASLE ALARM CHECK
 * 3============================================================
 */
void phydm_false_alarm_counter_reg_reset(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_fa_struct *falm_cnt = &dm->false_alm_cnt;
#ifdef PHYDM_TDMA_DIG_SUPPORT
	struct phydm_dig_struct *dig_t = &dm->dm_dig_table;
	struct phydm_fa_acc_struct *falm_cnt_acc = &dm->false_alm_cnt_acc;
#endif
	u32 false_alm_cnt = 0;

#ifdef PHYDM_TDMA_DIG_SUPPORT
	if (!(dm->original_dig_restore)) {
		if (dig_t->cur_ig_value_tdma == 0)
			dig_t->cur_ig_value_tdma = dig_t->cur_ig_value;

		false_alm_cnt = falm_cnt_acc->cnt_all_1sec;
	} else
#endif
	{
		false_alm_cnt = falm_cnt->cnt_all;
	}

	/* @reset CCK FA counter */
	odm_set_bb_reg(dm, R_0x1a2c, BIT(15) | BIT(14), 0);
	odm_set_bb_reg(dm, R_0x1a2c, BIT(15) | BIT(14), 2);

	/* @reset CCK CCA counter */
	odm_set_bb_reg(dm, R_0x1a2c, BIT(13) | BIT(12), 0);
	odm_set_bb_reg(dm, R_0x1a2c, BIT(13) | BIT(12), 2);
	/* @Disable common rx clk gating => WLANBB-1106*/
	odm_set_bb_reg(dm, R_0x1d2c, BIT(31), 0);
	/* @reset OFDM CCA counter, OFDM FA counter*/
	phydm_reset_bb_hw_cnt(dm);
	/* @Enable common rx clk gating => WLANBB-1106*/
	odm_set_bb_reg(dm, R_0x1d2c, BIT(31), 1);
}

void phydm_false_alarm_counter_reg_hold(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	/* @hold cck counter */
	odm_set_bb_reg(dm, R_0x1a2c, BIT(12), 1);
	odm_set_bb_reg(dm, R_0x1a2c, BIT(14), 1);
}

u32 phydm_get_edcca_report(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_fa_struct *fa_t = &dm->false_alm_cnt;
	u32 dbg_port = dm->adaptivity.adaptivity_dbg_port;
	u32 val = 0;

	val = odm_get_bb_reg(dm, R_0x2d38, BIT(24));

	return val;
}

void phydm_get_dbg_port_info(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_fa_struct *fa_t = &dm->false_alm_cnt;

	fa_t->dbg_port0 = odm_get_bb_reg(dm, R_0x2db4, MASKDWORD);

	fa_t->edcca_flag = (boolean)phydm_get_edcca_report(dm);

	PHYDM_DBG(dm, DBG_FA_CNT, "FA_Cnt: Dbg port 0x0 = 0x%x, EDCCA = %d\n",
		  fa_t->dbg_port0, fa_t->edcca_flag);
}

void phydm_set_crc32_cnt2_rate(void *dm_void, u8 rate_idx)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_fa_struct *fa_t = &dm->false_alm_cnt;
	boolean is_ofdm_rate = phydm_is_ofdm_rate(dm, rate_idx);
	boolean is_ht_rate = phydm_is_ht_rate(dm, rate_idx);
	boolean is_vht_rate = phydm_is_vht_rate(dm, rate_idx);
	u32 reg_addr = 0x0;
	u32 ofdm_rate_bitmask = 0x0;
	u32 ht_mcs_bitmask = 0x0;
	u32 vht_mcs_bitmask = 0x0;
	u32 vht_ss_bitmask = 0x0;
	u8 rate = 0x0;
	u8 ss = 0x0;

	if (!is_ofdm_rate && !is_ht_rate && !is_vht_rate)
		PHYDM_DBG(dm, DBG_FA_CNT,
			  "[FA CNT] rate_idx = (0x%x) is not supported !\n",
			  rate_idx);

	switch (dm->ic_ip_series) {
	case PHYDM_IC_N:
		reg_addr = R_0xf04;
		ofdm_rate_bitmask = 0x0000f000;
		ht_mcs_bitmask = 0x007f0000;
		break;
	case PHYDM_IC_AC:
		reg_addr = R_0xb04;
		ofdm_rate_bitmask = 0x0000f000;
		ht_mcs_bitmask = 0x007f0000;
		vht_mcs_bitmask = 0x0f000000;
		vht_ss_bitmask = 0x30000000;
		break;
	case PHYDM_IC_JGR3:
		reg_addr = R_0x1eb8;
		ofdm_rate_bitmask = 0x00000f00;
		ht_mcs_bitmask = 0x007f0000;
		vht_mcs_bitmask = 0x0000f000;
		vht_ss_bitmask = 0x000000c0;
		break;
	default:
		break;
	}

	if (is_ofdm_rate) {
		rate = phydm_legacy_rate_2_spec_rate(dm, rate_idx);

		odm_set_bb_reg(dm, reg_addr, ofdm_rate_bitmask, rate);
		fa_t->ofdm2_rate_idx = rate_idx;
	} else if (is_ht_rate) {
		rate = phydm_rate_2_rate_digit(dm, rate_idx);

		odm_set_bb_reg(dm, reg_addr, ht_mcs_bitmask, rate);
		fa_t->ht2_rate_idx = rate_idx;
	} else if (is_vht_rate) {
		rate = phydm_rate_2_rate_digit(dm, rate_idx);
		ss = phydm_rate_to_num_ss(dm, rate_idx);

		odm_set_bb_reg(dm, reg_addr, vht_mcs_bitmask, rate);
		odm_set_bb_reg(dm, reg_addr, vht_ss_bitmask, ss - 1);
		fa_t->vht2_rate_idx = rate_idx;
	}
}

void phydm_fa_cnt_cal_fa_duration(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct ccx_info *ccx = &dm->dm_ccx_info;
	struct phydm_fa_struct *fa_t = &dm->false_alm_cnt;
	u8 norm = 0; /*normalization*/
	boolean fahm_chk = false;

	fa_t->time_fa_all = fa_t->cnt_fast_fsync * 12 +
			    fa_t->cnt_sb_search_fail * 12 +
			    fa_t->cnt_parity_fail * 28 +
			    fa_t->cnt_rate_illegal * 28 +
			    fa_t->cnt_crc8_fail * 20 +
			    fa_t->cnt_crc8_fail_vhta * 28 +
			    fa_t->cnt_mcs_fail_vht * 36 +
			    fa_t->cnt_mcs_fail * 32 +
			    fa_t->cnt_cck_fail * 80;

	fa_t->time_fa_exp = fa_t->cnt_ofdm_fail * OFDM_FA_EXP_DURATION +
			    fa_t->cnt_cck_fail * CCK_FA_EXP_DURATION;

	fa_t->time_fa_ifs_clm = 0;
	fa_t->time_fa_fahm = 0;

	#ifdef IFS_CLM_SUPPORT
	if (ccx->ccx_watchdog_result & IFS_CLM_SUCCESS) {
		norm = (u8)PHYDM_DIV(PHYDM_WATCH_DOG_PERIOD * S_TO_US,
				     ccx->ifs_clm_period);
		fa_t->time_fa_ifs_clm = (ccx->ifs_clm_cckfa +
					ccx->ifs_clm_ofdmfa) * norm;
	}
	#endif

	#ifdef FAHM_SUPPORT
	if (ccx->ccx_watchdog_result & FAHM_SUCCESS) {
		if (fa_t->cnt_cck_fail) {
			if (ccx->fahm_inclu_cck)
				fahm_chk = true;
		} else {
			fahm_chk = true;
		}
	}

	if (fahm_chk) {
		norm = (u8)PHYDM_DIV(PHYDM_WATCH_DOG_PERIOD * S_TO_US,
				     ccx->fahm_period);
		fa_t->time_fa_fahm = ccx->fahm_result_sum * norm;
	}
	#endif
}

void phydm_false_alarm_counter_statistics(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_fa_struct *fa_t = &dm->false_alm_cnt;
	char dbg_buf[PHYDM_SNPRINT_SIZE] = {0};
	u32 tmp = 0;

	if (!(dm->support_ability & ODM_BB_FA_CNT))
		return;

	PHYDM_DBG(dm, DBG_FA_CNT, "%s======>\n", __func__);

	phydm_fa_cnt_statistics_jgr3(dm);

	phydm_get_dbg_port_info(dm);
	phydm_false_alarm_counter_reg_reset(dm_void);

	phydm_fa_cnt_cal_fa_duration(dm);

	fa_t->cnt_crc32_error_all = fa_t->cnt_vht_crc32_error +
				    fa_t->cnt_ht_crc32_error +
				    fa_t->cnt_ofdm_crc32_error +
				    fa_t->cnt_cck_crc32_error;

	fa_t->cnt_crc32_ok_all = fa_t->cnt_vht_crc32_ok +
				 fa_t->cnt_ht_crc32_ok +
				 fa_t->cnt_ofdm_crc32_ok +
				 fa_t->cnt_cck_crc32_ok;

	PHYDM_DBG(dm, DBG_FA_CNT,
		  "[Tx cnt] {CCK_TxEN, CCK_TxON, OFDM_TxEN, OFDM_TxON} = {%d, %d, %d, %d}\n",
		  fa_t->cnt_cck_txen, fa_t->cnt_cck_txon, fa_t->cnt_ofdm_txen,
		  fa_t->cnt_ofdm_txon);
	PHYDM_DBG(dm, DBG_FA_CNT,
		  "[CCA Cnt] {CCK, OFDM, Total} = {%d, %d, %d}\n",
		  fa_t->cnt_cck_cca, fa_t->cnt_ofdm_cca, fa_t->cnt_cca_all);
	PHYDM_DBG(dm, DBG_FA_CNT,
		  "[FA Cnt] {CCK, OFDM, Total} = {%d, %d, %d}\n",
		  fa_t->cnt_cck_fail, fa_t->cnt_ofdm_fail, fa_t->cnt_all);
	PHYDM_DBG(dm, DBG_FA_CNT,
		  "[FA duration(us)] {exp, ifs_clm, fahm} = {%d, %d, %d}\n",
		  fa_t->time_fa_exp, fa_t->time_fa_ifs_clm,
		  fa_t->time_fa_fahm);
	PHYDM_DBG(dm, DBG_FA_CNT,
		  "[OFDM FA] Parity=%d, Rate=%d, Fast_Fsync=%d, SBD=%d\n",
		  fa_t->cnt_parity_fail, fa_t->cnt_rate_illegal,
		  fa_t->cnt_fast_fsync, fa_t->cnt_sb_search_fail);
	PHYDM_DBG(dm, DBG_FA_CNT, "[HT FA] CRC8=%d, MCS=%d\n",
		  fa_t->cnt_crc8_fail, fa_t->cnt_mcs_fail);
	PHYDM_DBG(dm, DBG_FA_CNT,
		  "[VHT FA] SIGA_CRC8=%d, SIGB_CRC8=%d, MCS=%d\n",
		  fa_t->cnt_crc8_fail_vhta, fa_t->cnt_crc8_fail_vhtb,
		  fa_t->cnt_mcs_fail_vht);

	PHYDM_DBG(dm, DBG_FA_CNT,
		  "[CRC32 OK Cnt] {CCK, OFDM, HT, VHT, Total} = {%d, %d, %d, %d, %d}\n",
		  fa_t->cnt_cck_crc32_ok, fa_t->cnt_ofdm_crc32_ok,
		  fa_t->cnt_ht_crc32_ok, fa_t->cnt_vht_crc32_ok,
		  fa_t->cnt_crc32_ok_all);
	PHYDM_DBG(dm, DBG_FA_CNT,
		  "[CRC32 Err Cnt] {CCK, OFDM, HT, VHT, Total} = {%d, %d, %d, %d, %d}\n",
		  fa_t->cnt_cck_crc32_error, fa_t->cnt_ofdm_crc32_error,
		  fa_t->cnt_ht_crc32_error, fa_t->cnt_vht_crc32_error,
		  fa_t->cnt_crc32_error_all);

	if (fa_t->ofdm2_rate_idx) {
		tmp = fa_t->cnt_ofdm2_crc32_error + fa_t->cnt_ofdm2_crc32_ok;
		fa_t->ofdm2_pcr = (u8)PHYDM_DIV(fa_t->cnt_ofdm2_crc32_ok * 100,
						tmp);
		phydm_print_rate_2_buff(dm, fa_t->ofdm2_rate_idx, dbg_buf,
					PHYDM_SNPRINT_SIZE);
		PHYDM_DBG(dm, DBG_FA_CNT,
			  "[OFDM:%s CRC32 Cnt] {error, ok}= {%d, %d} (%d percent)\n",
			  dbg_buf, fa_t->cnt_ofdm2_crc32_error,
			  fa_t->cnt_ofdm2_crc32_ok, fa_t->ofdm2_pcr);
	} else {
		phydm_set_crc32_cnt2_rate(dm, ODM_RATE6M);
	}

	if (fa_t->ht2_rate_idx) {
		tmp = fa_t->cnt_ht2_crc32_error + fa_t->cnt_ht2_crc32_ok;
		fa_t->ht2_pcr = (u8)PHYDM_DIV(fa_t->cnt_ht2_crc32_ok * 100,
					      tmp);
		phydm_print_rate_2_buff(dm, fa_t->ht2_rate_idx, dbg_buf,
					PHYDM_SNPRINT_SIZE);
		PHYDM_DBG(dm, DBG_FA_CNT,
			  "[HT:%s CRC32 Cnt] {error, ok}= {%d, %d} (%d percent)\n",
			  dbg_buf, fa_t->cnt_ht2_crc32_error,
			  fa_t->cnt_ht2_crc32_ok, fa_t->ht2_pcr);
	} else {
		phydm_set_crc32_cnt2_rate(dm, ODM_RATEMCS0);
	}

	if (fa_t->vht2_rate_idx) {
		tmp = fa_t->cnt_vht2_crc32_error +
		      fa_t->cnt_vht2_crc32_ok;
		fa_t->vht2_pcr = (u8)PHYDM_DIV(fa_t->cnt_vht2_crc32_ok *
					       100, tmp);
		phydm_print_rate_2_buff(dm, fa_t->vht2_rate_idx,
					dbg_buf, PHYDM_SNPRINT_SIZE);
		PHYDM_DBG(dm, DBG_FA_CNT,
			  "[VHT:%s CRC32 Cnt] {error, ok}= {%d, %d} (%d percent)\n",
			  dbg_buf, fa_t->cnt_vht2_crc32_error,
			  fa_t->cnt_vht2_crc32_ok, fa_t->vht2_pcr);
	}
}

void phydm_fill_fw_dig_info(void *dm_void, boolean *enable,
			    u8 *para4, u8 *para8) {
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_dig_struct *dig_t = &dm->dm_dig_table;

	dig_t->fw_dig_enable = *enable;
	para8[0] = dig_t->rx_gain_range_max;
	para8[1] = dig_t->rx_gain_range_min;
	para8[2] = dm->number_linked_client;
	para4[0] = (u8)DIG_LPS_MODE;
}

void phydm_crc32_cnt_dbg(void *dm_void, char input[][16], u32 *_used,
			 char *output, u32 *_out_len)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_dig_struct *dig_t = &dm->dm_dig_table;
	char help[] = "-h";
	u32 var1[10] = {0};
	u32 used = *_used;
	u32 out_len = *_out_len;
	u8 i = 0;
	u8 rate = 0x0;

	if ((strcmp(input[1], help) == 0)) {
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "[CRC32 Cnt] {rate_idx}\n");
	} else {
		PHYDM_SSCANF(input[1], DCMD_DECIMAL, &var1[0]);
		rate = (u8)var1[0];

		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "{rate}={0x%x}", rate);

		phydm_set_crc32_cnt2_rate(dm, rate);
	}
	*_used = used;
	*_out_len = out_len;
}

#ifdef PHYDM_TDMA_DIG_SUPPORT
void phydm_set_tdma_dig_timer(void *dm_void)
{
}

void phydm_tdma_dig_timer_check(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_dig_struct *dig_t = &dm->dm_dig_table;

	PHYDM_DBG(dm, DBG_DIG, "tdma_dig_cnt=%d, pre_tdma_dig_cnt=%d\n",
		  dig_t->tdma_dig_cnt, dig_t->pre_tdma_dig_cnt);

	if (dig_t->tdma_dig_cnt == 0 ||
	    dig_t->tdma_dig_cnt == dig_t->pre_tdma_dig_cnt) {
		if (dm->support_ability & ODM_BB_DIG) {
#ifdef IS_USE_NEW_TDMA
			PHYDM_DBG(dm, DBG_DIG,
				  "Check fail, Restart timer\n\n");
			phydm_false_alarm_counter_reset(dm);
			odm_set_timer(dm, &dm->tdma_dig_timer,
				      dm->tdma_dig_timer_ms);
#else
			/*@if interrupt mask info is got.*/
			/*Reg0xb0 is no longer needed*/
#if 0
			/*regb0 = odm_get_bb_reg(dm, R_0xb0, bMaskDWord);*/
#endif
			PHYDM_DBG(dm, DBG_DIG,
				  "Check fail, Mask[0]=0x%x, restart timer\n",
				  *dm->interrupt_mask);

			phydm_tdma_dig_add_interrupt_mask_handler(dm);
			phydm_enable_rx_related_interrupt_handler(dm);
			phydm_set_tdma_dig_timer(dm);
#endif
		}
	} else {
		PHYDM_DBG(dm, DBG_DIG, "Check pass, update pre_tdma_dig_cnt\n");
	}

	dig_t->pre_tdma_dig_cnt = dig_t->tdma_dig_cnt;
}

/*@different IC/team may use different timer for tdma-dig*/
void phydm_tdma_dig_add_interrupt_mask_handler(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

}

/*@============================================================*/
/*@FASLE ALARM CHECK*/
/*@============================================================*/
void phydm_tdma_false_alarm_counter_check(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_fa_struct *falm_cnt = &dm->false_alm_cnt;
	struct phydm_fa_acc_struct *falm_cnt_acc = &dm->false_alm_cnt_acc;
	struct phydm_dig_struct *dig_t = &dm->dm_dig_table;
	boolean rssi_dump_en = 0;
	u32 timestamp = 0;
	u8 tdma_dig_state_number = 0;
	u32 start_th = 0;

	if (dig_t->tdma_dig_state == 1)
		phydm_false_alarm_counter_reset(dm);
	/* Reset FalseAlarmCounterStatistics */
	/* @fa_acc_1sec_tsf = fa_acc_1sec_tsf, keep */
	/* @fa_end_tsf = fa_start_tsf = TSF */
	else {
		phydm_false_alarm_counter_statistics(dm);
		PHYDM_DBG(dm, DBG_DIG, "NOT 97F! NOT start\n");
		return;
	}
}

void phydm_false_alarm_counter_acc(void *dm_void, boolean rssi_dump_en)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_fa_struct *falm_cnt = &dm->false_alm_cnt;
	struct phydm_fa_acc_struct *falm_cnt_acc = &dm->false_alm_cnt_acc;
	struct phydm_dig_struct *dig_t = &dm->dm_dig_table;

	falm_cnt_acc->cnt_parity_fail += falm_cnt->cnt_parity_fail;
	falm_cnt_acc->cnt_rate_illegal += falm_cnt->cnt_rate_illegal;
	falm_cnt_acc->cnt_crc8_fail += falm_cnt->cnt_crc8_fail;
	falm_cnt_acc->cnt_mcs_fail += falm_cnt->cnt_mcs_fail;
	falm_cnt_acc->cnt_ofdm_fail += falm_cnt->cnt_ofdm_fail;
	falm_cnt_acc->cnt_cck_fail += falm_cnt->cnt_cck_fail;
	falm_cnt_acc->cnt_all += falm_cnt->cnt_all;
	falm_cnt_acc->cnt_fast_fsync += falm_cnt->cnt_fast_fsync;
	falm_cnt_acc->cnt_sb_search_fail += falm_cnt->cnt_sb_search_fail;
	falm_cnt_acc->cnt_ofdm_cca += falm_cnt->cnt_ofdm_cca;
	falm_cnt_acc->cnt_cck_cca += falm_cnt->cnt_cck_cca;
	falm_cnt_acc->cnt_cca_all += falm_cnt->cnt_cca_all;
	falm_cnt_acc->cnt_cck_crc32_error += falm_cnt->cnt_cck_crc32_error;
	falm_cnt_acc->cnt_cck_crc32_ok += falm_cnt->cnt_cck_crc32_ok;
	falm_cnt_acc->cnt_ofdm_crc32_error += falm_cnt->cnt_ofdm_crc32_error;
	falm_cnt_acc->cnt_ofdm_crc32_ok += falm_cnt->cnt_ofdm_crc32_ok;
	falm_cnt_acc->cnt_ht_crc32_error += falm_cnt->cnt_ht_crc32_error;
	falm_cnt_acc->cnt_ht_crc32_ok += falm_cnt->cnt_ht_crc32_ok;
	falm_cnt_acc->cnt_vht_crc32_error += falm_cnt->cnt_vht_crc32_error;
	falm_cnt_acc->cnt_vht_crc32_ok += falm_cnt->cnt_vht_crc32_ok;
	falm_cnt_acc->cnt_crc32_error_all += falm_cnt->cnt_crc32_error_all;
	falm_cnt_acc->cnt_crc32_ok_all += falm_cnt->cnt_crc32_ok_all;

	if (rssi_dump_en == 1) {
		falm_cnt_acc->cnt_all_1sec =
			falm_cnt_acc->cnt_all * dig_t->sec_factor;
		falm_cnt_acc->cnt_cca_all_1sec =
			falm_cnt_acc->cnt_cca_all * dig_t->sec_factor;
		falm_cnt_acc->cnt_cck_fail_1sec =
			falm_cnt_acc->cnt_cck_fail * dig_t->sec_factor;
	}
}

void phydm_false_alarm_counter_acc_reset(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_fa_acc_struct *falm_cnt_acc = NULL;

#ifdef IS_USE_NEW_TDMA
	struct phydm_fa_acc_struct *falm_cnt_acc_low = NULL;
	u32 tmp_cca_1sec = 0;
	u32 tmp_fa_1sec = 0;

	/*@clear L-fa_acc struct*/
	falm_cnt_acc_low = &dm->false_alm_cnt_acc_low;
	tmp_cca_1sec = falm_cnt_acc_low->cnt_cca_all_1sec;
	tmp_fa_1sec = falm_cnt_acc_low->cnt_all_1sec;
	odm_memory_set(dm, falm_cnt_acc_low, 0, sizeof(dm->false_alm_cnt_acc));
	falm_cnt_acc_low->cnt_cca_all_1sec = tmp_cca_1sec;
	falm_cnt_acc_low->cnt_all_1sec = tmp_fa_1sec;

	/*@clear H-fa_acc struct*/
	falm_cnt_acc = &dm->false_alm_cnt_acc;
	tmp_cca_1sec = falm_cnt_acc->cnt_cca_all_1sec;
	tmp_fa_1sec = falm_cnt_acc->cnt_all_1sec;
	odm_memory_set(dm, falm_cnt_acc, 0, sizeof(dm->false_alm_cnt_acc));
	falm_cnt_acc->cnt_cca_all_1sec = tmp_cca_1sec;
	falm_cnt_acc->cnt_all_1sec = tmp_fa_1sec;
#else
	falm_cnt_acc = &dm->false_alm_cnt_acc;
	/* @Cnt_all_for_rssi_dump & Cnt_CCA_all_for_rssi_dump */
	/* @do NOT need to be reset */
	odm_memory_set(dm, falm_cnt_acc, 0, sizeof(falm_cnt_acc));
#endif
}

void phydm_false_alarm_counter_reset(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_fa_struct *falm_cnt;
	struct phydm_dig_struct *dig_t;
	u32 timestamp;

	falm_cnt = &dm->false_alm_cnt;
	dig_t = &dm->dm_dig_table;

	memset(falm_cnt, 0, sizeof(dm->false_alm_cnt));
	phydm_false_alarm_counter_reg_reset(dm);

#ifdef IS_USE_NEW_TDMA
	return;
#endif
	if (dig_t->tdma_dig_state != 1)
		dig_t->fa_acc_1sec_timestamp = 0;
	else
		dig_t->fa_acc_1sec_timestamp = dig_t->fa_acc_1sec_timestamp;

	/*REG_FREERUN_CNT*/
	timestamp = odm_get_bb_reg(dm, R_0x568, bMaskDWord);
	dig_t->fa_start_timestamp = timestamp;
	dig_t->fa_end_timestamp = timestamp;
}

void phydm_tdma_dig_para_upd(void *dm_void, enum upd_type type, u8 input)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	switch (type) {
	case ENABLE_TDMA:
		dm->original_dig_restore = !((boolean)input);
		break;
	case MODE_DECISION:
		if (input == (u8)MODE_PERFORMANCE)
			dm->tdma_dig_state_number = DIG_NUM_OF_TDMA_STATES + 2;
		else if (input == (u8)MODE_COVERAGE)
			dm->tdma_dig_state_number = DIG_NUM_OF_TDMA_STATES;
		else
			dm->tdma_dig_state_number = DIG_NUM_OF_TDMA_STATES;
		break;
	}
}

#ifdef IS_USE_NEW_TDMA

u8 get_new_igi_bound(struct dm_struct *dm, u8 igi, u32 fa_cnt, u8 *rx_gain_max,
		     u8 *rx_gain_min, boolean is_dfs_band)
{
	struct phydm_dig_struct *dig_t = &dm->dm_dig_table;
	u8 step[3] = {0};
	u8 cur_igi = igi;

	if (dm->is_linked) {
		if (dm->pre_rssi_min <= dm->rssi_min) {
			PHYDM_DBG(dm, DBG_DIG, "pre_rssi_min <= rssi_min\n");
			step[0] = 2;
			step[1] = 1;
			step[2] = 2;
		} else {
			step[0] = 4;
			step[1] = 2;
			step[2] = 2;
		}
	} else {
		step[0] = 2;
		step[1] = 1;
		step[2] = 2;
	}

	PHYDM_DBG(dm, DBG_DIG, "step = {-%d, +%d, +%d}\n", step[2], step[1],
		  step[0]);

	if (dm->first_connect) {
		if (is_dfs_band) {
			if (dm->rssi_min > DIG_MAX_DFS)
				igi = DIG_MAX_DFS;
			else
				igi = dm->rssi_min;
			PHYDM_DBG(dm, DBG_DIG, "DFS band:IgiMax=0x%x\n",
				  *rx_gain_max);
		} else {
			igi = *rx_gain_min;
		}

		PHYDM_DBG(dm, DBG_DIG, "First connect: foce IGI=0x%x\n", igi);
	} else {
		/* @2 Before link */
		PHYDM_DBG(dm, DBG_DIG, "Adjust IGI before link\n");

		if (dm->first_disconnect) {
			igi = dig_t->dm_dig_min;
			PHYDM_DBG(dm, DBG_DIG,
				  "First disconnect:foce IGI to lower bound\n");
		} else {
			PHYDM_DBG(dm, DBG_DIG, "Pre_IGI=((0x%x)), FA=((%d))\n",
				  igi, fa_cnt);

			igi = phydm_new_igi_by_fa(dm, igi, fa_cnt, step);
		}
	}
	/*@Check IGI by dyn-upper/lower bound */
	if (igi < *rx_gain_min)
		igi = *rx_gain_min;

	if (igi > *rx_gain_max)
		igi = *rx_gain_max;

	PHYDM_DBG(dm, DBG_DIG, "fa_cnt = %d, IGI: 0x%x -> 0x%x\n",
		  fa_cnt, cur_igi, igi);

	return igi;
}

void phydm_write_tdma_dig(void *dm_void, u8 new_igi)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_dig_struct *dig_t = &dm->dm_dig_table;
	struct phydm_adaptivity_struct *adaptivity = &dm->adaptivity;

	PHYDM_DBG(dm, DBG_DIG, "%s===>\n", __func__);
#if 0
	/* @1 Check IGI by upper bound */
	if (adaptivity->igi_lmt_en &&
	    new_igi > adaptivity->adapt_igi_up && dm->is_linked) {
		new_igi = adaptivity->adapt_igi_up;

		PHYDM_DBG(dm, DBG_DIG, "Force Adaptivity Up-bound=((0x%x))\n",
			  new_igi);
	}
#endif
	phydm_write_dig_reg(dm, new_igi);

	PHYDM_DBG(dm, DBG_DIG, "New %s-IGI=((0x%x))\n",
		  (dig_t->tdma_dig_state == TDMA_DIG_LOW_STATE) ? "L" : "H",
		  new_igi);
}

/*@============================================================*/
/*@FASLE ALARM CHECK*/
/*@============================================================*/
void phydm_tdma_fa_cnt_chk(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_fa_struct *falm_cnt = &dm->false_alm_cnt;
	struct phydm_fa_acc_struct *fa_t_acc = &dm->false_alm_cnt_acc;
	struct phydm_fa_acc_struct *fa_t_acc_low = &dm->false_alm_cnt_acc_low;
	struct phydm_dig_struct *dig_t = &dm->dm_dig_table;
	boolean tdma_dig_block_1sec_flag = false;
	u32 timestamp = 0;
	u8 states_per_block = dm->tdma_dig_state_number;
	u8 cur_tdma_dig_state = 0;
	u32 start_th = 0;
	u8 state_diff = 0;
	u32 tdma_dig_block_period_ms = 0;
	u32 tdma_dig_block_cnt_thd = 0;
	u32 timestamp_diff = 0;

	/*@calculate duration of a tdma block*/
	tdma_dig_block_period_ms = dm->tdma_dig_timer_ms * states_per_block;

	/*@
	 *caution!ONE_SEC_MS must be divisible by tdma_dig_block_period_ms,
	 *or FA will be fewer.
	 */
	tdma_dig_block_cnt_thd = ONE_SEC_MS / tdma_dig_block_period_ms;

	/*@tdma_dig_state == 0, collect H-state FA, else, collect L-state FA*/
	if (dig_t->tdma_dig_state == TDMA_DIG_LOW_STATE)
		cur_tdma_dig_state = TDMA_DIG_LOW_STATE;
	else if (dig_t->tdma_dig_state >= TDMA_DIG_HIGH_STATE)
		cur_tdma_dig_state = TDMA_DIG_HIGH_STATE;
	/*@
	 *PHYDM_DBG(dm, DBG_DIG, "in state %d, dig count %d\n",
	 *	  cur_tdma_dig_state, dig_t->tdma_dig_cnt);
	 */
	if (cur_tdma_dig_state == 0) {
		/*@L-state indicates next block*/
		dig_t->tdma_dig_block_cnt++;

		/*@1sec dump check*/
		if (dig_t->tdma_dig_block_cnt >= tdma_dig_block_cnt_thd)
			tdma_dig_block_1sec_flag = true;

		/*@
		 *PHYDM_DBG(dm, DBG_DIG,"[L-state] tdma_dig_block_cnt=%d\n",
		 *	  dig_t->tdma_dig_block_cnt);
		 */

		/*@collect FA till this block end*/
		phydm_false_alarm_counter_statistics(dm);
		phydm_fa_cnt_acc(dm, tdma_dig_block_1sec_flag,
				 cur_tdma_dig_state);
		/*@1s L-FA collect end*/

		/*@1sec dump reached*/
		if (tdma_dig_block_1sec_flag) {
			/*@L-DIG*/
			phydm_noisy_detection(dm);
			#ifdef PHYDM_SUPPORT_CCKPD
			phydm_cck_pd_th(dm);
			#endif
			PHYDM_DBG(dm, DBG_DIG, "run tdma L-state dig ====>\n");
			phydm_tdma_low_dig(dm);
			PHYDM_DBG(dm, DBG_DIG, "\n\n");
		}
	} else if (cur_tdma_dig_state == 1) {
		/*@1sec dump check*/
		if (dig_t->tdma_dig_block_cnt >= tdma_dig_block_cnt_thd)
			tdma_dig_block_1sec_flag = true;

		/*@
		 *PHYDM_DBG(dm, DBG_DIG,"[H-state] tdma_dig_block_cnt=%d\n",
		 *	  dig_t->tdma_dig_block_cnt);
		 */

		/*@collect FA till this block end*/
		phydm_false_alarm_counter_statistics(dm);
		phydm_fa_cnt_acc(dm, tdma_dig_block_1sec_flag,
				 cur_tdma_dig_state);
		/*@1s H-FA collect end*/

		/*@1sec dump reached*/
		state_diff = dm->tdma_dig_state_number - dig_t->tdma_dig_state;
		if (tdma_dig_block_1sec_flag && state_diff == 1) {
			/*@H-DIG*/
			phydm_noisy_detection(dm);
			#ifdef PHYDM_SUPPORT_CCKPD
			phydm_cck_pd_th(dm);
			#endif
			PHYDM_DBG(dm, DBG_DIG, "run tdma H-state dig ====>\n");
			phydm_tdma_high_dig(dm);
			PHYDM_DBG(dm, DBG_DIG, "\n\n");
			PHYDM_DBG(dm, DBG_DIG, "1 sec reached, is_linked=%d\n",
				  dm->is_linked);
			PHYDM_DBG(dm, DBG_DIG, "1 sec L-CCA=%d, L-FA=%d\n",
				  fa_t_acc_low->cnt_cca_all_1sec,
				  fa_t_acc_low->cnt_all_1sec);
			PHYDM_DBG(dm, DBG_DIG, "1 sec H-CCA=%d, H-FA=%d\n",
				  fa_t_acc->cnt_cca_all_1sec,
				  fa_t_acc->cnt_all_1sec);
			PHYDM_DBG(dm, DBG_DIG,
				  "1 sec TOTAL-CCA=%d, TOTAL-FA=%d\n\n",
				  fa_t_acc->cnt_cca_all +
				  fa_t_acc_low->cnt_cca_all,
				  fa_t_acc->cnt_all + fa_t_acc_low->cnt_all);

			/*@Reset AccFalseAlarmCounterStatistics */
			phydm_false_alarm_counter_acc_reset(dm);
			dig_t->tdma_dig_block_cnt = 0;
		}
	}
	/*@Reset FalseAlarmCounterStatistics */
	phydm_false_alarm_counter_reset(dm);
}

void phydm_tdma_low_dig(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_dig_struct *dig_t = &dm->dm_dig_table;
	struct phydm_fa_struct *falm_cnt = &dm->false_alm_cnt;
	struct phydm_fa_acc_struct *falm_cnt_acc = &dm->false_alm_cnt_acc_low;
#ifdef CFG_DIG_DAMPING_CHK
	struct phydm_dig_recorder_strcut *dig_rc = &dig_t->dig_recorder_t;
#endif
	u8 igi = dig_t->cur_ig_value;
	u8 new_igi = 0x20;
	u8 tdma_l_igi = dig_t->low_ig_value;
	u8 tdma_l_dym_min = dig_t->tdma_rx_gain_min[TDMA_DIG_LOW_STATE];
	u8 tdma_l_dym_max = dig_t->tdma_rx_gain_max[TDMA_DIG_LOW_STATE];
	u32 fa_cnt = falm_cnt->cnt_all;
	boolean dfs_mode_en = false, is_performance = true;
	u8 rssi_min = dm->rssi_min;
	u8 igi_upper_rssi_min = 0;
	u8 offset = 15;

	if (!(dm->original_dig_restore)) {
		if (tdma_l_igi == 0)
			tdma_l_igi = igi;

		fa_cnt = falm_cnt_acc->cnt_all_1sec;
	}

	if (phydm_dig_abort(dm)) {
		dig_t->low_ig_value = phydm_get_igi(dm, BB_PATH_A);
		return;
	}

	/*@Mode Decision*/
	dfs_mode_en = false;
	is_performance = true;

	/* @Abs Boundary Decision*/
	dig_t->dm_dig_max = DIG_MAX_COVERAGR; //0x26
	dig_t->dm_dig_min = DIG_MIN_PERFORMANCE; //0x20
	dig_t->dig_max_of_min = DIG_MAX_OF_MIN_COVERAGE; //0x22

	if (dm->is_dfs_band) {
		if (*dm->band_width == CHANNEL_WIDTH_20){
			dig_t->dm_dig_min = DIG_MIN_DFS;
		}
		else
			dig_t->dm_dig_min = DIG_MIN_DFS;

	} else {
	}

	PHYDM_DBG(dm, DBG_DIG, "Abs{Max, Min}={0x%x, 0x%x}, Max_of_min=0x%x\n",
		  dig_t->dm_dig_max, dig_t->dm_dig_min, dig_t->dig_max_of_min);

	/* @Dyn Boundary by RSSI*/
	if (!dm->is_linked) {
		/*@if no link, always stay at lower bound*/
		tdma_l_dym_max = 0x26;
		tdma_l_dym_min = dig_t->dm_dig_min;

		PHYDM_DBG(dm, DBG_DIG, "No-Link, Dyn{Max, Min}={0x%x, 0x%x}\n",
			  tdma_l_dym_max, tdma_l_dym_min);
	} else {
		PHYDM_DBG(dm, DBG_DIG, "rssi_min=%d, ofst=%d\n",
			  dm->rssi_min, offset);

		/* @DIG lower bound in L-state*/
		tdma_l_dym_min = dig_t->dm_dig_min;
		if (dm->is_dfs_band)
			tdma_l_dym_min = DIG_MIN_DFS;
		/*@
		 *#ifdef CFG_DIG_DAMPING_CHK
		 *@Limit Dyn min by damping
		 *if (dig_t->dig_dl_en &&
		 *   dig_rc->damping_limit_en &&
		 *   tdma_l_dym_min < dig_rc->damping_limit_val) {
		 *	PHYDM_DBG(dm, DBG_DIG,
		 *		  "[Limit by Damping] dyn_min=0x%x -> 0x%x\n",
		 *		  tdma_l_dym_min, dig_rc->damping_limit_val);
		 *
		 *	tdma_l_dym_min = dig_rc->damping_limit_val;
		 *}
		 *#endif
		 */

		/*@DIG upper bound in L-state*/
		igi_upper_rssi_min = rssi_min + offset;
		if (igi_upper_rssi_min > dig_t->dm_dig_max)
			tdma_l_dym_max = dig_t->dm_dig_max;
		else if (igi_upper_rssi_min < dig_t->dm_dig_min)
			tdma_l_dym_max = dig_t->dm_dig_min;
		else
			tdma_l_dym_max = igi_upper_rssi_min;

		/* @1 Force Lower Bound for AntDiv */
		/*@
		 *if (!dm->is_one_entry_only &&
		 *(dm->support_ability & ODM_BB_ANT_DIV) &&
		 *(dm->ant_div_type == CG_TRX_HW_ANTDIV ||
		 *dm->ant_div_type == CG_TRX_SMART_ANTDIV)) {
		 *if (dig_t->ant_div_rssi_max > dig_t->dig_max_of_min)
		 *	dig_t->rx_gain_range_min = dig_t->dig_max_of_min;
		 *else
		 *	dig_t->rx_gain_range_min = (u8)dig_t->ant_div_rssi_max;
		 *
		 *PHYDM_DBG(dm, DBG_DIG, "Force Dyn-Min=0x%x, RSSI_max=0x%x\n",
		 *	  dig_t->rx_gain_range_min, dig_t->ant_div_rssi_max);
		 *}
		 */

		PHYDM_DBG(dm, DBG_DIG, "Dyn{Max, Min}={0x%x, 0x%x}\n",
			  tdma_l_dym_max, tdma_l_dym_min);
	}

	/*@Abnormal Case Check*/
	/*@Abnormal lower bound case*/
	if (tdma_l_dym_min > tdma_l_dym_max)
		tdma_l_dym_min = tdma_l_dym_max;

	PHYDM_DBG(dm, DBG_DIG,
		  "Abnoraml chk, force {Max, Min}={0x%x, 0x%x}\n",
		  tdma_l_dym_max, tdma_l_dym_min);

	/*@False Alarm Threshold Decision*/
	phydm_fa_threshold_check(dm, dfs_mode_en);

	/*@Adjust Initial Gain by False Alarm*/
	/*Select new IGI by FA */
	if (!(dm->original_dig_restore)) {
		tdma_l_igi = get_new_igi_bound(dm, tdma_l_igi, fa_cnt,
					       &tdma_l_dym_max,
					       &tdma_l_dym_min,
					       dfs_mode_en);
	} else {
		new_igi = phydm_get_new_igi(dm, igi, fa_cnt, dfs_mode_en);
	}

	/*Update status*/
	if (!(dm->original_dig_restore)) {
		if (dig_t->tdma_force_l_igi == 0xff)
			dig_t->low_ig_value = tdma_l_igi;
		else
			dig_t->low_ig_value = dig_t->tdma_force_l_igi;
		dig_t->tdma_rx_gain_min[TDMA_DIG_LOW_STATE] = tdma_l_dym_min;
		dig_t->tdma_rx_gain_max[TDMA_DIG_LOW_STATE] = tdma_l_dym_max;
	} else {
		odm_write_dig(dm, new_igi);
	}
}

void phydm_tdma_high_dig(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_dig_struct *dig_t = &dm->dm_dig_table;
	struct phydm_fa_struct *falm_cnt = &dm->false_alm_cnt;
	struct phydm_fa_acc_struct *falm_cnt_acc = &dm->false_alm_cnt_acc;
#ifdef CFG_DIG_DAMPING_CHK
	struct phydm_dig_recorder_strcut *dig_rc = &dig_t->dig_recorder_t;
#endif
	u8 igi = dig_t->cur_ig_value;
	u8 new_igi = 0x20;
	u8 tdma_h_igi = dig_t->cur_ig_value_tdma;
	u8 tdma_h_dym_min = dig_t->tdma_rx_gain_min[TDMA_DIG_HIGH_STATE];
	u8 tdma_h_dym_max = dig_t->tdma_rx_gain_max[TDMA_DIG_HIGH_STATE];
	u32 fa_cnt = falm_cnt->cnt_all;
	boolean dfs_mode_en = false, is_performance = true;
	u8 rssi_min = dm->rssi_min;
	u8 igi_upper_rssi_min = 0;
	u8 offset = 15;

	if (!(dm->original_dig_restore)) {
		if (tdma_h_igi == 0)
			tdma_h_igi = igi;

		fa_cnt = falm_cnt_acc->cnt_all_1sec;
	}

	if (phydm_dig_abort(dm)) {
		dig_t->cur_ig_value_tdma = phydm_get_igi(dm, BB_PATH_A);
		return;
	}

	/*@Mode Decision*/
	dfs_mode_en = false;
	is_performance = true;

	/*@Abs Boundary Decision*/
	dig_t->dig_max_of_min = DIG_MAX_OF_MIN_BALANCE_MODE; // 0x2a

	if (!dm->is_linked) {
		dig_t->dm_dig_max = DIG_MAX_COVERAGR;
		dig_t->dm_dig_min = DIG_MIN_PERFORMANCE; // 0x20
	} else if (dm->is_dfs_band) {
		if (*dm->band_width == CHANNEL_WIDTH_20){
			dig_t->dm_dig_min = DIG_MIN_DFS;
		}
		else
			dig_t->dm_dig_min = DIG_MIN_DFS;

		dig_t->dig_max_of_min = DIG_MAX_OF_MIN_BALANCE_MODE;
		dig_t->dm_dig_max = DIG_MAX_BALANCE_MODE;
	} else {
		if (*dm->bb_op_mode == PHYDM_BALANCE_MODE) {
		/*service > 2 devices*/
			dig_t->dm_dig_max = DIG_MAX_BALANCE_MODE;
			#if (DIG_HW == 1)
			dig_t->dig_max_of_min = DIG_MIN_COVERAGE;
			#else
			dig_t->dig_max_of_min = DIG_MAX_OF_MIN_BALANCE_MODE;
			#endif
		} else if (*dm->bb_op_mode == PHYDM_PERFORMANCE_MODE) {
		/*service 1 devices*/
			dig_t->dm_dig_max = DIG_MAX_PERFORMANCE_MODE;
			dig_t->dig_max_of_min = DIG_MAX_OF_MIN_PERFORMANCE_MODE;
		}

		dig_t->dm_dig_min = DIG_MIN_PERFORMANCE;
	}
	PHYDM_DBG(dm, DBG_DIG, "Abs{Max, Min}={0x%x, 0x%x}, Max_of_min=0x%x\n",
		  dig_t->dm_dig_max, dig_t->dm_dig_min, dig_t->dig_max_of_min);

	/*@Dyn Boundary by RSSI*/
	if (!dm->is_linked) {
		/*@if no link, always stay at lower bound*/
		tdma_h_dym_max = dig_t->dig_max_of_min;
		tdma_h_dym_min = dig_t->dm_dig_min;

		PHYDM_DBG(dm, DBG_DIG, "No-Link, Dyn{Max, Min}={0x%x, 0x%x}\n",
			  tdma_h_dym_max, tdma_h_dym_min);
	} else {
		PHYDM_DBG(dm, DBG_DIG, "rssi_min=%d, ofst=%d\n",
			  dm->rssi_min, offset);

		/* @DIG lower bound in H-state*/
		if (dm->is_dfs_band)
			tdma_h_dym_min = DIG_MIN_DFS;
		else if (rssi_min < dig_t->dm_dig_min)
			tdma_h_dym_min = dig_t->dm_dig_min;
		else
			tdma_h_dym_min = rssi_min; // turbo not considered yet

#ifdef CFG_DIG_DAMPING_CHK
		/*@Limit Dyn min by damping*/
		if (dig_t->dig_dl_en &&
		    dig_rc->damping_limit_en &&
		    tdma_h_dym_min < dig_rc->damping_limit_val) {
			PHYDM_DBG(dm, DBG_DIG,
				  "[Limit by Damping] dyn_min=0x%x -> 0x%x\n",
				  tdma_h_dym_min, dig_rc->damping_limit_val);

			tdma_h_dym_min = dig_rc->damping_limit_val;
		}
#endif

		/*@DIG upper bound in H-state*/
		igi_upper_rssi_min = rssi_min + offset;
		if (igi_upper_rssi_min > dig_t->dm_dig_max)
			tdma_h_dym_max = dig_t->dm_dig_max;
		else
			tdma_h_dym_max = igi_upper_rssi_min;

		/* @1 Force Lower Bound for AntDiv */
		/*@
		 *if (!dm->is_one_entry_only &&
		 *(dm->support_ability & ODM_BB_ANT_DIV) &&
		 *(dm->ant_div_type == CG_TRX_HW_ANTDIV ||
		 *dm->ant_div_type == CG_TRX_SMART_ANTDIV)) {
		 *	if (dig_t->ant_div_rssi_max > dig_t->dig_max_of_min)
		 *	dig_t->rx_gain_range_min = dig_t->dig_max_of_min;
		 *	else
		 *	dig_t->rx_gain_range_min = (u8)dig_t->ant_div_rssi_max;
		 */
		/*@
		 *PHYDM_DBG(dm, DBG_DIG, "Force Dyn-Min=0x%x, RSSI_max=0x%x\n",
		 *	  dig_t->rx_gain_range_min, dig_t->ant_div_rssi_max);
		 *}
		 */
		PHYDM_DBG(dm, DBG_DIG, "Dyn{Max, Min}={0x%x, 0x%x}\n",
			  tdma_h_dym_max, tdma_h_dym_min);
	}

	/*@Abnormal Case Check*/
	/*@Abnormal low higher bound case*/
	if (tdma_h_dym_max < dig_t->dm_dig_min)
		tdma_h_dym_max = dig_t->dm_dig_min;
	/*@Abnormal lower bound case*/
	if (tdma_h_dym_min > tdma_h_dym_max)
		tdma_h_dym_min = tdma_h_dym_max;

	PHYDM_DBG(dm, DBG_DIG, "Abnoraml chk, force {Max, Min}={0x%x, 0x%x}\n",
		  tdma_h_dym_max, tdma_h_dym_min);

	/*@False Alarm Threshold Decision*/
	phydm_fa_threshold_check(dm, dfs_mode_en);

	/*@Adjust Initial Gain by False Alarm*/
	/*Select new IGI by FA */
	if (!(dm->original_dig_restore)) {
		tdma_h_igi = get_new_igi_bound(dm, tdma_h_igi, fa_cnt,
					       &tdma_h_dym_max,
					       &tdma_h_dym_min,
					       dfs_mode_en);
	} else {
		new_igi = phydm_get_new_igi(dm, igi, fa_cnt, dfs_mode_en);
	}

	/*Update status*/
	if (!(dm->original_dig_restore)) {
		if (dig_t->tdma_force_h_igi == 0xff)
			dig_t->cur_ig_value_tdma = tdma_h_igi;
		else
			dig_t->cur_ig_value_tdma = dig_t->tdma_force_h_igi;
		dig_t->tdma_rx_gain_min[TDMA_DIG_HIGH_STATE] = tdma_h_dym_min;
		dig_t->tdma_rx_gain_max[TDMA_DIG_HIGH_STATE] = tdma_h_dym_max;
	} else {
		odm_write_dig(dm, new_igi);
	}
}

void phydm_fa_cnt_acc(void *dm_void, boolean tdma_dig_block_1sec_flag,
		      u8 cur_tdma_dig_state)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_fa_struct *falm_cnt = &dm->false_alm_cnt;
	struct phydm_fa_acc_struct *falm_cnt_acc = NULL;
	struct phydm_dig_struct *dig_t = &dm->dm_dig_table;
	u8 factor_num = 0;
	u8 factor_denum = 1;
	u8 total_state_number = 0;

	if (cur_tdma_dig_state == TDMA_DIG_LOW_STATE)
		falm_cnt_acc = &dm->false_alm_cnt_acc_low;
	else if (cur_tdma_dig_state == TDMA_DIG_HIGH_STATE)

		falm_cnt_acc = &dm->false_alm_cnt_acc;
	/*@
	 *PHYDM_DBG(dm, DBG_DIG,
	 *	  "[%s] ==> dig_state=%d, one_sec=%d\n", __func__,
	 *	  cur_tdma_dig_state, tdma_dig_block_1sec_flag);
	 */
	falm_cnt_acc->cnt_parity_fail += falm_cnt->cnt_parity_fail;
	falm_cnt_acc->cnt_rate_illegal += falm_cnt->cnt_rate_illegal;
	falm_cnt_acc->cnt_crc8_fail += falm_cnt->cnt_crc8_fail;
	falm_cnt_acc->cnt_mcs_fail += falm_cnt->cnt_mcs_fail;
	falm_cnt_acc->cnt_ofdm_fail += falm_cnt->cnt_ofdm_fail;
	falm_cnt_acc->cnt_cck_fail += falm_cnt->cnt_cck_fail;
	falm_cnt_acc->cnt_all += falm_cnt->cnt_all;
	falm_cnt_acc->cnt_fast_fsync += falm_cnt->cnt_fast_fsync;
	falm_cnt_acc->cnt_sb_search_fail += falm_cnt->cnt_sb_search_fail;
	falm_cnt_acc->cnt_ofdm_cca += falm_cnt->cnt_ofdm_cca;
	falm_cnt_acc->cnt_cck_cca += falm_cnt->cnt_cck_cca;
	falm_cnt_acc->cnt_cca_all += falm_cnt->cnt_cca_all;
	falm_cnt_acc->cnt_cck_crc32_error += falm_cnt->cnt_cck_crc32_error;
	falm_cnt_acc->cnt_cck_crc32_ok += falm_cnt->cnt_cck_crc32_ok;
	falm_cnt_acc->cnt_ofdm_crc32_error += falm_cnt->cnt_ofdm_crc32_error;
	falm_cnt_acc->cnt_ofdm_crc32_ok += falm_cnt->cnt_ofdm_crc32_ok;
	falm_cnt_acc->cnt_ht_crc32_error += falm_cnt->cnt_ht_crc32_error;
	falm_cnt_acc->cnt_ht_crc32_ok += falm_cnt->cnt_ht_crc32_ok;
	falm_cnt_acc->cnt_vht_crc32_error += falm_cnt->cnt_vht_crc32_error;
	falm_cnt_acc->cnt_vht_crc32_ok += falm_cnt->cnt_vht_crc32_ok;
	falm_cnt_acc->cnt_crc32_error_all += falm_cnt->cnt_crc32_error_all;
	falm_cnt_acc->cnt_crc32_ok_all += falm_cnt->cnt_crc32_ok_all;

	/*@
	 *PHYDM_DBG(dm, DBG_DIG,
	 *	"[CCA Cnt]     {CCK, OFDM, Total} = {%d, %d, %d}\n",
	 *	falm_cnt->cnt_cck_cca,
	 *	falm_cnt->cnt_ofdm_cca,
	 *	falm_cnt->cnt_cca_all);
	 *PHYDM_DBG(dm, DBG_DIG,
	 *	"[FA Cnt]      {CCK, OFDM, Total} = {%d, %d, %d}\n",
	 *	falm_cnt->cnt_cck_fail,
	 *	falm_cnt->cnt_ofdm_fail,
	 *	falm_cnt->cnt_all);
	 */
	if (tdma_dig_block_1sec_flag) {
		total_state_number = dm->tdma_dig_state_number;

		if (cur_tdma_dig_state == TDMA_DIG_HIGH_STATE) {
			factor_num = total_state_number;
			factor_denum = total_state_number - 1;
		} else if (cur_tdma_dig_state == TDMA_DIG_LOW_STATE) {
			factor_num = total_state_number;
			factor_denum = 1;
		}

		falm_cnt_acc->cnt_all_1sec =
			falm_cnt_acc->cnt_all * factor_num / factor_denum;
		falm_cnt_acc->cnt_cca_all_1sec =
			falm_cnt_acc->cnt_cca_all * factor_num / factor_denum;
		falm_cnt_acc->cnt_cck_fail_1sec =
			falm_cnt_acc->cnt_cck_fail * factor_num / factor_denum;

		PHYDM_DBG(dm, DBG_DIG,
			  "[ACC CCA Cnt] {CCK, OFDM, Total} = {%d, %d, %d}\n",
			  falm_cnt_acc->cnt_cck_cca,
			  falm_cnt_acc->cnt_ofdm_cca,
			  falm_cnt_acc->cnt_cca_all);
		PHYDM_DBG(dm, DBG_DIG,
			  "[ACC FA Cnt]  {CCK, OFDM, Total} = {%d, %d, %d}\n\n",
			  falm_cnt_acc->cnt_cck_fail,
			  falm_cnt_acc->cnt_ofdm_fail,
			  falm_cnt_acc->cnt_all);

	}
}
#endif /*@#ifdef IS_USE_NEW_TDMA*/
#endif /*@#ifdef PHYDM_TDMA_DIG_SUPPORT*/

void phydm_dig_debug(void *dm_void, char input[][16], u32 *_used, char *output,
		     u32 *_out_len)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_dig_struct *dig_t = &dm->dm_dig_table;
	char help[] = "-h";
	u32 var1[10] = {0};
	u32 used = *_used;
	u32 out_len = *_out_len;
	u8 i = 0;

	if ((strcmp(input[1], help) == 0)) {
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "{0} {en} fa_th[0] fa_th[1] fa_th[2]\n");
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "{1} {Damping Limit en}\n");
		#ifdef PHYDM_TDMA_DIG_SUPPORT
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "{2} {original_dig_restore = %d}\n",
			 dm->original_dig_restore);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "{3} {tdma_dig_timer_ms = %d}\n",
			 dm->tdma_dig_timer_ms);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "{4} {tdma_dig_state_number = %d}\n",
			 dm->tdma_dig_state_number);
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "{5} {0:L-state,1:H-state} {force IGI} (L,H)=(%2x,%2x)\n",
			 dig_t->tdma_force_l_igi, dig_t->tdma_force_h_igi);
		#endif
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "{6} {fw_dig_en}\n");
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "{7} FA source:{0:original/1:Experimental duration/2:IFS_CLM/3:FAHM}\n");
	} else {
		PHYDM_SSCANF(input[1], DCMD_DECIMAL, &var1[0]);

		for (i = 1; i < 10; i++)
			PHYDM_SSCANF(input[i + 1], DCMD_DECIMAL, &var1[i]);

		if (var1[0] == 0) {
			if (var1[1] == 1) {
				dig_t->is_dbg_fa_th = true;
				dig_t->fa_th[0] = (u32)var1[2];
				dig_t->fa_th[1] = (u32)var1[3];
				dig_t->fa_th[2] = (u32)var1[4];

				PDM_SNPF(out_len, used, output + used,
					 out_len - used,
					 "Set DIG fa_th[0:2]= {%d, %d, %d}\n",
					 dig_t->fa_th[0], dig_t->fa_th[1],
					 dig_t->fa_th[2]);
			} else {
				dig_t->is_dbg_fa_th = false;
			}
		#ifdef PHYDM_TDMA_DIG_SUPPORT
		} else if (var1[0] == 2) {
			dm->original_dig_restore = (u8)var1[1];
			if (dm->original_dig_restore == 1) {
				PDM_SNPF(out_len, used, output + used,
					 out_len - used, "Disable TDMA-DIG\n");
			} else {
				PDM_SNPF(out_len, used, output + used,
					 out_len - used, "Enable TDMA-DIG\n");
			}
		} else if (var1[0] == 3) {
			dm->tdma_dig_timer_ms = (u8)var1[1];
			PDM_SNPF(out_len, used, output + used,
				 out_len - used, "tdma_dig_timer_ms = %d\n",
				 dm->tdma_dig_timer_ms);
		} else if (var1[0] == 4) {
			dm->tdma_dig_state_number = (u8)var1[1];
			PDM_SNPF(out_len, used, output + used,
				 out_len - used, "tdma_dig_state_number = %d\n",
				 dm->tdma_dig_state_number);
		} else if (var1[0] == 5) {
			PHYDM_SSCANF(input[3], DCMD_HEX, &var1[2]);
			if (var1[1] == 0) {
				dig_t->tdma_force_l_igi = (u8)var1[2];
				PDM_SNPF(out_len, used, output + used,
					 out_len - used,
					 "force L-state IGI = %2x\n",
					 dig_t->tdma_force_l_igi);
			} else if (var1[1] == 1) {
				dig_t->tdma_force_h_igi = (u8)var1[2];
				PDM_SNPF(out_len, used, output + used,
					 out_len - used,
					 "force H-state IGI = %2x\n",
					 dig_t->tdma_force_h_igi);
			}
		#endif
		}

		#ifdef CFG_DIG_DAMPING_CHK
		else if (var1[0] == 1) {
			dig_t->dig_dl_en = (u8)var1[1];
			/*@*/
		}
		#endif
		else if (var1[0] == 6) {
			phydm_fw_dm_ctrl_en(dm, F00_DIG, (boolean)var1[1]);
			PDM_SNPF(out_len, used, output + used, out_len - used,
				 "fw_dig_enable = %2x\n", dig_t->fw_dig_enable);
		} else if (var1[0] == 7) {
			dig_t->fa_source = (u8)var1[1];
			PDM_SNPF(out_len, used, output + used, out_len - used,
				 "FA source = %d\n", dig_t->fa_source);
		}
	}
	*_used = used;
	*_out_len = out_len;
}

#ifdef CONFIG_MCC_DM
#if (RTL8822B_SUPPORT || RTL8822C_SUPPORT|| RTL8723F_SUPPORT)
void phydm_mcc_igi_clr(void *dm_void, u8 clr_port)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _phydm_mcc_dm_ *mcc_dm = &dm->mcc_dm;

	mcc_dm->mcc_rssi[clr_port] = 0xff;
	mcc_dm->mcc_dm_val[0][clr_port] = 0xff; /* 0xc50 clr */
	mcc_dm->mcc_dm_val[1][clr_port] = 0xff; /* 0xe50 clr */
}

void phydm_mcc_igi_chk(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _phydm_mcc_dm_ *mcc_dm = &dm->mcc_dm;

	if (mcc_dm->mcc_dm_val[0][0] == 0xff &&
	    mcc_dm->mcc_dm_val[0][1] == 0xff) {
		mcc_dm->mcc_dm_reg[0] = 0xffff;
		mcc_dm->mcc_reg_id[0] = 0xff;
	}
	if (mcc_dm->mcc_dm_val[1][0] == 0xff &&
	    mcc_dm->mcc_dm_val[1][1] == 0xff) {
		mcc_dm->mcc_dm_reg[1] = 0xffff;
		mcc_dm->mcc_reg_id[1] = 0xff;
	}
}

void phydm_mcc_igi_cal(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _phydm_mcc_dm_ *mcc_dm = &dm->mcc_dm;
	struct phydm_dig_struct *dig_t = &dm->dm_dig_table;
	u8	shift = 0;
	u8	igi_val0, igi_val1;

	if (mcc_dm->mcc_rssi[0] == 0xff)
		phydm_mcc_igi_clr(dm, 0);
	if (mcc_dm->mcc_rssi[1] == 0xff)
		phydm_mcc_igi_clr(dm, 1);
	phydm_mcc_igi_chk(dm);
	igi_val0 = mcc_dm->mcc_rssi[0] - shift;
	igi_val1 = mcc_dm->mcc_rssi[1] - shift;

	if (igi_val0 < DIG_MIN_PERFORMANCE)
		igi_val0 = DIG_MIN_PERFORMANCE;

	if (igi_val1 < DIG_MIN_PERFORMANCE)
		igi_val1 = DIG_MIN_PERFORMANCE;

	switch (dm->ic_ip_series) {
	#ifdef PHYDM_IC_JGR3_SERIES_SUPPORT
	case PHYDM_IC_JGR3:
		phydm_fill_mcccmd(dm, 0, R_0x1d70, igi_val0, igi_val1);
		phydm_fill_mcccmd(dm, 1, R_0x1d70 + 1, igi_val0, igi_val1);
		break;
	#endif
	default:
		phydm_fill_mcccmd(dm, 0, R_0xc50, igi_val0, igi_val1);
		phydm_fill_mcccmd(dm, 1, R_0xe50, igi_val0, igi_val1);
		break;
	}

	PHYDM_DBG(dm, DBG_COMP_MCC, "RSSI_min: %d %d, MCC_igi: %d %d\n",
		  mcc_dm->mcc_rssi[0], mcc_dm->mcc_rssi[1],
		  mcc_dm->mcc_dm_val[0][0], mcc_dm->mcc_dm_val[0][1]);
}
#endif /*#if (RTL8822B_SUPPORT)*/
#endif /*#ifdef CONFIG_MCC_DM*/
