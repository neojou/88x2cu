/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation.
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
 *****************************************************************************/
#ifndef _RTW_GENERAL_DEF_H_
#define _RTW_GENERAL_DEF_H_

#define SEC_CAP_CHK_BMC	BIT0

#define BIT0	0x00000001
#define BIT1	0x00000002
#define BIT2	0x00000004
#define BIT3	0x00000008
#define BIT4	0x00000010
#define BIT5	0x00000020
#define BIT6	0x00000040
#define BIT7	0x00000080
#define BIT8	0x00000100
#define BIT9	0x00000200
#define BIT10	0x00000400
#define BIT11	0x00000800
#define BIT12	0x00001000
#define BIT13	0x00002000
#define BIT14	0x00004000
#define BIT15	0x00008000
#define BIT16	0x00010000
#define BIT17	0x00020000
#define BIT18	0x00040000
#define BIT19	0x00080000
#define BIT20	0x00100000
#define BIT21	0x00200000
#define BIT22	0x00400000
#define BIT23	0x00800000
#define BIT24	0x01000000
#define BIT25	0x02000000
#define BIT26	0x04000000
#define BIT27	0x08000000
#define BIT28	0x10000000
#define BIT29	0x20000000
#define BIT30	0x40000000
#define BIT31	0x80000000

#define RTW_U32_MAX 0xFFFFFFFF

// NEO : add RTL8814A / RTL8822B / RTL8822C
enum rtl_ic_id {
	RTL8814A,
	RTL8822B,
	RTL8822C,
	RTL8852A,
	RTL8834A,
	RTL8852B,
	RTL8852C,
	MAX_IC_ID
};

/* BIT definition for combination */
enum rtw_hci_type {
	RTW_HCI_PCIE = BIT0,
	RTW_HCI_USB = BIT1,
	RTW_HCI_SDIO = BIT2,
	RTW_HCI_GSPI = BIT3,
	RTW_HCI_MAX,
};

#define	SM_PS_STATIC	0
#define	SM_PS_DYNAMIC	1
#define	SM_PS_INVALID	2
#define	SM_PS_DISABLE	3

#define MAC_ADDRESS_LENGTH 6
#define IPV4_ADDRESS_LENGTH 4
#define IPV6_ADDRESS_LENGTH 16

/* Core shall translate system condition into device state for PHL controller */
enum rtw_dev_state {
	RTW_DEV_WORKING = BIT0,
	RTW_DEV_SURPRISE_REMOVAL = BIT1,
	RTW_DEV_MAX
};

enum rtw_rate_mode {
	RTW_LEGACY_MODE = 0,
	RTW_HT_MODE = 1,
	RTW_VHT_MODE = 2,
	RTW_HE_MODE = 3
};

enum rtw_data_rate {
	RTW_DATA_RATE_CCK1		= 0x0,
	RTW_DATA_RATE_CCK2		= 0x1,
	RTW_DATA_RATE_CCK5_5	= 0x2,
	RTW_DATA_RATE_CCK11		= 0x3,
	RTW_DATA_RATE_OFDM6		= 0x4,
	RTW_DATA_RATE_OFDM9		= 0x5,
	RTW_DATA_RATE_OFDM12	= 0x6,
	RTW_DATA_RATE_OFDM18	= 0x7,
	RTW_DATA_RATE_OFDM24	= 0x8,
	RTW_DATA_RATE_OFDM36	= 0x9,
	RTW_DATA_RATE_OFDM48	= 0xA,
	RTW_DATA_RATE_OFDM54	= 0xB,
	RTW_DATA_RATE_MCS0		= 0x80,
	RTW_DATA_RATE_MCS1		= 0x81,
	RTW_DATA_RATE_MCS2		= 0x82,
	RTW_DATA_RATE_MCS3		= 0x83,
	RTW_DATA_RATE_MCS4		= 0x84,
	RTW_DATA_RATE_MCS5		= 0x85,
	RTW_DATA_RATE_MCS6		= 0x86,
	RTW_DATA_RATE_MCS7		= 0x87,
	RTW_DATA_RATE_MCS8		= 0x88,
	RTW_DATA_RATE_MCS9		= 0x89,
	RTW_DATA_RATE_MCS10		= 0x8A,
	RTW_DATA_RATE_MCS11		= 0x8B,
	RTW_DATA_RATE_MCS12		= 0x8C,
	RTW_DATA_RATE_MCS13		= 0x8D,
	RTW_DATA_RATE_MCS14		= 0x8E,
	RTW_DATA_RATE_MCS15		= 0x8F,
	RTW_DATA_RATE_MCS16		= 0x90,
	RTW_DATA_RATE_MCS17		= 0x91,
	RTW_DATA_RATE_MCS18		= 0x92,
	RTW_DATA_RATE_MCS19		= 0x93,
	RTW_DATA_RATE_MCS20		= 0x94,
	RTW_DATA_RATE_MCS21		= 0x95,
	RTW_DATA_RATE_MCS22		= 0x96,
	RTW_DATA_RATE_MCS23		= 0x97,
	RTW_DATA_RATE_MCS24		= 0x98,
	RTW_DATA_RATE_MCS25		= 0x99,
	RTW_DATA_RATE_MCS26		= 0x9A,
	RTW_DATA_RATE_MCS27		= 0x9B,
	RTW_DATA_RATE_MCS28		= 0x9C,
	RTW_DATA_RATE_MCS29		= 0x9D,
	RTW_DATA_RATE_MCS30		= 0x9E,
	RTW_DATA_RATE_MCS31		= 0x9F,
	RTW_DATA_RATE_VHT_NSS1_MCS0	= 0x100,
	RTW_DATA_RATE_VHT_NSS1_MCS1	= 0x101,
	RTW_DATA_RATE_VHT_NSS1_MCS2	= 0x102,
	RTW_DATA_RATE_VHT_NSS1_MCS3	= 0x103,
	RTW_DATA_RATE_VHT_NSS1_MCS4	= 0x104,
	RTW_DATA_RATE_VHT_NSS1_MCS5	= 0x105,
	RTW_DATA_RATE_VHT_NSS1_MCS6	= 0x106,
	RTW_DATA_RATE_VHT_NSS1_MCS7	= 0x107,
	RTW_DATA_RATE_VHT_NSS1_MCS8	= 0x108,
	RTW_DATA_RATE_VHT_NSS1_MCS9	= 0x109,
	RTW_DATA_RATE_VHT_NSS2_MCS0	= 0x110,
	RTW_DATA_RATE_VHT_NSS2_MCS1	= 0x111,
	RTW_DATA_RATE_VHT_NSS2_MCS2	= 0x112,
	RTW_DATA_RATE_VHT_NSS2_MCS3	= 0x113,
	RTW_DATA_RATE_VHT_NSS2_MCS4	= 0x114,
	RTW_DATA_RATE_VHT_NSS2_MCS5	= 0x115,
	RTW_DATA_RATE_VHT_NSS2_MCS6	= 0x116,
	RTW_DATA_RATE_VHT_NSS2_MCS7	= 0x117,
	RTW_DATA_RATE_VHT_NSS2_MCS8	= 0x118,
	RTW_DATA_RATE_VHT_NSS2_MCS9	= 0x119,
	RTW_DATA_RATE_VHT_NSS3_MCS0	= 0x120,
	RTW_DATA_RATE_VHT_NSS3_MCS1	= 0x121,
	RTW_DATA_RATE_VHT_NSS3_MCS2	= 0x122,
	RTW_DATA_RATE_VHT_NSS3_MCS3	= 0x123,
	RTW_DATA_RATE_VHT_NSS3_MCS4	= 0x124,
	RTW_DATA_RATE_VHT_NSS3_MCS5	= 0x125,
	RTW_DATA_RATE_VHT_NSS3_MCS6	= 0x126,
	RTW_DATA_RATE_VHT_NSS3_MCS7	= 0x127,
	RTW_DATA_RATE_VHT_NSS3_MCS8	= 0x128,
	RTW_DATA_RATE_VHT_NSS3_MCS9	= 0x129,
	RTW_DATA_RATE_VHT_NSS4_MCS0	= 0x130,
	RTW_DATA_RATE_VHT_NSS4_MCS1	= 0x131,
	RTW_DATA_RATE_VHT_NSS4_MCS2	= 0x132,
	RTW_DATA_RATE_VHT_NSS4_MCS3	= 0x133,
	RTW_DATA_RATE_VHT_NSS4_MCS4	= 0x134,
	RTW_DATA_RATE_VHT_NSS4_MCS5	= 0x135,
	RTW_DATA_RATE_VHT_NSS4_MCS6	= 0x136,
	RTW_DATA_RATE_VHT_NSS4_MCS7	= 0x137,
	RTW_DATA_RATE_VHT_NSS4_MCS8	= 0x138,
	RTW_DATA_RATE_VHT_NSS4_MCS9	= 0x139,
	RTW_DATA_RATE_HE_NSS1_MCS0	= 0x180,
	RTW_DATA_RATE_HE_NSS1_MCS1	= 0x181,
	RTW_DATA_RATE_HE_NSS1_MCS2	= 0x182,
	RTW_DATA_RATE_HE_NSS1_MCS3	= 0x183,
	RTW_DATA_RATE_HE_NSS1_MCS4	= 0x184,
	RTW_DATA_RATE_HE_NSS1_MCS5	= 0x185,
	RTW_DATA_RATE_HE_NSS1_MCS6	= 0x186,
	RTW_DATA_RATE_HE_NSS1_MCS7	= 0x187,
	RTW_DATA_RATE_HE_NSS1_MCS8	= 0x188,
	RTW_DATA_RATE_HE_NSS1_MCS9	= 0x189,
	RTW_DATA_RATE_HE_NSS1_MCS10	= 0x18A,
	RTW_DATA_RATE_HE_NSS1_MCS11	= 0x18B,
	RTW_DATA_RATE_HE_NSS2_MCS0	= 0x190,
	RTW_DATA_RATE_HE_NSS2_MCS1	= 0x191,
	RTW_DATA_RATE_HE_NSS2_MCS2	= 0x192,
	RTW_DATA_RATE_HE_NSS2_MCS3	= 0x193,
	RTW_DATA_RATE_HE_NSS2_MCS4	= 0x194,
	RTW_DATA_RATE_HE_NSS2_MCS5	= 0x195,
	RTW_DATA_RATE_HE_NSS2_MCS6	= 0x196,
	RTW_DATA_RATE_HE_NSS2_MCS7	= 0x197,
	RTW_DATA_RATE_HE_NSS2_MCS8	= 0x198,
	RTW_DATA_RATE_HE_NSS2_MCS9	= 0x199,
	RTW_DATA_RATE_HE_NSS2_MCS10	= 0x19A,
	RTW_DATA_RATE_HE_NSS2_MCS11	= 0x19B,
	RTW_DATA_RATE_HE_NSS3_MCS0	= 0x1A0,
	RTW_DATA_RATE_HE_NSS3_MCS1	= 0x1A1,
	RTW_DATA_RATE_HE_NSS3_MCS2	= 0x1A2,
	RTW_DATA_RATE_HE_NSS3_MCS3	= 0x1A3,
	RTW_DATA_RATE_HE_NSS3_MCS4	= 0x1A4,
	RTW_DATA_RATE_HE_NSS3_MCS5	= 0x1A5,
	RTW_DATA_RATE_HE_NSS3_MCS6	= 0x1A6,
	RTW_DATA_RATE_HE_NSS3_MCS7	= 0x1A7,
	RTW_DATA_RATE_HE_NSS3_MCS8	= 0x1A8,
	RTW_DATA_RATE_HE_NSS3_MCS9	= 0x1A9,
	RTW_DATA_RATE_HE_NSS3_MCS10	= 0x1AA,
	RTW_DATA_RATE_HE_NSS3_MCS11	= 0x1AB,
	RTW_DATA_RATE_HE_NSS4_MCS0	= 0x1B0,
	RTW_DATA_RATE_HE_NSS4_MCS1	= 0x1B1,
	RTW_DATA_RATE_HE_NSS4_MCS2	= 0x1B2,
	RTW_DATA_RATE_HE_NSS4_MCS3	= 0x1B3,
	RTW_DATA_RATE_HE_NSS4_MCS4	= 0x1B4,
	RTW_DATA_RATE_HE_NSS4_MCS5	= 0x1B5,
	RTW_DATA_RATE_HE_NSS4_MCS6	= 0x1B6,
	RTW_DATA_RATE_HE_NSS4_MCS7	= 0x1B7,
	RTW_DATA_RATE_HE_NSS4_MCS8	= 0x1B8,
	RTW_DATA_RATE_HE_NSS4_MCS9	= 0x1B9,
	RTW_DATA_RATE_HE_NSS4_MCS10	= 0x1BA,
	RTW_DATA_RATE_HE_NSS4_MCS11	= 0x1BB,
	RTW_DATA_RATE_MAX = 0x1FF
};

enum rtw_gi_ltf {
	RTW_GILTF_LGI_4XHE32 = 0,
	RTW_GILTF_SGI_4XHE08 = 1,
	RTW_GILTF_2XHE16 = 2,
	RTW_GILTF_2XHE08 = 3,
	RTW_GILTF_1XHE16 = 4,
	RTW_GILTF_1XHE08 = 5,
	RTW_GILTF_MAX
};


/* 11ax spec define for HE Trigger Frame, only used for HE Trigger Frame! */
enum rtw_gi_ltf_ul_tb {
	RTW_TB_GILTF_1XHE16 = 0,
	RTW_TB_GILTF_2XHE16 = 1,
	RTW_TB_GILTF_4XHE32 = 2,
	RTW_TB_GILTF_MAX
};

#define RTW_PHL_MAX_RF_PATH 4
enum rf_path {
	RF_PATH_A = 0,
	RF_PATH_B = 1,
	RF_PATH_C = 2,
	RF_PATH_D = 3,
	RF_PATH_AB,
	RF_PATH_AC,
	RF_PATH_AD,
	RF_PATH_BC,
	RF_PATH_BD,
	RF_PATH_CD,
	RF_PATH_ABC,
	RF_PATH_ABD,
	RF_PATH_ACD,
	RF_PATH_BCD,
	RF_PATH_ABCD,
};

/*HW SPEC & SW/HW CAP*/
#define PROTO_CAP_11B		BIT0
#define PROTO_CAP_11G		BIT1
#define PROTO_CAP_11N		BIT2
#define PROTO_CAP_11AC		BIT3
#define PROTO_CAP_11AX		BIT4
#define PROTO_CAP_BIT_NUM	4

enum wlan_mode {
	WLAN_MD_INVALID = 0,
	WLAN_MD_11B	= BIT0,
	WLAN_MD_11A	= BIT1,
	WLAN_MD_11G	= BIT2,
	WLAN_MD_11N	= BIT3,
	WLAN_MD_11AC	= BIT4,
	WLAN_MD_11AX	= BIT5,

	/* Type for current wireless mode */
	WLAN_MD_11BG	= (WLAN_MD_11B | WLAN_MD_11G),
	WLAN_MD_11GN	= (WLAN_MD_11G | WLAN_MD_11N),
	WLAN_MD_11AN	= (WLAN_MD_11A | WLAN_MD_11N),
	WLAN_MD_11BN	= (WLAN_MD_11B | WLAN_MD_11N),
	WLAN_MD_11BGN	= (WLAN_MD_11B | WLAN_MD_11G | WLAN_MD_11N),
	WLAN_MD_11BGAC = (WLAN_MD_11B | WLAN_MD_11G | WLAN_MD_11AC),
	WLAN_MD_11BGAX = (WLAN_MD_11B | WLAN_MD_11G | WLAN_MD_11AX),
	WLAN_MD_11GAC  = (WLAN_MD_11G | WLAN_MD_11AC),
	WLAN_MD_11GAX  = (WLAN_MD_11G | WLAN_MD_11AX),
	WLAN_MD_11A_AC = (WLAN_MD_11A | WLAN_MD_11AC),
	WLAN_MD_11A_AX = (WLAN_MD_11A | WLAN_MD_11AX),

	/* Capability -Type for registry default wireless mode */
	WLAN_MD_11AGN	= (WLAN_MD_11A | WLAN_MD_11G | WLAN_MD_11N ),
	WLAN_MD_11ABGN	= (WLAN_MD_11A | WLAN_MD_11B | WLAN_MD_11G | WLAN_MD_11N ),
	WLAN_MD_11ANAC	= (WLAN_MD_11A | WLAN_MD_11N | WLAN_MD_11AC),
	WLAN_MD_11BGNAC = (WLAN_MD_11B | WLAN_MD_11G | WLAN_MD_11N | WLAN_MD_11AC),
	WLAN_MD_11GNAC  = (WLAN_MD_11G | WLAN_MD_11N | WLAN_MD_11AC),
	WLAN_MD_24G_MIX = (WLAN_MD_11B | WLAN_MD_11G | WLAN_MD_11N | WLAN_MD_11AC | WLAN_MD_11AX),
	WLAN_MD_5G_MIX	= (WLAN_MD_11A | WLAN_MD_11N | WLAN_MD_11AC | WLAN_MD_11AX),
	WLAN_MD_MAX	= (WLAN_MD_24G_MIX|WLAN_MD_5G_MIX),
};

typedef enum band_type {
	BAND_ON_24G	= 0,
	BAND_ON_5G	= 1,
	BAND_ON_6G	= 2,
	BAND_MAX,
} BAND_TYPE, *PBAND_TYPE; // NEO : add BAND_TYPE / PBAND_TYPE first

/*HW SPEC & SW/HW CAP*/
#define BAND_CAP_2G	BIT(BAND_ON_24G)
#define BAND_CAP_5G	BIT(BAND_ON_5G)
#define BAND_CAP_6G	BIT(BAND_ON_6G)
#define BAND_CAP_BIT_NUM	3

enum channel_width {
	CHANNEL_WIDTH_20	= 0,
	CHANNEL_WIDTH_40	= 1,
	CHANNEL_WIDTH_80	= 2,
	CHANNEL_WIDTH_160	= 3,
	CHANNEL_WIDTH_80_80	= 4,
	CHANNEL_WIDTH_5	= 5,
	CHANNEL_WIDTH_10	= 6,
	CHANNEL_WIDTH_MAX	= 7,
};

/*HW SPEC & SW/HW CAP*/
#if 0 // NEO : G6's definition is different with rtk_wifi_driver
#define BW_CAP_20M		BIT(CHANNEL_WIDTH_20)
#define BW_CAP_40M		BIT(CHANNEL_WIDTH_40)
#define BW_CAP_80M		BIT(CHANNEL_WIDTH_80)
#define BW_CAP_160M		BIT(CHANNEL_WIDTH_160)
#define BW_CAP_80_80M	BIT(CHANNEL_WIDTH_80_80)
#define BW_CAP_5M		BIT(CHANNEL_WIDTH_5)
#define BW_CAP_10M		BIT(CHANNEL_WIDTH_10)
#define BW_CAP_BIT_NUM	7
#endif


/*
 * Represent Extention Channel Offset in HT Capabilities
 * Secondary Channel Offset
 * 0 -SCN, 1 -SCA, 2 -RSVD, 3 - SCB
 *
 */
enum chan_offset {
	CHAN_OFFSET_NO_EXT = 0,	/*SCN - no secondary channel*/
	CHAN_OFFSET_UPPER = 1,		/*SCA - secondary channel above*/
	CHAN_OFFSET_NO_DEF = 2,	/*Reserved*/
	CHAN_OFFSET_LOWER = 3,		/*SCB - secondary channel below*/
};

// NEO : add RF_4T3R ... RF_1T3R first
enum rf_type {
	RF_1T1R	= 0,
	RF_1T2R	= 1,
	RF_2T2R	= 2,
	RF_2T3R	= 3,
	RF_2T4R	= 4,
	RF_3T3R	= 5,
	RF_3T4R	= 6,
	RF_4T4R	= 7,
	RF_4T3R = 8,
	RF_4T2R = 9,
	RF_4T1R = 10,
	RF_3T2R = 11,
	RF_3T1R = 12,
	RF_2T1R = 13,
	RF_1T4R = 14,
	RF_1T3R = 15,
	RF_TYPE_MAX,
};

enum rtw_rf_state {
	RTW_RF_ON,
	RTW_RF_OFF,
	RTW_RF_MAX
};

enum rtw_usb_speed {
	RTW_USB_SPEED_LOW	= 0,	/*U2 (2.0)- 1.0 - 1.5 Mbps - 0.192MBs*/
	RTW_USB_SPEED_FULL	= 1,	/*U2 (2.0)- 1.1 - 12 Mbps - 1.5MBs*/
	RTW_USB_SPEED_HIGH	= 2,	/*U2 (2.0)- 2.1 - 480 Mbps - 60MBs*/
	RTW_USB_SPEED_SUPER	= 3,	/*U3 (3.2 Gen 1)- 3.0 - 5 Gbps - 640MBs*/
	RTW_USB_SPEED_SUPER_10G = 4,	/*U3 (3.2 Gen 2)- 3.1 - 10 Gbps - 1280MBs*/
	RTW_USB_SPEED_SUPER_20G = 5,	/*U3 (3.2 Gen 2x2)- 3.2 - 20 Gbps - 2560MBs*/

	/* keep last */
	RTW_USB_SPEED_MAX,
	RTW_USB_SPEED_UNKNOWN = RTW_USB_SPEED_MAX,
};

#define USB_SUPER_SPEED_BULK_SIZE	1024	/* usb 3.0 */
#define USB_HIGH_SPEED_BULK_SIZE	512	/* usb 2.0 */
#define USB_FULL_SPEED_BULK_SIZE	64	/* usb 1.1 */

#define IV_LENGTH 8

enum rtw_enc_algo {
	RTW_ENC_NONE,
	RTW_ENC_WEP40,
	RTW_ENC_WEP104,
	RTW_ENC_TKIP,
	RTW_ENC_WAPI,
	RTW_ENC_GCMSMS4,
	RTW_ENC_CCMP,
	RTW_ENC_CCMP256,
	RTW_ENC_GCMP,
	RTW_ENC_GCMP256,
	RTW_ENC_BIP_CCMP128,
	RTW_ENC_MAX
};

enum rtw_sec_ent_mode {
	RTW_SEC_ENT_MODE_0,		/* No key */
	RTW_SEC_ENT_MODE_1,		/* WEP */
	RTW_SEC_ENT_MODE_2,		/* 2 unicast + 3 multicast + 2 BIP keys */
	RTW_SEC_ENT_MODE_3,		/* 2 unicast + 4 multicast + 1 BIP keys */
};

enum rtw_sec_key_type {
	RTW_SEC_KEY_UNICAST,
	RTW_SEC_KEY_MULTICAST,
	RTW_SEC_KEY_BIP,
	RTW_SEC_KEY_MAX
};

/**
 * Figure 27-7 + Table 9-31h from Ax Spec D4.2
 * B7-B1:
 * 	RU26 : 0 1 2 3 4 5 6 7 8 9 10 ... 36
 * 	RU52 : 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52
 * 	RU106 : 53 54 55 56 57 58 59 60
 * 	RU242 : 61 62 63 64
 * 	RU484 : 65 66
 * 	RU996 : 67
 * 	RU996x2: 68
 **/
enum rtw_he_ru_idx {
	/* 20MHz - 1 */
	RTW_HE_RU26_1 = 0,
	RTW_HE_RU26_2,
	RTW_HE_RU26_3,
	RTW_HE_RU26_4,
	RTW_HE_RU26_5,
	RTW_HE_RU26_6,
	RTW_HE_RU26_7,
	RTW_HE_RU26_8,
	RTW_HE_RU26_9,
	/* 20MHz - 2 */
	RTW_HE_RU26_10,
	RTW_HE_RU26_11,
	RTW_HE_RU26_12,
	RTW_HE_RU26_13,
	RTW_HE_RU26_14,
	RTW_HE_RU26_15,
	RTW_HE_RU26_16,
	RTW_HE_RU26_17,
	RTW_HE_RU26_18,
	/* Center 26-tone */
	RTW_HE_RU26_19,
	/* 20MHz - 3 */
	RTW_HE_RU26_20,
	RTW_HE_RU26_21,
	RTW_HE_RU26_22,
	RTW_HE_RU26_23,
	RTW_HE_RU26_24,
	RTW_HE_RU26_25,
	RTW_HE_RU26_26,
	RTW_HE_RU26_27,
	RTW_HE_RU26_28,
	/* 20MHz - 4 */
	RTW_HE_RU26_29,
	RTW_HE_RU26_30,
	RTW_HE_RU26_31,
	RTW_HE_RU26_32,
	RTW_HE_RU26_33,
	RTW_HE_RU26_34,
	RTW_HE_RU26_35,
	RTW_HE_RU26_36,
	RTW_HE_RU26_37 = 36,
	/* 20MHz - 1 */
	RTW_HE_RU52_1 = 37,
	RTW_HE_RU52_2,
	RTW_HE_RU52_3,
	RTW_HE_RU52_4,
	/* 20MHz - 2 */
	RTW_HE_RU52_5,
	RTW_HE_RU52_6,
	RTW_HE_RU52_7,
	RTW_HE_RU52_8,
	/* 20MHz - 3 */
	RTW_HE_RU52_9,
	RTW_HE_RU52_10,
	RTW_HE_RU52_11,
	RTW_HE_RU52_12,
	/* 20MHz - 4 */
	RTW_HE_RU52_13,
	RTW_HE_RU52_14,
	RTW_HE_RU52_15,
	RTW_HE_RU52_16 = 52,
	/* 20MHz - 1 */
	RTW_HE_RU106_1 = 53,
	RTW_HE_RU106_2,
	/* 20MHz - 2 */
	RTW_HE_RU106_3,
	RTW_HE_RU106_4,
	/* 20MHz - 3 */
	RTW_HE_RU106_5,
	RTW_HE_RU106_6,
	/* 20MHz - 4 */
	RTW_HE_RU106_7,
	RTW_HE_RU106_8 = 60,
	/* 20MHz  */
	RTW_HE_RU242_1 = 61,
	RTW_HE_RU242_2,
	RTW_HE_RU242_3,
	RTW_HE_RU242_4 = 64,
	/* 40MHz  */
	RTW_HE_RU484_1 = 65,
	RTW_HE_RU484_2 = 66,
	/* 80MHz  */
	RTW_HE_RU996_1 = 67,
	/* 160MHz  */
	RTW_HE_RU2x996_1 = 68,
};

enum rtw_protect_mode {
	RTW_PROTECT_DISABLE = 0,
	RTW_PROTECT_RTS = 1,
	RTW_PROTECT_CTS2SELF = 2,
	RTW_PROTECT_HW_RTS = 3
};

enum rtw_ac {
	RTW_AC_BE = 0,
	RTW_AC_BK = 1,
	RTW_AC_VI = 2,
	RTW_AC_VO = 3
};

enum rtw_edcca_mode {
	RTW_EDCCA_NORMAL,
	RTW_EDCCA_ETSI,
	RTW_EDCCA_JP,
	RTW_EDCCA_MAX
};

enum rtw_mac_pwr_st {
	RTW_MAC_PWR_NONE = 0,
	RTW_MAC_PWR_OFF = 1,
	RTW_MAC_PWR_ON = 2,
	RTW_MAC_PWR_LPS = 3,
	RTW_MAC_PWR_MAX = 0x4
};

enum rtw_pcie_bus_func_cap_t {
	RTW_PCIE_BUS_FUNC_DISABLE = 0,
	RTW_PCIE_BUS_FUNC_ENABLE = 1,
	RTW_PCIE_BUS_FUNC_DEFAULT = 2,
	RTW_PCIE_BUS_FUNC_IGNORE = 3
};

/*MAC_AX_PCIE_L0SDLY_IGNORE = 0xFF, MAC_AX_PCIE_L1DLY_IGNORE = 0xFF, MAC_AX_PCIE_CLKDLY_IGNORE = 0xFF */
#define RTW_PCIE_BUS_ASPM_DLY_IGNORE 0xFF /* Fully controlled by HW */

#define RTW_FRAME_TYPE_MGNT 0
#define RTW_FRAME_TYPE_CTRL 1
#define RTW_FRAME_TYPE_DATA 2
#define RTW_FRAME_TYPE_EXT_RSVD 3
/* Association Related PKT Type + SubType */

#define FRAME_OFFSET_FRAME_CONTROL		0
#define FRAME_OFFSET_DURATION			2
#define FRAME_OFFSET_ADDRESS1			4
#define FRAME_OFFSET_ADDRESS2			10
#define FRAME_OFFSET_ADDRESS3			16
#define FRAME_OFFSET_SEQUENCE			22
#define FRAME_OFFSET_ADDRESS4			24
#define PHL_GET_80211_HDR_TYPE(_hdr)	LE_BITS_TO_2BYTE((u8 *)_hdr, 2, 6)
#define PHL_GET_80211_HDR_MORE_FRAG(_hdr) LE_BITS_TO_2BYTE((u8 *)_hdr, 10, 1)
#define PHL_GET_80211_HDR_RETRY(_hdr)	LE_BITS_TO_2BYTE((u8 *)_hdr, 11, 1)
#define PHL_GET_80211_HDR_FRAG_NUM(_hdr)	LE_BITS_TO_2BYTE((u8 *)_hdr + 22, 0, 4)
#define PHL_GET_80211_HDR_SEQUENCE(_hdr)	LE_BITS_TO_2BYTE((u8 *)_hdr + 22, 4, 12)
#define PHL_GET_80211_HDR_ADDRESS2(_d, _hdr, _val) \
		_os_mem_cpy(_d, (u8 *)_val, (u8 *)_hdr + FRAME_OFFSET_ADDRESS2, 6)
#define PHL_GET_80211_HDR_ADDRESS3(_d, _hdr, _val) \
		_os_mem_cpy(_d, (u8 *)_val, (u8 *)_hdr + FRAME_OFFSET_ADDRESS3, 6)

#define RTW_FRAME_TYPE_BEACON 32
#define RTW_FRAME_TYPE_PROBE_RESP 20
#define RTW_FRAME_TYPE_ASOC_REQ 0
#define RTW_FRAME_TYPE_ASOC_RESP 4
#define RTW_FRAME_TYPE_REASOC_REQ 8
#define RTW_FRAME_TYPE_REASOC_RESP 12
#define RTW_IS_ASOC_PKT(_TYPE) \
	((_TYPE == RTW_FRAME_TYPE_REASOC_RESP) || \
	 (_TYPE == RTW_FRAME_TYPE_REASOC_REQ) || \
	 (_TYPE == RTW_FRAME_TYPE_ASOC_RESP) || \
	 (_TYPE == RTW_FRAME_TYPE_ASOC_REQ)) ? true : false

#define RTW_IS_ASOC_REQ_PKT(_TYPE) \
		((_TYPE == RTW_FRAME_TYPE_REASOC_REQ) || \
		 (_TYPE == RTW_FRAME_TYPE_ASOC_REQ)) ? true : false

#define RTW_IS_BEACON_OR_PROBE_RESP_PKT(_TYPE) \
	((_TYPE == RTW_FRAME_TYPE_BEACON) || \
	 (_TYPE == RTW_FRAME_TYPE_PROBE_RESP)) ? true : false

#define TU 1024 /* Time Unit (TU): 1024 us*/

#define RTW_MAX_ETH_PKT_LEN 1536

#endif /*_RTW_GENERAL_DEF_H_*/
