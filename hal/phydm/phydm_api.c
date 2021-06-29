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

enum channel_width phydm_rxsc_2_bw(void *dm_void, u8 rxsc)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	enum channel_width bw = 0;

	/* @Check RX bandwidth */
	if (rxsc == 0)
		bw = *dm->band_width; /*@full bw*/
	else if (rxsc >= 1 && rxsc <= 8)
		bw = CHANNEL_WIDTH_20;
	else if (rxsc >= 9 && rxsc <= 12)
		bw = CHANNEL_WIDTH_40;
	else /*if (rxsc >= 13)*/
		bw = CHANNEL_WIDTH_80;

	return bw;
}

void phydm_reset_bb_hw_cnt(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	/*@ Reset all counter when 1 */
	odm_set_bb_reg(dm, R_0x1eb4, BIT(25), 1);
	odm_set_bb_reg(dm, R_0x1eb4, BIT(25), 0);
}

void phydm_config_ofdm_tx_path(void *dm_void, enum bb_path path)
{
}

void phydm_config_ofdm_rx_path(void *dm_void, enum bb_path path)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u8 val = 0;
}

void phydm_config_cck_rx_antenna_init(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	/*@CCK 2R CCA parameters*/
	odm_set_bb_reg(dm, R_0xa00, BIT(15), 0x0); /*@Disable Ant diversity*/
	odm_set_bb_reg(dm, R_0xa70, BIT(7), 0); /*@Concurrent CCA at LSB & USB*/
	odm_set_bb_reg(dm, R_0xa74, BIT(8), 0); /*RX path diversity enable*/
	odm_set_bb_reg(dm, R_0xa14, BIT(7), 0); /*r_en_mrc_antsel*/
	odm_set_bb_reg(dm, R_0xa20, (BIT(5) | BIT(4)), 1); /*@MBC weighting*/
}

void phydm_config_cck_rx_path(void *dm_void, enum bb_path path)
{
#if (defined(PHYDM_COMPILE_ABOVE_2SS))
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u8 path_div_select = 0;
	u8 cck_path[2] = {0};
	u8 en_2R_path = 0;
	u8 en_2R_mrc = 0;
	u8 i = 0, j = 0;
	u8 num_enable_path = 0;
	u8 cck_mrc_max_path = 2;

	for (i = 0; i < 4; i++) {
		if (path & BIT(i)) { /*@ex: PHYDM_ABCD*/
			num_enable_path++;
			cck_path[j] = i;
			j++;
		}
		if (num_enable_path >= cck_mrc_max_path)
			break;
	}

	if (num_enable_path > 1) {
		path_div_select = 1;
		en_2R_path = 1;
		en_2R_mrc = 1;
	} else {
		path_div_select = 0;
		en_2R_path = 0;
		en_2R_mrc = 0;
	}
	/*@CCK_1 input signal path*/
	odm_set_bb_reg(dm, R_0xa04, (BIT(27) | BIT(26)), cck_path[0]);
	/*@CCK_2 input signal path*/
	odm_set_bb_reg(dm, R_0xa04, (BIT(25) | BIT(24)), cck_path[1]);
	/*@enable Rx path diversity*/
	odm_set_bb_reg(dm, R_0xa74, BIT(8), path_div_select);
	/*@enable 2R Rx path*/
	odm_set_bb_reg(dm, R_0xa2c, BIT(18), en_2R_path);
	/*@enable 2R MRC*/
	odm_set_bb_reg(dm, R_0xa2c, BIT(22), en_2R_mrc);
#endif
}

void phydm_config_cck_tx_path(void *dm_void, enum bb_path path)
{
#if (defined(PHYDM_COMPILE_ABOVE_2SS))
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	if (path == BB_PATH_A)
		odm_set_bb_reg(dm, R_0xa04, 0xf0000000, 0x8);
	else if (path == BB_PATH_B)
		odm_set_bb_reg(dm, R_0xa04, 0xf0000000, 0x4);
	else /*if (path == BB_PATH_AB)*/
		odm_set_bb_reg(dm, R_0xa04, 0xf0000000, 0xc);
#endif
}

void phydm_stop_3_wire(void *dm_void, u8 set_type)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	if (set_type == PHYDM_SET) {
		odm_set_bb_reg(dm, R_0x180c, 0x3, 0x0);
		odm_set_bb_reg(dm, R_0x180c, BIT(28), 0x1);

		odm_set_bb_reg(dm, R_0x410c, 0x3, 0x0);
		odm_set_bb_reg(dm, R_0x410c, BIT(28), 0x1);
	} else { /*@if (set_type == PHYDM_REVERT)*/
		odm_set_bb_reg(dm, R_0x180c, 0x3, 0x3);
		odm_set_bb_reg(dm, R_0x180c, BIT(28), 0x1);

		odm_set_bb_reg(dm, R_0x410c, 0x3, 0x3);
		odm_set_bb_reg(dm, R_0x410c, BIT(28), 0x1);
	}
}

u8 phydm_stop_ic_trx(void *dm_void, u8 set_type)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_api_stuc *api = &dm->api_table;
	u8 i = 0;
	boolean trx_idle_success = false;
	u32 dbg_port_value = 0;

	if (set_type == PHYDM_SET) {
	/*[Stop TRX]---------------------------------------------------------*/
		#if (RTL8723F_SUPPORT)
		/*Judy 2020-0515*/
		/*set debug port to 0x0*/
		if (!phydm_set_bb_dbg_port(dm, DBGPORT_PRI_3, 0x0))
			return PHYDM_SET_FAIL;
		#endif
		for (i = 0; i < 100; i++) {
			dbg_port_value = odm_get_bb_reg(dm, R_0x2db4,
							MASKDWORD);
			/* BB idle */
			if ((dbg_port_value & 0x1FFEFF3F) == 0 &&
			    (dbg_port_value & 0xC0010000) ==
			    0xC0010000) {
				PHYDM_DBG(dm, ODM_COMP_API,
					  "Stop trx wait for (%d) times\n",
					  i);

				trx_idle_success = true;
				break;
			}
		}

		if (trx_idle_success) {
			api->tx_queue_bitmap = odm_read_1byte(dm, R_0x522);

			/*pause all TX queue*/
			odm_set_mac_reg(dm, R_0x520, 0xff0000, 0xff);

			/*disable OFDM RX CCA*/
			odm_set_bb_reg(dm, R_0x1d58, 0xff8, 0x1ff);
			phydm_dis_cck_trx(dm, PHYDM_SET);
		} else {
			return PHYDM_SET_FAIL;
		}

		return PHYDM_SET_SUCCESS;

	} else { /*@if (set_type == PHYDM_REVERT)*/
		/*Release all TX queue*/
		odm_write_1byte(dm, R_0x522, api->tx_queue_bitmap);

		/*@enable OFDM RX CCA*/
		odm_set_bb_reg(dm, R_0x1d58, 0xff8, 0x0);
		phydm_dis_cck_trx(dm, PHYDM_REVERT);
		return PHYDM_SET_SUCCESS;
	}
}

void phydm_dis_cck_trx(void *dm_void, u8 set_type)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_api_stuc *api = &dm->api_table;

	if (set_type == PHYDM_SET) {
		api->ccktx_path = (u8)odm_get_bb_reg(dm, R_0x1a04,
					     	0xf0000000);
		/* @CCK RxIQ weighting = [0,0] */
		odm_set_bb_reg(dm, R_0x1a14, 0x300, 0x3);
		/* @disable CCK Tx */
		odm_set_bb_reg(dm, R_0x1a04, 0xf0000000, 0x0);
	} else if (set_type == PHYDM_REVERT) {
		/* @CCK RxIQ weighting = [1,1] */
		odm_set_bb_reg(dm, R_0x1a14, 0x300, 0x0);
		/* @enable CCK Tx */
		odm_set_bb_reg(dm, R_0x1a04, 0xf0000000,
			       	api->ccktx_path);
	}
}

void phydm_bw_fixed_enable(void *dm_void, u8 enable)
{
#ifdef CONFIG_BW_INDICATION
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	boolean val = (enable == FUNC_ENABLE) ? 1 : 0;

	odm_set_bb_reg(dm, R_0x878, BIT(28), val);
#endif
}

void phydm_bw_fixed_setting(void *dm_void)
{
#ifdef CONFIG_BW_INDICATION
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct phydm_api_stuc *api = &dm->api_table;
	u8 bw = *dm->band_width;
	u32 reg = 0, reg_mask = 0, reg_value = 0;

	reg = R_0x878;
	reg_mask = 0xc0000000;
	reg_value = 0x0;

	switch (bw) {
	case CHANNEL_WIDTH_80:
		odm_set_bb_reg(dm, reg, reg_mask, reg_value);
		break;
	case CHANNEL_WIDTH_40:
		odm_set_bb_reg(dm, reg, reg_mask, reg_value);
		break;
	default:
		odm_set_bb_reg(dm, reg, reg_mask, 0x0);
	}

	phydm_bw_fixed_enable(dm, FUNC_ENABLE);
#endif
}

void phydm_csi_mask_enable(void *dm_void, u32 enable)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	boolean en = false;

	en = (enable == FUNC_ENABLE) ? true : false;

	odm_set_bb_reg(dm, R_0xc0c, BIT(3), en);
	PHYDM_DBG(dm, ODM_COMP_API,
		  "Enable CSI Mask:  Reg 0xc0c[3] = ((0x%x))\n", en);
}

void phydm_clean_all_csi_mask(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u8 i = 0, idx_lmt = 0;

	idx_lmt = 127;

	odm_set_bb_reg(dm, R_0x1ee8, 0x3, 0x3);
	odm_set_bb_reg(dm, R_0x1d94, BIT(31) | BIT(30), 0x1);
	for (i = 0; i < idx_lmt; i++) {
		odm_set_bb_reg(dm, R_0x1d94, MASKBYTE2, i);
		odm_set_bb_reg(dm, R_0x1d94, MASKBYTE0, 0x0);
	}
	odm_set_bb_reg(dm, R_0x1ee8, 0x3, 0x0);
}

void phydm_set_csi_mask(void *dm_void, u32 tone_idx_tmp, u8 tone_direction)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u8 byte_offset = 0, bit_offset = 0;
	u32 target_reg = 0;
	u8 reg_tmp_value = 0;
	u32 tone_num = 64;
	u32 tone_num_shift = 0;
	u32 csi_mask_reg_p = 0, csi_mask_reg_n = 0;

	/* @calculate real tone idx*/
	if ((tone_idx_tmp % 10) >= 5)
		tone_idx_tmp += 10;

	tone_idx_tmp = (tone_idx_tmp / 10);

	if (tone_direction == FREQ_POSITIVE) {
		if (tone_idx_tmp >= (tone_num - 1))
			tone_idx_tmp = (tone_num - 1);

		byte_offset = (u8)(tone_idx_tmp >> 3);
		bit_offset = (u8)(tone_idx_tmp & 0x7);
		target_reg = csi_mask_reg_p + byte_offset;

	} else {
		tone_num_shift = tone_num;

		if (tone_idx_tmp >= tone_num)
			tone_idx_tmp = tone_num;

		tone_idx_tmp = tone_num - tone_idx_tmp;

		byte_offset = (u8)(tone_idx_tmp >> 3);
		bit_offset = (u8)(tone_idx_tmp & 0x7);
		target_reg = csi_mask_reg_n + byte_offset;
	}

	reg_tmp_value = odm_read_1byte(dm, target_reg);
	PHYDM_DBG(dm, ODM_COMP_API,
		  "Pre Mask tone idx[%d]:  Reg0x%x = ((0x%x))\n",
		  (tone_idx_tmp + tone_num_shift), target_reg, reg_tmp_value);
	reg_tmp_value |= BIT(bit_offset);
	odm_write_1byte(dm, target_reg, reg_tmp_value);
	PHYDM_DBG(dm, ODM_COMP_API,
		  "New Mask tone idx[%d]:  Reg0x%x = ((0x%x))\n",
		  (tone_idx_tmp + tone_num_shift), target_reg, reg_tmp_value);
}

void phydm_set_nbi_reg(void *dm_void, u32 tone_idx_tmp, u32 bw)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	/*tone_idx X 10*/
	u32 nbi_128[NBI_128TONE] = {25, 55, 85, 115, 135,
				    155, 185, 205, 225, 245,
				    265, 285, 305, 335, 355,
				    375, 395, 415, 435, 455,
				    485, 505, 525, 555, 585, 615, 635};
	/*tone_idx X 10*/
	u32 nbi_256[NBI_256TONE] = {25, 55, 85, 115, 135,
				    155, 175, 195, 225, 245,
				    265, 285, 305, 325, 345,
				    365, 385, 405, 425, 445,
				    465, 485, 505, 525, 545,
				    565, 585, 605, 625, 645,
				    665, 695, 715, 735, 755,
				    775, 795, 815, 835, 855,
				    875, 895, 915, 935, 955,
				    975, 995, 1015, 1035, 1055,
				    1085, 1105, 1125, 1145, 1175,
				    1195, 1225, 1255, 1275};
	u32 reg_idx = 0;
	u32 i;
	u8 nbi_table_idx = FFT_128_TYPE;

	if (nbi_table_idx == FFT_128_TYPE) {
		for (i = 0; i < NBI_128TONE; i++) {
			if (tone_idx_tmp < nbi_128[i]) {
				reg_idx = i + 1;
				break;
			}
		}

	} else if (nbi_table_idx == FFT_256_TYPE) {
		for (i = 0; i < NBI_256TONE; i++) {
			if (tone_idx_tmp < nbi_256[i]) {
				reg_idx = i + 1;
				break;
			}
		}
	}

	odm_set_bb_reg(dm, R_0x87c, 0xfc000, reg_idx);
	PHYDM_DBG(dm, ODM_COMP_API,
		  "Set tone idx: Reg0x87C[19:14] = ((0x%x))\n",
		  reg_idx);
}

void phydm_nbi_enable(void *dm_void, u32 enable)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 val = 0;

	val = (enable == FUNC_ENABLE) ? 1 : 0;

	PHYDM_DBG(dm, ODM_COMP_API, "Enable NBI=%d\n", val);

}

u8 phydm_find_fc(void *dm_void, u32 channel, u32 bw, u32 second_ch, u32 *fc_in)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 fc = *fc_in;
	u32 start_ch_per_40m[NUM_START_CH_40M] = {36, 44, 52, 60, 100,
						  108, 116, 124, 132, 140,
						  149, 157, 165, 173};
	u32 start_ch_per_80m[NUM_START_CH_80M] = {36, 52, 100, 116, 132,
						  149, 165};
	u32 *start_ch = &start_ch_per_40m[0];
	u32 num_start_channel = NUM_START_CH_40M;
	u32 channel_offset = 0;
	u32 i;

	/*@2.4G*/
	if (channel <= 14 && channel > 0) {
		if (bw == 80)
			return PHYDM_SET_FAIL;

		fc = 2412 + (channel - 1) * 5;

		if (bw == 40 && second_ch == PHYDM_ABOVE) {
			if (channel >= 10) {
				PHYDM_DBG(dm, ODM_COMP_API,
					  "CH = ((%d)), Scnd_CH = ((%d)) Error setting\n",
					  channel, second_ch);
				return PHYDM_SET_FAIL;
			}
			fc += 10;
		} else if (bw == 40 && (second_ch == PHYDM_BELOW)) {
			if (channel <= 2) {
				PHYDM_DBG(dm, ODM_COMP_API,
					  "CH = ((%d)), Scnd_CH = ((%d)) Error setting\n",
					  channel, second_ch);
				return PHYDM_SET_FAIL;
			}
			fc -= 10;
		}
	}
	/*@5G*/
	else if (channel >= 36 && channel <= 177) {
		if (bw != 20) {
			if (bw == 40) {
				num_start_channel = NUM_START_CH_40M;
				start_ch = &start_ch_per_40m[0];
				channel_offset = CH_OFFSET_40M;
			} else if (bw == 80) {
				num_start_channel = NUM_START_CH_80M;
				start_ch = &start_ch_per_80m[0];
				channel_offset = CH_OFFSET_80M;
			}

			for (i = 0; i < (num_start_channel - 1); i++) {
				if (channel < start_ch[i + 1]) {
					channel = start_ch[i] + channel_offset;
					break;
				}
			}
			PHYDM_DBG(dm, ODM_COMP_API, "Mod_CH = ((%d))\n",
				  channel);
		}

		fc = 5180 + (channel - 36) * 5;

	} else {
		PHYDM_DBG(dm, ODM_COMP_API, "CH = ((%d)) Error setting\n",
			  channel);
		return PHYDM_SET_FAIL;
	}

	*fc_in = fc;

	return PHYDM_SET_SUCCESS;
}

u8 phydm_find_intf_distance(void *dm_void, u32 bw, u32 fc, u32 f_interference,
			    u32 *tone_idx_tmp_in)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 bw_up = 0, bw_low = 0;
	u32 int_distance = 0;
	u32 tone_idx_tmp = 0;
	u8 set_result = PHYDM_SET_NO_NEED;

	bw_up = fc + bw / 2;
	bw_low = fc - bw / 2;

	PHYDM_DBG(dm, ODM_COMP_API,
		  "[f_l, fc, fh] = [ %d, %d, %d ], f_int = ((%d))\n", bw_low,
		  fc, bw_up, f_interference);

	if (f_interference >= bw_low && f_interference <= bw_up) {
		int_distance = DIFF_2(fc, f_interference);
		/*@10*(int_distance /0.3125)*/
		tone_idx_tmp = (int_distance << 5);
		PHYDM_DBG(dm, ODM_COMP_API,
			  "int_distance = ((%d MHz)) Mhz, tone_idx_tmp = ((%d.%d))\n",
			  int_distance, tone_idx_tmp / 10,
			  tone_idx_tmp % 10);
		*tone_idx_tmp_in = tone_idx_tmp;
		set_result = PHYDM_SET_SUCCESS;
	}

	return set_result;
}

u8 phydm_csi_mask_setting(void *dm_void, u32 enable, u32 ch, u32 bw,
			  u32 f_intf, u32 sec_ch)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 fc = 2412;
	u8 direction = FREQ_POSITIVE;
	u32 tone_idx = 0;
	u8 set_result = PHYDM_SET_SUCCESS;
	u8 rpt = 0;

	if (enable == FUNC_DISABLE) {
		set_result = PHYDM_SET_SUCCESS;
		phydm_clean_all_csi_mask(dm);

	} else {
		PHYDM_DBG(dm, ODM_COMP_API,
			  "[Set CSI MASK_] CH = ((%d)), BW = ((%d)), f_intf = ((%d)), Scnd_CH = ((%s))\n",
			  ch, bw, f_intf,
			  (((bw == 20) || (ch > 14)) ? "Don't care" :
			  (sec_ch == PHYDM_ABOVE) ? "H" : "L"));

		/*@calculate fc*/
		if (phydm_find_fc(dm, ch, bw, sec_ch, &fc) == PHYDM_SET_FAIL) {
			set_result = PHYDM_SET_FAIL;
		} else {
			/*@calculate interference distance*/
			rpt = phydm_find_intf_distance(dm, bw, fc, f_intf,
						       &tone_idx);
			if (rpt == PHYDM_SET_SUCCESS) {
				if (f_intf >= fc)
					direction = FREQ_POSITIVE;
				else
					direction = FREQ_NEGATIVE;

				phydm_set_csi_mask(dm, tone_idx, direction);
				set_result = PHYDM_SET_SUCCESS;
			} else {
				set_result = PHYDM_SET_NO_NEED;
			}
		}
	}

	if (set_result == PHYDM_SET_SUCCESS)
		phydm_csi_mask_enable(dm, enable);
	else
		phydm_csi_mask_enable(dm, FUNC_DISABLE);

	return set_result;
}

boolean phydm_spur_case_mapping(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u8 channel = *dm->channel, bw = *dm->band_width;
	boolean mapping_result = false;
#if (RTL8814B_SUPPORT == 1)
	if (channel == 153 && bw == CHANNEL_WIDTH_20)
		mapping_result =  true;
	else if (channel == 151 && bw == CHANNEL_WIDTH_40)
		mapping_result =  true;
	else if (channel == 155 && bw == CHANNEL_WIDTH_80)
		mapping_result =  true;
#endif
	return mapping_result;
}

enum odm_rf_band phydm_ch_to_rf_band(void *dm_void, u8 central_ch)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	enum odm_rf_band rf_band = ODM_RF_BAND_5G_LOW;

	if (central_ch <= 14)
		rf_band = ODM_RF_BAND_2G;
	else if (central_ch >= 36 && central_ch <= 64)
		rf_band = ODM_RF_BAND_5G_LOW;
	else if ((central_ch >= 100) && (central_ch <= 144))
		rf_band = ODM_RF_BAND_5G_MID;
	else if (central_ch >= 149)
		rf_band = ODM_RF_BAND_5G_HIGH;
	else
		PHYDM_DBG(dm, ODM_COMP_API, "mapping channel to band fail\n");

	return rf_band;
}

#ifdef PHYDM_IC_JGR3_SERIES_SUPPORT
u32 phydm_rf_psd_jgr3(void *dm_void, u8 path, u32 tone_idx)
{
#if (RTL8198F_SUPPORT || RTL8814B_SUPPORT)
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 reg_1b04 = 0, reg_1b08 = 0, reg_1b0c_11_10 = 0;
	u32 reg_1b14 = 0, reg_1b18 = 0, reg_1b1c = 0;
	u32 reg_1b28 = 0;
	u32 reg_1bcc_5_0 = 0;
	u32 reg_1b2c_27_16 = 0, reg_1b34 = 0, reg_1bd4 = 0;
	u32 reg_180c = 0, reg_410c = 0, reg_520c = 0, reg_530c = 0;
	u32 igi = 0;
	u32 i = 0;
	u32 psd_val = 0, psd_val_msb = 0, psd_val_lsb = 0, psd_max = 0;
	u32 psd_status_temp = 0;
	u16 poll_cnt = 0;

	/*read and record the ori. value*/
	reg_1b04 = odm_get_bb_reg(dm, R_0x1b04, MASKDWORD);
	reg_1b08 = odm_get_bb_reg(dm, R_0x1b08, MASKDWORD);
	reg_1b0c_11_10 = odm_get_bb_reg(dm, R_0x1b0c, 0xc00);
	reg_1b14 = odm_get_bb_reg(dm, R_0x1b14, MASKDWORD);
	reg_1b18 = odm_get_bb_reg(dm, R_0x1b18, MASKDWORD);
	reg_1b1c = odm_get_bb_reg(dm, R_0x1b1c, MASKDWORD);
	reg_1b28 = odm_get_bb_reg(dm, R_0x1b28, MASKDWORD);
	reg_1bcc_5_0 = odm_get_bb_reg(dm, R_0x1bcc, 0x3f);
	reg_1b2c_27_16 = odm_get_bb_reg(dm, R_0x1b2c, 0xfff0000);
	reg_1b34 = odm_get_bb_reg(dm, R_0x1b34, MASKDWORD);
	reg_1bd4 = odm_get_bb_reg(dm, R_0x1bd4, MASKDWORD);
	igi = odm_get_bb_reg(dm, R_0x1d70, MASKDWORD);
	reg_180c = odm_get_bb_reg(dm, R_0x180c, 0x3);
	reg_410c = odm_get_bb_reg(dm, R_0x410c, 0x3);
	reg_520c = odm_get_bb_reg(dm, R_0x520c, 0x3);
	reg_530c = odm_get_bb_reg(dm, R_0x530c, 0x3);

	/*rf psd reg setting*/
	odm_set_bb_reg(dm, R_0x1b00, 0x6, path); /*path is RF_path*/
	odm_set_bb_reg(dm, R_0x1b04, MASKDWORD, 0x0);
	odm_set_bb_reg(dm, R_0x1b08, MASKDWORD, 0x80);
	odm_set_bb_reg(dm, R_0x1b0c, 0xc00, 0x3);
	odm_set_bb_reg(dm, R_0x1b14, MASKDWORD, 0x0);
	odm_set_bb_reg(dm, R_0x1b18, MASKDWORD, 0x1);
/*#if (DM_ODM_SUPPORT_TYPE == ODM_AP)*/
	odm_set_bb_reg(dm, R_0x1b1c, MASKDWORD, 0x82103D21);
/*#else*/
	/*odm_set_bb_reg(dm, R_0x1b1c, MASKDWORD, 0x821A3D21);*/
/*#endif*/
	odm_set_bb_reg(dm, R_0x1b28, MASKDWORD, 0x0);
	odm_set_bb_reg(dm, R_0x1bcc, 0x3f, 0x3f);
	odm_set_bb_reg(dm, R_0x8a0, 0xf, 0x0); /* AGC off */
	odm_set_bb_reg(dm, R_0x1d70, MASKDWORD, 0x20202020);

	for (i = tone_idx - 1; i <= tone_idx + 1; i++) {
		/*set psd tone_idx for detection*/
		odm_set_bb_reg(dm, R_0x1b2c, 0xfff0000, i);
		/*one shot for RXIQK psd*/
		odm_set_bb_reg(dm, R_0x1b34, MASKDWORD, 0x1);
		odm_set_bb_reg(dm, R_0x1b34, MASKDWORD, 0x0);

		ODM_delay_us(250);

		/*read RxIQK power*/
		odm_set_bb_reg(dm, R_0x1bd4, MASKDWORD, 0x00250001);

		odm_set_bb_reg(dm, R_0x1bd4, MASKDWORD, 0x002e0001);
		psd_val_lsb = odm_get_bb_reg(dm, R_0x1bfc, MASKDWORD);

		if (psd_val > psd_max)
			psd_max = psd_val;
	}

	/*refill the ori. value*/
	odm_set_bb_reg(dm, R_0x1b00, 0x6, path);
	odm_set_bb_reg(dm, R_0x1b04, MASKDWORD, reg_1b04);
	odm_set_bb_reg(dm, R_0x1b08, MASKDWORD, reg_1b08);
	odm_set_bb_reg(dm, R_0x1b0c, 0xc00, reg_1b0c_11_10);
	odm_set_bb_reg(dm, R_0x1b14, MASKDWORD, reg_1b14);
	odm_set_bb_reg(dm, R_0x1b18, MASKDWORD, reg_1b18);
	odm_set_bb_reg(dm, R_0x1b1c, MASKDWORD, reg_1b1c);
	odm_set_bb_reg(dm, R_0x1b28, MASKDWORD, reg_1b28);
	odm_set_bb_reg(dm, R_0x1bcc, 0x3f, reg_1bcc_5_0);
	odm_set_bb_reg(dm, R_0x1b2c, 0xfff0000, reg_1b2c_27_16);
	odm_set_bb_reg(dm, R_0x1b34, MASKDWORD, reg_1b34);
	odm_set_bb_reg(dm, R_0x1bd4, MASKDWORD, reg_1bd4);
	odm_set_bb_reg(dm, R_0x8a0, 0xf, 0xf); /* AGC on */
	odm_set_bb_reg(dm, R_0x1d70, MASKDWORD, igi);
	PHYDM_DBG(dm, ODM_COMP_API, "psd_max %d\n", psd_max);

	return psd_max;
#else
	return 0;
#endif
}

u8 phydm_find_intf_distance_jgr3(void *dm_void, u32 bw, u32 fc,
				 u32 f_interference, u32 *tone_idx_tmp_in)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 bw_up = 0, bw_low = 0;
	u32 int_distance = 0;
	u32 tone_idx_tmp = 0;
	u8 set_result = PHYDM_SET_NO_NEED;
	u8 channel = *dm->channel;

	bw_up = 1000 * (fc + bw / 2);
	bw_low = 1000 * (fc - bw / 2);
	fc = 1000 * fc;

	PHYDM_DBG(dm, ODM_COMP_API,
		  "[f_l, fc, fh] = [ %d, %d, %d ], f_int = ((%d))\n", bw_low,
		  fc, bw_up, f_interference);

	if (f_interference >= bw_low && f_interference <= bw_up) {
		int_distance = DIFF_2(fc, f_interference);
		/*@10*(int_distance /0.3125)*/
		tone_idx_tmp = ((int_distance + 156) / 312);
		PHYDM_DBG(dm, ODM_COMP_API,
			  "int_distance = ((%d)) , tone_idx_tmp = ((%d))\n",
			  int_distance, tone_idx_tmp);
		*tone_idx_tmp_in = tone_idx_tmp;
		set_result = PHYDM_SET_SUCCESS;
	}

	return set_result;
}

u8 phydm_csi_mask_setting_jgr3(void *dm_void, u32 enable, u32 ch, u32 bw,
			       u32 f_intf, u32 sec_ch, u8 wgt)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 fc = 2412;
	u8 direction = FREQ_POSITIVE;
	u32 tone_idx = 0;
	u8 set_result = PHYDM_SET_SUCCESS;
	u8 rpt = 0;

	if (enable == FUNC_DISABLE) {
		phydm_csi_mask_enable(dm, FUNC_ENABLE);
		phydm_clean_all_csi_mask(dm);
		phydm_csi_mask_enable(dm, FUNC_DISABLE);
		set_result = PHYDM_SET_SUCCESS;
	} else {
		PHYDM_DBG(dm, ODM_COMP_API,
			  "[Set CSI MASK] CH = ((%d)), BW = ((%d)), f_intf = ((%d)), Scnd_CH = ((%s)), wgt = ((%d))\n",
			  ch, bw, f_intf,
			  (((bw == 20) || (ch > 14)) ? "Don't care" :
			  (sec_ch == PHYDM_ABOVE) ? "H" : "L"), wgt);

		/*@calculate fc*/
		if (phydm_find_fc(dm, ch, bw, sec_ch, &fc) == PHYDM_SET_FAIL) {
			set_result = PHYDM_SET_FAIL;
		} else {
			/*@calculate interference distance*/
			rpt = phydm_find_intf_distance_jgr3(dm, bw, fc, f_intf,
							    &tone_idx);
			if (rpt == PHYDM_SET_SUCCESS) {
				if (f_intf >= 1000 * fc)
					direction = FREQ_POSITIVE;
				else
					direction = FREQ_NEGATIVE;

				phydm_csi_mask_enable(dm, FUNC_ENABLE);
				phydm_set_csi_mask_jgr3(dm, tone_idx, direction,
							wgt);
				set_result = PHYDM_SET_SUCCESS;
			} else {
				set_result = PHYDM_SET_NO_NEED;
			}
		}
		if (!(set_result == PHYDM_SET_SUCCESS))
			phydm_csi_mask_enable(dm, FUNC_DISABLE);
	}

	return set_result;
}

void phydm_set_csi_mask_jgr3(void *dm_void, u32 tone_idx_tmp, u8 tone_direction,
			     u8 wgt)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 multi_tone_idx_tmp = 0;
	u32 reg_tmp = 0;
	u32 tone_num = 64;
	u32 table_addr = 0;
	u32 addr = 0;
	u8 rf_bw = 0;
	u8 value = 0;
	u8 channel = *dm->channel;

	rf_bw = odm_read_1byte(dm, R_0x9b0);
	if (((rf_bw & 0xc) >> 2) == 0x2)
		tone_num = 128; /* @RF80 : tone(-1) at tone_idx=255 */
	else
		tone_num = 64; /* @RF20/40 : tone(-1) at tone_idx=127 */

	if (tone_direction == FREQ_POSITIVE) {
		if (tone_idx_tmp >= (tone_num - 1))
			tone_idx_tmp = (tone_num - 1);
	} else {
		if (tone_idx_tmp >= tone_num)
			tone_idx_tmp = tone_num;

		tone_idx_tmp = (tone_num << 1) - tone_idx_tmp;
	}
	table_addr = tone_idx_tmp >> 1;

	reg_tmp = odm_read_4byte(dm, R_0x1d94);
	PHYDM_DBG(dm, ODM_COMP_API,
		  "Pre Mask tone idx[%d]: Reg0x1d94 = ((0x%x))\n",
		  tone_idx_tmp, reg_tmp);
	odm_set_bb_reg(dm, R_0x1ee8, 0x3, 0x3);
	odm_set_bb_reg(dm, R_0x1d94, BIT(31) | BIT(30), 0x1);

	odm_set_bb_reg(dm, R_0x1d94, MASKBYTE2, (table_addr &
		       0xff));
	if (tone_idx_tmp % 2)
		value = (BIT(3) | (wgt & 0x7)) << 4;
	else
		value = BIT(3) | (wgt & 0x7);

	odm_set_bb_reg(dm, R_0x1d94, 0xff, value);
	reg_tmp = odm_read_4byte(dm, R_0x1d94);
	PHYDM_DBG(dm, ODM_COMP_API,
		  "New Mask tone idx[%d]: Reg0x1d94 = ((0x%x))\n",
		  tone_idx_tmp, reg_tmp);
	odm_set_bb_reg(dm, R_0x1ee8, 0x3, 0x0);
}

void phydm_nbi_reset_jgr3(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	odm_set_bb_reg(dm, R_0x818, BIT(3), 1);
	odm_set_bb_reg(dm, R_0x1d3c, 0x78000000, 0);
	odm_set_bb_reg(dm, R_0x818, BIT(3), 0);
	odm_set_bb_reg(dm, R_0x818, BIT(11), 0);
}

u8 phydm_nbi_setting_jgr3(void *dm_void, u32 enable, u32 ch, u32 bw, u32 f_intf,
			  u32 sec_ch, u8 path)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 fc = 2412;
	u8 direction = FREQ_POSITIVE;
	u32 tone_idx = 0;
	u8 set_result = PHYDM_SET_SUCCESS;
	u8 rpt = 0;

	if (enable == FUNC_DISABLE) {
		phydm_nbi_reset_jgr3(dm);
		set_result = PHYDM_SET_SUCCESS;
	} else {
		PHYDM_DBG(dm, ODM_COMP_API,
			  "[Set NBI] CH = ((%d)), BW = ((%d)), f_intf = ((%d)), Scnd_CH = ((%s))\n",
			  ch, bw, f_intf,
			  (((sec_ch == PHYDM_DONT_CARE) || (bw == 20) ||
			  (ch > 14)) ? "Don't care" :
			  (sec_ch == PHYDM_ABOVE) ? "H" : "L"));

		/*@calculate fc*/
		if (phydm_find_fc(dm, ch, bw, sec_ch, &fc) == PHYDM_SET_FAIL) {
			set_result = PHYDM_SET_FAIL;
		} else {
			/*@calculate interference distance*/
			rpt = phydm_find_intf_distance_jgr3(dm, bw, fc, f_intf,
							    &tone_idx);
			if (rpt == PHYDM_SET_SUCCESS) {
				if (f_intf >= 1000 * fc)
					direction = FREQ_POSITIVE;
				else
					direction = FREQ_NEGATIVE;

				phydm_set_nbi_reg_jgr3(dm, tone_idx, direction,
						       path);
				set_result = PHYDM_SET_SUCCESS;
			} else {
				set_result = PHYDM_SET_NO_NEED;
			}
		}
	}

	if (set_result == PHYDM_SET_SUCCESS)
		phydm_nbi_enable_jgr3(dm, enable, path);
	else
		phydm_nbi_enable_jgr3(dm, FUNC_DISABLE, path);

	return set_result;
}

void phydm_set_nbi_reg_jgr3(void *dm_void, u32 tone_idx_tmp, u8 tone_direction,
			    u8 path)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 reg_tmp_value = 0;
	u32 tone_num = 64;
	u32 addr = 0;
	u8 rf_bw = 0;

	rf_bw = odm_read_1byte(dm, R_0x9b0);
	if (((rf_bw & 0xc) >> 2) == 0x2)
		tone_num = 128; /* RF80 : tone-1 at tone_idx=255 */
	else
		tone_num = 64; /* RF20/40 : tone-1 at tone_idx=127 */

	if (tone_direction == FREQ_POSITIVE) {
		if (tone_idx_tmp >= (tone_num - 1))
			tone_idx_tmp = (tone_num - 1);
	} else {
		if (tone_idx_tmp >= tone_num)
			tone_idx_tmp = tone_num;

		tone_idx_tmp = (tone_num << 1) - tone_idx_tmp;
	}
	/*Mark the tone idx for Packet detection*/
	switch (path) {
	case RF_PATH_A:
		odm_set_bb_reg(dm, R_0x1944, 0x001FF000, tone_idx_tmp);
		PHYDM_DBG(dm, ODM_COMP_API,
			  "Set tone idx[%d]:PATH-A = ((0x%x))\n",
			  tone_idx_tmp, tone_idx_tmp);
		break;
	#if (defined(PHYDM_COMPILE_ABOVE_2SS))
	case RF_PATH_B:
		odm_set_bb_reg(dm, R_0x4044, 0x001FF000, tone_idx_tmp);
		PHYDM_DBG(dm, ODM_COMP_API,
			  "Set tone idx[%d]:PATH-B = ((0x%x))\n",
			  tone_idx_tmp, tone_idx_tmp);
		break;
	#endif
	#if (defined(PHYDM_COMPILE_ABOVE_3SS))
	case RF_PATH_C:
		odm_set_bb_reg(dm, R_0x5044, 0x001FF000, tone_idx_tmp);
		PHYDM_DBG(dm, ODM_COMP_API,
			  "Set tone idx[%d]:PATH-C = ((0x%x))\n",
			  tone_idx_tmp, tone_idx_tmp);
		break;
	#endif
	#if (defined(PHYDM_COMPILE_ABOVE_4SS))
	case RF_PATH_D:
		odm_set_bb_reg(dm, R_0x5144, 0x001FF000, tone_idx_tmp);
		PHYDM_DBG(dm, ODM_COMP_API,
			  "Set tone idx[%d]:PATH-D = ((0x%x))\n",
			  tone_idx_tmp, tone_idx_tmp);
		break;
	#endif
	default:
		break;
	}
}

void phydm_nbi_enable_jgr3(void *dm_void, u32 enable, u8 path)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	boolean val = false;

	val = (enable == FUNC_ENABLE) ? true : false;

	PHYDM_DBG(dm, ODM_COMP_API, "Enable NBI=%d\n", val);

	odm_set_bb_reg(dm, R_0x818, BIT(3), !val);
	odm_set_bb_reg(dm, R_0x818, BIT(11), val);
	odm_set_bb_reg(dm, R_0x1d3c, 0x78000000, 0xf);

	if (enable == FUNC_ENABLE) {
		switch (path) {
		case RF_PATH_A:
			odm_set_bb_reg(dm, R_0x1940, BIT(31), val);
			break;
		#if (defined(PHYDM_COMPILE_ABOVE_2SS))
		case RF_PATH_B:
			odm_set_bb_reg(dm, R_0x4040, BIT(31), val);
			break;
		#endif
		#if (defined(PHYDM_COMPILE_ABOVE_3SS))
		case RF_PATH_C:
			odm_set_bb_reg(dm, R_0x5040, BIT(31), val);
			break;
		#endif
		#if (defined(PHYDM_COMPILE_ABOVE_4SS))
		case RF_PATH_D:
			odm_set_bb_reg(dm, R_0x5140, BIT(31), val);
			break;
		#endif
		default:
			break;
		}
	} else {
		odm_set_bb_reg(dm, R_0x1940, BIT(31), val);
		#if (defined(PHYDM_COMPILE_ABOVE_2SS))
		odm_set_bb_reg(dm, R_0x4040, BIT(31), val);
		#endif
		#if (defined(PHYDM_COMPILE_ABOVE_3SS))
		odm_set_bb_reg(dm, R_0x5040, BIT(31), val);
		#endif
		#if (defined(PHYDM_COMPILE_ABOVE_4SS))
		odm_set_bb_reg(dm, R_0x5140, BIT(31), val);
		#endif
	}
}

u8 phydm_phystat_rpt_jgr3(void *dm_void, enum phystat_rpt info,
			  enum rf_path ant_path)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	s8 evm_org, cfo_org, rxsnr_org;
	u8 i, return_info = 0, tmp_lsb = 0, tmp_msb = 0, tmp_info = 0;

	/* Update the status for each pkt */
	odm_set_bb_reg(dm, R_0x8c4, 0xfff000, 0x448);
	odm_set_bb_reg(dm, R_0x8c0, MASKLWORD, 0x4001);
	/* PHY status Page1 */
	odm_set_bb_reg(dm, R_0x8c0, 0x3C00000, 0x1);
	/*choose debug port for phystatus */
	odm_set_bb_reg(dm, R_0x1c3c, 0xFFF00, 0x380);

	if (info == PHY_PWDB) {
		/* Choose the report of the diff path */
		if (ant_path == RF_PATH_A)
			odm_set_bb_reg(dm, R_0x8c4, 0x3ff, 0x1);
		else if (ant_path == RF_PATH_B)
			odm_set_bb_reg(dm, R_0x8c4, 0x3ff, 0x2);
		else if (ant_path == RF_PATH_C)
			odm_set_bb_reg(dm, R_0x8c4, 0x3ff, 0x3);
		else if (ant_path == RF_PATH_D)
			odm_set_bb_reg(dm, R_0x8c4, 0x3ff, 0x4);
	} else if (info == PHY_EVM) {
		/* Choose the report of the diff path */
		if (ant_path == RF_PATH_A)
			odm_set_bb_reg(dm, R_0x8c4, 0x3ff, 0x10);
		else if (ant_path == RF_PATH_B)
			odm_set_bb_reg(dm, R_0x8c4, 0x3ff, 0x11);
		else if (ant_path == RF_PATH_C)
			odm_set_bb_reg(dm, R_0x8c4, 0x3ff, 0x12);
		else if (ant_path == RF_PATH_D)
			odm_set_bb_reg(dm, R_0x8c4, 0x3ff, 0x13);
		return_info = (u8)odm_get_bb_reg(dm, R_0x2dbc, 0xff);
	} else if (info == PHY_CFO) {
		/* Choose the report of the diff path */
		if (ant_path == RF_PATH_A)
			odm_set_bb_reg(dm, R_0x8c4, 0x3ff, 0x14);
		else if (ant_path == RF_PATH_B)
			odm_set_bb_reg(dm, R_0x8c4, 0x3ff, 0x15);
		else if (ant_path == RF_PATH_C)
			odm_set_bb_reg(dm, R_0x8c4, 0x3ff, 0x16);
		else if (ant_path == RF_PATH_D)
			odm_set_bb_reg(dm, R_0x8c4, 0x3ff, 0x17);
		return_info = (u8)odm_get_bb_reg(dm, R_0x2dbc, 0xff);
	} else if (info == PHY_RXSNR) {
		/* Choose the report of the diff path */
		if (ant_path == RF_PATH_A)
			odm_set_bb_reg(dm, R_0x8c4, 0x3ff, 0x18);
		else if (ant_path == RF_PATH_B)
			odm_set_bb_reg(dm, R_0x8c4, 0x3ff, 0x19);
		else if (ant_path == RF_PATH_C)
			odm_set_bb_reg(dm, R_0x8c4, 0x3ff, 0x1a);
		else if (ant_path == RF_PATH_D)
			odm_set_bb_reg(dm, R_0x8c4, 0x3ff, 0x1b);
		return_info = (u8)odm_get_bb_reg(dm, R_0x2dbc, 0xff);
	} else if (info == PHY_LGAIN) {
		/* choose page */
		odm_set_bb_reg(dm, R_0x8c0, 0x3c00000, 0x2);
		/* Choose the report of the diff path */
		if (ant_path == RF_PATH_A) {
			odm_set_bb_reg(dm, R_0x8c4, 0x3ff, 0xd);
			tmp_info = (u8)odm_get_bb_reg(dm, R_0x2dbc, 0x3f);
			return_info = tmp_info;
		} else if (ant_path == RF_PATH_B) {
			odm_set_bb_reg(dm, R_0x8c4, 0x3ff, 0xd);
			tmp_lsb = (u8)odm_get_bb_reg(dm, R_0x2dbc, 0xc0);
			odm_set_bb_reg(dm, R_0x8c4, 0x3ff, 0xe);
			tmp_msb = (u8)odm_get_bb_reg(dm, R_0x2dbc, 0xf);
			tmp_info |= (tmp_msb << 2) | tmp_lsb;
			return_info = tmp_info;
		} else if (ant_path == RF_PATH_C) {
			odm_set_bb_reg(dm, R_0x8c4, 0x3ff, 0xe);
			tmp_lsb = (u8)odm_get_bb_reg(dm, R_0x2dbc, 0xf0);
			odm_set_bb_reg(dm, R_0x8c4, 0x3ff, 0xf);
			tmp_msb = (u8)odm_get_bb_reg(dm, R_0x2dbc, 0x3);
			tmp_info |= (tmp_msb << 4) | tmp_lsb;
			return_info = tmp_info;
		} else if (ant_path == RF_PATH_D) {
			odm_set_bb_reg(dm, R_0x8c4, 0x3ff, 0x10);
			tmp_info = (u8)odm_get_bb_reg(dm, R_0x2dbc, 0x3f);
			return_info = tmp_info;
		}
	} else if (info == PHY_HT_AAGC_GAIN) {
		/* choose page */
		odm_set_bb_reg(dm, R_0x8c0, 0x3c00000, 0x2);
		/* Choose the report of the diff path */
		if (ant_path == RF_PATH_A)
			odm_set_bb_reg(dm, R_0x8c4, 0x3ff, 0x12);
		else if (ant_path == RF_PATH_B)
			odm_set_bb_reg(dm, R_0x8c4, 0x3ff, 0x13);
		else if (ant_path == RF_PATH_C)
			odm_set_bb_reg(dm, R_0x8c4, 0x3ff, 0x14);
		else if (ant_path == RF_PATH_D)
			odm_set_bb_reg(dm, R_0x8c4, 0x3ff, 0x15);
		return_info = (u8)odm_get_bb_reg(dm, R_0x2dbc, 0xff);
	}
	return return_info;
}

void phydm_ex_hal8814b_wifi_only_hw_config(void *dm_void)
{
	/*BB control*/
	/*halwifionly_phy_set_bb_reg(pwifionlycfg, 0x4c, 0x01800000, 0x2);*/
	/*SW control*/
	/*halwifionly_phy_set_bb_reg(pwifionlycfg, 0xcb4, 0xff, 0x77);*/
	/*antenna mux switch */
	/*halwifionly_phy_set_bb_reg(pwifionlycfg, 0x974, 0x300, 0x3);*/

	/*halwifionly_phy_set_bb_reg(pwifionlycfg, 0x1990, 0x300, 0x0);*/

	/*halwifionly_phy_set_bb_reg(pwifionlycfg, 0xcbc, 0x80000, 0x0);*/
	/*switch to WL side controller and gnt_wl gnt_bt debug signal */
	/*halwifionly_phy_set_bb_reg(pwifionlycfg, 0x70, 0xff000000, 0x0e);*/
	/*gnt_wl=1 , gnt_bt=0*/
	/*halwifionly_phy_set_bb_reg(pwifionlycfg, 0x1704, 0xffffffff,
	 *			     0x7700);
	 */
	/*halwifionly_phy_set_bb_reg(pwifionlycfg, 0x1700, 0xffffffff,
	 *			     0xc00f0038);
	 */
}

void phydm_user_position_for_sniffer(void *dm_void, u8 user_position)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;

	/* user position valid */
	odm_set_bb_reg(dm, R_0xa68, BIT(17), 1);
	/* Select user seat from pmac */
	odm_set_bb_reg(dm, R_0xa68, BIT(16), 1);
	/*user seat*/
	odm_set_bb_reg(dm, R_0xa68, (BIT(19) | BIT(18)), user_position);
}

boolean
phydm_bb_ctrl_txagc_ofst_jgr3(void *dm_void, s8 pw_offset, /*@(unit: dB)*/
			      u8 add_half_db /*@(+0.5 dB)*/)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	s8 pw_idx = pw_offset * 4; /*@ 7Bit, 0.25dB unit*/

	if (pw_offset < -16 || pw_offset > 15) {
		pr_debug("[Warning][%s]Ofst error=%d", __func__, pw_offset);
		return false;
	}

	if (add_half_db)
		pw_idx += 2;

	PHYDM_DBG(dm, ODM_COMP_API, "Pw_ofst=0x%x\n", pw_idx);

	odm_set_bb_reg(dm, R_0x18a0, 0x3f, pw_idx);

	if (dm->num_rf_path >= 2)
		odm_set_bb_reg(dm, R_0x41a0, 0x3f, pw_idx);

	if (dm->num_rf_path >= 3)
		odm_set_bb_reg(dm, R_0x52a0, 0x3f, pw_idx);

	if (dm->num_rf_path >= 4)
		odm_set_bb_reg(dm, R_0x53a0, 0x3f, pw_idx);

	return true;
}

#endif
u8 phydm_nbi_setting(void *dm_void, u32 enable, u32 ch, u32 bw, u32 f_intf,
		     u32 sec_ch)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 fc = 2412;
	u8 direction = FREQ_POSITIVE;
	u32 tone_idx = 0;
	u8 set_result = PHYDM_SET_SUCCESS;
	u8 rpt = 0;

	if (enable == FUNC_DISABLE) {
		set_result = PHYDM_SET_SUCCESS;
	} else {
		PHYDM_DBG(dm, ODM_COMP_API,
			  "[Set NBI] CH = ((%d)), BW = ((%d)), f_intf = ((%d)), Scnd_CH = ((%s))\n",
			  ch, bw, f_intf,
			  (((sec_ch == PHYDM_DONT_CARE) || (bw == 20) ||
			  (ch > 14)) ? "Don't care" :
			  (sec_ch == PHYDM_ABOVE) ? "H" : "L"));

		/*@calculate fc*/
		if (phydm_find_fc(dm, ch, bw, sec_ch, &fc) == PHYDM_SET_FAIL) {
			set_result = PHYDM_SET_FAIL;
		} else {
			/*@calculate interference distance*/
			rpt = phydm_find_intf_distance(dm, bw, fc, f_intf,
						       &tone_idx);
			if (rpt == PHYDM_SET_SUCCESS) {
				if (f_intf >= fc)
					direction = FREQ_POSITIVE;
				else
					direction = FREQ_NEGATIVE;

				phydm_set_nbi_reg(dm, tone_idx, bw);

				set_result = PHYDM_SET_SUCCESS;
			} else {
				set_result = PHYDM_SET_NO_NEED;
		}
	}
	}

	if (set_result == PHYDM_SET_SUCCESS)
		phydm_nbi_enable(dm, enable);
	else
		phydm_nbi_enable(dm, FUNC_DISABLE);

	return set_result;
}

void phydm_nbi_debug(void *dm_void, char input[][16], u32 *_used, char *output,
		     u32 *_out_len)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 used = *_used;
	u32 out_len = *_out_len;
	u32 val[10] = {0};
	char help[] = "-h";
	u8 i = 0, input_idx = 0, idx_lmt = 0;
	u32 enable = 0; /*@function enable*/
	u32 ch = 0;
	u32 bw = 0;
	u32 f_int = 0; /*@interference frequency*/
	u32 sec_ch = 0; /*secondary channel*/
	u8 rpt = 0;
	u8 path = 0;

	idx_lmt = 6;
	for (i = 0; i < idx_lmt; i++) {
		PHYDM_SSCANF(input[i + 1], DCMD_DECIMAL, &val[i]);
		input_idx++;
	}

	if (input_idx == 0)
		return;

	enable = val[0];
	ch = val[1];
	bw = val[2];
	f_int = val[3];
	sec_ch = val[4];
	#ifdef PHYDM_IC_JGR3_SERIES_SUPPORT
	path = (u8)val[5];
	#endif

	if ((strcmp(input[1], help) == 0)) {
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "{en:1 Dis:2} {ch} {BW:20/40/80} {f_intf(khz)} {Scnd_CH(L=1, H=2)} {Path:A~D(0~3)}\n");
		*_used = used;
		*_out_len = out_len;
		return;
	} else if (val[0] == FUNC_ENABLE) {
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "[Enable NBI] CH = ((%d)), BW = ((%d)), f_intf = ((%d)), Scnd_CH = ((%s))\n",
			 ch, bw, f_int,
			 ((sec_ch == PHYDM_DONT_CARE) ||
			 (bw == 20) || (ch > 14)) ? "Don't care" :
			 ((sec_ch == PHYDM_ABOVE) ? "H" : "L"));
		rpt = phydm_nbi_setting_jgr3(dm, enable, ch, bw, f_int,
					     sec_ch, path);
	} else if (val[0] == FUNC_DISABLE) {
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "[Disable NBI]\n");
		rpt = phydm_nbi_setting_jgr3(dm, enable, ch, bw, f_int,
					     sec_ch, path);
	} else {
		rpt = PHYDM_SET_FAIL;
	}

	PDM_SNPF(out_len, used, output + used, out_len - used,
		 "[NBI set result: %s]\n",
		 (rpt == PHYDM_SET_SUCCESS) ? "Success" :
		 ((rpt == PHYDM_SET_NO_NEED) ? "No need" : "Error"));

	*_used = used;
	*_out_len = out_len;
}

void phydm_csi_debug(void *dm_void, char input[][16], u32 *_used, char *output,
		     u32 *_out_len)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 used = *_used;
	u32 out_len = *_out_len;
	u32 val[10] = {0};
	char help[] = "-h";
	u8 i = 0, input_idx = 0, idx_lmt = 0;
	u32 enable = 0;  /*@function enable*/
	u32 ch = 0;
	u32 bw = 0;
	u32 f_int = 0; /*@interference frequency*/
	u32 sec_ch = 0;  /*secondary channel*/
	u8 rpt = 0;
	u8 wgt = 0;

	idx_lmt = 6;

	for (i = 0; i < idx_lmt; i++) {
		PHYDM_SSCANF(input[i + 1], DCMD_DECIMAL, &val[i]);
		input_idx++;
	}

	if (input_idx == 0)
		return;

	enable = val[0];
	ch = val[1];
	bw = val[2];
	f_int = val[3];
	sec_ch = val[4];
	#ifdef PHYDM_IC_JGR3_SERIES_SUPPORT
	wgt = (u8)val[5];
	#endif

	if ((strcmp(input[1], help) == 0)) {
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "{en:1 Dis:2} {ch} {BW:20/40/80} {f_intf(KHz)} {Scnd_CH(L=1, H=2)}\n{wgt:(7:3/4),(6~1: 1/2 ~ 1/64),(0:0)}\n");
		*_used = used;
		*_out_len = out_len;
		return;

	} else if (val[0] == FUNC_ENABLE) {
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "[Enable CSI MASK] CH = ((%d)), BW = ((%d)), f_intf = ((%d)), Scnd_CH = ((%s))\n",
			 ch, bw, f_int,
			 (ch > 14) ? "Don't care" :
			 (((sec_ch == PHYDM_DONT_CARE) ||
			 (bw == 20) || (ch > 14)) ? "H" : "L"));
		rpt = phydm_csi_mask_setting_jgr3(dm, enable, ch, bw,
						  f_int, sec_ch, wgt);
	} else if (val[0] == FUNC_DISABLE) {
		PDM_SNPF(out_len, used, output + used, out_len - used,
			 "[Disable CSI MASK]\n");
		rpt = phydm_csi_mask_setting_jgr3(dm, enable, ch, bw,
						  f_int, sec_ch, wgt);
	} else {
		rpt = PHYDM_SET_FAIL;
	}
	PDM_SNPF(out_len, used, output + used, out_len - used,
		 "[CSI MASK set result: %s]\n",
		 (rpt == PHYDM_SET_SUCCESS) ? "Success" :
		 ((rpt == PHYDM_SET_NO_NEED) ? "No need" : "Error"));

	*_used = used;
	*_out_len = out_len;
}

#ifdef PHYDM_COMMON_API_SUPPORT
void phydm_reset_txagc(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u32 r_txagc_cck[4] = {R_0x18a0, R_0x41a0, R_0x52a0, R_0x53a0};
	u32 r_txagc_ofdm[4] = {R_0x18e8, R_0x41e8, R_0x52e8, R_0x53e8};
	u32 r_txagc_diff = R_0x3a00;
	u8 i = 0;

	for (i = RF_PATH_A; i < dm->num_rf_path; i++) {
		odm_set_bb_reg(dm, r_txagc_cck[i], 0x7f0000, 0x0);
		odm_set_bb_reg(dm, r_txagc_ofdm[i], 0x1fc00, 0x0);
	}

	for (i = 0; i <= ODM_RATEVHTSS4MCS6; i = i + 4)
		odm_set_bb_reg(dm, r_txagc_diff + i, MASKDWORD, 0x0);
}
boolean
phydm_api_shift_txagc(void *dm_void, u32 pwr_offset, enum rf_path path,
		      boolean is_positive) {
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	boolean ret = false;
	u32 txagc_cck = 0;
	u32 txagc_ofdm = 0;
	u32 r_txagc_ofdm[4] = {R_0x18e8, R_0x41e8, R_0x52e8, R_0x53e8};
	u32 r_txagc_cck[4] = {R_0x18a0, R_0x41a0, R_0x52a0, R_0x53a0};
	u32 r_new_txagc[1] = {R_0x4308};

	if (path > RF_PATH_B) {
		PHYDM_DBG(dm, ODM_PHY_CONFIG, "Unsupported path (%d)\n",
			  path);
		return false;
	}
	txagc_cck = (u8)odm_get_bb_reg(dm, r_txagc_cck[path],
					   0x7F0000);
	txagc_ofdm = (u8)odm_get_bb_reg(dm, r_txagc_ofdm[path],
					    0x1FC00);
	if (is_positive) {
		if (((txagc_cck + pwr_offset) > 127) ||
		    ((txagc_ofdm + pwr_offset) > 127))
			return false;

		txagc_cck += pwr_offset;
		txagc_ofdm += pwr_offset;
	} else {
		if (pwr_offset > txagc_cck || pwr_offset > txagc_ofdm)
			return false;

		txagc_cck -= pwr_offset;
		txagc_ofdm -= pwr_offset;
	}
	ret = config_phydm_write_txagc_ref_8822c(dm, (u8)txagc_cck,
						 path, PDM_CCK);
	ret &= config_phydm_write_txagc_ref_8822c(dm, (u8)txagc_ofdm,
						 path, PDM_OFDM);
	PHYDM_DBG(dm, ODM_PHY_CONFIG,
		  "%s: path-%d txagc_cck_ref=%x txagc_ofdm_ref=0x%x\n",
		  __func__, path, txagc_cck, txagc_ofdm);

	return ret;
}

boolean
phydm_api_set_txagc(void *dm_void, u32 pwr_idx, enum rf_path path,
		    u8 rate, boolean is_single_rate)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	boolean ret = false;
	u8 base = 0;
	u8 txagc_tmp = 0;
	s8 pw_by_rate_tmp = 0;
	s8 pw_by_rate_new = 0;

	if (rate < 0x4)
		txagc_tmp = config_phydm_read_txagc_8822c(dm, path,
							  rate,
							  PDM_CCK);
	else
		txagc_tmp = config_phydm_read_txagc_8822c(dm, path,
							  rate,
							  PDM_OFDM);

	pw_by_rate_tmp = config_phydm_read_txagc_diff_8822c(dm, rate);
	base = txagc_tmp - pw_by_rate_tmp;
	base = base & 0x7f;
	if (DIFF_2((pwr_idx & 0x7f), base) > 63 || pwr_idx > 127)
		return false;

	pw_by_rate_new = (s8)(pwr_idx - base);
	ret = phydm_write_txagc_1byte_8822c(dm, pw_by_rate_new, rate);
	PHYDM_DBG(dm, ODM_PHY_CONFIG,
		  "%s: path-%d rate_idx=%x base=0x%x new_diff=0x%x\n",
		  __func__, path, rate, base, pw_by_rate_new);
	return ret;
}

u8 phydm_api_get_txagc(void *dm_void, enum rf_path path, u8 hw_rate)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u8 ret = 0;

	if (hw_rate < 0x4) {
		ret = config_phydm_read_txagc_8822c(dm, path, hw_rate,
						    PDM_CCK);
	} else {
		ret = config_phydm_read_txagc_8822c(dm, path, hw_rate,
						    PDM_OFDM);
	}
	return ret;
}

boolean
phydm_api_switch_bw_channel(void *dm_void, u8 ch, u8 pri_ch,
			    enum channel_width bw)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	boolean ret = false;

	ret = config_phydm_switch_channel_bw_8822c(dm, ch, pri_ch, bw);
	return ret;
}

#endif

#ifdef PHYDM_COMMON_API_NOT_SUPPORT
u8 config_phydm_read_txagc_n(void *dm_void, enum rf_path path, u8 hw_rate)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	u8 read_back_data = INVALID_TXAGC_DATA;
	u32 reg_txagc;
	u32 reg_mask;
	/* This function is for 92E/88E etc... */
	/* @Input need to be HW rate index, not driver rate index!!!! */

	/* @Error handling */
	if (path > RF_PATH_B || hw_rate > ODM_RATEMCS15) {
		PHYDM_DBG(dm, ODM_PHY_CONFIG, "%s: unsupported path (%d)\n",
			  __func__, path);
		return INVALID_TXAGC_DATA;
	}

	if (path == RF_PATH_A) {
		switch (hw_rate) {
		case ODM_RATE1M:
			reg_txagc = R_0xe08;
			reg_mask = 0x00007f00;
			break;
		case ODM_RATE2M:
			reg_txagc = R_0x86c;
			reg_mask = 0x00007f00;
			break;
		case ODM_RATE5_5M:
			reg_txagc = R_0x86c;
			reg_mask = 0x007f0000;
			break;
		case ODM_RATE11M:
			reg_txagc = R_0x86c;
			reg_mask = 0x7f000000;
			break;

		case ODM_RATE6M:
			reg_txagc = R_0xe00;
			reg_mask = 0x0000007f;
			break;
		case ODM_RATE9M:
			reg_txagc = R_0xe00;
			reg_mask = 0x00007f00;
			break;
		case ODM_RATE12M:
			reg_txagc = R_0xe00;
			reg_mask = 0x007f0000;
			break;
		case ODM_RATE18M:
			reg_txagc = R_0xe00;
			reg_mask = 0x7f000000;
			break;
		case ODM_RATE24M:
			reg_txagc = R_0xe04;
			reg_mask = 0x0000007f;
			break;
		case ODM_RATE36M:
			reg_txagc = R_0xe04;
			reg_mask = 0x00007f00;
			break;
		case ODM_RATE48M:
			reg_txagc = R_0xe04;
			reg_mask = 0x007f0000;
			break;
		case ODM_RATE54M:
			reg_txagc = R_0xe04;
			reg_mask = 0x7f000000;
			break;

		case ODM_RATEMCS0:
			reg_txagc = R_0xe10;
			reg_mask = 0x0000007f;
			break;
		case ODM_RATEMCS1:
			reg_txagc = R_0xe10;
			reg_mask = 0x00007f00;
			break;
		case ODM_RATEMCS2:
			reg_txagc = R_0xe10;
			reg_mask = 0x007f0000;
			break;
		case ODM_RATEMCS3:
			reg_txagc = R_0xe10;
			reg_mask = 0x7f000000;
			break;
		case ODM_RATEMCS4:
			reg_txagc = R_0xe14;
			reg_mask = 0x0000007f;
			break;
		case ODM_RATEMCS5:
			reg_txagc = R_0xe14;
			reg_mask = 0x00007f00;
			break;
		case ODM_RATEMCS6:
			reg_txagc = R_0xe14;
			reg_mask = 0x007f0000;
			break;
		case ODM_RATEMCS7:
			reg_txagc = R_0xe14;
			reg_mask = 0x7f000000;
			break;
		case ODM_RATEMCS8:
			reg_txagc = R_0xe18;
			reg_mask = 0x0000007f;
			break;
		case ODM_RATEMCS9:
			reg_txagc = R_0xe18;
			reg_mask = 0x00007f00;
			break;
		case ODM_RATEMCS10:
			reg_txagc = R_0xe18;
			reg_mask = 0x007f0000;
			break;
		case ODM_RATEMCS11:
			reg_txagc = R_0xe18;
			reg_mask = 0x7f000000;
			break;
		case ODM_RATEMCS12:
			reg_txagc = R_0xe1c;
			reg_mask = 0x0000007f;
			break;
		case ODM_RATEMCS13:
			reg_txagc = R_0xe1c;
			reg_mask = 0x00007f00;
			break;
		case ODM_RATEMCS14:
			reg_txagc = R_0xe1c;
			reg_mask = 0x007f0000;
			break;
		case ODM_RATEMCS15:
			reg_txagc = R_0xe1c;
			reg_mask = 0x7f000000;
			break;

		default:
			PHYDM_DBG(dm, ODM_PHY_CONFIG, "Invalid HWrate!\n");
			break;
		}
	} else if (path == RF_PATH_B) {
		switch (hw_rate) {
		case ODM_RATE1M:
			reg_txagc = R_0x838;
			reg_mask = 0x00007f00;
			break;
		case ODM_RATE2M:
			reg_txagc = R_0x838;
			reg_mask = 0x007f0000;
			break;
		case ODM_RATE5_5M:
			reg_txagc = R_0x838;
			reg_mask = 0x7f000000;
			break;
		case ODM_RATE11M:
			reg_txagc = R_0x86c;
			reg_mask = 0x0000007f;
			break;

		case ODM_RATE6M:
			reg_txagc = R_0x830;
			reg_mask = 0x0000007f;
			break;
		case ODM_RATE9M:
			reg_txagc = R_0x830;
			reg_mask = 0x00007f00;
			break;
		case ODM_RATE12M:
			reg_txagc = R_0x830;
			reg_mask = 0x007f0000;
			break;
		case ODM_RATE18M:
			reg_txagc = R_0x830;
			reg_mask = 0x7f000000;
			break;
		case ODM_RATE24M:
			reg_txagc = R_0x834;
			reg_mask = 0x0000007f;
			break;
		case ODM_RATE36M:
			reg_txagc = R_0x834;
			reg_mask = 0x00007f00;
			break;
		case ODM_RATE48M:
			reg_txagc = R_0x834;
			reg_mask = 0x007f0000;
			break;
		case ODM_RATE54M:
			reg_txagc = R_0x834;
			reg_mask = 0x7f000000;
			break;

		case ODM_RATEMCS0:
			reg_txagc = R_0x83c;
			reg_mask = 0x0000007f;
			break;
		case ODM_RATEMCS1:
			reg_txagc = R_0x83c;
			reg_mask = 0x00007f00;
			break;
		case ODM_RATEMCS2:
			reg_txagc = R_0x83c;
			reg_mask = 0x007f0000;
			break;
		case ODM_RATEMCS3:
			reg_txagc = R_0x83c;
			reg_mask = 0x7f000000;
			break;
		case ODM_RATEMCS4:
			reg_txagc = R_0x848;
			reg_mask = 0x0000007f;
			break;
		case ODM_RATEMCS5:
			reg_txagc = R_0x848;
			reg_mask = 0x00007f00;
			break;
		case ODM_RATEMCS6:
			reg_txagc = R_0x848;
			reg_mask = 0x007f0000;
			break;
		case ODM_RATEMCS7:
			reg_txagc = R_0x848;
			reg_mask = 0x7f000000;
			break;

		case ODM_RATEMCS8:
			reg_txagc = R_0x84c;
			reg_mask = 0x0000007f;
			break;
		case ODM_RATEMCS9:
			reg_txagc = R_0x84c;
			reg_mask = 0x00007f00;
			break;
		case ODM_RATEMCS10:
			reg_txagc = R_0x84c;
			reg_mask = 0x007f0000;
			break;
		case ODM_RATEMCS11:
			reg_txagc = R_0x84c;
			reg_mask = 0x7f000000;
			break;
		case ODM_RATEMCS12:
			reg_txagc = R_0x868;
			reg_mask = 0x0000007f;
			break;
		case ODM_RATEMCS13:
			reg_txagc = R_0x868;
			reg_mask = 0x00007f00;
			break;
		case ODM_RATEMCS14:
			reg_txagc = R_0x868;
			reg_mask = 0x007f0000;
			break;
		case ODM_RATEMCS15:
			reg_txagc = R_0x868;
			reg_mask = 0x7f000000;
			break;

		default:
			PHYDM_DBG(dm, ODM_PHY_CONFIG, "Invalid HWrate!\n");
			break;
		}
	} else {
		PHYDM_DBG(dm, ODM_PHY_CONFIG, "Invalid RF path!!\n");
	}
	read_back_data = (u8)odm_get_bb_reg(dm, reg_txagc, reg_mask);
	PHYDM_DBG(dm, ODM_PHY_CONFIG, "%s: path-%d rate index 0x%x = 0x%x\n",
		  __func__, path, hw_rate, read_back_data);
	return read_back_data;
}
#endif

#ifdef CONFIG_MCC_DM

void phydm_mcc_init(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _phydm_mcc_dm_ *mcc_dm = &dm->mcc_dm;
	u8	i;

	/*PHYDM_DBG(dm, DBG_COMP_MCC, ("MCC init\n"));*/
	PHYDM_DBG(dm, DBG_COMP_MCC, "MCC init\n");
	for (i = 0; i < MCC_DM_REG_NUM; i++) {
		mcc_dm->mcc_reg_id[i] = 0xff;
		mcc_dm->mcc_dm_reg[i] = 0;
		mcc_dm->mcc_dm_val[i][0] = 0;
		mcc_dm->mcc_dm_val[i][1] = 0;
	}
	for (i = 0; i < NUM_STA; i++) {
		mcc_dm->sta_macid[0][i] = 0xff;
		mcc_dm->sta_macid[1][i] = 0xff;
	}
	/* Function init */
	dm->is_stop_dym_ant_weighting = 0;
}

u8 phydm_check(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _phydm_mcc_dm_ *mcc_dm = &dm->mcc_dm;
	struct cmn_sta_info			*p_entry = NULL;
	u8	shift = 0;
	u8	i = 0;
	u8	j = 0;
	u8	rssi_min[2] = {0xff, 0xff};
	u8	sta_num = 8;
	u8 mcc_macid = 0;

	for (i = 0; i <= 1; i++) {
		for (j = 0; j < sta_num; j++) {
			if (mcc_dm->sta_macid[i][j] != 0xff) {
				mcc_macid = mcc_dm->sta_macid[i][j];
				p_entry = dm->phydm_sta_info[mcc_macid];
				if (!p_entry) {
					PHYDM_DBG(dm, DBG_COMP_MCC,
						  "PEntry NULL(mac=%d)\n",
						  mcc_dm->sta_macid[i][j]);
					return _FAIL;
				}
				PHYDM_DBG(dm, DBG_COMP_MCC,
					  "undec_smoothed_pwdb=%d\n",
					  p_entry->rssi_stat.rssi);
				if (p_entry->rssi_stat.rssi < rssi_min[i])
					rssi_min[i] = p_entry->rssi_stat.rssi;
			}
		}
	}
	mcc_dm->mcc_rssi[0] = (u8)rssi_min[0];
	mcc_dm->mcc_rssi[1] = (u8)rssi_min[1];
	return _SUCCESS;
}

void phydm_mcc_h2ccmd_rst(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _phydm_mcc_dm_ *mcc_dm = &dm->mcc_dm;
	u8 i;
	u8 regid;
	u8 h2c_mcc[H2C_MAX_LENGTH];

	/* RST MCC */
	for (i = 0; i < H2C_MAX_LENGTH; i++)
		h2c_mcc[i] = 0xff;
	h2c_mcc[0] = 0x00;
	odm_fill_h2c_cmd(dm, PHYDM_H2C_MCC, H2C_MAX_LENGTH, h2c_mcc);
	PHYDM_DBG(dm, DBG_COMP_MCC, "MCC H2C RST\n");
}

void phydm_mcc_h2ccmd(void *dm_void)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _phydm_mcc_dm_ *mcc_dm = &dm->mcc_dm;
	u8 i;
	u8 regid;
	u8 h2c_mcc[H2C_MAX_LENGTH];

	if (mcc_dm->mcc_rf_ch[0] == 0xff && mcc_dm->mcc_rf_ch[1] == 0xff) {
		PHYDM_DBG(dm, DBG_COMP_MCC, "MCC channel Error\n");
		return;
	}
	/* Set Channel number */
	for (i = 0; i < H2C_MAX_LENGTH; i++)
		h2c_mcc[i] = 0xff;
	h2c_mcc[0] = 0xe0;
	h2c_mcc[1] = (u8)(mcc_dm->mcc_rf_ch[0]);
	h2c_mcc[2] = (u8)(mcc_dm->mcc_rf_ch[0] >> 8);
	h2c_mcc[3] = (u8)(mcc_dm->mcc_rf_ch[1]);
	h2c_mcc[4] = (u8)(mcc_dm->mcc_rf_ch[1] >> 8);
	h2c_mcc[5] = 0xff;
	h2c_mcc[6] = 0xff;
	odm_fill_h2c_cmd(dm, PHYDM_H2C_MCC, H2C_MAX_LENGTH, h2c_mcc);
	PHYDM_DBG(dm, DBG_COMP_MCC,
		  "MCC H2C SetCH: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
		  h2c_mcc[0], h2c_mcc[1], h2c_mcc[2], h2c_mcc[3],
		  h2c_mcc[4], h2c_mcc[5], h2c_mcc[6]);

	/* Set Reg and value*/
	for (i = 0; i < H2C_MAX_LENGTH; i++)
		h2c_mcc[i] = 0xff;

	for (i = 0; i < MCC_DM_REG_NUM; i++) {
		regid = mcc_dm->mcc_reg_id[i];
		if (regid != 0xff) {
			h2c_mcc[0] = 0xa0 | (regid & 0x1f);
			h2c_mcc[1] = (u8)(mcc_dm->mcc_dm_reg[i]);
			h2c_mcc[2] = (u8)(mcc_dm->mcc_dm_reg[i] >> 8);
			h2c_mcc[3] = mcc_dm->mcc_dm_val[i][0];
			h2c_mcc[4] = mcc_dm->mcc_dm_val[i][1];
			h2c_mcc[5] = 0xff;
			h2c_mcc[6] = 0xff;
			odm_fill_h2c_cmd(dm, PHYDM_H2C_MCC, H2C_MAX_LENGTH,
					 h2c_mcc);
			PHYDM_DBG(dm, DBG_COMP_MCC,
				  "MCC H2C: 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x 0x%x\n",
				  h2c_mcc[0], h2c_mcc[1], h2c_mcc[2],
				  h2c_mcc[3], h2c_mcc[4],
				  h2c_mcc[5], h2c_mcc[6]);
		}
	}
}


void phydm_fill_mcccmd(void *dm_void, u8 regid, u16 reg_add,
		       u8 val0, u8 val1)
{
	struct dm_struct *dm = (struct dm_struct *)dm_void;
	struct _phydm_mcc_dm_ *mcc_dm = &dm->mcc_dm;

	mcc_dm->mcc_reg_id[regid] = regid;
	mcc_dm->mcc_dm_reg[regid] = reg_add;
	mcc_dm->mcc_dm_val[regid][0] = val0;
	mcc_dm->mcc_dm_val[regid][1] = val1;
}

#endif

#if (DM_ODM_SUPPORT_TYPE == ODM_WIN)
void phydm_normal_driver_rx_sniffer(
	struct dm_struct *dm,
	u8 *desc,
	PRT_RFD_STATUS rt_rfd_status,
	u8 *drv_info,
	u8 phy_status)
{
#if (defined(CONFIG_PHYDM_RX_SNIFFER_PARSING))
	u32 *msg;
	u16 seq_num;

	if (rt_rfd_status->packet_report_type != NORMAL_RX)
		return;

	if (!dm->is_linked) {
		if (rt_rfd_status->is_hw_error)
			return;
	}

	if (phy_status == true) {
		if (dm->rx_pkt_type == type_block_ack ||
		    dm->rx_pkt_type == type_rts || dm->rx_pkt_type == type_cts)
			seq_num = 0;
		else
			seq_num = rt_rfd_status->seq_num;

		PHYDM_DBG_F(dm, ODM_COMP_SNIFFER,
			    "%04d , %01s, rate=0x%02x, L=%04d , %s , %s",
			    seq_num,
			    /*rt_rfd_status->mac_id,*/
			    (rt_rfd_status->is_crc ? "C" :
			    rt_rfd_status->is_ampdu ? "A" : "_"),
			    rt_rfd_status->data_rate,
			    rt_rfd_status->length,
			    ((rt_rfd_status->band_width == 0) ? "20M" :
			    ((rt_rfd_status->band_width == 1) ? "40M" : "80M")),
			    (rt_rfd_status->is_ldpc ? "LDP" : "BCC"));

		if (dm->rx_pkt_type == type_asoc_req)
			PHYDM_DBG_F(dm, ODM_COMP_SNIFFER, " , [%s]", "AS_REQ");
		else if (dm->rx_pkt_type == type_asoc_rsp)
			PHYDM_DBG_F(dm, ODM_COMP_SNIFFER, " , [%s]", "AS_RSP");
		else if (dm->rx_pkt_type == type_probe_req)
			PHYDM_DBG_F(dm, ODM_COMP_SNIFFER, " , [%s]", "PR_REQ");
		else if (dm->rx_pkt_type == type_probe_rsp)
			PHYDM_DBG_F(dm, ODM_COMP_SNIFFER, " , [%s]", "PR_RSP");
		else if (dm->rx_pkt_type == type_deauth)
			PHYDM_DBG_F(dm, ODM_COMP_SNIFFER, " , [%s]", "DEAUTH");
		else if (dm->rx_pkt_type == type_beacon)
			PHYDM_DBG_F(dm, ODM_COMP_SNIFFER, " , [%s]", "BEACON");
		else if (dm->rx_pkt_type == type_block_ack_req)
			PHYDM_DBG_F(dm, ODM_COMP_SNIFFER, " , [%s]", "BA_REQ");
		else if (dm->rx_pkt_type == type_rts)
			PHYDM_DBG_F(dm, ODM_COMP_SNIFFER, " , [%s]", "__RTS_");
		else if (dm->rx_pkt_type == type_cts)
			PHYDM_DBG_F(dm, ODM_COMP_SNIFFER, " , [%s]", "__CTS_");
		else if (dm->rx_pkt_type == type_ack)
			PHYDM_DBG_F(dm, ODM_COMP_SNIFFER, " , [%s]", "__ACK_");
		else if (dm->rx_pkt_type == type_block_ack)
			PHYDM_DBG_F(dm, ODM_COMP_SNIFFER, " , [%s]", "__BA__");
		else if (dm->rx_pkt_type == type_data)
			PHYDM_DBG_F(dm, ODM_COMP_SNIFFER, " , [%s]", "_DATA_");
		else if (dm->rx_pkt_type == type_data_ack)
			PHYDM_DBG_F(dm, ODM_COMP_SNIFFER, " , [%s]", "Data_Ack");
		else if (dm->rx_pkt_type == type_qos_data)
			PHYDM_DBG_F(dm, ODM_COMP_SNIFFER, " , [%s]", "QoS_Data");
		else
			PHYDM_DBG_F(dm, ODM_COMP_SNIFFER, " , [0x%x]",
				    dm->rx_pkt_type);

		PHYDM_DBG_F(dm, ODM_COMP_SNIFFER, " , [RSSI=%d,%d,%d,%d ]",
			    dm->rssi_a,
			    dm->rssi_b,
			    dm->rssi_c,
			    dm->rssi_d);

		msg = (u32 *)drv_info;

		PHYDM_DBG_F(dm, ODM_COMP_SNIFFER,
			    " , P-STS[28:0]=%08x-%08x-%08x-%08x-%08x-%08x-%08x\n",
			    msg[6], msg[5], msg[4], msg[3],
			    msg[2], msg[1], msg[1]);
	} else {
		PHYDM_DBG_F(dm, ODM_COMP_SNIFFER,
			    "%04d , %01s, rate=0x%02x, L=%04d , %s , %s\n",
			    rt_rfd_status->seq_num,
			    /*rt_rfd_status->mac_id,*/
			    (rt_rfd_status->is_crc ? "C" :
			    (rt_rfd_status->is_ampdu) ? "A" : "_"),
			    rt_rfd_status->data_rate,
			    rt_rfd_status->length,
			    ((rt_rfd_status->band_width == 0) ? "20M" :
			    ((rt_rfd_status->band_width == 1) ? "40M" : "80M")),
			    (rt_rfd_status->is_ldpc ? "LDP" : "BCC"));
	}

#endif
}

#endif
