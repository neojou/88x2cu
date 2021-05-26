/******************************************************************************
 *
 * Copyright(c) 2019 Realtek Corporation. All rights reserved.
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
 ******************************************************************************/

#ifndef _MAC_DEF_H_
#define _MAC_DEF_H_



#include "pltfm_cfg.h"
#include "feature_cfg.h"
#include "chip_cfg.h"
#include "mac_ac/state_mach.h"
#include "errors.h"

#if MAC_AX_FEATURE_HV
#include "hv_type.h"
#endif

/*--------------------Define -------------------------------------------*/
#ifdef CONFIG_NEW_HALMAC_INTERFACE
#define PLTFM_SDIO_CMD52_R8(addr)                                              \
	hal_sdio_cmd52_r8(adapter->drv_adapter, addr)
#define PLTFM_SDIO_CMD53_R8(addr)                                              \
	hal_sdio_cmd53_r8(adapter->drv_adapter, addr)
#define PLTFM_SDIO_CMD53_R16(addr)                                             \
	hal_sdio_cmd53_r16(adapter->drv_adapter, addr)
#define PLTFM_SDIO_CMD53_R32(addr)                                             \
	hal_sdio_cmd53_r32(adapter->drv_adapter, addr)
#define PLTFM_SDIO_CMD53_RN(addr, size, val)                                   \
	hal_sdio_cmd53_rn(adapter->drv_adapter, addr, size, val)
#define PLTFM_SDIO_CMD52_W8(addr, val)                                         \
	hal_sdio_cmd52_w8(adapter->drv_adapter, addr, val)
#define PLTFM_SDIO_CMD53_W8(addr, val)                                         \
	hal_sdio_cmd53_w8(adapter->drv_adapter, addr, val)
#define PLTFM_SDIO_CMD53_WN(addr, size, val)                                   \
	hal_sdio_cmd53_wn(adapter->drv_adapter, addr, size, val)
#define PLTFM_SDIO_CMD53_W16(addr, val)                                        \
	hal_sdio_cmd53_w16(adapter->drv_adapter, addr, val)
#define PLTFM_SDIO_CMD53_W32(addr, val)                                        \
	hal_sdio_cmd53_w32(adapter->drv_adapter, addr, val)
#define PLTFM_SDIO_CMD52_CIA_R8(addr)                                          \
	hal_sdio_read_cia_r8(adapter->drv_adapter, addr)

#define PLTFM_TX(buf, len)                                                     \
	hal_tx(adapter->drv_adapter, buf, len)

#define PLTFM_FREE(buf, size)                                                  \
	hal_mem_free(adapter->drv_adapter, buf, size)
#define PLTFM_MALLOC(size)                                                     \
	hal_mem_alloc(adapter->drv_adapter, size)
#define PLTFM_MEMCPY(dest, src, size)                                          \
	hal_mem_cpy(adapter->drv_adapter, dest, src, size)
#define PLTFM_MEMSET(addr, value, size)                                        \
	hal_mem_set(adapter->drv_adapter, addr, value, size)
#define PLTFM_MEMCMP(ptr1, ptr2, num)                                          \
	hal_mem_cmp(adapter->drv_adapter, ptr1, ptr2, num)

#define PLTFM_DELAY_US(us)                                                     \
	hal_udelay(adapter->drv_adapter, us)
#define PLTFM_DELAY_MS(ms)                                                     \
	hal_mdelay(adapter->drv_adapter, ms)
#define PLTFM_MUTEX_INIT(mutex)                                                \
	hal_mutex_init(adapter->drv_adapter, mutex)
#define PLTFM_MUTEX_DEINIT(mutex)                                              \
	hal_mutex_deinit(adapter->drv_adapter, mutex)
#define PLTFM_MUTEX_LOCK(mutex)                                                \
	hal_mutex_lock(adapter->drv_adapter, mutex)
#define PLTFM_MUTEX_UNLOCK(mutex)                                              \
	hal_mutex_unlock(adapter->drv_adapter, mutex)

#define PLTFM_MSG_PRINT(...)	\
	hal_mac_msg_print(drv_adapter, __VA_ARGS__)

#define adapter_to_mac_ops(adapter) ((struct mac_ops *)((adapter)->ops))
#define adapter_to_intf_ops(adapter)                                           \
	((struct mac_intf_ops *)((adapter)->ops->intf_ops))

#define PLTFM_REG_R8(addr)                                                     \
	hal_read8(adapter->drv_adapter, addr)
#define PLTFM_REG_R16(addr)                                                    \
	hal_read16(adapter->drv_adapter, addr)
#define PLTFM_REG_R32(addr)                                                    \
	hal_read32(adapter->drv_adapter, addr)
#define PLTFM_REG_W8(addr, val)                                                \
	hal_write8(adapter->drv_adapter, addr, val)
#define PLTFM_REG_W16(addr, val)                                               \
	hal_write16(adapter->drv_adapter, addr, val)
#define PLTFM_REG_W32(addr, val)                                               \
	hal_write32(adapter->drv_adapter, addr, val)

#define MAC_REG_R8(addr) hal_read8(adapter->drv_adapter, addr)
#define MAC_REG_R16(addr) hal_read16(adapter->drv_adapter, addr)
#define MAC_REG_R32(addr) hal_read32(adapter->drv_adapter, addr)
#define MAC_REG_W8(addr, val) hal_write8(adapter->drv_adapter, addr, val)
#define MAC_REG_W16(addr, val) hal_write16(adapter->drv_adapter, addr, val)
#define MAC_REG_W32(addr, val) hal_write32(adapter->drv_adapter, addr, val)

#else

/* platform callback */
#define PLTFM_SDIO_CMD52_R8(addr)                                              \
	adapter->pltfm_cb->sdio_cmd52_r8(adapter->drv_adapter, addr)
#define PLTFM_SDIO_CMD53_R8(addr)                                              \
	adapter->pltfm_cb->sdio_cmd53_r8(adapter->drv_adapter, addr)
#define PLTFM_SDIO_CMD53_R16(addr)                                             \
	adapter->pltfm_cb->sdio_cmd53_r16(adapter->drv_adapter, addr)
#define PLTFM_SDIO_CMD53_R32(addr)                                             \
	adapter->pltfm_cb->sdio_cmd53_r32(adapter->drv_adapter, addr)
#define PLTFM_SDIO_CMD53_RN(addr, size, val)                                   \
	adapter->pltfm_cb->sdio_cmd53_rn(adapter->drv_adapter, addr, size, val)
#define PLTFM_SDIO_CMD52_W8(addr, val)                                         \
	adapter->pltfm_cb->sdio_cmd52_w8(adapter->drv_adapter, addr, val)
#define PLTFM_SDIO_CMD53_W8(addr, val)                                         \
	adapter->pltfm_cb->sdio_cmd53_w8(adapter->drv_adapter, addr, val)
#define PLTFM_SDIO_CMD53_W16(addr, val)                                        \
	adapter->pltfm_cb->sdio_cmd53_w16(adapter->drv_adapter, addr, val)
#define PLTFM_SDIO_CMD53_W32(addr, val)                                        \
	adapter->pltfm_cb->sdio_cmd53_w32(adapter->drv_adapter, addr, val)
#define PLTFM_SDIO_CMD53_WN(addr, size, val)                                   \
	adapter->pltfm_cb->sdio_cmd53_wn(adapter->drv_adapter, addr, size, val)
#define PLTFM_SDIO_CMD52_CIA_R8(addr)                                          \
	adapter->pltfm_cb->sdio_cmd52_cia_r8(adapter->drv_adapter, addr)

#define PLTFM_REG_R8(addr)                                                     \
	adapter->pltfm_cb->reg_r8(adapter->drv_adapter, addr)
#define PLTFM_REG_R16(addr)                                                    \
	adapter->pltfm_cb->reg_r16(adapter->drv_adapter, addr)
#define PLTFM_REG_R32(addr)                                                    \
	adapter->pltfm_cb->reg_r32(adapter->drv_adapter, addr)
#define PLTFM_REG_W8(addr, val)                                                \
	adapter->pltfm_cb->reg_w8(adapter->drv_adapter, addr, val)
#define PLTFM_REG_W16(addr, val)                                               \
	adapter->pltfm_cb->reg_w16(adapter->drv_adapter, addr, val)
#define PLTFM_REG_W32(addr, val)                                               \
	adapter->pltfm_cb->reg_w32(adapter->drv_adapter, addr, val)

#define PLTFM_SEND_RSVD_PAGE(skb)                                                          \
	adapter->pltfm_cb->send_rsvd_page(adapter->phl_adapter, adapter->drv_adapter, skb)

#if MAC_PHL_H2C
#define PLTFM_TX(buf)                                                          \
	adapter->pltfm_cb->tx(adapter->phl_adapter, adapter->drv_adapter, buf)
#define PLTFM_QUERY_H2C(type)                                                  \
	adapter->pltfm_cb->rtl_query_h2c(adapter->phl_adapter,                 \
					 adapter->drv_adapter, type)
#else
#define PLTFM_TX(buf, len)                                                     \
	adapter->pltfm_cb->tx(adapter->drv_adapter, buf, len)
#endif
#define PLTFM_FREE(buf, size)                                                  \
	adapter->pltfm_cb->rtl_free(adapter->drv_adapter, buf, size)
#define PLTFM_MALLOC(size)                                                     \
	adapter->pltfm_cb->rtl_malloc(adapter->drv_adapter, size)
#define PLTFM_MEMCPY(dest, src, size)                                          \
	adapter->pltfm_cb->rtl_memcpy(adapter->drv_adapter, dest, src, size)
#define PLTFM_MEMSET(addr, value, size)                                        \
	adapter->pltfm_cb->rtl_memset(adapter->drv_adapter, addr, value, size)
#define PLTFM_MEMCMP(ptr1, ptr2, num)                                          \
	adapter->pltfm_cb->rtl_memcmp(adapter->drv_adapter, ptr1, ptr2, num)
#define PLTFM_DELAY_US(us)                                                     \
	adapter->pltfm_cb->rtl_delay_us(adapter->drv_adapter, us)
#define PLTFM_DELAY_MS(ms)                                                     \
	adapter->pltfm_cb->rtl_delay_ms(adapter->drv_adapter, ms)

#define PLTFM_MUTEX_INIT(mutex)                                                \
	adapter->pltfm_cb->rtl_mutex_init(adapter->drv_adapter, mutex)
#define PLTFM_MUTEX_DEINIT(mutex)                                              \
	adapter->pltfm_cb->rtl_mutex_deinit(adapter->drv_adapter, mutex)
#define PLTFM_MUTEX_LOCK(mutex)                                                \
	adapter->pltfm_cb->rtl_mutex_lock(adapter->drv_adapter, mutex)
#define PLTFM_MUTEX_UNLOCK(mutex)                                              \
	adapter->pltfm_cb->rtl_mutex_unlock(adapter->drv_adapter, mutex)

#define PLTFM_EVENT_NOTIFY(mac_ft, stat, buf, size)                            \
	adapter->pltfm_cb->event_notify(adapter->drv_adapter, mac_ft, stat,    \
					buf, size)

#define PLTFM_MSG_PRINT(...)	\
	adapter->pltfm_cb->msg_print(drv_adapter, __VA_ARGS__)

#define adapter_to_mac_ops(adapter) ((struct mac_ops *)((adapter)->ops))
#define adapter_to_intf_ops(adapter)                                           \
	((struct mac_intf_ops *)((adapter)->ops->intf_ops))

#define MAC_REG_R8(addr) ops->reg_read8(adapter, addr)
#define MAC_REG_R16(addr) ops->reg_read16(adapter, addr)
#define MAC_REG_R32(addr) ops->reg_read32(adapter, addr)
#define MAC_REG_W8(addr, val) ops->reg_write8(adapter, addr, val)
#define MAC_REG_W16(addr, val) ops->reg_write16(adapter, addr, val)
#define MAC_REG_W32(addr, val) ops->reg_write32(adapter, addr, val)

//NEO
#define MAC_REG_W8_CLR(offset, mask)					\
	do {								\
		u32 __offset = (u32)offset;				\
		MAC_REG_W8(__offset, MAC_REG_R8(__offset) & ~(mask));	\
	} while (0)

#define MAC_REG_W16_CLR(offset, mask)					\
	do {								\
		u32 __offset = (u32)offset;				\
		MAC_REG_W16(__offset, MAC_REG_R16(__offset) & ~(mask));	\
	} while (0)

#define MAC_REG_W32_CLR(offset, mask)					\
	do {								\
		u32 __offset = (u32)offset;				\
		MAC_REG_W32(__offset, MAC_REG_R32(__offset) & ~(mask));	\
	} while (0)

#define MAC_REG_W8_SET(offset, mask)					\
	do {								\
		u8 __offset = (u32)offset;				\
		MAC_REG_W8(__offset, MAC_REG_R8(__offset) | mask);	\
	} while (0)

#define MAC_REG_W16_SET(offset, mask)					\
	do {								\
		u32 __offset = (u32)offset;				\
		MAC_REG_W16(__offset, MAC_REG_R16(__offset) | mask);	\
	} while (0)

#define MAC_REG_W32_SET(offset, mask)					\
	do {								\
		u32 __offset = (u32)offset;				\
		MAC_REG_W32(__offset, MAC_REG_R32(__offset) | mask);	\
	} while (0)

#endif /*CONFIG_NEW_HALMAC_INTERFACE*/

/*--------------------Define MACRO--------------------------------------*/
#define MAC_AX_MAX_RU_NUM	4
#define WLAN_ADDR_LEN			6
#define MAX_VHT_SUPPORT_SOUND_STA	4
#define MAX_HE_SUPPORT_SOUND_STA	8
#define MAC_AX_BCN_INTERVAL_DEFAULT 100
/*--------------------Define Enum---------------------------------------*/
enum mac_intf {
	MAC_INTF_USB,
	MAC_INTF_SDIO,
	MAC_INTF_PCIE,

	/* keep last */
	MAC_INTF_LAST,
	MAC_INTF_MAX = MAC_INTF_LAST,
	MAC_INTF_INVALID = MAC_INTF_LAST,
};


enum mac_feature {
	MAC_FT_DUMP_EFUSE,

	/* keep last */
	MAC_FT_LAST,
	MAC_FT_MAX = MAC_FT_LAST,
	MAC_FT_INVALID = MAC_FT_LAST,
};

enum mac_status {
	MAC_STATUS_IDLE,
	MAC_STATUS_PROC,
	MAC_STATUS_DONE,
	MAC_STATUS_ERR,
};

#if 0 // NEO
enum mac_ax_sdio_4byte_mode {
	MAC_AX_SDIO_4BYTE_MODE_DISABLE,
	MAC_AX_SDIO_4BYTE_MODE_RW,

	/* keep last */
	MAC_AX_SDIO_4BYTE_MODE_LAST,
	MAC_AX_SDIO_4BYTE_MODE_MAX = MAC_AX_SDIO_4BYTE_MODE_LAST,
	MAC_AX_SDIO_4BYTE_MODE_INVALID = MAC_AX_SDIO_4BYTE_MODE_LAST,
};

enum mac_ax_sdio_tx_mode {
	MAC_AX_SDIO_TX_MODE_AGG,
	MAC_AX_SDIO_TX_MODE_DUMMY_BLOCK,
	MAC_AX_SDIO_TX_MODE_DUMMY_AUTO,

	/* keep last */
	MAC_AX_SDIO_TX_MODE_LAST,
	MAC_AX_SDIO_TX_MODE_MAX = MAC_AX_SDIO_TX_MODE_LAST,
	MAC_AX_SDIO_TX_MODE_INVALID = MAC_AX_SDIO_TX_MODE_LAST,
};

enum mac_ax_sdio_spec_ver {
	MAC_AX_SDIO_SPEC_VER_2_00,
	MAC_AX_SDIO_SPEC_VER_3_00,

	/* keep last */
	MAC_AX_SDIO_SPEC_VER_LAST,
	MAC_AX_SDIO_SPEC_VER_MAX = MAC_AX_SDIO_SPEC_VER_LAST,
	MAC_AX_SDIO_SPEC_VER_INVALID = MAC_AX_SDIO_SPEC_VER_LAST,
};

enum mac_ax_lv1_rcvy_step {
	MAC_AX_LV1_RCVY_STEP_1 = 0,
	MAC_AX_LV1_RCVY_STEP_2,

	/* keep last */
	MAC_AX_LV1_RCVY_STEP_LAST,
	MAC_AX_LV1_RCVY_STEP_MAX = MAC_AX_LV1_RCVY_STEP_LAST,
	MAC_AX_LV1_RCVY_STEP_INVALID = MAC_AX_LV1_RCVY_STEP_LAST,
};

enum mac_ax_ex_shift {
	MAC_AX_NO_SHIFT    = 0,
	MAC_AX_BYTE_ALIGNED_4 = 1,
	MAC_AX_BYTE_ALIGNED_8   = 2
};

enum mac_ax_ps_mode {
	MAC_AX_PS_MODE_ACTIVE = 0,
	MAC_AX_PS_MODE_LEGACY = 1,
	MAC_AX_PS_MODE_WMMPS  = 2,
	MAC_AX_PS_MODE_MAX    = 3,
};

enum mac_ax_pwr_state_action {
	MAC_AX_PWR_STATE_ACT_REQ = 0,
	MAC_AX_PWR_STATE_ACT_CHK = 1,
	MAC_AX_PWR_STATE_ACT_MAX,
};

enum mac_ax_rpwm_req_pwr_state {
	MAC_AX_RPWM_REQ_PWR_STATE_ACTIVE = 0,
	MAC_AX_RPWM_REQ_PWR_STATE_BAND0_RFON = 1,
	MAC_AX_RPWM_REQ_PWR_STATE_BAND1_RFON = 2,
	MAC_AX_RPWM_REQ_PWR_STATE_BAND0_RFOFF = 3,
	MAC_AX_RPWM_REQ_PWR_STATE_BAND1_RFOFF = 4,
	MAC_AX_RPWM_REQ_PWR_STATE_CLK_GATED = 5,
	MAC_AX_RPWM_REQ_PWR_STATE_PWR_GATED = 6,
	MAC_AX_RPWM_REQ_PWR_STATE_HIOE_PWR_GATED = 7,
	MAC_AX_RPWM_REQ_PWR_STATE_MAX,
};

enum mac_ax_port_cfg_type {
	MAC_AX_PCFG_FUNC_SW = 0,
	MAC_AX_PCFG_TX_SW,
	MAC_AX_PCFG_TX_RPT,
	MAC_AX_PCFG_RX_SW,
	MAC_AX_PCFG_RX_RPT,
	MAC_AX_PCFG_RX_SYNC,
	MAC_AX_PCFG_BCN_PRCT,
	MAC_AX_PCFG_TBTT_AGG,
	MAC_AX_PCFG_TBTT_SHIFT,
	MAC_AX_PCFG_RST_TSF,
	MAC_AX_PCFG_RST_TPR,
	MAC_AX_PCFG_BCAID,
	MAC_AX_PCFG_HIQ_WIN,
	MAC_AX_PCFG_HIQ_DTIM,
	MAC_AX_PCFG_HIQ_NOLIMIT,
	MAC_AX_PCFG_NET_TYPE,
	MAC_AX_PCFG_BCN_INTV,
	MAC_AX_PCFG_BCN_SETUP_TIME,
	MAC_AX_PCFG_BCN_HOLD_TIME,
	MAC_AX_PCFG_MBSSID_EN,
	MAC_AX_PCFG_BCN_ERLY,
	MAC_AX_PCFG_BCN_MASK_AREA,
	MAC_AX_PCFG_TBTT_ERLY,
	MAC_AX_PCFG_BSS_CLR,
	MAC_AX_PCFG_BCN_DRP_EN,
};

#endif // NEO

enum mac_band {
	MAC_AX_BAND_0 = 0,
	MAC_AX_BAND_1 = 1,
	MAC_AX_BAND_NUM = 2
};


enum mac_port {
	MAC_PORT_0 = 0,
	MAC_PORT_1 = 1,
	MAC_PORT_2 = 2,
	MAC_PORT_3 = 3,
	MAC_PORT_4 = 4,
	MAC_PORT_NUM
};

#if 0 //NEO

enum mac_ax_mbssid_idx {
	MAC_AX_P0_ROOT = 0,
	MAC_AX_P0_MBID1,
	MAC_AX_P0_MBID2,
	MAC_AX_P0_MBID3,
	MAC_AX_P0_MBID4,
	MAC_AX_P0_MBID5,
	MAC_AX_P0_MBID6,
	MAC_AX_P0_MBID7,
	MAC_AX_P0_MBID8,
	MAC_AX_P0_MBID9,
	MAC_AX_P0_MBID10,
	MAC_AX_P0_MBID11,
	MAC_AX_P0_MBID12,
	MAC_AX_P0_MBID13,
	MAC_AX_P0_MBID14,
	MAC_AX_P0_MBID15,

	/* keep last */
	MAC_AX_P0_MBID_LAST,
	MAC_AX_P0_MBID_MAX = MAC_AX_P0_MBID_LAST,
	MAC_AX_P0_MBID_INVALID = MAC_AX_P0_MBID_LAST,
};

enum mac_ax_hwmod_sel {
	MAC_AX_DMAC_SEL = 0,
	MAC_AX_CMAC_SEL = 1,

	/* keep last */
	MAC_AX_MAC_LAST,
	MAC_AX_MAC_MAX = MAC_AX_MAC_LAST,
	MAC_AX_MAC_INVALID = MAC_AX_MAC_LAST,
};

#endif // NEO

enum mac_hw_id {
	/* Get HW value */
	MAC_HW_MAPPING = 0x00,
	MAC_HW_SDIO_MON_INT,
	MAC_HW_SDIO_MON_CNT,
	MAC_HW_GET_ID_PAUSE,
	MAC_HW_SDIO_TX_AGG_SIZE,
	MAC_HW_GET_AMPDU_CFG,
	MAC_HW_GET_EDCA_PARAM,
	MAC_HW_GET_EDCCA_PARAM,
	MAC_HW_GET_MUEDCA_PARAM,
	MAC_HW_GET_MUEDCA_TIMER,
	MAC_HW_GET_TBPPDU_CTRL,
	MAC_HW_GET_SCH_TXEN_STATUS,
	MAC_HW_GET_MUEDCA_CTRL,
	MAC_HW_GET_DELAYTX_CFG,
	MAC_HW_GET_SS_WMM_TBL,
	MAC_HW_GET_EFUSE_SIZE,
	MAC_HW_GET_LOGICAL_EFUSE_SIZE,
	MAC_HW_GET_LIMIT_LOG_EFUSE_SIZE,
	MAC_HW_GET_BT_EFUSE_SIZE,
	MAC_HW_GET_BT_LOGICAL_EFUSE_SIZE,
	MAC_HW_GET_EFUSE_MASK_SIZE,
	MAC_HW_GET_LIMIT_EFUSE_MASK_SIZE,
	MAC_HW_GET_BT_EFUSE_MASK_SIZE,
	MAC_HW_GET_CH_STAT_CNT,
	MAC_HW_GET_LIFETIME_CFG,
	MAC_HW_GET_APP_FCS,
	MAC_HW_GET_RX_ICVERR,
	MAC_HW_GET_PWR_STATE,
	MAC_HW_GET_WAKE_REASON,
	MAC_HW_GET_SCOREBOARD,
	MAC_HW_GET_COEX_GNT,
	MAC_HW_GET_RRSR,
	MAC_HW_GET_COEX_CTRL,
	MAC_HW_GET_TX_CNT,
	MAC_AX_TX_TF_INFO,
	MAC_HW_GET_TSF,
	MAC_HW_GET_MAX_TX_TIME,
	MAC_HW_GET_POLLUTED_CNT,
	MAC_HW_GET_DATA_RTY_LMT,
	MAC_HW_GET_DFLT_NAV,
	/* Set HW value */
	MAC_HW_SETTING = 0x60,
	MAC_HW_SDIO_INFO,
	MAC_HW_SDIO_TX_MODE,
	MAC_HW_SDIO_RX_AGG,
	MAC_HW_SDIO_TX_AGG,
	MAC_HW_SDIO_AVAL_PAGE,
	MAC_HW_SDIO_MON_WT,
	MAC_HW_SDIO_MON_CLK,
	MAC_HW_PCIE_CFGSPC_SET,
	MAC_HW_PCIE_RST_BDRAM,
	MAC_HW_PCIE_LTR_SW_TRIGGER,
	MAC_HW_PCIE_MIT,
	MAC_HW_PCIE_L2_LEAVE,
	MAC_HW_SET_ID_PAUSE,
	MAC_HW_SET_AMPDU_CFG,
	MAC_HW_SET_USR_EDCA_PARAM,
	MAC_HW_SET_EDCA_PARAM,
	MAC_HW_SET_EDCCA_PARAM,
	MAC_HW_SET_MUEDCA_PARAM,
	MAC_HW_SET_TBPPDU_CTRL,
	MAC_HW_SET_SCH_TXEN_CFG,
	MAC_HW_SET_MUEDCA_TIMER,
	MAC_HW_SET_HOST_RPR,
	MAC_HW_SET_MUEDCA_CTRL,
	MAC_HW_SET_DELAYTX_CFG,
	MAC_HW_SET_BW_CFG,
	MAC_HW_SET_BT_BLOCK_TX,
	MAC_HW_SET_CH_BUSY_STAT_CFG,
	MAC_HW_SET_LIFETIME_CFG,
	MAC_HW_EN_BB_RF,
	MAC_HW_SET_APP_FCS,
	MAC_HW_SET_RX_ICVERR,
	MAC_HW_SET_CCTL_RTY_LMT,
	MAC_HW_SET_CR_RTY_LMT,
	MAC_HW_SET_COEX_GNT,
	MAC_HW_SET_SCOREBOARD,
	MAC_HW_SET_POLLUTED,
	MAC_HW_SET_RRSR,
	MAC_HW_SET_COEX_CTRL,
	MAC_HW_SET_CLR_TX_CNT,
	MAC_HW_SET_SLOT_TIME,
	MAC_HW_SET_XTAL_AAC_MODE,
	MAC_HW_SET_NAV_PADDING,
	MAC_HW_SET_MAX_TX_TIME,
};

#if 0 //NEO

enum mac_ax_rx_agg_mode {
	MAC_AX_RX_AGG_MODE_NONE,
	MAC_AX_RX_AGG_MODE_DMA,
	MAC_AX_RX_AGG_MODE_USB,

	/* keep last */
	MAC_AX_RX_AGG_MODE_LAST,
	MAC_AX_RX_AGG_MODE_MAX = MAC_AX_RX_AGG_MODE_LAST,
	MAC_AX_RX_AGG_MODE_INVALID = MAC_AX_RX_AGG_MODE_LAST,
};

enum mac_ax_cmac_ac_sel {
	MAC_AX_CMAC_AC_SEL_BE = 0,
	MAC_AX_CMAC_AC_SEL_BK = 1,
	MAC_AX_CMAC_AC_SEL_VI = 2,
	MAC_AX_CMAC_AC_SEL_VO = 3,

	/* keep last */
	MAC_AX_CMAC_AC_SEL_LAST,
	MAC_AX_CMAC_AC_SEL_MAX = MAC_AX_CMAC_AC_SEL_LAST,
	MAC_AX_CMAC_AC_SEL_INVALID = MAC_AX_CMAC_AC_SEL_LAST,
};

enum mac_ax_cmac_path_sel {
	MAC_AX_CMAC_PATH_SEL_BE0,
	MAC_AX_CMAC_PATH_SEL_BK0,
	MAC_AX_CMAC_PATH_SEL_VI0,
	MAC_AX_CMAC_PATH_SEL_VO0,
	MAC_AX_CMAC_PATH_SEL_BE1,
	MAC_AX_CMAC_PATH_SEL_BK1,
	MAC_AX_CMAC_PATH_SEL_VI1,
	MAC_AX_CMAC_PATH_SEL_VO1,
	MAC_AX_CMAC_PATH_SEL_MG0_1,
	MAC_AX_CMAC_PATH_SEL_MG2,
	MAC_AX_CMAC_PATH_SEL_BCN,
	MAC_AX_CMAC_PATH_SEL_TF,
	MAC_AX_CMAC_PATH_SEL_TWT0,
	MAC_AX_CMAC_PATH_SEL_TWT1,

	/* keep last */
	MAC_AX_CMAC_PATH_SEL_LAST,
	MAC_AX_CMAC_PATH_SEL_MAX = MAC_AX_CMAC_PATH_SEL_LAST,
	MAC_AX_CMAC_PATH_SEL_INVALID = MAC_AX_CMAC_PATH_SEL_LAST,
};

enum mac_ax_cmac_usr_edca_idx {
	MAC_AX_CMAC_USR_EDCA_IDX_0 = 0,
	MAC_AX_CMAC_USR_EDCA_IDX_1 = 1,
	MAC_AX_CMAC_USR_EDCA_IDX_2 = 2,
	MAC_AX_CMAC_USR_EDCA_IDX_3 = 3,
};

enum mac_ax_cmac_wmm_sel {
	MAC_AX_CMAC_WMM0_SEL = 0,
	MAC_AX_CMAC_WMM1_SEL = 1,
};

enum mac_ax_ss_wmm_tbl {
	MAC_AX_SS_WMM_TBL_C0_WMM0 = 0,
	MAC_AX_SS_WMM_TBL_C0_WMM1 = 1,
	MAC_AX_SS_WMM_TBL_C1_WMM0 = 2,
	MAC_AX_SS_WMM_TBL_C1_WMM1 = 3,
};

enum mac_ax_mcc_status {
	MAC_AX_MCC_ADD_ROLE_OK = 0,
	MAC_AX_MCC_START_GROUP_OK = 1,
	MAC_AX_MCC_STOP_GROUP_OK = 2,
	MAC_AX_MCC_DEL_GROUP_OK = 3,
	MAC_AX_MCC_RESET_GROUP_OK = 4,
	MAC_AX_MCC_SWITCH_CH_OK = 5,
	MAC_AX_MCC_TXNULL0_OK = 6,
	MAC_AX_MCC_TXNULL1_OK = 7,

	MAC_AX_MCC_SWITCH_EARLY = 10,
	MAC_AX_MCC_TBTT = 11,

	MAC_AX_MCC_ADD_ROLE_FAIL = 20,
	MAC_AX_MCC_START_GROUP_FAIL = 21,
	MAC_AX_MCC_STOP_GROUP_FAIL = 22,
	MAC_AX_MCC_DEL_GROUP_FAIL = 23,
	MAC_AX_MCC_RESET_GROUP_FAIL = 24,
	MAC_AX_MCC_SWITCH_CH_FAIL = 25,
	MAC_AX_MCC_TXNULL0_FAIL = 26,
	MAC_AX_MCC_TXNULL1_FAIL = 27,
};

enum mac_ax_trx_mitigation_timer_unit {
	MAC_AX_MIT_64US,
	MAC_AX_MIT_128US,
	MAC_AX_MIT_256US,
	MAC_AX_MIT_512US
};

enum mac_ax_wow_wake_reason {
	MAC_AX_WOW_RX_PAIRWISEKEY = 0x01,
	MAC_AX_WOW_RX_GTK = 0x02,
	MAC_AX_WOW_RX_FOURWAY_HANDSHAKE = 0x03,
	MAC_AX_WOW_RX_DISASSOC = 0x04,
	MAC_AX_WOW_RX_DEAUTH = 0x08,
	MAC_AX_WOW_RX_ARP_REQUEST = 0x09,
	MAC_AX_WOW_RX_NS = 0x0A,
	MAC_AX_WOW_RX_EAPREQ_IDENTIFY = 0x0B,
	MAC_AX_WOW_FW_DECISION_DISCONNECT = 0x10,
	MAC_AX_WOW_RX_MAGIC_PKT = 0x21,
	MAC_AX_WOW_RX_UNICAST_PKT = 0x22,
	MAC_AX_WOW_RX_PATTERN_PKT = 0x23,
	MAC_AX_WOW_RTD3_SSID_MATCH = 0x24,
	MAC_AX_WOW_RX_DATA_PKT = 0x25,
	MAC_AX_WOW_RX_SSDP_MATCH = 0x26,
	MAC_AX_WOW_RX_WSD_MATCH = 0x27,
	MAC_AX_WOW_RX_SLP_MATCH = 0x28,
	MAC_AX_WOW_RX_LLTD_MATCH = 0x29,
	MAC_AX_WOW_RX_MDNS_MATCH = 0x2A,
	MAC_AX_WOW_RX_REALWOW_V2_WAKEUP_PKT = 0x30,
	MAC_AX_WOW_RX_REALWOW_V2_ACK_LOST = 0x31,
	MAC_AX_WOW_RX_REALWOW_V2_TX_KAPKT = 0x32,
	MAC_AX_WOW_ENABLE_FAIL_DMA_IDLE = 0x40,
	MAC_AX_WOW_ENABLE_FAIL_DMA_PAUSE = 0x41,
	MAC_AX_WOW_RTIME_FAIL_DMA_IDLE = 0x42,
	MAC_AX_WOW_RTIME_FAIL_DMA_PAUSE = 0x43,
	MAC_AX_WOW_RX_SNMP_MISMATCHED_PKT = 0x50,
	MAC_AX_WOW_RX_DESIGNATED_MAC_PKT = 0x51,
	MAC_AX_WOW_NLO_SSID_MACH = 0x55,
	MAC_AX_WOW_AP_OFFLOAD_WAKEUP = 0x66,
	MAC_AX_WOW_DMAC_ERROR_OCCURRED = 0x70,
	MAC_AX_WOW_EXCEPTION_OCCURRED = 0x71,
	MAC_AX_WOW_RX_ACTION = 0xD0,
	MAC_AX_WOW_CLK_32K_UNLOCK = 0xFD,
	MAC_AX_WOW_CLK_32K_LOCK = 0xFE
};

enum mac_ax_wow_fw_status {
	MAC_AX_WOW_NOT_READY,
	MAC_AX_WOW_SLEEP,
	MAC_AX_WOW_RESUME
};

enum mac_ax_wow_ctrl {
	MAC_AX_WOW_ENTER,
	MAC_AX_WOW_LEAVE
};

#endif //NEO

enum mac_pwr_st {
	MAC_PWR_MAC_OFF = 0,
	MAC_PWR_MAC_ON = 1,
	MAC_PWR_MAC_LPS = 2
};

#if 0 //NEO
/*--------------------Define DBG and recovery related enum--------------------*/
enum mac_ax_err_info {
	// Get error info
	// L0
	MAC_AX_ERR_L0_ERR_CMAC0 = 0x0001,
	MAC_AX_ERR_L0_ERR_CMAC1 = 0x0002,
	MAC_AX_ERR_L0_RESET_DONE = 0x0003,
	MAC_AX_ERR_L0_PROMOTE_TO_L1 = 0x0010,
	// L1
	MAC_AX_ERR_L1_ERR_DMAC = 0x1000,
	MAC_AX_ERR_L1_RESET_DISABLE_DMAC_DONE = 0x1001,
	MAC_AX_ERR_L1_RESET_RECOVERY_DONE = 0x1002,
	MAC_AX_ERR_L1_PROMOTE_TO_L2 = 0x1010,
	// L2
	// address hole (master)
	MAC_AX_ERR_L2_ERR_AH_DMA = 0x2000,
	MAC_AX_ERR_L2_ERR_AH_HCI = 0x2010,
	MAC_AX_ERR_L2_ERR_AH_RLX4081 = 0x2020,
	MAC_AX_ERR_L2_ERR_AH_IDDMA = 0x2030,
	MAC_AX_ERR_L2_ERR_AH_HIOE = 0x2040,
	MAC_AX_ERR_L2_ERR_AH_IPSEC = 0x2050,
	MAC_AX_ERR_L2_ERR_AH_RX4281 = 0x2060,
	MAC_AX_ERR_L2_ERR_AH_OTHERS = 0x2070,
	// AHB bridge timeout (master)
	MAC_AX_ERR_L2_ERR_AHB_TO_DMA = 0x2100,
	MAC_AX_ERR_L2_ERR_AHB_TO_HCI = 0x2110,
	MAC_AX_ERR_L2_ERR_AHB_TO_RLX4081 = 0x2120,
	MAC_AX_ERR_L2_ERR_AHB_TO_IDDMA = 0x2130,
	MAC_AX_ERR_L2_ERR_AHB_TO_HIOE = 0x2140,
	MAC_AX_ERR_L2_ERR_AHB_TO_IPSEC = 0x2150,
	MAC_AX_ERR_L2_ERR_AHB_TO_RX4281 = 0x2160,
	MAC_AX_ERR_L2_ERR_AHB_TO_OTHERS = 0x2170,
	// APB_SA bridge timeout (master + slave)
	MAC_AX_ERR_L2_ERR_APB_SA_TO_DMA_WVA = 0x2200,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_DMA_UART = 0x2201,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_DMA_CPULOCAL = 0x2202,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_DMA_AXIDMA = 0x2203,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_DMA_HIOE = 0x2204,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_DMA_IDDMA = 0x2205,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_DMA_IPSEC = 0x2206,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_DMA_WON = 0x2207,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_DMA_WDMAC = 0x2208,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_DMA_WCMAC = 0x2209,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_DMA_OTHERS = 0x220A,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HCI_WVA = 0x2210,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HCI_UART = 0x2211,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HCI_CPULOCAL = 0x2212,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HCI_AXIDMA = 0x2213,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HCI_HIOE = 0x2214,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HCI_IDDMA = 0x2215,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HCI_IPSEC = 0x2216,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HCI_WDMAC = 0x2218,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HCI_WCMAC = 0x2219,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HCI_OTHERS = 0x221A,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RLX4081_WVA = 0x2220,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RLX4081_UART = 0x2221,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RLX4081_CPULOCAL = 0x2222,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RLX4081_AXIDMA = 0x2223,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RLX4081_HIOE = 0x2224,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RLX4081_IDDMA = 0x2225,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RLX4081_IPSEC = 0x2226,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RLX4081_WON = 0x2227,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RLX4081_WDMAC = 0x2228,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RLX4081_WCMAC = 0x2229,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RLX4081_OTHERS = 0x222A,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IDDMA_WVA = 0x2230,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IDDMA_UART = 0x2231,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IDDMA_CPULOCAL = 0x2232,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IDDMA_AXIDMA = 0x2233,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IDDMA_HIOE = 0x2234,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IDDMA_IDDMA = 0x2235,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IDDMA_IPSEC = 0x2236,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IDDMA_WON = 0x2237,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IDDMA_WDMAC = 0x2238,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IDDMA_WCMAC = 0x2239,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IDDMA_OTHERS = 0x223A,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HIOE_WVA = 0x2240,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HIOE_UART = 0x2241,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HIOE_CPULOCAL = 0x2242,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HIOE_AXIDMA = 0x2243,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HIOE_HIOE = 0x2244,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HIOE_IDDMA = 0x2245,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HIOE_IPSEC = 0x2246,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HIOE_WON = 0x2247,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HIOE_WDMAC = 0x2248,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HIOE_WCMAC = 0x2249,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_HIOE_OTHERS = 0x224A,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IPSEC_WVA = 0x2250,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IPSEC_UART = 0x2251,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IPSEC_CPULOCAL = 0x2252,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IPSEC_AXIDMA = 0x2253,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IPSEC_HIOE = 0x2254,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IPSEC_IDDMA = 0x2255,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IPSEC_IPSEC = 0x2256,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IPSEC_WON = 0x2257,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IPSEC_WDMAC = 0x2258,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IPSEC_WCMAC = 0x2259,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_IPSEC_OTHERS = 0x225A,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RX4281_WVA = 0x2260,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RX4281_UART = 0x2261,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RX4281_CPULOCAL = 0x2262,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RX4281_AXIDMA = 0x2263,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RX4281_HIOE = 0x2264,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RX4281_IDDMA = 0x2265,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RX4281_IPSEC = 0x2266,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RX4281_WON = 0x2267,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RX4281_WDMAC = 0x2268,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RX4281_WCMAC = 0x2269,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_RX4281_OTHERS = 0x226A,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_OTHERS_WVA = 0x2270,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_OTHERS_UART = 0x2271,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_OTHERS_CPULOCAL = 0x2272,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_OTHERS_AXIDMA = 0x2273,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_OTHERS_HIOE = 0x2274,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_OTHERS_IDDMA = 0x2275,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_OTHERS_IPSEC = 0x2276,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_OTHERS_WON = 0x2277,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_OTHERS_WDMAC = 0x2278,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_OTHERS_WCMAC = 0x2279,
	MAC_AX_ERR_L2_ERR_APB_SA_TO_OTHERS_OTHERS = 0x227A,
	// APB_BBRF bridge timeout (master)
	MAC_AX_ERR_L2_ERR_APB_BBRF_TO_DMA = 0x2300,
	MAC_AX_ERR_L2_ERR_APB_BBRF_TO_HCI = 0x2310,
	MAC_AX_ERR_L2_ERR_APB_BBRF_TO_RLX4081 = 0x2320,
	MAC_AX_ERR_L2_ERR_APB_BBRF_TO_IDDMA = 0x2330,
	MAC_AX_ERR_L2_ERR_APB_BBRF_TO_HIOE = 0x2340,
	MAC_AX_ERR_L2_ERR_APB_BBRF_TO_IPSEC = 0x2350,
	MAC_AX_ERR_L2_ERR_APB_BBRF_TO_RX4281 = 0x2360,
	MAC_AX_ERR_L2_ERR_APB_BBRF_TO_OTHERS = 0x2370,
	MAC_AX_ERR_L2_RESET_DONE = 0x2400,
	MAC_AX_GET_ERR_MAX,

	// set error info
	MAC_AX_ERR_L1_DISABLE_EN = 0x0001,
	MAC_AX_ERR_L1_RCVY_EN = 0x0002,
	MAC_AX_ERR_L0_CFG_NOTIFY = 0x0010,
	MAC_AX_ERR_L0_CFG_DIS_NOTIFY = 0x0011,
	MAC_AX_ERR_L0_CFG_HANDSHAKE = 0x0012,
	MAC_AX_ERR_L0_RCVY_EN = 0x0013,
	MAC_AX_SET_ERR_MAX,
};

enum mac_ax_mem_sel {
	MAC_AX_MEM_AXIDMA,
	MAC_AX_MEM_SHARED_BUF,
	MAC_AX_MEM_DMAC_TBL,
	MAC_AX_MEM_SHCUT_MACHDR,
	MAC_AX_MEM_STA_SCHED,
	MAC_AX_MEM_RXPLD_FLTR_CAM,
	MAC_AX_MEM_SECURITY_CAM,
	MAC_AX_MEM_WOW_CAM,
	MAC_AX_MEM_CMAC_TBL,
	MAC_AX_MEM_ADDR_CAM,
	MAC_AX_MEM_BA_CAM,
	MAC_AX_MEM_BCN_IE_CAM0,
	MAC_AX_MEM_BCN_IE_CAM1,
	MAC_AX_MEM_TXD_FIFO_0,
	MAC_AX_MEM_TXD_FIFO_1,

	/* keep last */
	MAC_AX_MEM_LAST,
	MAC_AX_MEM_MAX = MAC_AX_MEM_LAST,
	MAC_AX_MEM_INVALID = MAC_AX_MEM_LAST,
};

enum mac_ax_reg_sel {
	MAC_AX_REG_MAC,
	MAC_AX_REG_BB,
	MAC_AX_REG_IQK,
	MAC_AX_REG_RFC,

	/* keep last */
	MAC_AX_REG_LAST,
	MAC_AX_REG_MAX = MAC_AX_REG_LAST,
	MAC_AX_REG_INVALID = MAC_AX_REG_LAST,
};

/*--------------------Define GPIO related enum-------------------------------*/
enum mac_ax_gpio_func {
	MAC_AX_GPIO_SW_IO_0,
	MAC_AX_GPIO_SW_IO_1,
	MAC_AX_GPIO_SW_IO_2,
	MAC_AX_GPIO_SW_IO_3,
	MAC_AX_GPIO_SW_IO_4,
	MAC_AX_GPIO_SW_IO_5,
	MAC_AX_GPIO_SW_IO_6,
	MAC_AX_GPIO_SW_IO_7,
	MAC_AX_GPIO_SW_IO_8,
	MAC_AX_GPIO_SW_IO_9,
	MAC_AX_GPIO_SW_IO_10,
	MAC_AX_GPIO_SW_IO_11,
	MAC_AX_GPIO_SW_IO_12,
	MAC_AX_GPIO_SW_IO_13,
	MAC_AX_GPIO_SW_IO_14,
	MAC_AX_GPIO_SW_IO_15,
	MAC_AX_GPIO_UART_TX_GPIO5,
	MAC_AX_GPIO_UART_TX_GPIO7,
	MAC_AX_GPIO_UART_TX_GPIO8,
	MAC_AX_GPIO_UART_RX_GPIO6,
	MAC_AX_GPIO_UART_RX_GPIO14,
};

enum mac_ax_uart_tx_pin {
	MAC_AX_UART_TX_GPIO5,
	MAC_AX_UART_TX_GPIO7,
	MAC_AX_UART_TX_GPIO8,
	MAC_AX_UART_TX_GPIO5_GPIO8,
};

enum mac_ax_uart_rx_pin {
	MAC_AX_UART_RX_GPIO6,
	MAC_AX_UART_RX_GPIO14,
};

enum mac_ax_gfunc {
	MAC_AX_GPIO_WL_PD,
	MAC_AX_GPIO_BT_PD,
	MAC_AX_GPIO_WL_EXTWOL,
	MAC_AX_GPIO_BT_GPIO,
	MAC_AX_GPIO_WL_SDIO_INT,
	MAC_AX_GPIO_BT_SDIO_INT,
	MAC_AX_GPIO_WL_FLASH,
	MAC_AX_GPIO_BT_FLASH,
	MAC_AX_GPIO_SIC,
	MAC_AX_GPIO_LTE_UART,
	MAC_AX_GPIO_LTE_3W,
	MAC_AX_GPIO_WL_PTA,
	MAC_AX_GPIO_BT_PTA,
	MAC_AX_GPIO_MAILBOX,
	MAC_AX_GPIO_WL_LED,
	MAC_AX_GPIO_OSC,
	MAC_AX_GPIO_XTAL_CLK,
	MAC_AX_GPIO_EXT_XTAL_CLK,
	MAC_AX_GPIO_DBG_GNT,
	MAC_AX_GPIO_WL_RFE_CTRL,
	MAC_AX_GPIO_BT_UART_RQB,
	MAC_AX_GPIO_BT_WAKE_HOST,
	MAC_AX_GPIO_HOST_WAKE_BT,
	MAC_AX_GPIO_DBG,
	MAC_AX_GPIO_WL_UART_TX,
	MAC_AX_GPIO_WL_UART_RX,
	MAC_AX_GPIO_WL_JTAG,
	MAC_AX_GPIO_SW_IO,

	/* keep last */
	MAC_AX_GPIO_LAST,
	MAC_AX_GPIO_MAX = MAC_AX_GPIO_LAST,
	MAC_AX_GPIO_INVALID = MAC_AX_GPIO_LAST,
	MAC_AX_GPIO_DFLT = MAC_AX_GPIO_LAST,
};

enum mac_ax_led_mode {
	MAC_AX_LED_MODE_TRX_ON,
	MAC_AX_LED_MODE_TX_ON,
	MAC_AX_LED_MODE_RX_ON,
	MAC_AX_LED_MODE_SW_CTRL_OD,
	MAC_AX_LED_MODE_SW_CTRL_PP,

	/* keep last */
	MAC_AX_LED_MODE_LAST,
	MAC_AX_LED_MODE_MAX = MAC_AX_LED_MODE_LAST,
	MAC_AX_LED_MODE_INVALID = MAC_AX_LED_MODE_LAST,
};

enum mac_ax_sw_io_mode {
	MAC_AX_SW_IO_MODE_INPUT,
	MAC_AX_SW_IO_MODE_OUTPUT_OD,
	MAC_AX_SW_IO_MODE_OUTPUT_PP,

	/* keep last */
	MAC_AX_SW_IO_MODE_LAST,
	MAC_AX_SW_IO_MODE_MAX = MAC_AX_SW_IO_MODE_LAST,
	MAC_AX_SW_IO_MODE_INVALID = MAC_AX_SW_IO_MODE_LAST,
};

#endif //NEO

/*--------------------Define Efuse related enum-------------------------------*/
enum mac_efuse_read_cfg {
	MAC_EFUSE_R_AUTO,
	MAC_EFUSE_R_DRV,
	MAC_EFUSE_R_FW,

	/* keep last */
	MAC_EFUSE_R_LAST,
	MAC_EFUSE_R_MAX = MAC_EFUSE_R_LAST,
	MAC_EFUSE_R_INVALID = MAC_EFUSE_R_LAST,
};

enum mac_efuse_bank {
	MAC_EFUSE_BANK_WIFI,
	MAC_EFUSE_BANK_BT,

	/* keep last */
	MAC_EFUSE_BANK_LAST,
	MAC_EFUSE_BANK_MAX = MAC_EFUSE_BANK_LAST,
	MAC_EFUSE_BANK_INVALID = MAC_EFUSE_BANK_LAST,
};

enum mac_efuse_parser_cfg {
	MAC_EFUSE_PARSER_MAP,
	MAC_EFUSE_PARSER_MASK,

	/* keep last */
	MAC_EFUSE_PARSER_LAST,
	MAC_EFUSE_PARSER_MAX = MAC_EFUSE_PARSER_LAST,
	MAC_EFUSE_PARSER_INVALID = MAC_EFUSE_PARSER_LAST,
};

enum mac_efuse_feature_id {
	MAC_DUMP_PHYSICAL_EFUSE,     /* Support */
	MAC_DUMP_LOGICAL_EFUSE,      /* Support */
	MAC_DUMP_LOGICAL_EFUSE_MASK, /* Support */
};

/*--------------------Define TRX PKT INFO/RPT related enum--------------------*/
enum mac_trx_mode {
	MAC_TRX_NORMAL,
	MAC_TRX_LOOPBACK,

	/* keep last */
	MAC_TRX_LAST,
	MAC_TRX_MAX = MAC_TRX_LAST,
	MAC_TRX_INVALID = MAC_TRX_LAST,
};

enum mac_qta_mode {
	MAC_QTA_SCC,
	MAC_QTA_DBCC,
	MAC_QTA_SCC_STF,
	MAC_QTA_DBCC_STF,
	MAC_QTA_SU_TP,
	MAC_QTA_DLFW,
	MAC_QTA_BCN_TEST,
	MAC_QTA_LAMODE,

	/* keep last */
	MAC_QTA_LAST,
	MAC_QTA_MAX = MAC_QTA_LAST,
	MAC_QTA_INVALID = MAC_QTA_LAST,
};


enum mac_pkt_t {
	MAC_PKT_DATA,
	MAC_PKT_MGNT,
	MAC_PKT_CTRL,
	MAC_PKT_8023,
	MAC_PKT_H2C,
	MAC_PKT_FWDL,
	MAC_PKT_C2H,
	MAC_PKT_PPDU,
	MAC_PKT_CH_INFO,
	MAC_PKT_DFS,

	/* keep last */
	MAC_PKT_LAST,
	MAC_PKT_MAX = MAC_PKT_LAST,
	MAC_PKT_INVALID = MAC_PKT_LAST,
};

#if 0 // NEO

enum mac_ax_amsdu_pkt_num {
	MAC_AX_AMSDU_AGG_NUM_1 = 0,
	MAC_AX_AMSDU_AGG_NUM_2 = 1,
	MAC_AX_AMSDU_AGG_NUM_3 = 2,
	MAC_AX_AMSDU_AGG_NUM_4 = 3,
	MAC_AX_AMSDU_AGG_NUM_MAX
};

enum mac_ax_phy_rpt {
	MAC_AX_PPDU_STATUS,
	MAC_AX_CH_INFO,
	MAC_AX_DFS,
};

enum mac_ax_pkt_drop_sel {
	MAC_AX_PKT_DROP_SEL_MACID_BE_ONCE,
	MAC_AX_PKT_DROP_SEL_MACID_BK_ONCE,
	MAC_AX_PKT_DROP_SEL_MACID_VI_ONCE,
	MAC_AX_PKT_DROP_SEL_MACID_VO_ONCE,
	MAC_AX_PKT_DROP_SEL_MACID_ALL,
	MAC_AX_PKT_DROP_SEL_MG0_ONCE,
	MAC_AX_PKT_DROP_SEL_HIQ_ONCE,
	MAC_AX_PKT_DROP_SEL_HIQ_PORT,
	MAC_AX_PKT_DROP_SEL_HIQ_MBSSID,
	MAC_AX_PKT_DROP_SEL_BAND,
	MAC_AX_PKT_DROP_SEL_REL_MACID,
	MAC_AX_PKT_DROP_SEL_REL_HIQ_PORT,
	MAC_AX_PKT_DROP_SEL_REL_HIQ_MBSSID,

	/* keep last */
	MAC_AX_PKT_DROP_SEL_LAST,
	MAC_AX_PKT_DROP_SEL_MAX = MAC_AX_PKT_DROP_SEL_LAST,
	MAC_AX_PKT_DROP_SEL_INVALID = MAC_AX_PKT_DROP_SEL_LAST,
};

/*need to check and move to other */
enum mac_ax_fwd_target {
	MAC_AX_FWD_DONT_CARE    = 0,
	MAC_AX_FWD_TO_HOST      = 1,
	MAC_AX_FWD_TO_WLAN_CPU  = 2
};

enum mac_ax_action_frame {
	MAC_AX_AF_CSA       = 0,
	MAC_AX_AF_ADDTS_REQ = 1,
	MAC_AX_AF_ADDTS_RES = 2,
	MAC_AX_AF_DELTS     = 3,
	MAC_AX_AF_ADDBA_REQ = 4,
	MAC_AX_AF_ADDBA_RES = 5,
	MAC_AX_AF_DELBA     = 6,
	MAC_AX_AF_NCW       = 7,
	MAC_AX_AF_GID_MGNT  = 8,
	MAC_AX_AF_OP_MODE   = 9,
	MAC_AX_AF_CSI       = 10,
	MAC_AX_AF_HT_CBFM   = 11,
	MAC_AX_AF_VHT_CBFM  = 12
};

enum mac_ax_af_user_define_index {
	MAC_AX_AF_UD_0      = 0,
	MAC_AX_AF_UD_1      = 1,
	MAC_AX_AF_UD_2      = 2,
	MAC_AX_AF_UD_3      = 3
};

enum mac_ax_trigger_frame {
	MAC_AX_TF_BT            = 0,
	MAC_AX_TF_BFRP          = 1,
	MAC_AX_TF_MU_BAR        = 2,
	MAC_AX_TF_MU_RTS        = 3,
	MAC_AX_TF_BSRP          = 4,
	MAC_AX_TF_GCR_MU_BAR    = 5,
	MAC_AX_TF_BQRP          = 6,
	MAC_AX_TF_NFRP          = 7,
	MAC_AX_TF_TF8           = 8,
	MAC_AX_TF_TF9           = 9,
	MAC_AX_TF_TF10          = 10,
	MAC_AX_TF_TF11          = 11,
	MAC_AX_TF_TF12          = 12,
	MAC_AX_TF_TF13          = 13,
	MAC_AX_TF_TF14          = 14,
	MAC_AX_TF_TF15          = 15
};

enum mac_ax_frame_type {
	MAC_AX_FT_ACTION    = 0,
	MAC_AX_FT_ACTION_UD = 1,
	MAC_AX_FT_TRIGGER   = 2,
	MAC_AX_FT_PM_CAM    = 3
};

enum mac_ax_bd_trunc_mode {
	MAC_AX_BD_NORM,
	MAC_AX_BD_TRUNC,
	MAC_AX_BD_DEF = 0xFE
};

enum mac_ax_rxbd_mode {
	MAC_AX_RXBD_PKT,
	MAC_AX_RXBD_SEP,
	MAC_AX_RXBD_DEF = 0xFE
};

enum mac_ax_tag_mode {
	MAC_AX_TAG_SGL,
	MAC_AX_TAG_MULTI,
	MAC_AX_TAG_DEF = 0xFE
};

enum mac_ax_rx_fecth {
	MAC_AX_RX_NORM_FETCH,
	MAC_AX_RX_PRE_FETCH,
	MAC_AX_RX_FETCH_DEF = 0xFE
};

enum mac_ax_tx_burst {
	MAC_AX_TX_BURST_16B,
	MAC_AX_TX_BURST_32B,
	MAC_AX_TX_BURST_64B,
	MAC_AX_TX_BURST_128B,
	MAC_AX_TX_BURST_256B,
	MAC_AX_TX_BURST_512B,
	MAC_AX_TX_BURST_1024B,
	MAC_AX_TX_BURST_2048B,
	MAC_AX_TX_BURST_DEF = 0xFE
};

enum mac_ax_rx_burst {
	MAC_AX_RX_BURST_16B,
	MAC_AX_RX_BURST_32B,
	MAC_AX_RX_BURST_64B,
	MAC_AX_RX_BURST_128B,
	MAC_AX_RX_BURST_DEF = 0xFE
};

enum mac_ax_wd_dma_intvl {
	MAC_AX_WD_DMA_INTVL_0S,
	MAC_AX_WD_DMA_INTVL_256NS,
	MAC_AX_WD_DMA_INTVL_512NS,
	MAC_AX_WD_DMA_INTVL_768NS,
	MAC_AX_WD_DMA_INTVL_1US,
	MAC_AX_WD_DMA_INTVL_1_5US,
	MAC_AX_WD_DMA_INTVL_2US,
	MAC_AX_WD_DMA_INTVL_4US,
	MAC_AX_WD_DMA_INTVL_8US,
	MAC_AX_WD_DMA_INTVL_16US,
	MAC_AX_WD_DMA_INTVL_DEF = 0xFE
};

enum mac_ax_multi_tag_num {
	MAC_AX_TAG_NUM_1,
	MAC_AX_TAG_NUM_2,
	MAC_AX_TAG_NUM_3,
	MAC_AX_TAG_NUM_4,
	MAC_AX_TAG_NUM_5,
	MAC_AX_TAG_NUM_6,
	MAC_AX_TAG_NUM_7,
	MAC_AX_TAG_NUM_8,
	MAC_AX_TAG_NUM_DEF = 0xFE
};

enum mac_ax_rx_adv_pref {
	MAC_AX_RX_APREF_1BD = 0,
	MAC_AX_RX_APREF_2BD,
	MAC_AX_RX_APREF_4BD,
	MAC_AX_RX_APREF_8BD,
	/* keep last */
	MAC_AX_RX_APREF_LAST,
	MAC_AX_RX_APREF_MAX = MAC_AX_RX_APREF_LAST,
	MAC_AX_RX_APREF_INVALID = MAC_AX_RX_APREF_LAST,
};

enum mac_ax_lbc_tmr {
	MAC_AX_LBC_TMR_8US = 0,
	MAC_AX_LBC_TMR_16US,
	MAC_AX_LBC_TMR_32US,
	MAC_AX_LBC_TMR_64US,
	MAC_AX_LBC_TMR_128US,
	MAC_AX_LBC_TMR_256US,
	MAC_AX_LBC_TMR_512US,
	MAC_AX_LBC_TMR_1MS,
	MAC_AX_LBC_TMR_2MS,
	MAC_AX_LBC_TMR_4MS,
	MAC_AX_LBC_TMR_8MS,
	MAC_AX_LBC_TMR_DEF = 0xFE
};

/*END need to check and move to other */

enum mac_ax_edcca_sel {
	MAC_AX_EDCCA_IN_TB_CHK,
	MAC_AX_EDCCA_IN_SIFS_CHK,
	MAC_AX_EDCCA_IN_CTN_CHK,

	/* keep last */
	MAC_AX_EDCCA_SEL_LAST,
	MAC_AX_EDCCA_SEL_MAX = MAC_AX_EDCCA_SEL_LAST,
	MAC_AX_EDCCA_SEL_INVALID = MAC_AX_EDCCA_SEL_LAST,
};

#endif // if 0

enum mac_chip_id {
	MAC_CHIP_ID_8852A = 0,
	MAC_CHIP_ID_8852B,
	MAC_CHIP_ID_8822C,

	/* keep last */
	MAC_CHIP_ID_LAST,
	MAC_CHIP_ID_MAX = MAC_CHIP_ID_LAST,
	MAC_CHIP_ID_INVALID = MAC_CHIP_ID_LAST,
};

#if 0 // NEO

enum mac_ax_wdbk_mode {
	MAC_AX_WDBK_MODE_SINGLE_BK = 0,
	MAC_AX_WDBK_MODE_GRP_BK = 1,

	/* keep last */
	MAC_AX_WDBK_MODE_LAST,
	MAC_AX_WDBK_MODE_MAX = MAC_AX_WDBK_MODE_LAST,
	MAC_AX_WDBK_MODE_INVALID = MAC_AX_WDBK_MODE_LAST,
};

enum mac_ax_rty_bk_mode {
	MAC_AX_RTY_BK_MODE_AGG = 0x0,
	MAC_AX_RTY_BK_MODE_RATE_FB = 0x1,
	MAC_AX_RTY_BK_MODE_BK = 0x2,

	/* keep last */
	MAC_AX_RTY_BK_MODE_LAST,
	MAC_AX_RTY_BK_MODE_MAX = MAC_AX_RTY_BK_MODE_LAST,
	MAC_AX_RTY_BK_MODE_INVALID = MAC_AX_RTY_BK_MODE_LAST,
};

enum mac_ax_ch_busy_cnt_ctrl {
	MAC_AX_CH_BUSY_CNT_CTRL_CNT_REF,
	MAC_AX_CH_BUSY_CNT_CTRL_CNT_BUSY_RST,
	MAC_AX_CH_BUSY_CNT_CTRL_CNT_IDLE_RST,
	MAC_AX_CH_BUSY_CNT_CTRL_CNT_EN,
	MAC_AX_CH_BUSY_CNT_CTRL_CNT_DIS,

	/* keep last */
	MAC_AX_CH_BUSY_CNT_CTRL_LAST,
	MAC_AX_CH_BUSY_CNT_CTRL_MAX = MAC_AX_CH_BUSY_CNT_CTRL_LAST,
	MAC_AX_CH_BUSY_CNT_CTRL_INVALID = MAC_AX_CH_BUSY_CNT_CTRL_LAST,
};

#endif // NEO

enum mac_func_sw {
	MAC_FUNC_DIS = 0,
	MAC_FUNC_EN,
	MAC_FUNC_DEF
};

#if 0 //NEO

enum mac_ax_twt_nego_tp {
	MAC_AX_TWT_NEGO_TP_IND,
	MAC_AX_TWT_NEGO_TP_WAKE,
	MAC_AX_TWT_NEGO_TP_BRC,

	/* keep last */
	MAC_AX_TWT_NEGO_TP_LAST,
	MAC_AX_TWT_NEGO_TP_MAX = MAC_AX_TWT_NEGO_TP_LAST,
	MAC_AX_TWT_NEGO_TP_INVALID = MAC_AX_TWT_NEGO_TP_LAST,
};

enum mac_ax_twt_act_tp {
	MAC_AX_TWT_ACT_TP_ADD,
	MAC_AX_TWT_ACT_TP_DEL,
	MAC_AX_TWT_ACT_TP_MOD,

	/* keep last */
	MAC_AX_TWT_ACT_TP_LAST,
	MAC_AX_TWT_ACT_TP_MAX = MAC_AX_TWT_ACT_TP_LAST,
	MAC_AX_TWT_ACT_TP_INVALID = MAC_AX_TWT_ACT_TP_LAST,
};

enum mac_ax_twtact_act_tp {
	MAC_AX_TWTACT_ACT_TP_ADD,
	MAC_AX_TWTACT_ACT_TP_DEL,
	MAC_AX_TWTACT_ACT_TP_TRMNT,
	MAC_AX_TWTACT_ACT_TP_SUS,
	MAC_AX_TWTACT_ACT_TP_RSUM,

	/* keep last */
	MAC_AX_TWTACT_ACT_TP_LAST,
	MAC_AX_TWTACT_ACT_TP_MAX = MAC_AX_TWTACT_ACT_TP_LAST,
	MAC_AX_TWTACT_ACT_TP_INVALID = MAC_AX_TWTACT_ACT_TP_LAST,
};

enum mac_ax_twt_waitanno_tp {
	MAC_AX_TWT_ANNOWAIT_DIS_MACID,
	MAC_AX_TWT_ANNOWAIT_EN_MACID,
};

enum mac_ax_tsf_sync_act {
	MAC_AX_TSF_SYNC_NOW_ONCE,
	MAC_AX_TSF_EN_SYNC_AUTO,
	MAC_AX_TSF_DIS_SYNC_AUTO
};

enum mac_ax_slot_time {
	MAC_AX_SLOT_TIME_BAND0_9US,
	MAC_AX_SLOT_TIME_BAND0_20US,
	MAC_AX_SLOT_TIME_BAND1_9US,
	MAC_AX_SLOT_TIME_BAND1_20US,
};

/*------------------------Define HCI related enum ----------------------------*/

enum mac_ax_pcie_func_ctrl {
	MAC_AX_PCIE_DISABLE = 0,
	MAC_AX_PCIE_ENABLE = 1,
	MAC_AX_PCIE_DEFAULT = 0xFE,
	MAC_AX_PCIE_IGNORE = 0xFF
};

enum mac_ax_pcie_clkdly {
	MAC_AX_PCIE_CLKDLY_0 = 0,
	MAC_AX_PCIE_CLKDLY_5US = 1,
	MAC_AX_PCIE_CLKDLY_6US = 2,
	MAC_AX_PCIE_CLKDLY_11US = 3,
	MAC_AX_PCIE_CLKDLY_15US = 4,
	MAC_AX_PCIE_CLKDLY_19US = 5,
	MAC_AX_PCIE_CLKDLY_25US = 6,
	MAC_AX_PCIE_CLKDLY_30US = 7,
	MAC_AX_PCIE_CLKDLY_38US = 8,
	MAC_AX_PCIE_CLKDLY_50US = 9,
	MAC_AX_PCIE_CLKDLY_64US = 10,
	MAC_AX_PCIE_CLKDLY_100US = 11,
	MAC_AX_PCIE_CLKDLY_128US = 12,
	MAC_AX_PCIE_CLKDLY_150US = 13,
	MAC_AX_PCIE_CLKDLY_192US = 14,
	MAC_AX_PCIE_CLKDLY_200US = 15,
	MAC_AX_PCIE_CLKDLY_300US = 16,
	MAC_AX_PCIE_CLKDLY_400US = 17,
	MAC_AX_PCIE_CLKDLY_500US = 18,
	MAC_AX_PCIE_CLKDLY_1MS = 19,
	MAC_AX_PCIE_CLKDLY_3MS = 20,
	MAC_AX_PCIE_CLKDLY_5MS = 21,
	MAC_AX_PCIE_CLKDLY_10MS = 22,
	MAC_AX_PCIE_CLKDLY_R_ERR = 0xFD,
	MAC_AX_PCIE_CLKDLY_DEF = 0xFE,
	MAC_AX_PCIE_CLKDLY_IGNORE = 0xFF
};

enum mac_ax_pcie_l1dly {
	MAC_AX_PCIE_L1DLY_16US = 0,
	MAC_AX_PCIE_L1DLY_32US = 1,
	MAC_AX_PCIE_L1DLY_64US = 2,
	MAC_AX_PCIE_L1DLY_INFI = 3,
	MAC_AX_PCIE_L1DLY_R_ERR = 0xFD,
	MAC_AX_PCIE_L1DLY_DEF = 0xFE,
	MAC_AX_PCIE_L1DLY_IGNORE = 0xFF
};

enum mac_ax_pcie_l0sdly {
	MAC_AX_PCIE_L0SDLY_1US = 0,
	MAC_AX_PCIE_L0SDLY_2US = 1,
	MAC_AX_PCIE_L0SDLY_3US = 2,
	MAC_AX_PCIE_L0SDLY_4US = 3,
	MAC_AX_PCIE_L0SDLY_5US = 4,
	MAC_AX_PCIE_L0SDLY_6US = 5,
	MAC_AX_PCIE_L0SDLY_7US = 6,
	MAC_AX_PCIE_L0SDLY_R_ERR = 0xFD,
	MAC_AX_PCIE_L0SDLY_DEF = 0xFE,
	MAC_AX_PCIE_L0SDLY_IGNORE = 0xFF
};

enum mac_ax_pcie_ltr_spc {
	MAC_AX_PCIE_LTR_SPC_10US = 0,
	MAC_AX_PCIE_LTR_SPC_100US = 1,
	MAC_AX_PCIE_LTR_SPC_500US = 2,
	MAC_AX_PCIE_LTR_SPC_1MS = 3,
	MAC_AX_PCIE_LTR_SPC_R_ERR = 0xFD,
	MAC_AX_PCIE_LTR_SPC_DEF = 0xFE,
	MAC_AX_PCIE_LTR_SPC_IGNORE = 0xFF
};

enum mac_ax_pcie_ltr_idle_timer {
	MAC_AX_PCIE_LTR_IDLE_TIMER_1US = 0,
	MAC_AX_PCIE_LTR_IDLE_TIMER_10US = 1,
	MAC_AX_PCIE_LTR_IDLE_TIMER_100US = 2,
	MAC_AX_PCIE_LTR_IDLE_TIMER_200US = 3,
	MAC_AX_PCIE_LTR_IDLE_TIMER_400US = 4,
	MAC_AX_PCIE_LTR_IDLE_TIMER_800US = 5,
	MAC_AX_PCIE_LTR_IDLE_TIMER_1_6MS = 6,
	MAC_AX_PCIE_LTR_IDLE_TIMER_3_2MS = 7,
	MAC_AX_PCIE_LTR_IDLE_TIMER_R_ERR = 0xFD,
	MAC_AX_PCIE_LTR_IDLE_TIMER_DEF = 0xFE,
	MAC_AX_PCIE_LTR_IDLE_TIMER_IGNORE = 0xFF
};

enum mac_ax_pcie_ltr_sw_ctrl {
	MAC_AX_PCIE_LTR_SW_ACT,
	MAC_AX_PCIE_LTR_SW_IDLE
};

enum mac_ax_sdio_clk_mon {
	MAC_AX_SDIO_CLK_MON_SHORT,
	MAC_AX_SDIO_CLK_MON_LONG,
	MAC_AX_SDIO_CLK_MON_USER_DEFINE,

	/* keep last */
	MAC_AX_SDIO_CLK_MON_LAST,
	MAC_AX_SDIO_CLK_MON_MAX = MAC_AX_SDIO_CLK_MON_LAST,
	MAC_AX_SDIO_CLK_MON_INVALID = MAC_AX_SDIO_CLK_MON_LAST,
};

enum mac_ax_rx_ppdu_type {
	MAC_AX_RX_CCK,
	MAC_AX_RX_OFDM,
	MAC_AX_RX_HT,
	MAC_AX_RX_VHT_SU,
	MAC_AX_RX_VHT_MU,
	MAC_AX_RX_HE_SU,
	MAC_AX_RX_HE_MU,
	MAC_AX_RX_HE_TB,

	MAC_AX_RX_PPDU_LAST,
	MAC_AX_RX_PPDU_MAX = MAC_AX_RX_PPDU_LAST,
	MAC_AX_RX_PPDU_INVLAID = MAC_AX_RX_PPDU_LAST,
};

#endif //NEO

enum mac_net_type {
	MAC_NET_TYPE_NO_LINK,
	MAC_NET_TYPE_ADHOC,
	MAC_NET_TYPE_INFRA,
	MAC_NET_TYPE_AP
};

enum mac_self_role {
	MAC_SELF_ROLE_CLIENT,
	MAC_SELF_ROLE_AP,
	MAC_SELF_ROLE_AP_CLIENT
};

enum mac_wifi_role {
	MAC_WIFI_ROLE_NONE,
	MAC_WIFI_ROLE_STATION,
	MAC_WIFI_ROLE_AP,
	MAC_WIFI_ROLE_VAP,
	MAC_WIFI_ROLE_ADHOC,
	MAC_WIFI_ROLE_ADHOC_MASTER,
	MAC_WIFI_ROLE_MESH,
	MAC_WIFI_ROLE_MONITOR,
	MAC_WIFI_ROLE_P2P_DEVICE,
	MAC_WIFI_ROLE_P2P_GC,
	MAC_WIFI_ROLE_P2P_GO,
	MAC_WIFI_ROLE_NAN,
	MAC_WIFI_ROLE_MLME_MAX
};

enum mac_opmode {
	MAC_ROLE_CONNECT,
	MAC_ROLE_DISCONN
};

enum mac_upd_mode {
	MAC_ROLE_CREATE,
	MAC_ROLE_REMOVE,
	MAC_ROLE_TYPE_CHANGE,
	MAC_ROLE_INFO_CHANGE,
	MAC_ROLE_CON_DISCONN
};


enum mac_host_rpr_mode {
	MAC_RPR_MODE_POH = 0,
	MAC_RPR_MODE_STF
};


/*--------------------Define Struct-------------------------------------*/
struct mac_sch_tx_en {
	u8 be0:1;
	u8 bk0:1;
	u8 vi0:1;
	u8 vo0:1;
	u8 be1:1;
	u8 bk1:1;
	u8 vi1:1;
	u8 vo1:1;
	u8 mg0:1;
	u8 mg1:1;
	u8 mg2:1;
	u8 hi:1;
	u8 bcn:1;
	u8 ul:1;
	u8 twt0:1;
	u8 twt1:1;
};


struct mac_hw_info {
	u8 done;
	u8 chip_id;
	u8 cv;
	enum mac_intf intf;
	u8 tx_ch_num;
	u8 tx_data_ch_num;
	u8 wd_body_len;
	u8 wd_info_len;
	struct mac_pwr_cfg **pwr_on_seq;
	struct mac_pwr_cfg **pwr_off_seq;
	u8 pwr_seq_ver;
	u32 txff_size;
	u32 rxff_size;
	u8 csi_buf_pg_num;
	u16 macid_num;
	u8 bssid_num;
	u32 wl_efuse_size;
	u32 efuse_size;
	u32 log_efuse_size;
	u32 limit_efuse_size_pcie;
	u32 limit_efuse_size_usb;
	u32 limit_efuse_size_sdio;
	u32 bt_efuse_size;
	u32 bt_log_efuse_size;
	u8 hidden_efuse_size;
	u32 sec_ctrl_efuse_size;
	u32 sec_data_efuse_size;
	struct sec_cam_table_t *sec_cam_table;
	u8 ple_rsvd_space;
	u8 payload_desc_size;
	u8 efuse_version_size;
	u32 dav_full_efuse_size;
	u32 dav_efuse_size;
	u32 dav_hidden_efuse_size;
	u32 dav_log_efuse_size;
	u32 wl_efuse_start_addr;
	u32 dav_efuse_start_addr;
	u32 bt_efuse_start_addr;
	u8 wd_checksum_en;
	u32 sw_amsdu_max_size;
	u32 (*pwr_on)(void *vadapter);
	u32 (*pwr_off)(void *vadapter);
	u32 ind_aces_cnt;
	u32 dbg_port_cnt;
	u8 core_swr_volt;
/*
	enum mac_ax_core_swr_volt core_swr_volt_sel;
*/
	mac_mutex ind_access_lock;
	mac_mutex lte_rlock;
	mac_mutex lte_wlock;
	mac_mutex dbg_port_lock;
	mac_mutex err_set_lock;
	mac_mutex err_get_lock;
	mac_mutex dbi_lock;
	mac_mutex mdio_lock;
};

struct mac_fw_info {
	u8 major_ver;
	u8 minor_ver;
	u8 sub_ver;
	u8 sub_idx;
	u16 build_year;
	u16 build_mon;
	u16 build_date;
	u16 build_hour;
	u16 build_min;
	u8 h2c_seq;
	u8 rec_seq;
	mac_mutex seq_lock;
};

struct mac_pwr_info {
	u8 pwr_seq_proc;
	u8 pwr_in_lps;
	u32 (*intf_pwr_switch)(void *vadapter,
			       u8 pre_switch, u8 on);
};

#if 0 // NEO
struct mac_ax_ft_status {
	enum mac_ax_feature mac_ft;
	enum mac_ax_status status;
	u8 *buf;
	u32 size;
};

struct mac_ax_dle_info {
	enum mac_ax_qta_mode qta_mode;
	u16 wde_pg_size;
	u16 ple_pg_size;
	u16 c0_rx_qta;
	u16 c1_rx_qta;
};

struct mac_ax_gpio_info {
#define MAC_AX_GPIO_NUM 16
	/* byte0 */
	u8 sw_io_0:1;
	u8 sw_io_1:1;
	u8 sw_io_2:1;
	u8 sw_io_3:1;
	u8 sw_io_4:1;
	u8 sw_io_5:1;
	u8 sw_io_6:1;
	u8 sw_io_7:1;
	/* byte1 */
	u8 sw_io_8:1;
	u8 sw_io_9:1;
	u8 sw_io_10:1;
	u8 sw_io_11:1;
	u8 sw_io_12:1;
	u8 sw_io_13:1;
	u8 sw_io_14:1;
	u8 sw_io_15:1;
	/* byte2 */
	u8 uart_tx_gpio5:1;
	u8 uart_tx_gpio7:1;
	u8 uart_tx_gpio8:1;
	u8 uart_rx_gpio6:1;
	u8 uart_rx_gpio14:1;
	enum mac_ax_gfunc status[MAC_AX_GPIO_NUM];
#define MAC_AX_SW_IO_OUT_PP 0
#define MAC_AX_SW_IO_OUT_OD 1
	u8 sw_io_output[MAC_AX_GPIO_NUM];
};

#endif // if 0 NEO

struct mac_trx_info {
	enum mac_trx_mode trx_mode;
	enum mac_qta_mode qta_mode;
	struct mac_host_rpr_cfg *rpr_cfg;
};


struct mac_fwdl_info {
	u8 fw_en;
	u8 dlrom_en;
	u8 dlram_en;
	u8 fw_from_hdr;
	enum rtw_fw_type fw_cat;
	u8 *rom_buff;
	u32 rom_size;
	u8 *ram_buff;
	u32 ram_size;
};

#if 0 //NEO
struct mac_txdma_ch_map {
	enum mac_pcie_func_ctrl ch0;
	enum mac_pcie_func_ctrl ch1;
	enum mac_pcie_func_ctrl ch2;
	enum mac_pcie_func_ctrl ch3;
	enum mac_pcie_func_ctrl ch4;
	enum mac_pcie_func_ctrl ch5;
	enum mac_pcie_func_ctrl ch6;
	enum mac_pcie_func_ctrl ch7;
	enum mac_pcie_func_ctrl ch8;
	enum mac_pcie_func_ctrl ch9;
	enum mac_pcie_func_ctrl ch10;
	enum mac_pcie_func_ctrl ch11;
	enum mac_pcie_func_ctrl ch12;
};

struct mac_rxdma_ch_map {
	enum mac_pcie_func_ctrl rxq;
	enum mac_pcie_func_ctrl rpq;
};
#endif //NEO


struct mac_intf_info {
#if 0 //NEO
	enum mac_ax_bd_trunc_mode txbd_trunc_mode;
	enum mac_ax_bd_trunc_mode rxbd_trunc_mode;
	enum mac_ax_rxbd_mode rxbd_mode;
	enum mac_ax_tag_mode tag_mode;
	enum mac_ax_tx_burst tx_burst;
	enum mac_ax_rx_burst rx_burst;
	enum mac_ax_wd_dma_intvl wd_dma_idle_intvl;
	enum mac_ax_wd_dma_intvl wd_dma_act_intvl;
	enum mac_ax_multi_tag_num multi_tag_num;
#endif // if 0 NEO
	u16 rx_sep_append_len;
	u8 *txbd_buf;
	u8 *rxbd_buf;
	u8 skip_all;
#if 0 // NEO
	struct mac_ax_txdma_ch_map *txch_map;
	enum mac_ax_pcie_func_ctrl lbc_en;
	enum mac_ax_lbc_tmr lbc_tmr;
	enum mac_ax_pcie_func_ctrl autok_en;
#endif // if 0 NEO
};

#if 0 //NEO

struct mac_ax_pcie_trx_mitigation {
	struct mac_ax_txdma_ch_map *txch_map;
	enum mac_ax_trx_mitigation_timer_unit tx_timer_unit;
	u8 tx_timer;
	u8 tx_counter;
	struct mac_ax_rxdma_ch_map *rxch_map;
	enum mac_ax_trx_mitigation_timer_unit rx_timer_unit;
	u8 rx_timer;
	u8 rx_counter;
};

struct mac_mu_table {
	u32 mu_score_tbl_ctrl;
	u32 mu_score_tbl_0;
	u32 mu_score_tbl_1;
	u32 mu_score_tbl_2;
	u32 mu_score_tbl_3;
	u32 mu_score_tbl_4;
	u32 mu_score_tbl_5;
};

struct mac_ax_ss_dl_grp_upd {
	u8 grp_valid:1; //0: non valid 1: valid
	u8 grp_id:5; //grp 0~16
	u8 is_hwgrp:1;
	u8 rsvd:1;
	u8 macid_u0;
	u8 macid_u1;
	u8 macid_u2;
	u8 macid_u3;
	u8 macid_u4;
	u8 macid_u5;
	u8 macid_u6;
	u8 macid_u7;
	u8 ac_bitmap_u0:4;
	u8 ac_bitmap_u1:4;
	u8 ac_bitmap_u2:4;
	u8 ac_bitmap_u3:4;
	u8 ac_bitmap_u4:4;
	u8 ac_bitmap_u5:4;
	u8 ac_bitmap_u6:4;
	u8 ac_bitmap_u7:4;
	u8 next_protecttype:4;
	u8 next_rsptype:4;
};

struct mac_ax_ss_ul_grp_upd {
	u8 macid_u0;
	u8 macid_u1;
	u16 grp_bitmap;
};

struct mac_ax_ss_ul_sta_upd {
	u32 mode:8; //0:del; 1: add
	u32 rsvd:24;
	u8 macid[4];
	u16 bsr_len[2];
};

struct mac_ax_2nav_info {
	u8 plcp_upd_nav_en;
	u8 tgr_fram_upd_nav_en;
	u8 nav_up;
};

struct mac_ax_bcn_info {
	u8 port;
	u8 mbssid;
	u8 band;
	u8 grp_ie_ofst;
	u8 macid;
	u8 ssn_sel;
	u8 ssn_mode;
	u16 rate_sel;
	u8 txpwr;
	u8 txinfo_ctrl_en;
	u8 ntx_path_en;
	u8 path_map_a;
	u8 path_map_b;
	u8 path_map_c;
	u8 path_map_d;
	u8 antsel_a;
	u8 antsel_b;
	u8 antsel_c;
	u8 antsel_d;
	u8 sw_tsf;
	u8 *pld_buf;
	u16 pld_len;
};

struct mac_ax_twt_para {
	enum mac_ax_twt_nego_tp nego_tp;
	enum mac_ax_twt_act_tp act;
	u32 trig:1;
	u32 flow_tp:1;
	u32 proct:1;
	u32 flow_id:3;
	u32 id:3;
	u32 wake_exp:5;
	u32 band:1;
	u32 port:3;
	u32 rsp_pm:1;
	u32 wake_unit:1;
	u32 impt:1;
	u32 rsvd:11;

	u16 wake_man;
	u8 dur;
	u32 trgt_l;
	u32 trgt_h;
};

struct mac_ax_twtact_para {
	enum mac_ax_twtact_act_tp act;
	u16 macid;
	u8 id:3;
	u8 rsvd:5;
};

struct mac_ax_twtanno_para {
	u8 macid;
};

struct mac_ax_twtanno_c2hpara {
	u32 wait_case:4;
	u32 rsvd:4;
	u32 macid0:8;
	u32 macid1:8;
	u32 macid2:8;
};

struct mac_ax_port_cfg_para {
	u32 mbssid_idx;
	u32 val;
	u8 port;
	u8 band;
};

struct mac_ax_port_init_para {
	enum mac_ax_port port_idx;
	enum mac_ax_band band_idx;
	enum mac_ax_net_type net_type;
	u8 dtim_period;
	u8 mbid_num;
	u8 bss_color;
	u16 bcn_interval;
	u32 hiq_win;
};

struct mac_ax_fw_log {
#define MAC_AX_FL_LV_OFF 0
#define MAC_AX_FL_LV_CRT 1
#define MAC_AX_FL_LV_SER 2
#define MAC_AX_FL_LV_WARN 3
#define MAC_AX_FL_LV_LOUD 4
#define MAC_AX_FL_LV_TR 5
	u32 level;
#define MAC_AX_FL_LV_UART BIT(0)
#define MAC_AX_FL_LV_C2H BIT(1)
#define MAC_AX_FL_LV_SNI BIT(2)
	u32 output;
#define MAC_AX_FL_COMP_VER BIT(0)
#define MAC_AX_FL_COMP_INIT BIT(1)
#define MAC_AX_FL_COMP_TASK BIT(2)
#define MAC_AX_FL_COMP_CNS BIT(3)
#define MAC_AX_FL_COMP_H2C BIT(4)
#define MAC_AX_FL_COMP_C2H BIT(5)
#define MAC_AX_FL_COMP_TX BIT(6)
#define MAC_AX_FL_COMP_RX BIT(7)
#define MAC_AX_FL_COMP_IPSEC BIT(8)
#define MAC_AX_FL_COMP_TIMER BIT(9)
#define MAC_AX_FL_COMP_DBGPKT BIT(10)
#define MAC_AX_FL_COMP_PS BIT(11)
#define MAC_AX_FL_COMP_ERROR BIT(12)
#define MAC_AX_FL_COMP_WOWLAN BIT(13)
#define MAC_AX_FL_COMP_SECURE_BOOT BIT(14)
#define MAC_AX_FL_COMP_BTC BIT(15)
#define MAC_AX_FL_COMP_BB BIT(16)
#define MAC_AX_FL_COMP_TWT BIT(17)
#define MAC_AX_FL_COMP_RF BIT(18)
#define MAC_AX_FL_COMP_MCC BIT(20)
	u32 comp;
	u32 comp_ext;
};

struct mac_ax_dbgpkg {
	//dbg return value
	u32 ss_dbg_0;
	u32 ss_dbg_1;
};

struct mac_ax_dbgpkg_en {
	//dbg return value
	u8 ss_dbg:1;
	u8 dle_dbg:1;
	u8 dmac_dbg:1;
	u8 cmac_dbg:1;
	u8 mac_dbg_port:1;
};

struct mac_ax_fwdbg_en {
	//dbg return value
	u8 status_dbg:1;
	u8 rsv_ple_dbg:1;
	u8 ps_dbg:1;
};

union mac_conf_ofld_hioe_param0 {
	u32 register_addr;
	u32 delay_value;
};

union mac_conf_ofld_hioe_param1 {
	u16 byte_data_h;
	u16 bit_mask;
};

union mac_conf_ofld_hioe_param2 {
	u16 byte_data_l;
	u16 bit_data;
};

struct mac_conf_ofld_hioe {
#define CONF_OFLD_HIOE_OP_RESTORE 0
#define CONF_OFLD_HIOE_OP_BACKUP 1
#define CONF_OFLD_HIOE_OP_BOTH 2
	u8 hioe_op;
#define CONF_OFLD_HIOE_INST_IO 0
#define CONF_OFLD_HIOE_INST_POLLING 1
#define CONF_OFLD_HIOE_INST_DELAY 2
	u8 inst_type;
	u8 rsvd;
#define CONF_OFLD_HIOE_INST_DATA_BYTE 0
#define CONF_OFLD_HIOE_INST_DATA_BIT 3
	u8 data_mode;
	union mac_conf_ofld_hioe_param0 param0;
	union mac_conf_ofld_hioe_param1 param1;
	union mac_conf_ofld_hioe_param2 param2;
};

struct mac_conf_ofld_ddma {
#define CONF_OFLD_DDMA_OP_RESTORE 0
#define CONF_OFLD_DDMA_OP_BACKUP 1
#define CONF_OFLD_DDMA_OP_BOTH 2
	u8 ddma_mode;
	u8 finish;
	u16 dma_len;
	u32 dma_src_addr;
	u32 dma_dst_addr;
};

union mac_conf_ofld_req_bd {
	struct mac_conf_ofld_hioe hioe;
	struct mac_conf_ofld_ddma ddma;
};

struct mac_ax_conf_ofld_req {
#define CONF_OFLD_DEVICE_HIOE 0
#define CONF_OFLD_DEVICE_DDMA 1
	u32 device:8;
	u32 rsvd:24;
	union mac_conf_ofld_req_bd req;
};

#endif //NEO

struct mac_defeature_value {
	u8 rx_spatial_stream;
	u8 bandwidth;
	u8 tx_spatial_stream;
	u8 protocol_80211;
	u8 NIC_router;
};

#if 0 //NEO

struct mac_ax_wowlan_info {
	u8 *aoac_report;
};

struct mac_ax_p2p_act_info {
	u8 macid;
	u8 p2pid;
	u8 noaid;
	u8 act;
	u8 type;
	u8 all_slep;
	u32 srt;
	u32 itvl;
	u32 dur;
	u8 cnt;
	u16 ctw;
};

/*-------------------- Define Struct needed to be moved-----------------------*/
struct mac_ax_tbl_hdr {
	u8 rw:1;
	u8 idx:7;
	u16 offset:5;
	u16 len:10;
	u16 type:1;
};

struct mac_ax_ru_rate_ent {
	u8 dcm:1;
	u8 ss:3;
	u8 mcs:4;
};

struct mac_ax_dl_fix_sta_ent {
	u8 mac_id;
	u8 ru_pos[3];
	u8 fix_rate:1;
	u8 fix_coding:1;
	u8 fix_txbf:1;
	u8 fix_pwr_fac:1;
	u8 rsvd0: 4;
	struct mac_ax_ru_rate_ent rate;
	u8 txbf:1;
	u8 coding:1;
	u8 pwr_boost_fac:5;
	u8 rsvd1: 1;
	u8 rsvd2;
};

struct mac_ax_dlru_fixtbl {
	struct mac_ax_tbl_hdr tbl_hdr;
	u8 max_sta_num:3;
	u8 min_sta_num:3;
	u8 doppler:1;
	u8 stbc:1;
	u8 gi_ltf:3;
	u8 ma_type:1;
	u8 fixru_flag:1;
	struct mac_ax_dl_fix_sta_ent sta[MAC_AX_MAX_RU_NUM];
};

struct mac_ax_ul_fix_sta_ent {
	u8 mac_id;
	u8 ru_pos[3];
	u8 tgt_rssi[3];
	u8 fix_tgt_rssi: 1;
	u8 fix_rate: 1;
	u8 fix_coding: 1;
	u8 coding: 1;
	u8 rsvd1: 4;
	struct mac_ax_ru_rate_ent rate;
};

struct mac_ax_ulru_fixtbl {
	struct mac_ax_tbl_hdr tbl_hdr;
	u8 max_sta_num: 3;
	u8 min_sta_num: 3;
	u8 doppler: 1;
	u8 ma_type: 1;
	u8 gi_ltf: 3;
	u8 stbc: 1;
	u8 fix_tb_t_pe_nom: 1;
	u8 tb_t_pe_nom: 2;
	u8 fixru_flag: 1;
	u16 rsvd;
	struct mac_ax_ul_fix_sta_ent sta[MAC_AX_MAX_RU_NUM];
};

/*--------------------END Define Struct needed to be moved--------------------*/
/*--------------------Define HCI related structure----------------------------*/

struct mac_ax_hfc_ch_cfg {
	u16 min;
	u16 max;
#define grp_0 0
#define grp_1 1
#define grp_num 2
	u8 grp;
};

struct mac_ax_hfc_ch_info {
	u16 aval;
	u16 used;
};

struct mac_ax_hfc_pub_cfg {
	u16 group0;
	u16 group1;
	u16 pub_max;
	u16 wp_thrd;
};

struct mac_ax_hfc_pub_info {
	u16 g0_used;
	u16 g1_used;
	u16 g0_aval;
	u16 g1_aval;
	u16 pub_aval;
	u16 wp_aval;
};

struct mac_ax_hfc_prec_cfg {
	u16 ch011_prec;
	u16 h2c_prec;
	u16 wp_ch07_prec;
	u16 wp_ch811_prec;
	u8 ch011_full_cond;
	u8 h2c_full_cond;
	u8 wp_ch07_full_cond;
	u8 wp_ch811_full_cond;
};

struct mac_ax_hfc_param {
	u8 en;
	u8 h2c_en;
	u8 mode;
	struct mac_ax_hfc_ch_cfg *ch_cfg;
	struct mac_ax_hfc_ch_info *ch_info;
	struct mac_ax_hfc_pub_cfg *pub_cfg;
	struct mac_ax_hfc_pub_info *pub_info;
	struct mac_ax_hfc_prec_cfg *prec_cfg;
};

#endif //NEO

struct mac_sdio_tx_info {
	u32 total_size;
	u8 dma_txagg_num;
	u8 ch_dma;
	u8 *pkt_size;
	u8 *wp_offset;
	u8 chk_cnt;
	u16 wde_rqd_num;
	u16 ple_rqd_num;
};

#if 0 //NEO

struct mac_ax_sdio_clk_mon_cfg {
	enum mac_ax_sdio_clk_mon mon;
	u32 cycle;
};

struct mac_ax_pcie_ltr_param {
	u8 write;
	u8 read;
	enum mac_ax_pcie_func_ctrl ltr_ctrl;
	enum mac_ax_pcie_ltr_spc ltr_spc_ctrl;
	enum mac_ax_pcie_ltr_idle_timer ltr_idle_timer_ctrl;
	struct mac_ax_pcie_ltr_rx0_th_ctrl {
		enum mac_ax_pcie_func_ctrl ctrl;
		u16 val;
	} ltr_rx0_th_ctrl;
	struct mac_ax_pcie_ltr_rx1_th_ctrl {
		enum mac_ax_pcie_func_ctrl ctrl;
		u16 val;
	} ltr_rx1_th_ctrl;
	struct mac_ax_pcie_ltr_idle_lat_ctrl {
		enum mac_ax_pcie_func_ctrl ctrl;
		u32 val;
	} ltr_idle_lat_ctrl;
	struct mac_ax_pcie_ltr_act_lat_ctrl {
		enum mac_ax_pcie_func_ctrl ctrl;
		u32 val;
	} ltr_act_lat_ctrl;
};

struct mac_ax_usb_tx_agg_cfg {
	u8 *pkt;
	u32 agg_num;
};

struct mac_ax_pcie_cfgspc_param {
	u8 write;
	u8 read;
	enum mac_ax_pcie_func_ctrl l0s_ctrl;
	enum mac_ax_pcie_func_ctrl l1_ctrl;
	enum mac_ax_pcie_func_ctrl l1ss_ctrl;
	enum mac_ax_pcie_func_ctrl wake_ctrl;
	enum mac_ax_pcie_func_ctrl crq_ctrl;
	enum mac_ax_pcie_clkdly clkdly_ctrl;
	enum mac_ax_pcie_l0sdly l0sdly_ctrl;
	enum mac_ax_pcie_l1dly l1dly_ctrl;
};

struct mac_ax_rx_agg_thold {
	u8 drv_define;
	u8 timeout;
	u8 size;
	u8 pkt_num;
};

struct mac_ax_lifetime_en {
	u8 acq_en;
	u8 mgq_en;
};

struct mac_ax_lifetime_val {
	u16 acq_val_1;
	u16 acq_val_2;
	u16 acq_val_3;
	u16 acq_val_4;
	u16 mgq_val;
};

struct mac_ax_cfg_bw {
	u8 pri_ch;
	u8 central_ch;
	u16 band: 1;
	u16 rsvd: 15;
	enum channel_width cbw;
};

/*-------------------- Define Efuse related structure ------------------------*/
struct mac_ax_pg_efuse_info {
	u8 *efuse_map;
	u32 efuse_map_size;
	u8 *efuse_mask;
	u32 efuse_mask_size;
};

#endif //NEO

struct mac_efuse_param {
	u8 *efuse_map;
	u8 *bt_efuse_map;
	u8 *log_efuse_map;
	u8 *bt_log_efuse_map;
	u32 efuse_end;
	u32 bt_efuse_end;
	u8 efuse_map_valid;
	u8 bt_efuse_map_valid;
	u8 log_efuse_map_valid;
	u8 bt_log_efuse_map_valid;
	u8 auto_ck_en;
};

#if 0 //NEO

/*-------------------- Define offload related Struct -------------------------*/
struct mac_ax_read_req {
	u16 value_len:11;
	u16 rsvd0: 4;
	u16 ls: 1;
	u8 ofld_id;
	u8 entry_num;
	u16 offset;
	u16 rsvd1;
};

struct mac_ax_read_ofld_info {
	u8 *buf;
	u8 *buf_wptr;
	struct mac_ax_read_req *last_req;
	u32 buf_size;
	u32 avl_buf_size;
	u32 used_size;
	u32 req_num;
};

struct mac_ax_read_ofld_value {
	u16 len;
	u16 rsvd;
	u8 *buf;
};

struct mac_ax_efuse_ofld_info {
	u8 *buf;
};

struct mac_ax_write_req {
	u16 value_len:11;
	u16 rsvd0: 2;
	u16 polling: 1;
	u16 mask_en: 1;
	u16 ls: 1;
	u8 ofld_id;
	u8 entry_num;
	u16 offset;
	u16 rsvd1;
};

struct mac_ax_write_ofld_info {
	u8 *buf;
	u8 *buf_wptr;
	struct mac_ax_write_req *last_req;
	u32 buf_size;
	u32 avl_buf_size;
	u32 used_size;
	u32 req_num;
};

struct mac_ax_conf_ofld_info {
	u8 *buf;
	u8 *buf_wptr;
	u32 buf_size;
	u32 avl_buf_size;
	u32 used_size;
	u16 req_num;
};

struct mac_ax_pkt_ofld_info {
#define PKT_OFLD_MAX_COUNT 256
	u8 last_op;
	u16 free_id_count;
	u16 used_id_count;
	u8 id_bitmap[PKT_OFLD_MAX_COUNT >> 3];
};

struct mac_ax_pkt_ofld_pkt {
	u8 pkt_id;
	u8 rsvd;
	u16 pkt_len;
	u8 *pkt;
};

struct mac_ax_general_pkt_ids {
	u8 macid;
	u8 probersp;
	u8 pspoll;
	u8 nulldata;
	u8 qosnull;
	u8 cts2self;
};

/*--------------------Define OutSrc related ----------------------------------*/
struct mac_ax_la_cfg {
	u32 la_func_en:1;
	u32 la_restart_en:1;
	u32 la_timeout_en:1;
	/* 2'h0: 1s, 2'h1: 2s, 2'h2: 4s, 2'h3: 8s */
	u32 la_timeout_val:2;
	/*Error flag mask bit for LA data loss due to pktbuffer busy */
	u32 la_data_loss_imr:1;
	/* TU (time unit) = 2^ B_AX_LA_TRIG_TU_SEL */
	u32 la_tgr_tu_sel:4;
	/* 6'h0: No delay, 6'h1: 1 TU, 6'h2: 2TU, ??*/
	u32 la_tgr_time_val:7;
	u32 rsvd:15;
};

struct mac_ax_la_status {
	/* LA data dump finish address = (la_buf_wptr -1) */
	u16 la_buf_wptr;
	/*1: round up, 0: No round up */
	u8 la_buf_rndup_ind:1;
	/*3'h0: LA idle ; 3'h1: LA start; 3'h2: LA finish stop;*/
	/*3'h3:LA finish timeout; 3'h4: LA re-start*/
	u8 la_sw_fsmst:3;
	/* LA data loss due to pktbuffer busy */
	u8 la_data_loss:1;
};

struct mac_ax_la_buf_param {
	u32 start_addr;
	u32 end_addr;
	u8 la_buf_sel; /*0: 64KB; 1: 128KB; 2: 192KB; 3: 256KB; 4: 320KB*/
};

#endif // if 0 NEO

/*--------------------Define TRX PKT INFO/RPT---------------------------------*/
struct mac_pkt_data {

#if 1 // NEO : ac
	u8 offset;
	u8 pkt_offset;
	u8 macid;
	u8 rate_id;
	u8 data_rate;
	u8 qsel;
	u8 data_bw;
	u8 sec_type;
	u8 agg_en;
	u8 max_agg_num;
	u8 ampdu_density;
	u16 wifi_seq;
	u8 data_stbc;
	u8 data_ldpc;
	u8 dis_data_fb;
	u8 bmc;
	u8 userate;
	u8 ls;
	u8 short_gi;
	u8 report;
	u8 dis_qselseq;
	u8 en_hwseq;
	u8 hw_ssn_sel;
	u8 nav_use_hdr;
#else // NEO : ax
	u8 tid;
	u16 wifi_seq;
	u8 hw_ssn_sel;
	u8 hw_seq_mode;
	u8 chk_en;
	u8 hw_amsdu;
	u8 shcut_camid;
	u8 headerwllc_len;
	u8 smh_en;
	u8 wd_page;
	u8 wp_offset;
	u8 wdinfo_en;
	u8 hw_aes_iv;
	u8 hdr_len;
	u8 ch;
	u8 bk;
	u8 lifetime_sel;
	u16 data_rate;
	u8 er_bw;
	u8 data_gi_ltf;
	u8 data_er;
	u8 data_dcm;
	u8 hw_sec_en;
	u8 sec_cam_idx;
	u8 sec_type;
	u8 dis_rts_fb;
	u8 rts_en;
	u8 cts2self;
	u8 cca_rts;
	u8 hw_rts_en;
	u8 ndpa;
	u8 snd_pkt_sel;
	u8 sifs_tx;
	u8 tx_cnt_lmt_sel;
	u8 tx_cnt_lmt;
	u16 ndpa_dur;
	u8 multiport_id;
	u8 mbssid;
	u8 null_0;
	u8 null_1;
	u8 tri_frame;
	u8 ack_ch_info;
	u8 pkt_offset;
	u8 a_ctrl_uph;
	u8 a_ctrl_bsr;
	u8 a_ctrl_cas;
	u8 rtt;
	u8 ht_data_snd;
	u8 no_ack;
	u8 sw_define;
#endif
};

struct mac_pkt_mgnt {
	u16 wifi_seq;
	u8 hw_ssn_sel;
	u8 hw_seq_mode;
	u8 chk_en;
	u8 hw_amsdu;
	u8 shcut_camid;
	u8 headerwllc_len;
	u8 smh_en;
	u8 wd_page;
	u8 wp_offset;
	u8 wdinfo_en;
	u8 hw_aes_iv;
	u8 hdr_len;
	u8 rsvd0;
	u8 macid;
	u8 rsvd1;
	u8 bk;
	u8 max_agg_num;
	u8 bmc;
	u8 lifetime_sel;
	u8 ampdu_density;
	u8 userate;
	u16 data_rate;
	u8 data_bw;
	u8 er_bw;
	u8 data_gi_ltf;
	u8 data_er;
	u8 data_dcm;
	u8 data_stbc;
	u8 data_ldpc;
	u8 hw_sec_en;
	u8 sec_cam_idx;
	u8 sec_type;
	u8 dis_data_fb;
	u8 dis_rts_fb;
	u8 tid;
	u8 rts_en;
	u8 cts2self;
	u8 cca_rts;
	u8 hw_rts_en;
	u8 ndpa;
	u8 snd_pkt_sel;
	u8 sifs_tx;
	u8 tx_cnt_lmt_sel;
	u8 tx_cnt_lmt;
	u16 ndpa_dur;
	u8 nav_use_hdr;
	u8 multiport_id;
	u8 mbssid;
	u8 null_0;
	u8 null_1;
	u8 tri_frame;
	u8 ack_ch_info;
	u8 pkt_offset;
	u8 a_ctrl_bsr;
	u8 rtt;
	u8 ht_data_snd;
	u8 no_ack;
};

struct mac_rpkt_data {
	u8 crc_err;
	u8 icv_err;
};

struct mac_txpkt_info {
	enum mac_pkt_t type;
	u32 pktsize;
	union {
		struct mac_pkt_data data;
		struct mac_pkt_mgnt mgnt;
	} u;
};

#if 0 // NEO

struct mac_ax_bcn_cnt {
	u8 port;
	u8 mbssid;
	u8 ok_cnt;
	u8 fail_cnt;
};

struct mac_ax_refill_info {
	u8 *pkt;
	u32 agg_num;
	u8 packet_offset;
};

struct mac_ax_rpkt_ppdu {
	u8 mac_info;
};

struct mac_ax_mac_tx_mode_sel {
	u8 sw_mode_band0_en;
	u8 txop_rot_wmm0_en;
	u8 txop_rot_wmm1_en;
	u8 txop_rot_wmm2_en;
	u8 txop_rot_wmm3_en;
};

struct mac_ax_rxpkt_info {
	enum mac_ax_pkt_t type;
	u16 rxdlen;
	u8 drvsize;
	u8 shift;
	u32 pktsize;
	union {
		struct mac_ax_rpkt_data data;
		struct mac_ax_rpkt_ppdu ppdu;
	} u;
};

struct mac_ax_pm_cam_ctrl_t {
	u32 pld_mask0;
	u32 pld_mask1;
	u32 pld_mask2;
	u32 pld_mask3;
	u8 entry_index;
	u8 valid;
	u8 type;
	u8 subtype;
	u8 skip_mac_iv_hdr;
	u8 target_ind;
	u16 crc16;
};

struct mac_ax_af_ud_ctrl_t {
	u8 index;
	u8 fwd_tg;
	u8 category;
	u8 action_field;
};

struct mac_ax_rx_fwd_ctrl_t {
	struct mac_ax_pm_cam_ctrl_t pm_cam_ctrl;
	struct mac_ax_af_ud_ctrl_t af_ud_ctrl;
	u8 type;
	u8 frame;
	u8 fwd_tg;
};

struct mac_ax_rx_fltr_ctrl_t {
	// mac fltr
	u8 sniffer_mode:1;
	u8 acpt_a1_match_pkt:1;
	u8 acpt_bc_pkt:1;
	u8 acpt_mc_pkt:1;
	u8 uc_pkt_chk_cam_match:1;
	u8 bc_pkt_chk_cam_match:1;
	u8 mc_pkt_white_lst_mode:1;
	u8 bcn_chk_en:1;
	u8 bcn_chk_rule:2;
	u8 acpt_pwr_mngt_pkt:1;
	u8 acpt_crc32_err_pkt:1;
	u8 acpt_unsupport_pkt:1;
	u8 acpt_mac_hdr_content_err_pkt:1;
	u8 acpt_ftm_req_pkt:1;
	u8 pkt_len_fltr:6;
	u8 unsp_pkt_target:2;
	u8 uid_fltr:2;
	// plcp fltr
	u8 cck_crc_chk_enable:1;
	u8 cck_sig_chk_enable:1;
	u8 lsig_parity_chk_enable:1;
	u8 siga_crc_chk_enable:1;
	u8 vht_su_sigb_crc_chk_enable:1;
	u8 vht_mu_sigb_crc_chk_enable:1;
	u8 he_sigb_crc_chk_enable:1;
	u8 min_len_chk_disable:1;
};

struct mac_ax_dfs_rpt {
	u8 *dfs_ptr;
	u16 drop_num;
	u16 max_cont_drop;
	u16 total_drop;
	u16 dfs_num;
};

struct mac_ax_ppdu_usr {
	u8 vld:1;
	u8 has_data:1;
	u8 has_ctrl:1;
	u8 has_mgnt:1;
	u8 has_bcn:1;
	u8 macid;
};

struct mac_ax_ppdu_stat {
	u8 band;
#define MAC_AX_PPDU_MAC_INFO BIT(1)
#define MAC_AX_PPDU_PLCP BIT(3)
#define MAC_AX_PPDU_RX_CNT BIT(2)
	u8 bmp_append_info;
#define MAC_AX_PPDU_HAS_A1M BIT(4)
#define MAC_AX_PPDU_HAS_CRC_OK BIT(5)
	u8 bmp_filter;
	u8 dup2fw_en;
	u8 dup2fw_len;
};

struct mac_ax_ch_info {
#define MAC_AX_CH_INFO_MACID 0
#define MAC_AX_CH_INFO_NDP 1
#define MAC_AX_CH_INFO_SND 2
#define MAC_AX_CH_INFO_ACK 3
	u8 trigger;
	u8 macid;
#define MAC_AX_CH_INFO_CRC_FAIL BIT(0)
#define MAC_AX_CH_INFO_DATA_FRM BIT(1)
#define MAC_AX_CH_INFO_CTRL_FRM BIT(2)
#define MAC_AX_CH_INFO_MGNT_FRM BIT(3)
	u8 bmp_filter;
	u8 dis_to;
#define MAC_AX_CH_IFNO_SEG_128 0
#define MAC_AX_CH_IFNO_SEG_256 1
#define MAC_AX_CH_IFNO_SEG_512 2
#define MAC_AX_CH_IFNO_SEG_1024 3
	u8 seg_size;
};

struct mac_ax_dfs {
#define MAC_AX_DFS_TH_29 0
#define MAC_AX_DFS_TH_61 1
#define MAC_AX_DFS_TH_93 2
#define MAC_AX_DFS_TH_125 3
	u8 num_th;
	u8 en_timeout;
};

struct mac_ax_ppdu_rpt {
#define MAC_AX_PPDU_MAX_USR 4
	u8 *rx_cnt_ptr;
	u8 *plcp_ptr;
	u8 *phy_st_ptr;
	u32 phy_st_size;
	u32 rx_cnt_size;
	u16 lsig_len;
	u16 service;
	u8 usr_num;
	u8 fw_def;
	u8 is_to_self;
	u8 plcp_size;
	struct mac_ax_ppdu_usr usr[MAC_AX_PPDU_MAX_USR];
};

struct mac_ax_phy_rpt_cfg {
	enum mac_ax_phy_rpt type;
	u8 en;
#define MAC_AX_PRPT_DEST_HOST 0
#define MAC_AX_PRPT_DEST_WLCPU 1
	u8 dest;
	union {
		struct mac_ax_ppdu_stat ppdu;
		struct mac_ax_ch_info chif;
		struct mac_ax_dfs dfs;
	} u;
};

struct mac_ax_pkt_drop_info {
	enum mac_ax_pkt_drop_sel sel;
	u8 macid;
	u8 band;
	u8 port;
	u8 mbssid;
};

struct mac_ax_ch_busy_cnt_ref {
	u8 basic_nav:1;
	u8 intra_nav:1;
	u8 data_on:1;
	u8 edcca_p20:1;
	u8 cca_p20:1;
	u8 cca_s20:1;
	u8 cca_s40:1;
	u8 cca_s80:1;
};

struct mac_ax_tx_queue_empty {
	u8 macid_txq_empty[16];
	u8 band0_mgnt_empty:1;
	u8 band1_mgnt_empty:1;
	u8 fw_txq_empty:1;
	u8 h2c_empty:1;
	u8 others_empty:1;
	u8 rsvd:3;
};

struct mac_ax_rx_queue_empty {
	u8 band0_rxq_empty:1;
	u8 band1_rxq_empty:1;
	u8 c2h_empty:1;
	u8 others_empty:1;
	u8 rsvd:4;
};

/*--------------------Define TF2PCMD related struct --------------------------*/
struct mac_ax_rura_report {
	u8 rt_tblcol: 6;
	u8 prtl_alloc: 1;
	u8 rate_chg: 1;
};

//for ul rua output
struct mac_ax_ulru_out_sta_ent {
	u8 dropping: 1;
	u8 tgt_rssi: 7;
	u8 mac_id;
	u8 ru_pos;
	u8 coding: 1;
	u8 vip_flag: 1;
	u8 rsvd1: 6;
	u16 bsr_length: 15;
	u16 rsvd2: 1;
	struct mac_ax_ru_rate_ent rate;
	struct mac_ax_rura_report rpt;
};

struct mac_ax_ulrua_output {
	u8 ru2su: 1;
	u8 ppdu_bw: 2;
	u8 gi_ltf: 3;
	u8 stbc: 1;
	u8 doppler: 1;
	u8 n_ltf_and_ma: 3;
	u8 sta_num: 4;
	u8 rsvd1: 1;
	u16 rf_gain_fix: 1;
	u16 rf_gain_idx: 10;
	u16 tb_t_pe_nom: 2;
	u16 rsvd2: 3;

	u32 grp_mode: 1;
	u32 grp_id: 6;
	u32 fix_mode: 1;
	u32 rsvd3: 24;
	struct mac_ax_ulru_out_sta_ent sta[MAC_AX_MAX_RU_NUM];
};

struct mac_ul_macid_info {
	u8 macid;
	u8 pref_AC:2;
	u8 rsvd:6;
};

struct mac_ul_mode_cfg {
	u32 mode:2; /* 0: peoridic ; 1: normal ; 2: non_tgr */
	u32 interval:6; /* unit: sec */
	u32 bsr_thold:8;
	u32 storemode:2;
	u32 rsvd:14;
};

struct mac_ax_ul_fixinfo {
	struct mac_ax_tbl_hdr tbl_hdr;
	struct mac_ul_mode_cfg cfg;

	u32 ndpa_dur:16;
	u32 tf_type:3;
	u32 sig_ta_pkten:1;
	u32 sig_ta_pktsc:4;
	u32 murts_flag:1;
	u32 ndpa:2;
	u32 snd_pkt_sel:2;
	u32 gi_ltf:3;

	u32 data_rate:9;
	u32 data_er:1;
	u32 data_bw:2;
	u32 data_stbc:2;
	u32 data_ldpc:1;
	u32 data_dcm:1;
	u32 apep_len:12;
	u32 more_tf:1;
	u32 data_bw_er:1;
	u32 istwt:1;
	u32 rsvd0:1;

	u32 multiport_id:3;
	u32 mbssid:4;
	u32 txpwr_mode:3;
	u32 ulfix_usage:3;
	u32 twtgrp_stanum_sel:2;
	u32 store_idx:4;
	u32 rsvd1:13;
	struct mac_ul_macid_info sta[4];
	struct mac_ax_ulrua_output ulrua;
};

struct mac_ax_mudecision_para {
	struct mac_ax_tbl_hdr tbl_hdr;
	u32 mu_thold:30;
	u32 bypass_thold:1; //macid bypass tx time thold check
	u32 bypass_tp:1; //T1 unit:us
};

struct mac_ax_protect_rsp_field {
	u8 protect: 4;
	u8 rsp: 4;
};

struct mac_ax_mu_protect_rsp_type {
	union {
		u8 byte_type;
		struct mac_ax_protect_rsp_field feld_type;
	} u;
};

struct mac_ax_mu_sta_upd {
	u8 macid;
	u8 mu_idx;
	struct mac_ax_mu_protect_rsp_type prot_rsp_type[5];
	u8 mugrp_bitmap: 5;
	u8 dis_256q: 1;
	u8 dis_1024q: 1;
	u8 rsvd: 1;
};

struct mac_ax_wlaninfo_get {
	u32 info_sel:4;
	u32 rsvd0:4;
	u32 argv0:8;
	u32 argv1:8;
	u32 argv2:8;
	u32 argv3:8;
	u32 argv4:8;
	u32 argv5:8;
	u32 argv6:8;
	u32 argv7:8;
	u32 rsvd1:24;
};

struct mac_ax_dumpwlanc {
	u32 cmdid:8;
	u32 rsvd0:24;
};

struct mac_ax_dumpwlans {
	u32 cmdid:8;
	u32 macid_grp:8;
	u32 rsvd0:16;
};

struct mac_ax_dumpwland {
	u32 cmdid:8;
	u32 grp_type:8;
	u32 grp_id:8;
	u32 muru:8;
	u8 macid[4];
};

struct mac_ax_fixmode_para {
	struct mac_ax_tbl_hdr tbl_hdr;
	u32 force_sumuru_en: 1;
	u32 forcesu: 1;
	u32 forcemu: 1;
	u32 forceru: 1;
	u32 fix_fe_su_en:1;
	u32 fix_fe_vhtmu_en:1;
	u32 fix_fe_hemu_en:1;
	u32 fix_fe_heru_en:1;
	u32 fix_fe_ul_en:1;
	u32 fix_frame_seq_su: 1;
	u32 fix_frame_seq_vhtmu: 1;
	u32 fix_frame_seq_hemu: 1;
	u32 fix_frame_seq_heru: 1;
	u32 fix_frame_seq_ul: 1;
	u32 is_dlruhwgrp: 1;
	u32 is_ulruhwgrp:1;
	u32 prot_type_su: 4;
	u32 prot_type_vhtmu: 4;
	u32 resp_type_vhtmu: 4;
	u32 prot_type_hemu: 4;
	u32 resp_type_hemu: 4;
	u32 prot_type_heru: 4;
	u32 resp_type_heru: 4;
	u32 ul_prot_type: 4;
	u32 rugrpid: 5;
	u32 mugrpid:5;
	u32 ulgrpid:5;
	u32 rsvd1:1;
};

struct mac_ax_tf_ba {
	u32 fix_ba:1;
	u32 ru_psd:9;
	u32 tf_rate:9;
	u32 rf_gain_fix:1;
	u32 rf_gain_idx:10;
	u32 tb_ppdu_bw:2;
	struct mac_ax_ru_rate_ent rate;
	u8 gi_ltf:3;
	u8 doppler:1;
	u8 stbc:1;
	u8 sta_coding:1;
	u8 tb_t_pe_nom:2;
	u8 pr20_bw_en:1;
	u8 ma_type: 1;
	u8 rsvd1: 6;
};

struct mac_ax_ba_infotbl {
	struct mac_ax_tbl_hdr tbl_hdr;
	struct mac_ax_tf_ba tfba;
};

struct mac_ax_dl_ru_grptbl {
	struct mac_ax_tbl_hdr tbl_hdr;
	u16 ppdu_bw:2;
	u16 tx_pwr:9;
	u16 pwr_boost_fac:5;
	u8 fix_mode_flag:1;
	u8 rsvd1:7;
	u8 rsvd;
	struct mac_ax_tf_ba tf;
};

struct mac_ax_ul_ru_grptbl {
	struct mac_ax_tbl_hdr tbl_hdr;
	u32 grp_psd_max: 9;
	u32 grp_psd_min: 9;
	u32 tf_rate: 9;
	u32 fix_tf_rate: 1;
	u32 rsvd2: 4;
	u16 ppdu_bw: 2;
	u16 rf_gain_fix: 1;
	u16 rf_gain_idx: 10;
	u16 fix_mode_flag: 1;
	u16 rsvd1: 2;
};

struct mac_ax_bb_stainfo {
	struct mac_ax_tbl_hdr tbl_hdr;
//sta capability
	u8 gi_ltf_48spt:1;
	u8 gi_ltf_18spt:1;
	u8 rsvd3:6;
//downlink su
	u8 dlsu_info_en:1;
	u8 dlsu_bw:2;
	u8 dlsu_gi_ltf:3;
	u8 dlsu_doppler_ctrl:2;
	u8 dlsu_coding:1;
	u8 dlsu_txbf:1;
	u8 dlsu_stbc:1;
	u8 dl_fwcqi_flag:1;
	u8 dlru_ratetbl_ridx:4;
	u8 csi_info_bitmap;
	u32 dl_swgrp_bitmap;
	u16 dlsu_dcm:1;
	u16 rsvd1:6;
	u16 dlsu_rate:9;
	u8 dlsu_pwr:6;
	u8 rsvd2:2;
	u8 rsvd4;
//uplink su
	u8 ulsu_info_en:1;
	u8 ulsu_bw:2;
	u8 ulsu_gi_ltf:3;
	u8 ulsu_doppler_ctrl:2;
	u8 ulsu_dcm:1;
	u8 ulsu_ss:3;
	u8 ulsu_mcs:4;
	u16 ul_fwcqi_flag:1;
	u16 ulru_ratetbl_ridx:4;
	u16 ulsu_stbc:1;
	u16 ulsu_coding:1;
	u16 ulsu_rssi_m:9;
	u32 ul_swgrp_bitmap;
//tb info
};

struct mac_ax_tf_depend_user_para {
	u8 pref_AC: 2;
	u8 rsvd: 6;
};

struct mac_ax_tf_user_para {
	u16 aid12: 12;
	u16 ul_mcs: 4;
	u8 macid;
	u8 ru_pos;

	u8 ul_fec_code: 1;
	u8 ul_dcm: 1;
	u8 ss_alloc: 6;
	u8 ul_tgt_rssi: 7;
	u8 rsvd: 1;
	u16 rsvd2;
};

struct mac_ax_tf_pkt_para {
	u8 ul_bw: 2;
	u8 gi_ltf: 2;
	u8 num_he_ltf: 3;
	u8 ul_stbc: 1;
	u8 doppler: 1;
	u8 ap_tx_power: 6;
	u8 rsvd0: 1;
	u8 user_num: 3;
	u8 pktnum: 3;
	u8 rsvd1: 2;
	u8 pri20_bitmap;

	struct mac_ax_tf_user_para user[MAC_AX_MAX_RU_NUM];
	struct mac_ax_tf_depend_user_para dep_user[MAC_AX_MAX_RU_NUM];
};

struct mac_ax_tf_wd_para {
	u16 datarate: 9;
	u16 mulport_id: 3;
	u16 pwr_ofset: 3;
	u16 rsvd: 1;
};

struct mac_ax_f2p_test_para {
	struct mac_ax_tf_pkt_para tf_pkt;
	struct mac_ax_tf_wd_para tf_wd;
	u8 mode: 2;
	u8 frexch_type: 6;
	u8 sigb_len;
};

struct mac_ax_f2p_wd {
	/* dword 0 */
	u32 cmd_qsel:6;
	u32 rsvd0:2;
	u32 rsvd1:2;
	u32 ls:1;
	u32 fs:1;
	u32 total_number:4;
	u32 seq:8;
	u32 length:8;
	/* dword 1 */
	u32 rsvd2;
};

struct mac_ax_f2p_tx_cmd {
	/* dword 0 */
	u32 cmd_type:8;
	u32 cmd_sub_type:8;
	u32 dl_user_num:5;
	u32 bw:2;
	u32 tx_power:9;
	/* dword 1 */
	u32 fw_define:16;
	u32 ss_sel_mode:2;
	u32 next_qsel:6;
	u32 twt_group:4;
	u32 dis_chk_slp:1;
	u32 ru_mu_2_su:1;
	u32 dl_t_pe:2;
	/* dword 2 */
	u32 sigb_ch1_len:8;
	u32 sigb_ch2_len:8;
	u32 sigb_sym_num:6;
	u32 sigb_ch2_ofs:5;
	u32 dis_htp_ack:1;
	u32 tx_time_ref:2;
	u32 pri_user_idx:2;
	/* dword 3 */
	u32 ampdu_max_txtime:14;
	u32 rsvd0:2;
	u32 group_id:6;
	u32 rsvd1:2;
	u32 rsvd2:4;
	u32 twt_chk_en:1;
	u32 twt_port_id:3;
	/* dword 4 */
	u32 twt_start_time:32;
	/* dword 5 */
	u32 twt_end_time:32;
	/* dword 6 */
	u32 apep_len:12;
	u32 tri_pad:2;
	u32 ul_t_pe:2;
	u32 rf_gain_idx:10;
	u32 fixed_gain_en:1;
	u32 ul_gi_ltf:3;
	u32 ul_doppler:1;
	u32 ul_stbc:1;
	/* dword 7 */
	u32 ul_mid_per:1;
	u32 ul_cqi_rrp_tri:1;
	u32 rsvd3:6;
	u32 rsvd4:8;
	u32 sigb_dcm:1;
	u32 sigb_comp:1;
	u32 doppler:1;
	u32 stbc:1;
	u32 mid_per:1;
	u32 gi_ltf_size:3;
	u32 sigb_mcs:3;
	u32 rsvd5:5;
	/* dword 8 */
	u32 macid_u0:8;
	u32 ac_type_u0:2;
	u32 mu_sta_pos_u0:2;
	u32 dl_rate_idx_u0:9;
	u32 dl_dcm_en_u0:1;
	u32 rsvd6:2;
	u32 ru_alo_idx_u0:8;
	/* dword 9 */
	u32 pwr_boost_u0:5;
	u32 agg_bmp_alo_u0:3;
	u32 ampdu_max_txnum_u0:8;
	u32 user_define_u0:8;
	u32 user_define_ext_u0:8;
	/* dword 10 */
	u32 ul_addr_idx_u0:8;
	u32 ul_dcm_u0:1;
	u32 ul_fec_cod_u0:1;
	u32 ul_ru_rate_u0:7;
	u32 rsvd8:7;
	u32 ul_ru_alo_idx_u0:8;
	/* dword 11 */
	u32 rsvd9:32;
	/* dword 12 */
	u32 macid_u1:8;
	u32 ac_type_u1:2;
	u32 mu_sta_pos_u1:2;
	u32 dl_rate_idx_u1:9;
	u32 dl_dcm_en_u1:1;
	u32 rsvd10:2;
	u32 ru_alo_idx_u1:8;
	/* dword 13 */
	u32 pwr_boost_u1:5;
	u32 agg_bmp_alo_u1:3;
	u32 ampdu_max_txnum_u1:8;
	u32 user_define_u1:8;
	u32 user_define_ext_u1:8;
	/* dword 14 */
	u32 ul_addr_idx_u1:8;
	u32 ul_dcm_u1:1;
	u32 ul_fec_cod_u1:1;
	u32 ul_ru_rate_u1:7;
	u32 rsvd12:7;
	u32 ul_ru_alo_idx_u1:8;
	/* dword 15 */
	u32 rsvd13:32;
	/* dword 16 */
	u32 macid_u2:8;
	u32 ac_type_u2:2;
	u32 mu_sta_pos_u2:2;
	u32 dl_rate_idx_u2:9;
	u32 dl_dcm_en_u2:1;
	u32 rsvd14:2;
	u32 ru_alo_idx_u2:8;
	/* dword 17 */
	u32 pwr_boost_u2:5;
	u32 agg_bmp_alo_u2:3;
	u32 ampdu_max_txnum_u2:8;
	u32 user_define_u2:8;
	u32 user_define_ext_u2:8;
	/* dword 18 */
	u32 ul_addr_idx_u2:8;
	u32 ul_dcm_u2:1;
	u32 ul_fec_cod_u2:1;
	u32 ul_ru_rate_u2:7;
	u32 rsvd16:7;
	u32 ul_ru_alo_idx_u2:8;
	/* dword 19 */
	u32 rsvd17:32;
	/* dword 20 */
	u32 macid_u3:8;
	u32 ac_type_u3:2;
	u32 mu_sta_pos_u3:2;
	u32 dl_rate_idx_u3:9;
	u32 dl_dcm_en_u3:1;
	u32 rsvd18:2;
	u32 ru_alo_idx_u3:8;
	/* dword 21 */
	u32 pwr_boost_u3:5;
	u32 agg_bmp_alo_u3:3;
	u32 ampdu_max_txnum_u3:8;
	u32 user_define_u3:8;
	u32 user_define_ext_u3:8;
	/* dword 22 */
	u32 ul_addr_idx_u3:8;
	u32 ul_dcm_u3:1;
	u32 ul_fec_cod_u3:1;
	u32 ul_ru_rate_u3:7;
	u32 rsvd20:7;
	u32 ul_ru_alo_idx_u3:8;
	/* dword 23 */
	u32 rsvd21:32;
	/* dword 24 */
	u32 pkt_id_0:12;
	u32 rsvd22:3;
	u32 valid_0:1;
	u32 ul_user_num_0:4;
	u32 rsvd23:12;
	/* dword 25 */
	u32 pkt_id_1:12;
	u32 rsvd24:3;
	u32 valid_1:1;
	u32 ul_user_num_1:4;
	u32 rsvd25:12;
	/* dword 26 */
	u32 pkt_id_2:12;
	u32 rsvd26:3;
	u32 valid_2:1;
	u32 ul_user_num_2:4;
	u32 rsvd27:12;
	/* dword 27 */
	u32 pkt_id_3:12;
	u32 rsvd28:3;
	u32 valid_3:1;
	u32 ul_user_num_3:4;
	u32 rsvd29:12;
	/* dword 28 */
	u32 pkt_id_4:12;
	u32 rsvd30:3;
	u32 valid_4:1;
	u32 ul_user_num_4:4;
	u32 rsvd31:12;
	/* dword 29 */
	u32 pkt_id_5:12;
	u32 rsvd32:3;
	u32 valid_5:1;
	u32 ul_user_num_5:4;
	u32 rsvd33:12;
};

/*--------------------Define Sounding related struct -------------------------*/
struct mac_reg_csi_para {
	u32 band: 1;
	u32 portsel: 1;
	u32 nc: 3;
	u32 nr: 3;
	u32 ng: 2;
	u32 cb: 2;
	u32 cs: 2;
	u32 ldpc_en: 1;
	u32 stbc_en: 1;
	u32 bf_en: 1;
};

struct mac_cctl_csi_para {
	u8 macid;
	u32 band: 1;
	u32 nc: 3;
	u32 nr: 3;
	u32 ng: 2;
	u32 cb: 2;
	u32 cs: 2;
	u32 bf_en: 1;
	u32 stbc_en: 1;
	u32 ldpc_en: 1;
	u32 rate: 9;
	u32 gi_ltf: 3;
	u32 gid_sel: 1;
	u32 bw: 2;
};

struct mac_ax_ndpa_hdr {
	u16 frame_ctl;
	u16 duration;
	u8 addr1[6];
	u8 addr2[6];
};

struct mac_ax_snd_dialog {
	u32 he: 1;
	u32 dialog: 6;
	u32 rsvd: 25;
};

struct mac_ax_ht_ndpa_para {
	u8 addr3[WLAN_ADDR_LEN];
	u16 seq_control;
};

struct mac_ax_vht_ndpa_sta_info {
	u16 aid: 12;
	u16 fb_type: 1;
	u16 nc: 3;
};

struct mac_ax_vht_ndpa_para {
	struct mac_ax_vht_ndpa_sta_info sta_info[MAX_VHT_SUPPORT_SOUND_STA];
};

struct mac_ax_he_ndpa_sta_info {
	u32 aid: 11;
	u32 bw: 14;
	u32 fb_ng: 2;
	u32 disambiguation: 1;
	u32 cb: 1;
	u32 nc: 3;
};

struct mac_ax_he_ndpa_para {
	struct mac_ax_he_ndpa_sta_info sta_info[MAX_HE_SUPPORT_SOUND_STA];
};

struct mac_ax_ndpa_para {
	struct mac_ax_ndpa_hdr common;
	struct mac_ax_snd_dialog snd_dialog;
	struct mac_ax_ht_ndpa_para ht_para;
	struct mac_ax_vht_ndpa_para vht_para;
	struct mac_ax_he_ndpa_para he_para;
};

struct mac_ax_bfrp_hdr {
	u16 frame_ctl;
	u16 duration;
	u8 addr1[WLAN_ADDR_LEN];
	u8 addr2[WLAN_ADDR_LEN];
};

struct mac_ax_vht_bfrp_para {
	u8 retransmission_bitmap;
};

struct mac_ax_he_bfrp_common {
	u32 tgr_info: 4;
	u32 ul_len: 12;
	u32 more_tf: 1;
	u32 cs_rqd: 1;
	u32 ul_bw: 2;
	u32 gi_ltf: 2;
	u32 mimo_ltfmode: 1;
	u32 num_heltf: 3;
	u32 ul_pktext: 3;
	u32 ul_stbc: 1;
	u32 ldpc_extra_sym: 1;
	u32 dplr: 1;

	u32 ap_tx_pwr: 6;
	u32 ul_sr: 16;
	u32 ul_siga2_rsvd: 9;
	u32 rsvd: 1;
};

struct mac_ax_he_bfrp_user {
	u32 aid12: 12;
	u32 ru_pos: 8;
	u32 ul_fec_code: 1;
	u32 ul_mcs: 4;
	u32 ul_dcm: 1;
	u32 ss_alloc: 6;

	u32 fbseg_rexmit_bmp: 8;
	u32 ul_tgt_rssi: 7;
	u32 rsvd: 17;
};

struct mac_ax_he_bfrp_para {
	struct mac_ax_he_bfrp_common common;
	struct mac_ax_he_bfrp_user user[4];
};

struct mac_ax_bfrp_para {
	struct mac_ax_bfrp_hdr hdr[3];
	struct mac_ax_he_bfrp_para he_para[2];
	struct mac_ax_vht_bfrp_para vht_para[3];
	u8 rsvd;
};

struct mac_ax_snd_wd_para {
	u16 txpktsize;
	u16 ndpa_duration;

	u16 datarate: 9;
	u16 macid: 7;//wd
	u8 force_txop: 1;
	u8 data_bw: 2;
	u8 gi_ltf: 3;
	u8 data_er: 1;
	u8 data_dcm: 1;
	u8 data_stbc: 1;
	u8 data_ldpc: 1;
	u8 data_bw_er : 1;
	u8 multiport_id: 1;
	u8 mbssid: 4;

	u8 signaling_ta_pkt_sc: 4;
	u8 sw_define: 4;
	u8 txpwr_ofset_type: 3;
	u8 lifetime_sel: 3;
	u8 stf_mode: 1;
	u8 disdatafb: 1;
	u8 data_txcnt_lmt_sel: 1;
	u8 data_txcnt_lmt: 6;
	u8 sifs_tx: 1;
	u8 snd_pkt_sel: 3;
	u8 ndpa: 2;
	u8 rsvd: 3;
};

struct mac_ax_snd_f2P {
	u16 csi_len_bfrp: 12;
	u16 tb_t_pe_bfrp: 2;
	u16 tri_pad_bfrp: 2;

	u16 ul_cqi_rpt_tri_bfrp: 1;
	u16 rf_gain_idx_bfrp: 10;
	u16 fix_gain_en_bfrp: 1;
	u16 rsvd: 4;
};

struct mac_ax_fwcmd_snd {
	u32 frexgtype: 6;
	u32 mode: 2;
	u32 bfrp0_user_num: 3;
	u32 bfrp1_user_num: 3;
	u32 rsvd: 18;
	u8 macid[8];
	struct mac_ax_ndpa_para pndpa;
	struct mac_ax_bfrp_para pbfrp;
	struct mac_ax_snd_wd_para wd[5];
	struct mac_ax_snd_f2P f2p[2];
};

struct mac_ax_ie_cam_info {
	u8 type;
	u8 ienum_ie;
	u8 ie_ofst_len;
	u8 ie_msk_crc;
	u8 ie_val;
	u8 rsvd0;
	u8 rsvd1;
	u8 rsvd2;
};

/*--------------------Define wowlan related struct ---------------------------*/

struct mac_ax_keep_alive_info {
	u8 keepalive_en: 1;
	u8 rsvd: 7;
	u8 packet_id;
	u8 period;
};

struct mac_ax_disconnect_det_info {
	u8 disconnect_detect_en: 1;
	u8 tryok_bcnfail_count_en: 1;
	u8 disconnect_en: 1;
	u8 rsvd: 5;
	u8 check_period;
	u8 try_pkt_count;
	u8 tryok_bcnfail_count_limit;
};

enum mac_ax_enc_alg {
	MAC_AX_RTW_ENC_NONE = 0,
	MAC_AX_RTW_ENC_WEP40 = 1,
	MAC_AX_RTW_ENC_WEP104,
	MAC_AX_RTW_ENC_TKIP,
	MAC_AX_RTW_ENC_WAPI,
	MAC_AX_RTW_ENC_GCMSMS4,
	MAC_AX_RTW_ENC_CCMP,
	MAC_AX_RTW_ENC_CCMP256,
	MAC_AX_RTW_ENC_GCMP,
	MAC_AX_RTW_ENC_GCMP256,
	MAC_AX_RTW_ENC_BIP_CCMP128,
	MAC_AX_RTW_ENC_MAX
};

enum bip_sec_algo_type {
	BIP_CMAC_128 = 0,
	BIP_CMAC_256 = 1,
	BIP_GMAC_128 = 2,
	BIP_GMAC_256 = 3
};

struct mac_ax_wow_wake_info {
	u8 wow_en: 1;
	u8 drop_all_pkt: 1;
	u8 rx_parse_after_wake: 1;
	u8 rsvd: 5;
	enum mac_ax_enc_alg pairwise_sec_algo;
	enum mac_ax_enc_alg group_sec_algo;
	u32 remotectrl_info_content;
	u8 pattern_match_en: 1;
	u8 magic_en: 1;
	u8 hw_unicast_en: 1;
	u8 fw_unicast_en: 1;
	u8 deauth_wakeup: 1;
	u8 rekey_wakeup: 1;
	u8 eap_wakeup: 1;
	u8 all_data_wakeup: 1;
};

#define IV_LENGTH 8

struct mac_ax_remotectrl_info_parm_ {
	u8  ptktxiv[IV_LENGTH];
	/* value = 0xdd */
	u8  validcheck;
	/* bit0 : check ptk, bit1 : check gtk */
	u8  symbolchecken;
	/* the last gtk index used by driver */
	u8  lastkeyid;
	u8  rsvd[5];
	/* unicast iv */
	u8  rxptkiv[IV_LENGTH];
	/* broadcast/mulicast iv, 4 gtk index */
	u8  rxgtkiv_0[IV_LENGTH];
	u8  rxgtkiv_1[IV_LENGTH];
	u8  rxgtkiv_2[IV_LENGTH];
	u8  rxgtkiv_3[IV_LENGTH];
};

struct mac_ax_wake_ctrl_info {
	u8 pattern_match_en: 1;
	u8 magic_en: 1;
	u8 hw_unicast_en: 1;
	u8 fw_unicast_en: 1;
	u8 deauth_wakeup: 1;
	u8 rekey_wakeup: 1;
	u8 eap_wakeup: 1;
	u8 all_data_wakeup: 1;
};

struct mac_ax_gtk_ofld_info {
	u8 gtk_en: 1;
	u8 tkip_en: 1;
	u8 ieee80211w_en: 1;
	u8 pairwise_wakeup: 1;
	u8 bip_sec_algo: 2;
	u8 rsvd: 2;
	u8 gtk_rsp_id: 8;
	u8 pmf_sa_query_id: 8;
	u8 algo_akm_suit: 8;
};

#define AOAC_REPORT_VERSION 1

struct mac_ax_aoac_report {
	u8 rpt_ver;
	u8 sec_type;
	u8 key_idx;
	u8 pattern_idx;
	u8 rekey_ok: 1;
	u8 rsvd0: 7;
	u8 rsvd1[3];
	u8 ptk_tx_iv[IV_LENGTH];
	u8 eapol_key_replay_count[8];
	u8 gtk[32];
	u8 ptk_rx_iv[IV_LENGTH];
	u8 gtk_rx_iv_0[IV_LENGTH];
	u8 gtk_rx_iv_1[IV_LENGTH];
	u8 gtk_rx_iv_2[IV_LENGTH];
	u8 gtk_rx_iv_3[IV_LENGTH];
	u8 igtk_key_id[8];
	u8 igtk_ipn[8];
	u8 igtk[32];
};

#define EAPOL_KCK_LENGTH 32
#define EAPOL_KEK_LENGTH 32
#define TKIP_TK_LENGTH 16
#define TKIP_MIC_KEY_LENGTH 8
#define IGTK_KEY_ID_LENGTH 4
#define IGTK_PKT_NUM_LENGTH 8
#define IGTK_LENGTH 16
#define IGTK_OFFSET 4

union keytype {
	u8 SKEY[32];
	u32 LKEY[4];
};

struct mac_ax_gtk_info_parm_ {
	/* eapol - key key confirmation key (kck) */
	u8  kck[EAPOL_KCK_LENGTH];
	/* eapol - key key encryption key (kek) */
	u8  kek[EAPOL_KEK_LENGTH];
	/* temporal key 1 (tk1) */
	u8  tk1[TKIP_TK_LENGTH];
	u8  txmickey[TKIP_MIC_KEY_LENGTH];
	u8  rxmickey[TKIP_MIC_KEY_LENGTH];
	u8 igtk_keyid[IGTK_KEY_ID_LENGTH];
	u8 ipn[IGTK_PKT_NUM_LENGTH];
	union keytype igtk[2];
	union keytype sk[1];
};

struct mac_ax_arp_ofld_info {
	u8 arp_en: 1;
	u8 arp_action: 1;
	u8 rsvd: 6;
	u8 arp_rsp_id: 8;
};

struct mac_ax_ndp_ofld_info {
	u8 ndp_en: 1;
	u8 rsvd: 7;
	u8 na_id: 8;
};

#define MAC_ADDRESS_LENGTH    6
#define IPV6_ADDRESS_LENGTH   16

struct mac_ax_ndp_info_parm_ {
	u8 enable: 1;
	/* need to check sender ip or not */
	u8 checkremoveip: 1;
	/* need to check sender ip or not */
	u8 rsvd: 6;
	/* number of check ip which na query ip */
	u8 numberoftargetip;
	/* maybe support change mac address !! */
	u8 targetlinkaddress[MAC_ADDRESS_LENGTH];
	/* just respond ip */
	u8 remoteipv6address[IPV6_ADDRESS_LENGTH];
	/* target ip */
	u8 targetip[2][IPV6_ADDRESS_LENGTH];
};

struct mac_ax_realwow_info {
	u8 realwow_en: 1;
	u8 auto_wakeup: 1;
	u8 rsvd0: 6;
	u8 keepalive_id: 8;
	u8 wakeup_pattern_id: 8;
	u8 ack_pattern_id: 8;
};

struct mac_ax_realwowv2_info_parm_ {
	u16 interval;   /*unit : 1 ms */
	u16 kapktsize;
	u16 acklostlimit;
	u16 ackpatternsize;
	u16 wakeuppatternsize;
	u16 rsvd;
	u32 wakeupsecnum;
};

struct mac_ax_nlo_info {
	u8 nlo_en: 1;
	u8 nlo_32k_en: 1;
	u8 ignore_cipher_type: 1;
	u8 rsvd: 5;
};

#define MAX_SUPPORT_NL_NUM   16
#define MAX_PROBE_REQ_NUM    8
#define SSID_MAX_LEN         32

struct mac_ax_nlo_networklist_parm_ {
	u8  numofentries;
	u8  numofhiddenap;
	u8  rsvd[2];
	u32 patterncheck;
	u32 rsvd1;
	u32 rsvd2;
	u8  ssidlen[MAX_SUPPORT_NL_NUM];
	u8  chipertype[MAX_SUPPORT_NL_NUM];
	u8  rsvd3[MAX_SUPPORT_NL_NUM];
	u8  locprobereq[MAX_PROBE_REQ_NUM];
	u8  ssid[MAX_SUPPORT_NL_NUM][SSID_MAX_LEN];
};

struct mac_ax_negative_pattern_info {
	u8 negative_pattern_en: 1;
	u8 rsvd: 3;
	u8 pattern_count: 4;
};

struct mac_ax_dev2hst_gpio_info {
	u8 dev2hst_gpio_en: 1;
	u8 gpio_active: 1;
	u8 gpio_input_en: 1;
	u8 gpio_input_for_low: 1;
	u8 disable_inband: 1;
	u8 data_pin_wakeup: 1;
	u8 rsvd0: 2;
	u8 gpio_pulse_en: 1;
	u8 gpio_pulse_nonstop: 1;
	u8 gpio_duration_unit: 1;
	u8 rsvd1: 5;
	u8 gpio_num: 8;
	u8 gpio_pulse_count: 8;
	u8 gpio_pulse_duration: 8;
	u8 customer_id: 8;
	u8 gpio_pulse_en_a: 1;
	u8 gpio_duration_unit_a: 1;
	u8 gpio_pulse_nonstop_a: 1;
	u8 rsvd4: 5;
	u8 special_reason_a: 8;
	u8 gpio_duration_a: 8;
	u8 gpio_pulse_count_a: 8;
	u8 gpio_pulse_en_b: 1;
	u8 gpio_duration_unit_b: 1;
	u8 gpio_pulse_nonstop_b: 1;
	u8 rsvd5: 5;
	u8 special_reason_b: 8;
	u8 gpio_duration_b: 8;
	u8 gpio_pulse_count_b: 8;
};

struct mac_ax_uphy_ctrl_info {
	u8 disable_uphy: 1;
	u8 handshake_mode: 3;
	u8 rsvd0: 4;
	u8 rise_hst2dev_dis_uphy: 1;
	u8 uphy_dis_delay_unit: 1;
	u8 pdn_as_uphy_dis: 1;
	u8 pdn_to_enable_uphy: 1;
	u8 rsvd1: 4;
	u8 hst2dev_gpio_num: 8;
	u8 uphy_dis_delay_count: 8;
};

struct mac_ax_wowcam_upd_info {
	u8 r_w: 1;
	u8 idx: 7;
	u8 rsvd0[3];
	u32 wkfm1: 32;
	u32 wkfm2: 32;
	u32 wkfm3: 32;
	u32 wkfm4: 32;
	u16 crc: 16;
	u8 rsvd1: 6;
	u8 negative_pattern_match: 1;
	u8 skip_mac_hdr: 1;
	u8 uc: 1;
	u8 mc: 1;
	u8 bc: 1;
	u8 rsvd2: 4;
	u8 valid: 1;
};

/*--------------------Define SET/GET HW VALUE struct -------------------------*/
struct mac_ax_sdio_info {
	enum mac_ax_sdio_4byte_mode sdio_4byte;
	enum mac_ax_sdio_tx_mode tx_mode;
	enum mac_ax_sdio_spec_ver spec_ver;
	u16 block_size;
	u8 tx_seq;
	u16 tx_align_size;
	u32 rpwm_bak;
};

struct mac_ax_sdio_txagg_cfg {
	u8 en;
	u16 align_size;
};

struct mac_ax_usb_info {
	u8 ep5;
	u8 ep6;
	u8 ep10;
	u8 ep11;
	u8 ep12;
	u8 max_bulkout_wd_num;
};

struct mac_ax_aval_page_cfg {
	u32 thold_wd;
	u32 thold_wp;
	u8 ch_dma;
	u8 en;
};

struct mac_ax_rx_agg_cfg {
	enum mac_ax_rx_agg_mode mode;
	struct mac_ax_rx_agg_thold thold;
};

struct mac_ax_ac_edca_param {
	u16 txop_32us;
	u8 ecw_max;
	u8 ecw_min;
	u8 aifs_us;
};

struct mac_ax_usr_edca_param {
	enum mac_ax_cmac_usr_edca_idx idx;
	u8 enable;
	u8 band;
	enum mac_ax_cmac_wmm_sel wmm;
	enum mac_ax_cmac_ac_sel ac;
	struct mac_ax_ac_edca_param aggressive;
	struct mac_ax_ac_edca_param moderate;
};

struct mac_ax_edca_param {
	u8 band;
	enum mac_ax_cmac_path_sel path;
	u16 txop_32us;
	u8 ecw_max;
	u8 ecw_min;
	u8 aifs_us;
};

struct mac_ax_muedca_param {
	u8 band;
	enum mac_ax_cmac_ac_sel ac;
	u16 muedca_timer_32us;
	u8 ecw_max;
	u8 ecw_min;
	u8 aifs_us;
};

struct mac_ax_muedca_timer {
	u8 band;
	enum mac_ax_cmac_ac_sel ac;
	u16 muedca_timer_32us;
};

struct mac_ax_muedca_cfg {
	u8 band;
	enum mac_ax_cmac_wmm_sel wmm_sel;
	u8 countdown_en;
	u8 tb_update_en;
};

struct mac_ax_sch_tx_en_cfg {
	u8 band;
	struct mac_ax_sch_tx_en tx_en;
	struct mac_ax_sch_tx_en tx_en_mask;
};

struct mac_ax_lifetime_cfg {
	u8 band;
	struct mac_ax_lifetime_en en;
	struct mac_ax_lifetime_val val;
};

struct mac_ax_tb_ppdu_ctrl {
	u8 band;
	enum mac_ax_cmac_ac_sel pri_ac;
	u8 be_dis;
	u8 bk_dis;
	u8 vi_dis;
	u8 vo_dis;
};

struct macid_tx_bak {
	struct mac_ax_sch_tx_en_cfg sch_bak;
	struct mac_ax_tb_ppdu_ctrl ac_dis_bak;
};

struct mac_ax_edcca_param {
	u8 band:1;
	u8 tb_check_en:1;
	u8 sifs_check_en:1;
	u8 ctn_check_en:1;
	u8 rsvd:4;
	enum mac_ax_edcca_sel sel;
};

#endif // NEO

struct mac_host_rpr_cfg {
	u8 agg;
	u8 tmr;
	u8 agg_def:1;
	u8 tmr_def:1;
	u8 rsvd:5;
	enum mac_func_sw txok_en;
	enum mac_func_sw rty_lmt_en;
	enum mac_func_sw lft_drop_en;
	enum mac_func_sw macid_drop_en;
};

#if 0 //NEO

struct mac_ax_macid_pause_cfg {
	u8 macid;
	u8 pause;
};

struct mac_ax_macid_pause_grp {
	u32 pause_grp[4];
	u32 mask_grp[4];
};

struct mac_ax_ampdu_cfg {
	u8 band;
	enum mac_ax_wdbk_mode wdbk_mode;
	enum mac_ax_rty_bk_mode rty_bk_mode;
	u16 max_agg_num;
	u8 max_agg_time_32us;
};

struct mac_ax_ch_stat_cnt {
	u8 band;
	u32 busy_cnt;
	u32 idle_cnt;
};

struct mac_ax_ch_busy_cnt_cfg {
	u8 band;
	enum mac_ax_ch_busy_cnt_ctrl cnt_ctrl;
	struct mac_ax_ch_busy_cnt_ref ref;
};

struct mac_ax_ss_wmm_tbl_ctrl {
	u8 wmm;
	enum mac_ax_ss_wmm_tbl wmm_mapping;
};

struct mac_ax_bt_block_tx {
	u8 band;
	u8 en;
};

struct mac_ax_rty_lmt {
	u32 tx_cnt;
	u8 macid;
};

struct mac_ax_cctl_rty_lmt_cfg {
	u8 macid;
	u8 data_lmt_sel:1;
	u8 data_lmt_val:6;
	u8 rsvd0:1;
	u8 rts_lmt_sel:1;
	u8 rts_lmt_val:4;
	u8 rsvd1:3;
};

struct mac_ax_cr_rty_lmt_cfg {
	u16 long_tx_cnt_lmt:6; /*CR: long rty*/
	u16 short_tx_cnt_lmt:6; /*CR: short rty*/
	enum mac_ax_band band;
};

struct mac_ax_rrsr_cfg {
	u32 rrsr_rate_en:4;
	u32 rsc:2;
	u32 doppler_en:1;
	u32 dcm_en:1;
	u32 ref_rate_sel:1;
	u32 ref_rate:9;
	u32 cck_cfg:4;
	u32 rsvd:10;

	u32 ofdm_cfg:8;
	u32 ht_cfg:8;
	u32 vht_cfg:8;
	u32 he_cfg:8;
};

struct mac_ax_bt_polt_cnt {
	u8 band;
	u16 cnt;
};

/*--------------------Define SRAM FIFO ---------------------------------------*/
struct mac_ax_bacam_info {
	u32 valid: 1;
	u32 init_req: 1;
	u32 entry_idx: 2;
	u32 tid: 4;
	u32 macid: 8;
	u32 bmap_size: 4;
	u32 ssn: 12;
};

struct mac_ax_shcut_mhdr {/*need to revise note by kkbomb 0204*/
// dword 0
	u32 mac_header_length:8;
	u32 dword0:24;
	u32 dword1;
	u32 dword2;
	u32 dword3;
	u32 dword4;
	u32 dword5;
	u32 dword6;
	u32 dword7;
	u32 dword8;
	u32 dword9;
	u32 dword10;
	u32 dword11;
	u32 dword12;
	u32 dword13;
};

struct mac_ax_ie_cam_cmd_info {
	u8 en:1;
	u8 band:1;
	u8 port:3;
	u8 hit_en:1;
	u8 miss_en:1;
	u8 rst:1;
	u8 hit_sel:2;
	u8 miss_sel:2;
	u8 rsvd0:4;
	u8 num:5;
	u8 rsvd1:3;
	u8 *buf;
	u32 buf_len;
};

#endif // NEO

struct mac_addr_cam_info {
	u8 addr_cam_idx;	/* Addr cam entry index */
	u8 offset;		/* Offset */
	u8 len;			/* Length */
	u8 valid : 1;
	u8 net_type : 2;
	u8 bcn_hit_cond : 2;
	u8 hit_rule : 2;
	u8 bb_sel : 1;
	u8 addr_mask : 6;
	u8 mask_sel : 2;
	u8 bssid_cam_idx : 6;
	u8 is_mul_ent : 1;
	u8 sma[6];
	u8 tma[6];
	u8 macid;
	u8 port_int: 3;
	u8 tsf_sync: 3;
	u8 tf_trs: 1;
	u8 lsig_txop: 1;
	u8 tgt_ind: 3;
	u8 frm_tgt_ind: 3;
	u16 aid12: 12;
	u8 wol_pattern: 1;
	u8 wol_uc: 1;
	u8 wol_magic: 1;
	u8 wapi: 1;
	u8 sec_ent_mode: 2;
	u8 sec_ent_keyid[7];
	u8 sec_ent_valid;
	u8 sec_ent[7];
};

struct mac_bssid_cam_info {
	u8 bssid_cam_idx;	/* BSSID cam entry index */
	u8 offset;		/* Offset */
	u8 len;			/* Length */
	u8 valid : 1;
	u8 bb_sel : 1;
	u8 bss_color : 7;
	u8 bssid[6];
};

struct mac_sec_cam_info {
	u8 sec_cam_idx;		/* Security cam entry index */
	u8 offset;		/* Offset */
	u8 len;			/* Length */
	u8 type : 4;
	u8 ext_key : 1;
	u8 spp_mode : 1;
	u32 key[4];
};

struct mac_macaddr {
	u8 macaddr[6];
};

struct mac_sta_init_info {
	u8 macid;
	u8 opmode:1;
	u8 band:1;
	u8 wmm:2;
	u8 trigger:1;
	u8 is_hesta: 1;
	u8 dl_bw: 2;
	u8 tf_mac_padding:2;
	u8 dl_t_pe:3;
	u8 port_id:3;
	u8 net_type:2;
	u8 wifi_role:4;
	u8 self_role:2;
};

struct mac_fwrole_maintain {
	u8 macid;
	u8 self_role : 2;
	u8 upd_mode : 3;
	u8 wifi_role : 4;
};

#if 0 //NEO

struct mac_ax_cctl_info {
	/* dword 0 */
	u32 datarate:9;
	u32 force_txop:1;
	u32 data_bw:2;
	u32 data_gi_ltf:3;
	u32 darf_tc_index:1;
	u32 arfr_ctrl:4;
	u32 acq_rpt_en:1;
	u32 mgq_rpt_en:1;
	u32 ulq_rpt_en:1;
	u32 twtq_rpt_en:1;
	u32 rsvd0:1;
	u32 disrtsfb:1;
	u32 disdatafb:1;
	u32 tryrate:1;
	u32 ampdu_density:4;
	/* dword 1 */
	u32 data_rty_lowest_rate:9;
	u32 ampdu_time_sel:1;
	u32 ampdu_len_sel:1;
	u32 rts_txcnt_lmt_sel:1;
	u32 rts_txcnt_lmt:4;
	u32 rtsrate:9;
	u32 rsvd1:2;
	u32 vcs_stbc:1;
	u32 rts_rty_lowest_rate:4;
	/* dword 2 */
	u32 data_tx_cnt_lmt:6;
	u32 data_txcnt_lmt_sel:1;
	u32 max_agg_num_sel:1;
	u32 rts_en:1;
	u32 cts2self_en:1;
	u32 cca_rts:2;
	u32 hw_rts_en:1;
	u32 rts_drop_data_mode:2;
	u32 rsvd2:1;
	u32 ampdu_max_len:11;
	u32 ul_mu_dis:1;
	u32 ampdu_max_time:4;
	/* dword 3 */
	u32 max_agg_num:8;
	u32 ba_bmap:2;
	u32 rsvd3:6;
	u32 vo_lftime_sel:3;
	u32 vi_lftime_sel:3;
	u32 be_lftime_sel:3;
	u32 bk_lftime_sel:3;
	u32 sectype:4;
	/* dword 4 */
	u32 multi_port_id:3;
	u32 bmc:1;
	u32 mbssid:4;
	u32 navusehdr:1;
	u32 txpwr_mode:3;
	u32 data_dcm:1;
	u32 data_er:1;
	u32 data_ldpc:1;
	u32 data_stbc:1;
	u32 a_ctrl_bqr:1;
	u32 a_ctrl_uph:1;
	u32 a_ctrl_bsr:1;
	u32 a_ctrl_cas:1;
	u32 data_bw_er:1;
	u32 lsig_txop_en:1;
	u32 rsvd4:5;
	u32 ctrl_cnt_vld:1;
	u32 ctrl_cnt:4;
	/* dword 5 */
	u32 resp_ref_rate:9;
	u32 rsvd5:3;
	u32 all_ack_support:1;
	u32 bsr_queue_size_format:1;
	u32 rsvd6:1;
	u32 rsvd7:1;
	u32 ntx_path_en:4;
	u32 path_map_a:2;
	u32 path_map_b:2;
	u32 path_map_c:2;
	u32 path_map_d:2;
	u32 antsel_a:1;
	u32 antsel_b:1;
	u32 antsel_c:1;
	u32 antsel_d:1;
	/* dword 6 */
	u32 addr_cam_index:8;
	u32 paid:9;
	u32 uldl:1;
	u32 doppler_ctrl:2;
	u32 nominal_pkt_padding:2;
	u32 nominal_pkt_padding40:2;
	u32 txpwr_tolerence:4;
	u32 rsvd9:2;
	u32 nominal_pkt_padding80:2;
	/* dword 7 */
	u32 nc:3;
	u32 nr:3;
	u32 ng:2;
	u32 cb:2;
	u32 cs:2;
	u32 csi_txbf_en:1;
	u32 csi_stbc_en:1;
	u32 csi_ldpc_en:1;
	u32 csi_para_en:1;
	u32 csi_fix_rate:9;
	u32 csi_gi_ltf:3;
	u32 nominal_pkt_padding160:2;
	u32 csi_bw:2;
};

struct mac_ax_dctl_info {
	/* dword 0 */
	u32 qos_field_h:8;
	u32 hw_exseq_macid:7;
	u32 qos_field_h_en:1;
	u32 aes_iv_l:16;
	/* dword 1 */
	u32 aes_iv_h:32;
	/* dword 2 */
	u32 seq0:12;
	u32 seq1:12;
	u32 amsdu_max_length:3;
	u32 sta_amsdu_en:1;
	u32 chksum_offload_en:1;
	u32 with_llc:1;
	u32 rsvd0:1;
	u32 sec_hw_enc:1;
	/* dword 3 */
	u32 seq2:12;
	u32 seq3:12;
	u32 sec_cam_idx:8;
};

#endif //NEO

/**
 * struct mac_role_info - role information
 * @macid: MAC ID.
 * @band: Band selection, band0 or band1.
 * @wmm: WMM selection, wmm0 ow wmm1.
 *	There are four sets about band and wmm,
 *	band0+wmm0, band0+wmm1, band1+wmm0,band1+wmm1.
 */
struct mac_role_info {
	enum mac_self_role self_role;
	enum mac_wifi_role wifi_role;
	enum mac_net_type net_type;
	enum mac_upd_mode upd_mode;
	enum mac_opmode opmode;
	enum mac_band band;
	enum mac_port port;
	u8 macid;
	u8 wmm:2;
	u8 self_mac[6];
	u8 target_mac[6];
	u8 bssid[6];
	u8 bss_color:6;
	u8 bcn_hit_cond:2;
	u8 hit_rule:2;
	u8 is_mul_ent:1;
	u8 tsf_sync:3;
	u8 trigger:1;
	u8 lsig_txop:1;
	u8 tgt_ind:3;
	u8 frm_tgt_ind:3;
	u8 wol_pattern:1;
	u8 wol_uc:1;
	u8 wol_magic:1;
	u8 wapi:1;
	u8 sec_ent_mode:2;
	u8 is_hesta:1;
	u8 dl_bw:2;
	u8 tf_mac_padding:2;
	u8 dl_t_pe: 3;
	u16 aid;
	struct mac_addr_cam_info a_info;
	struct mac_bssid_cam_info b_info;
	struct mac_sec_cam_info s_info;
	//struct mac_ax_cctl_info c_info;
};

struct mac_role_tbl {
	/* keep first */
	struct mac_role_tbl *next;
	struct mac_role_tbl *prev;
	struct mac_role_info info;
	u8 macid;
	u8 wmm;
};

struct mac_role_tbl_head {
	/* keep first */
	struct mac_role_tbl *next;
	struct mac_role_tbl *prev;
	struct mac_role_tbl_head *role_tbl_pool;
	u32 qlen;
	mac_mutex lock;
};

#if 0 //NEO

struct mac_ax_coex {
#define MAC_AX_COEX_RTK_MODE 0
#define MAC_AX_COEX_CSR_MODE 1
	u8 pta_mode;
#define MAC_AX_COEX_INNER 0
#define MAC_AX_COEX_OUTPUT 1
#define MAC_AX_COEX_INPUT 2
	u8 direction;
};

struct mac_ax_port_tsf {
	u32 tsf_l;
	u32 tsf_h;
	u8 port;
};

struct mac_ax_gnt {
	u8 gnt_bt_sw_en;
	u8 gnt_bt;
	u8 gnt_wl_sw_en;
	u8 gnt_wl;
};

struct mac_ax_coex_gnt {
	struct mac_ax_gnt band0;
	struct mac_ax_gnt band1;
};

struct mac_ax_plt {
#define MAC_AX_PLT_LTE_RX BIT(0)
#define MAC_AX_PLT_GNT_BT_TX BIT(1)
#define MAC_AX_PLT_GNT_BT_RX BIT(2)
#define MAC_AX_PLT_GNT_WL BIT(3)
	u8 band;
	u8 tx;
	u8 rx;
};

struct mac_ax_rx_cnt {
#define MAC_AX_RX_CRC_OK 0
#define MAC_AX_RX_CRC_FAIL 1
#define MAC_AX_RX_FA 2
#define MAC_AX_RX_PPDU 3
#define MAC_AX_RX_IDX 4
	u8 type;
#define MAC_AX_RXCNT_R 0
#define MAC_AX_RXCNT_RST_ALL 1
	u8 op;
	u8 idx;
	u8 band;
	u16 *buf;
};

struct mac_ax_tx_cnt {
#define MAC_AX_TX_LCCK 0
#define MAC_AX_TX_SCCK 1
#define MAC_AX_TX_OFDM 2
#define MAC_AX_TX_HT 3
#define MAC_AX_TX_HTGF 4
#define MAC_AX_TX_VHTSU 5
#define MAC_AX_TX_VHTMU 6
#define MAC_AX_TX_HESU 7
#define MAC_AX_TX_HEERSU 8
#define MAC_AX_TX_HEMU 9
#define MAC_AX_TX_HETB 10
#define MAC_AX_TX_ALLTYPE 11
	u8 band;
	u8 sel;
	u16 txcnt[MAC_AX_TX_ALLTYPE];
};

struct mac_ax_mcc_role {
	/* dword0 */
	u32 macid: 8;
	u32 central_ch_seg0: 8;
	u32 central_ch_seg1: 8;
	u32 primary_ch: 8;
	/* dword1 */
	enum channel_width bandwidth: 4;
	u32 group: 2;
#define MCC_C2H_RPT_OFF 0
#define MCC_C2H_RPT_FAIL_ONLY 1
#define MCC_C2H_RPT_ALL 2
	u32 c2h_rpt: 2;
	u32 dis_tx_null: 1;
	u32 dis_sw_retry: 1;
	u32 in_curr_ch: 1;
	u32 sw_retry_count: 3;
	u32 tx_null_early: 4;
	u32 rsvd0: 14;
	/* dword2 */
	u32 duration: 32;
};

struct mac_ax_mcc_duration_info {
	/* dword0 */
	u32 group: 2;
	u32 rsvd0: 6;
	u32 start_macid: 8;
	u32 macid_x: 8;
	u32 macid_y: 8;
	/* dword1 */
	u32 start_tsf_low;
	/* dword2 */
	u32 start_tsf_high;
	/* dword3 */
	u32 duration_x;
	/* dword4 */
	u32 duration_y;
};

struct mac_ax_mcc_group {
	u8 rpt_status;
	u8 rpt_macid;
	u8 macid_x;
	u8 macid_y;
	u32 rpt_tsf_high;
	u32 rpt_tsf_low;
	u32 tsf_x_high;
	u32 tsf_x_low;
	u32 tsf_y_high;
	u32 tsf_y_low;
};

struct mac_ax_mcc_group_info {
	struct mac_ax_mcc_group groups[4];
};

struct mac_ax_tx_tf_info {
	u32 tx_tf_infol;
	u32 tx_tf_infoh;
	u8 tx_tf_infosel;//4:common info; 0~3: user0 ~ user3 info
};

struct mac_ax_sr_info {
	u8 sr_en: 1;
	u8 sr_field_v15_allowed: 1;
	u8 srg_obss_pd_min;
	u8 srg_obss_pd_max;
	u8 non_srg_obss_pd_min;
	u8 non_srg_obss_pd_max;
	u32 srg_bsscolor_bitmap_0;
	u32 srg_bsscolor_bitmap_1;
	u32 srg_partbsid_bitmap_0;
	u32 srg_partbsid_bitmap_1;
};

struct mac_ax_nav_padding {
	u8 band;
	u8 nav_pad_en;
	u8 over_txop_en;
	u16 nav_padding;
};

struct mac_ax_max_tx_time {
	u8 macid;
	u8 is_cctrl;
	u32 max_tx_time; /* us */
};
#endif // if 0

/*--------------------Define Adapter & OPs------------------------------------*/
#ifndef CONFIG_NEW_HALMAC_INTERFACE
struct mac_pltfm_cb {
#if MAC_SDIO_SUPPORT
	u8 (*sdio_cmd52_r8)(void *drv_adapter, u32 addr);
	u8 (*sdio_cmd53_r8)(void *drv_adapter, u32 addr);
	u16 (*sdio_cmd53_r16)(void *drv_adapter, u32 addr);
	u32 (*sdio_cmd53_r32)(void *drv_adapter, u32 addr);
	u8 (*sdio_cmd53_rn)(void *drv_adapter, u32 addr, u32 size, u8 *val);
	void (*sdio_cmd52_w8)(void *drv_adapter, u32 addr, u8 val);
	void (*sdio_cmd53_w8)(void *drv_adapter, u32 addr, u8 val);
	void (*sdio_cmd53_w16)(void *drv_adapter, u32 addr, u16 val);
	void (*sdio_cmd53_w32)(void *drv_adapter, u32 addr, u32 val);
	u8 (*sdio_cmd53_wn)(void *drv_adapter, u32 addr, u32 size, u8 *val);
	u8 (*sdio_cmd52_cia_r8)(void *drv_adapter, u32 addr);
#endif
#if (MAC_USB_SUPPORT || MAC_PCIE_SUPPORT)
	u8 (*reg_r8)(void *drv_adapter, u32 addr);
	u16 (*reg_r16)(void *drv_adapter, u32 addr);
	u32 (*reg_r32)(void *drv_adapter, u32 addr);
	void (*reg_w8)(void *drv_adapter, u32 addr, u8 val);
	void (*reg_w16)(void *drv_adapter, u32 addr, u16 val);
	void (*reg_w32)(void *drv_adapter, u32 addr, u32 val);
#endif
	enum rtw_hal_status (*send_rsvd_page)(struct rtw_phl_com_t *phl_com,
					      struct rtw_hal_com_t *hal_com,
					      struct sk_buff *skb);
#if MAC_PHL_H2C
	enum rtw_hal_status (*tx)(struct rtw_phl_com_t *phl_com,
				  struct rtw_hal_com_t *hal_com,
				  struct rtw_h2c_pkt *pkt);
	struct rtw_h2c_pkt *(*rtl_query_h2c)(struct rtw_phl_com_t *phl_com,
					     struct rtw_hal_com_t *hal_com,
					     enum rtw_h2c_pkt_type type);
#else
	u32 (*tx)(void *drv_adapter, u8 *buf, u32 len);
#endif
	void (*rtl_free)(void *drv_adapter, void *buf, u32 size);
	void* (*rtl_malloc)(void *drv_adapter, u32 size);
	void (*rtl_memcpy)(void *drv_adapter, void *dest, void *src, u32 size);
	void (*rtl_memset)(void *drv_adapter, void *addr, u8 val, u32 size);
	s32 (*rtl_memcmp)(void *drv_adapter, void *ptr1, void *ptr2, u32 num);
	void (*rtl_delay_us)(void *drv_adapter, u32 us);
	void (*rtl_delay_ms)(void *drv_adapter, u32 ms);

	void (*rtl_mutex_init)(void *drv_adapter, mac_mutex *mutex);
	void (*rtl_mutex_deinit)(void *drv_adapter, mac_mutex *mutex);
	void (*rtl_mutex_lock)(void *drv_adapter, mac_mutex *mutex);
	void (*rtl_mutex_unlock)(void *drv_adapter, mac_mutex *mutex);

	void (*msg_print)(void *drv_adapter, s8 *fmt, ...);

	void (*event_notify)(void *drv_adapter,
			     enum mac_feature mac_ft,
			     enum mac_status stat, u8 *buf, u32 size);
};
#endif/*CONFIG_NEW_HALMAC_INTERFACE*/

struct mac_fifo_info {
	/* tx fifo information */
	u16 rsvd_boundary;
	u16 rsvd_pg_num;
	u16 rsvd_drv_pg_num;
	u16 txff_pg_num;
	u16 acq_pg_num;
	u16 rsvd_drv_addr;
	u16 rsvd_h2c_info_addr;
	u16 rsvd_h2c_sta_info_addr;
	u16 rsvd_h2cq_addr;
	u16 rsvd_cpu_instr_addr;
	u16 rsvd_fw_txbuf_addr;
	u16 rsvd_csibuf_addr;
};

struct mac_adapter {
	struct mac_ops *ops;
	void *drv_adapter; //hal_com adapter
	void *phl_adapter; //phl_com adapter

	struct mac_pltfm_cb *pltfm_cb;
	struct mac_state_mach sm;
	struct mac_hw_info *hw_info;
	struct mac_fw_info fw_info;
	struct mac_fifo_info fifo_info;
	struct mac_efuse_param efuse_param;
#if 0 // NEO
	struct mac_ax_mac_pwr_info mac_pwr_info;
	struct mac_ax_ft_status *ft_stat;
	struct mac_ax_hfc_param *hfc_param;
	struct mac_ax_dle_info dle_info;
	struct mac_ax_gpio_info gpio_info;
#endif //NEO
	struct mac_role_tbl_head *role_tbl;
#if 0 // NEO
	struct mac_ax_read_ofld_info read_ofld_info;
	struct mac_ax_read_ofld_value read_ofld_value;
	struct mac_ax_write_ofld_info write_ofld_info;
	struct mac_ax_efuse_ofld_info efuse_ofld_info;
	struct mac_ax_conf_ofld_info conf_ofld_info;
	struct mac_ax_pkt_ofld_info pkt_ofld_info;
	struct mac_ax_pkt_ofld_pkt pkt_ofld_pkt;
	struct mac_ax_mcc_group_info mcc_group_info;
	struct mac_ax_wowlan_info wowlan_info;
#if MAC_AX_SDIO_SUPPORT
	struct mac_ax_sdio_info sdio_info;
#endif
#if MAC_AX_USB_SUPPORT
	struct mac_ax_usb_info usb_info;
#endif

#if MAC_AX_FEATURE_HV
	struct hv_ax_ops *hv_ops;
#endif
#endif // if 0
};


/**
 * mac_intf_ops - interface related callbacks
 * @reg_read8:
 * @reg_write8:
 * @reg_read16:
 * @reg_write16:
 * @reg_read32:
 * @reg_write32:
 * @tx_allow_sdio:
 * @tx_cmd_addr_sdio:
 * @init_intf:
 * @reg_read_n_sdio:
 * @get_bulkout_id:
 */
struct mac_intf_ops {
	u8 (*reg_read8)(struct mac_adapter *adapter, u32 addr);
	void (*reg_write8)(struct mac_adapter *adapter, u32 addr, u8 val);
	u16 (*reg_read16)(struct mac_adapter *adapter, u32 addr);
	void (*reg_write16)(struct mac_adapter *adapter, u32 addr, u16 val);
	u32 (*reg_read32)(struct mac_adapter *adapter, u32 addr);
	void (*reg_write32)(struct mac_adapter *adapter, u32 addr, u32 val);
	/**
	 * @tx_allow_sdio
	 * Only support SDIO interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*tx_allow_sdio)(struct mac_adapter *adapter,
			     struct mac_sdio_tx_info *info);
	/**
	 * @tx_cmd_addr_sdio
	 * Only support SDIO interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*tx_cmd_addr_sdio)(struct mac_adapter *adapter,
				struct mac_sdio_tx_info *info,
				u32 *cmd_addr);
	u32 (*intf_pre_init)(struct mac_adapter *adapter, void *param);
	u32 (*intf_init)(struct mac_adapter *adapter, void *param);
	u32 (*intf_deinit)(struct mac_adapter *adapter, void *param);
	/**
	 * @reg_read_n_sdio
	 * Only support SDIO interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*reg_read_n_sdio)(struct mac_adapter *adapter, u32 addr,
			       u32 size, u8 *val);
	/**
	 * @get_bulkout_id
	 * Only support USB interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u8 (*get_bulkout_id)(struct mac_adapter *adapter, u8 ch_dma,
			     u8 mode);
	/**
	 * @ltr_set_pcie
	 * Only support PCIe interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*ltr_set_pcie)(struct mac_adapter *adapter,
			    struct mac_ax_pcie_ltr_param *param);
	/**
	 * @u2u3_switch
	 * Only support USB interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*u2u3_switch)(struct mac_adapter *adapter);
	/**
	 * @get_usb_mode
	 * Only support USB interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*get_usb_mode)(struct mac_adapter *adapter);
	/**
	 * @get_usb_support_ability
	 * Only support USB interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*get_usb_support_ability)(struct mac_adapter *adapter);
#if 0 // NEO
	/**
	 * @usb_tx_agg_cfg
	 * Only support USB interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*usb_tx_agg_cfg)(struct mac_adapter *adapter,
			      struct mac_ax_usb_tx_agg_cfg *agg);
	/**
	 * @usb_rx_agg_cfg
	 * Only support USB interface. Using this API in other interface
	 * may cause system crash or segmentation fault.
	 */
	u32 (*usb_rx_agg_cfg)(struct mac_adapter *adapter,
			      struct mac_ax_rx_agg_cfg *cfg);

	u32 (*set_wowlan)(struct mac_adapter *adapter,
			  enum mac_ax_wow_ctrl w_c);
#endif // if 0 NEO
};


/**
 * struct mac_ops - callbacks for mac control
 * All callbacks can be used after initializing mac_ops by mac_ops_init.
 * @intf_ops: interface related callbacks, refer struct mac_ax_intf_ops to get
 *	more deatails.
 * @get_hw_info: get mac hardware information
 * @mac_txdesc_len:
 * @build_mac_txdesc:
 */
struct mac_ops {
	struct mac_intf_ops *intf_ops;

	/*System level*/
	u32 (*hal_init)(struct mac_adapter *adapter,
			struct mac_trx_info *trx_info,
			struct mac_fwdl_info *fwdl_info,
			struct mac_intf_info *intf_info);
	u32 (*hal_fast_init)(struct mac_adapter *adapter,
			     struct mac_trx_info *trx_info,
			     struct mac_fwdl_info *fwdl_info,
			     struct mac_intf_info *intf_info);
#if 0 // NEO
	u32 (*hal_deinit)(struct mac_adapter *adapter);
	u32 (*add_role)(struct mac_adapter *adapter,
			struct mac_ax_role_info *info);
	u32 (*remove_role)(struct mac_adapter *adapter, u8 macid);
	u32 (*change_role)(struct mac_adapter *adapter,
			   struct mac_ax_role_info *info);
#endif // if 0 NEO
	u32 (*pwr_switch)(struct mac_adapter *adapter, u8 on);
#if 0 // NEO
	u32 (*sys_init)(struct mac_adapter *adapter);
	u32 (*trx_init)(struct mac_adapter *adapter,
			struct mac_ax_trx_info *info);
	u32 (*romdl)(struct mac_adapter *adapter, u8 *rom, u32 romaddr,
		     u32 len);
	u32 (*enable_cpu)(struct mac_adapter *adapter,
			  u8 boot_reason, u8 dlfw);
	u32 (*disable_cpu)(struct mac_adapter *adapter);
	u32 (*fwredl)(struct mac_adapter *adapter, u8 *fw, u32 len);
	u32 (*fwdl)(struct mac_adapter *adapter, u8 *fw, u32 len);
#endif // if 0 NEO
	u32 (*enable_fw)(struct mac_adapter *adapter,
			 enum rtw_fw_type cat);
#if 0 // NEO
	u32 (*lv1_rcvy)(struct mac_adapter *adapter,
			enum mac_ax_lv1_rcvy_step step);
	u32 (*get_macaddr)(struct mac_adapter *adapter,
			   struct mac_ax_macaddr *macaddr,
			   u8 role_idx);
#endif // if 0 NEO
	u32 (*build_txdesc)(struct mac_adapter *adapter,
			    struct mac_txpkt_info *info, u8 *buf, u32 len);
#if 0 // NEO
	u32 (*refill_txdesc)(struct mac_adapter *adapter,
			     struct mac_ax_txpkt_info *txpkt_info,
			     struct mac_ax_refill_info *mask,
			     struct mac_ax_refill_info *info);
	u32 (*parse_rxdesc)(struct mac_adapter *adapter,
			    struct mac_ax_rxpkt_info *info, u8 *buf, u32 len);
	/*FW offload related*/
	u32 (*reset_fwofld_state)(struct mac_adapter *adapter, u8 op);
	u32 (*check_fwofld_done)(struct mac_adapter *adapter, u8 op);
	u32 (*read_pkt_ofld)(struct mac_adapter *adapter, u8 id);
	u32 (*del_pkt_ofld)(struct mac_adapter *adapter, u8 id);
	u32 (*add_pkt_ofld)(struct mac_adapter *adapter, u8 *pkt,
			    u16 len, u8 *id);
	u32 (*pkt_ofld_packet)(struct mac_adapter *adapter,
			       u8 **pkt_buf, u16 *pkt_len, u8 *pkt_id);
	u32 (*dump_efuse_ofld)(struct mac_adapter *adapter, u32 efuse_size,
			       bool is_hidden);
	u32 (*efuse_ofld_map)(struct mac_adapter *adapter, u8 *efuse_map,
			      u32 efuse_size);
	u32 (*upd_dctl_info)(struct mac_adapter *adapter,
			     struct mac_ax_dctl_info *info,
			     struct mac_ax_dctl_info *mask, u8 macid,
			     u8 operation);
	u32 (*upd_cctl_info)(struct mac_adapter *adapter,
			     struct mac_ax_cctl_info *info,
			     struct mac_ax_cctl_info *mask, u8 macid,
			     u8 operation);
	u32 (*ie_cam_upd)(struct mac_adapter *adapter,
			  struct mac_ax_ie_cam_cmd_info *info);
	u32 (*twt_info_upd_h2c)(struct mac_adapter *adapter,
				struct mac_ax_twt_para *info);
	u32 (*twt_act_h2c)(struct mac_adapter *adapter,
			   struct mac_ax_twtact_para *info);
	u32 (*twt_anno_h2c)(struct mac_adapter *adapter,
			    struct mac_ax_twtanno_para *info);
	void (*twt_wait_anno)(struct mac_adapter *adapter,
			      u8 *c2h_content, u8 *upd_addr);
	u32 (*mac_host_getpkt_h2c)(struct mac_adapter *adapter,
				   u8 macid, u8 pkttype);
	u32 (*p2p_act_h2c)(struct mac_adapter *adapter,
			   struct mac_ax_p2p_act_info *info);
	u32 (*get_p2p_stat)(struct mac_adapter *adapter);
	/*Association, de-association related*/
	u32 (*sta_add_key)(struct mac_adapter *adapter,
			   struct mac_sec_cam_info *sec_cam_content,
			   u8 mac_id, u8 key_id, u8 key_type);
	u32 (*sta_del_key)(struct mac_adapter *adapter,
			   u8 mac_id, u8 key_id, u8 key_type);
	u32 (*sta_search_key_idx)(struct mac_adapter *adapter,
				  u8 mac_id, u8 key_id, u8 key_type);
	u32 (*sta_hw_security_support)(struct mac_adapter *adapter,
				       u8 hw_security_support_type, u8 enable);
	u32 (*set_mu_table)(struct mac_adapter *adapter,
			    struct mac_mu_table *mu_table);
	u32 (*ss_dl_grp_upd)(struct mac_adapter *adapter,
			     struct mac_ax_ss_dl_grp_upd *info);
	u32 (*ss_ul_grp_upd)(struct mac_adapter *adapter,
			     struct mac_ax_ss_ul_grp_upd *info);
	u32 (*ss_ul_sta_upd)(struct mac_adapter *adapter,
			     struct mac_ax_ss_ul_sta_upd *info);
	u32 (*bacam_info)(struct mac_adapter *adapter,
			  struct mac_ax_bacam_info *info);
#endif // if 0 NEO
	/*TRX related*/
	u32 (*txdesc_len)(struct mac_adapter *adapter,
			  struct mac_txpkt_info *info);
#if 0 // NEO
	u32 (*upd_shcut_mhdr)(struct mac_adapter *adapter,
			      struct mac_ax_shcut_mhdr *info, u8 macid);
	u32 (*enable_hwmasdu)(struct mac_adapter *adapter,
			      u8 enable,
			      enum mac_ax_amsdu_pkt_num max_num,
			      u8 en_single_amsdu,
			      u8 en_last_amsdu_padding);
	u32 (*enable_cut_hwamsdu)(struct mac_adapter *adapter,
				  u8 enable,
				  u8 low_th,
				  u16 high_th,
				  enum mac_ax_ex_shift aligned);
	u32 (*hdr_conv)(struct mac_adapter *adapter,
			u8 en_hdr_conv);
	u32 (*set_hwseq_reg)(struct mac_adapter *adapter,
			     u8 reg_seq_idx,
			     u16 reg_seq_val);
#endif // if 0 NEO
	u32 (*process_c2h)(struct mac_adapter *adapter, u8 *buf, u32 len,
			   u8 *ret);
#if 0 // NEO
	u32 (*parse_dfs)(struct mac_adapter *adapter,
			 u8 *buf, u32 dfs_len, struct mac_ax_dfs_rpt *rpt);
	u32 (*parse_ppdu)(struct mac_adapter *adapter,
			  u8 *buf, u32 ppdu_len, u8 mac_info,
			  struct mac_ax_ppdu_rpt *rpt);
	u32 (*cfg_phy_rpt)(struct mac_adapter *adapter,
			   struct mac_ax_phy_rpt_cfg *cfg);
	u32 (*set_rx_forwarding)(struct mac_adapter *adapter,
				 struct mac_ax_rx_fwd_ctrl_t *rf_ctrl_p);
	u32 (*get_rx_fltr_opt)(struct mac_adapter *adapter,
			       struct mac_ax_rx_fltr_ctrl_t *opt,
			       u8 band);
	u32 (*set_rx_fltr_opt)(struct mac_adapter *adapter,
			       struct mac_ax_rx_fltr_ctrl_t *opt,
			       struct mac_ax_rx_fltr_ctrl_t *opt_msk,
			       u8 band);
	u32 (*set_rx_fltr_typ_opt)(struct mac_adapter *adapter,
				   enum mac_ax_pkt_t type,
				   enum mac_ax_fwd_target fwd_target,
				   u8 band);
	u32 (*set_rx_fltr_typstyp_opt)(struct mac_adapter *adapter,
				       enum mac_ax_pkt_t type,
				       u8 subtype,
				       enum mac_ax_fwd_target fwd_target,
				       u8 band);
	u32 (*sr_update)(struct mac_adapter *adapter,
			 struct mac_ax_sr_info *sr_info, u8 band);
	u32 (*two_nav_cfg)(struct mac_adapter *adapter,
			   struct mac_ax_2nav_info *info);
	u32 (*pkt_drop)(struct mac_adapter *adapter,
			struct mac_ax_pkt_drop_info *info);
	u32 (*send_bcn_h2c)(struct mac_adapter *adapter,
			    struct mac_ax_bcn_info *info);
	u32 (*tx_mode_sel)(struct mac_adapter *adapter,
			   struct mac_ax_mac_tx_mode_sel *mode_sel);
	u32 (*tcpip_chksum_ofd)(struct mac_adapter *adapter,
				u8 en_tx_chksum_ofd,
				u8 en_rx_chksum_ofd);
	u32 (*chk_rx_tcpip_chksum_ofd)(struct mac_adapter *adapter,
				       u8 chksum_status);
	u32 (*chk_allq_empty)(struct mac_adapter *adapter, u8 *empty);
	u32 (*is_txq_empty)(struct mac_adapter *adapter,
			    struct mac_ax_tx_queue_empty *val);
	u32 (*is_rxq_empty)(struct mac_adapter *adapter,
			    struct mac_ax_rx_queue_empty *val);
	u32 (*parse_bcn_stats_c2h)(struct mac_adapter *adapter,
				   u8 *content,
				   struct mac_ax_bcn_cnt *val);
	/*frame exchange related*/
	u32 (*upd_mudecision_para)(struct mac_adapter *adapter,
				   struct mac_ax_mudecision_para *info);
	u32 (*mu_sta_upd)(struct mac_adapter *adapter,
			  struct mac_ax_mu_sta_upd *info);
	u32 (*upd_ul_fixinfo)(struct mac_adapter *adapter,
			      struct mac_ax_ul_fixinfo *info);
	u32 (*f2p_test_cmd)(struct mac_adapter *adapter,
			    struct mac_ax_f2p_test_para *info,
			    struct mac_ax_f2p_wd *f2pwd,
			    struct mac_ax_f2p_tx_cmd *ptxcmd,
			    u8 *psigb_addr);
	u32 (*snd_test_cmd)(struct mac_adapter *adapter,
			    u8 *cmd_buf);
	u32 (*set_fw_fixmode)(struct mac_adapter *adapter,
			      struct mac_ax_fixmode_para *info);
	u32 (*mac_dumpwlanc)(struct mac_adapter *adapter,
			     struct mac_ax_dumpwlanc *para);
	u32 (*mac_dumpwlans)(struct mac_adapter *adapter,
			     struct mac_ax_dumpwlans *para);
	u32 (*mac_dumpwland)(struct mac_adapter *adapter,
			     struct mac_ax_dumpwland *para);
	/*outsrcing related */
	u32 (*outsrc_h2c_common)(struct mac_adapter *adapter,
				 struct rtw_g6_h2c_hdr *hdr,
				 u32 *pvalue);
	u32 (*read_pwr_reg)(struct mac_adapter *adapter, u8 band,
			    const u32 offset, u32 *val);
	u32 (*write_pwr_reg)(struct mac_adapter *adapter, u8 band,
			     const u32 offset, u32 val);
	u32 (*write_pwr_ofst_mode)(struct mac_adapter *adapter,
				   u8 band, struct rtw_tpu_info *tpu);
	u32 (*write_pwr_ofst_bw)(struct mac_adapter *adapter,
				 u8 band, struct rtw_tpu_info *tpu);
	u32 (*write_pwr_ref_reg)(struct mac_adapter *adapter,
				 u8 band, struct rtw_tpu_info *tpu);
	u32 (*write_pwr_limit_en)(struct mac_adapter *adapter,
				  u8 band, struct rtw_tpu_info *tpu);
	u32 (*write_pwr_limit_rua_reg)(struct mac_adapter *adapter,
				       u8 band, struct rtw_tpu_info *tpu);
	u32 (*write_pwr_limit_reg)(struct mac_adapter *adapter,
				   u8 band, struct rtw_tpu_pwr_imt_info *tpu);
	u32 (*write_pwr_by_rate_reg)(struct mac_adapter *adapter,
				     u8 band,
				     struct rtw_tpu_pwr_by_rate_info *tpu);
	u32 (*lamode_cfg)(struct mac_adapter *adapter,
			  struct mac_ax_la_cfg *cfg);
	u32 (*lamode_trigger)(struct mac_adapter *adapter, u8 tgr);
	u32 (*lamode_buf_cfg)(struct mac_adapter *adapter,
			      struct mac_ax_la_buf_param *param);
	struct mac_ax_la_status (*get_lamode_st)
				 (struct mac_adapter *adapter);
	u32 (*read_xcap_reg)(struct mac_adapter *adapter, u8 sc_xo,
			     u32 *val);
	u32 (*write_xcap_reg)(struct mac_adapter *adapter, u8 sc_xo,
			      u32 val);
	u32 (*write_bbrst_reg)(struct mac_adapter *adapter, u8 val);
	/*sounding related*/
	u32 (*get_csi_buffer_index)(struct mac_adapter *adapter, u8 band,
				    u8 csi_buffer_id);
	u32 (*set_csi_buffer_index)(struct mac_adapter *adapter, u8 band,
				    u8 macid, u16 csi_buffer_id,
				    u16 buffer_idx);
	u32 (*get_snd_sts_index)(struct mac_adapter *adapter, u8 band,
				 u8 index);
	u32 (*set_snd_sts_index)(struct mac_adapter *adapter, u8 band,
				 u8 macid, u8 index);
	u32 (*init_snd_mer)(struct mac_adapter *adapter, u8 band);
	u32 (*init_snd_mee)(struct mac_adapter *adapter, u8 band);
	u32 (*csi_force_rate)(struct mac_adapter *adapter, u8 band,
			      u8 ht_rate, u8 vht_rate, u8 he_rate);
	u32 (*csi_rrsc)(struct mac_adapter *adapter, u8 band, u32 rrsc);
	u32 (*set_snd_para)(struct mac_adapter *adapter,
			    struct mac_ax_fwcmd_snd *snd_info);
	u32 (*set_csi_para_reg)(struct mac_adapter *adapter,
				struct mac_reg_csi_para *csi_para);
	u32 (*set_csi_para_cctl)(struct mac_adapter *adapter,
				 struct mac_cctl_csi_para *csi_para);
	u32 (*hw_snd_pause_release)(struct mac_adapter *adapter,
				    u8 band, u8 pr);
	u32 (*bypass_snd_sts)(struct mac_adapter *adapter);
	u32 (*deinit_mee)(struct mac_adapter *adapter, u8 band);
	/*lps related*/
	u32 (*cfg_lps)(struct mac_adapter *adapter,
		       u8 macid,
		       enum mac_ax_ps_mode ps_mode,
		       void *lps_info);
	u32 (*lps_pwr_state)(struct mac_adapter *adapter,
			     enum mac_ax_pwr_state_action action,
			     enum mac_ax_rpwm_req_pwr_state req_pwr_state);
	u32 (*chk_leave_lps)(struct mac_adapter *adapter, u8 macid);
	u32 (*lps_chk_access)(struct mac_adapter *adapter, u32 offset);
	/*Wowlan related*/
	u32 (*cfg_wow_wake)(struct mac_adapter *adapter,
			    u8 macid,
			    struct mac_ax_wow_wake_info *info,
			    struct mac_ax_remotectrl_info_parm_ *content);
	u32 (*cfg_disconnect_det)(struct mac_adapter *adapter,
				  u8 macid,
				  struct mac_ax_disconnect_det_info *info);
	u32 (*cfg_keepalive)(struct mac_adapter *adapter,
			     u8 macid,
			     struct mac_ax_keep_alive_info *info);
	u32 (*cfg_gtk_ofld)(struct mac_adapter *adapter,
			    u8 macid,
			    struct mac_ax_gtk_ofld_info *info,
			    struct mac_ax_gtk_info_parm_ *content);
	u32 (*cfg_arp_ofld)(struct mac_adapter *adapter,
			    u8 macid,
			    struct mac_ax_arp_ofld_info *info,
			    void *parp_info_content);
	u32 (*cfg_ndp_ofld)(struct mac_adapter *adapter,
			    u8 macid,
			    struct mac_ax_ndp_ofld_info *info,
			    struct mac_ax_ndp_info_parm_ *content);
	u32 (*cfg_realwow)(struct mac_adapter *adapter,
			   u8 macid,
			   struct mac_ax_realwow_info *info,
			   struct mac_ax_realwowv2_info_parm_ *content);
	u32 (*cfg_nlo)(struct mac_adapter *adapter,
		       u8 macid,
		       struct mac_ax_nlo_info *info,
		       struct mac_ax_nlo_networklist_parm_ *content);
	u32 (*cfg_dev2hst_gpio)(struct mac_adapter *adapter,
				struct mac_ax_dev2hst_gpio_info *info);
	u32 (*cfg_uphy_ctrl)(struct mac_adapter *adapter,
			     struct mac_ax_uphy_ctrl_info *info);
	u32 (*cfg_wowcam_upd)(struct mac_adapter *adapter,
			      struct mac_ax_wowcam_upd_info *info);
	u32 (*cfg_wow_sleep)(struct mac_adapter *adapter,
			     u8 sleep);
	u32 (*get_wow_fw_status)(struct mac_adapter *adapter,
				 u8 *status);
	u32 (*request_aoac_report)(struct mac_adapter *adapter,
				   u8 rx_ready);
	u32 (*read_aoac_report)(struct mac_adapter *adapter,
				struct mac_ax_aoac_report *rpt_buf, u8 rx_ready);
	/*system related*/
	u32 (*dbcc_enable)(struct mac_adapter *adapter,
			   struct mac_ax_trx_info *info, u8 dbcc_en);
	u32 (*port_cfg)(struct mac_adapter *adapter,
			enum mac_ax_port_cfg_type type,
			struct mac_ax_port_cfg_para *para);
	u32 (*port_init)(struct mac_adapter *adapter,
			 struct mac_ax_port_init_para *para);
	u32 (*enable_imr)(struct mac_adapter *adapter, u8 band,
			  enum mac_ax_hwmod_sel sel);
	u32 (*dump_efuse_map_wl)(struct mac_adapter *adapter,
				 enum mac_ax_efuse_read_cfg cfg,
				 u8 *efuse_map);
	u32 (*dump_efuse_map_bt)(struct mac_adapter *adapter,
				 enum mac_ax_efuse_read_cfg cfg,
				 u8 *efuse_map);
	u32 (*write_efuse)(struct mac_adapter *adapter, u32 addr, u8 val,
			   enum mac_ax_efuse_bank bank);
	u32 (*read_efuse)(struct mac_adapter *adapter, u32 addr, u32 size,
			  u8 *val, enum mac_ax_efuse_bank bank);
	u32 (*get_efuse_avl_size)(struct mac_adapter *adapter, u32 *size);
	u32 (*get_efuse_avl_size_bt)(struct mac_adapter *adapter, u32 *size);
#endif //NEO
	u32 (*dump_log_efuse)(struct mac_adapter *adapter,
			      enum mac_efuse_parser_cfg parser_cfg,
			      enum mac_efuse_read_cfg cfg,
			      u8 *efuse_map, bool is_limit);
#if 0 //NEO
	u32 (*read_log_efuse)(struct mac_adapter *adapter, u32 addr,
			      u32 size, u8 *val);
	u32 (*write_log_efuse)(struct mac_adapter *adapter, u32 addr,
			       u8 val);
	u32 (*dump_log_efuse_bt)(struct mac_adapter *adapter,
				 enum mac_ax_efuse_parser_cfg parser_cfg,
				 enum mac_ax_efuse_read_cfg cfg,
				 u8 *efuse_map);
	u32 (*read_log_efuse_bt)(struct mac_adapter *adapter, u32 addr,
				 u32 size, u8 *val);
	u32 (*write_log_efuse_bt)(struct mac_adapter *adapter, u32 addr,
				  u8 val);
	u32 (*pg_efuse_by_map)(struct mac_adapter *adapter,
			       struct mac_ax_pg_efuse_info *info,
			       enum mac_ax_efuse_read_cfg cfg,
			       bool part, bool is_limit);
	u32 (*pg_efuse_by_map_bt)(struct mac_adapter *adapter,
				  struct mac_ax_pg_efuse_info *info,
				  enum mac_ax_efuse_read_cfg cfg);
	u32 (*mask_log_efuse)(struct mac_adapter *adapter,
			      struct mac_ax_pg_efuse_info *info);
	u32 (*pg_sec_data_by_map)(struct mac_adapter *adapter,
				  struct mac_ax_pg_efuse_info *info);
	u32 (*cmp_sec_data_by_map)(struct mac_adapter *adapter,
				   struct mac_ax_pg_efuse_info *info);
#endif // NEO
	u32 (*get_efuse_info)(struct mac_adapter *adapter, u8 *efuse_map,
			      enum rtw_efuse_info id, void *value,
			      u32 length, u8 *autoload_status);
#if 0 //NEO
	u32 (*set_efuse_info)(struct mac_adapter *adapter, u8 *efuse_map,
			      enum rtw_efuse_info id, void *value, u32 length);
#endif // NEO
	u32 (*read_hidden_rpt)(struct mac_adapter *adapter,
			       struct mac_defeature_value *rpt);
#if 0 //NEO
	u32 (*check_efuse_autoload)(struct mac_adapter *adapter,
				    u8 *autoload_status);
	u32 (*pg_simulator)(struct mac_adapter *adapter,
			    struct mac_ax_pg_efuse_info *info, u8 *phy_map);
	u32 (*checksum_update)(struct mac_adapter *adapter);
	u32 (*checksum_rpt)(struct mac_adapter *adapter, u16 *chksum);
	u32 (*set_efuse_ctrl)(struct mac_adapter *adapter, bool is_secure);
	u32 (*otp_test)(struct mac_adapter *adapter, bool is_OTP_test);
	u32 (*get_mac_ft_status)(struct mac_adapter *adapter,
				 enum mac_ax_feature mac_ft,
				 enum mac_ax_status *stat, u8 *buf,
				 const u32 size, u32 *ret_size);
	u32 (*fw_log_cfg)(struct mac_adapter *adapter,
			  struct mac_ax_fw_log *log_cfg);
	u32 (*pinmux_set_func)(struct mac_adapter *adapter,
			       enum mac_ax_gpio_func func);
	u32 (*pinmux_free_func)(struct mac_adapter *adapter,
				enum mac_ax_gpio_func func);
	u32 (*sel_uart_tx_pin)(struct mac_adapter *adapter,
			       enum mac_ax_uart_tx_pin uart_pin);
	u32 (*sel_uart_rx_pin)(struct mac_adapter *adapter,
			       enum mac_ax_uart_rx_pin uart_pin);
	u32 (*set_gpio_func)(struct mac_adapter *adapter,
			     enum mac_ax_gfunc func, s8 gpio);
#endif // NEO
	struct mac_hw_info* (*get_hw_info)(struct mac_adapter *adapter);
#if 0 //NEO
	u32 (*set_hw_value)(struct mac_adapter *adapter,
			    enum mac_ax_hw_id hw_id, void *value);
#endif // NEO
	u32 (*get_hw_value)(struct mac_adapter *adapter,
			    enum mac_hw_id hw_id, void *value);
#if 0 //NEO
	u32 (*get_err_status)(struct mac_adapter *adapter,
			      enum mac_ax_err_info *err);
	u32 (*set_err_status)(struct mac_adapter *adapter,
			      enum mac_ax_err_info err);
	u32 (*general_pkt_ids)(struct mac_adapter *adapter,
			       struct mac_ax_general_pkt_ids *ids);
	u32 (*coex_init)(struct mac_adapter *adapter,
			 struct mac_ax_coex *coex);
	u32 (*coex_read)(struct mac_adapter *adapter,
			 const u32 offset, u32 *val);
	u32 (*coex_write)(struct mac_adapter *adapter,
			  const u32 offset, const u32 val);
	u32 (*trigger_cmac_err)(struct mac_adapter *adapter);
	u32 (*trigger_cmac1_err)(struct mac_adapter *adapter);
	u32 (*trigger_dmac_err)(struct mac_adapter *adapter);
	u32 (*tsf_sync)(struct mac_adapter *adapter, u8 from_port,
			u8 to_port, s32 sync_offset,
			enum mac_ax_tsf_sync_act action);
	/* mcc */
	u32 (*reset_mcc_group)(struct mac_adapter *adapter, u8 group);
	u32 (*reset_mcc_request)(struct mac_adapter *adapter, u8 group);
	u32 (*add_mcc)(struct mac_adapter *adapter,
		       struct mac_ax_mcc_role *info);
	u32 (*start_mcc)(struct mac_adapter *adapter,
			 u8 group, u8 macid, u32 tsf_high, u32 tsf_low);
	u32 (*stop_mcc)(struct mac_adapter *adapter, u8 group, u8 macid);
	u32 (*del_mcc_group)(struct mac_adapter *adapter, u8 group);
	u32 (*mcc_request_tsf)(struct mac_adapter *adapter, u8 group,
			       u8 macid_x, u8 macid_y);
	u32 (*mcc_macid_bitmap)(struct mac_adapter *adapter, u8 group,
				u8 macid, u8 *bitmap, u8 len);
	u32 (*mcc_sync_enable)(struct mac_adapter *adapter, u8 group,
			       u8 source, u8 target, u8 offset);
	u32 (*mcc_set_duration)(struct mac_adapter *adapter,
				struct mac_ax_mcc_duration_info *info);
	u32 (*get_mcc_tsf_rpt)(struct mac_adapter *adapter, u8 group,
			       u32 *tsf_x_high, u32 *tsf_x_low,
			       u32 *tsf_y_high, u32 *tsf_y_low);
	u32 (*get_mcc_status_rpt)(struct mac_adapter *adapter, u8 group,
				  u8 *status, u32 *tsf_high, u32 *tsf_low);
	u32 (*check_add_mcc_done)(struct mac_adapter *adapter, u8 group);
	u32 (*check_start_mcc_done)(struct mac_adapter *adapter, u8 group);
	u32 (*check_stop_mcc_done)(struct mac_adapter *adapter, u8 group);
	u32 (*check_del_mcc_group_done)(struct mac_adapter *adapter,
					u8 group);
	u32 (*check_mcc_request_tsf_done)(struct mac_adapter *adapter,
					  u8 group);
	u32 (*check_mcc_macid_bitmap_done)(struct mac_adapter *adapter,
					   u8 group);
	u32 (*check_mcc_sync_enable_done)(struct mac_adapter *adapter,
					  u8 group);
	u32 (*check_mcc_set_duration_done)(struct mac_adapter *adapter,
					   u8 group);
	/* not mcc */
	u32 (*check_access)(struct mac_adapter *adapter, u32 offset);
	u32 (*set_led_mode)(struct mac_adapter *adapter,
			    enum mac_ax_led_mode mode, u8 led_id);
	u32 (*led_ctrl)(struct mac_adapter *adapter, u8 high, u8 led_id);
	u32 (*set_sw_gpio_mode)(struct mac_adapter *adapter,
				enum mac_ax_sw_io_mode mode, u8 gpio);
	u32 (*sw_gpio_ctrl)(struct mac_adapter *adapter, u8 high, u8 gpio);
#if MAC_AX_FEATURE_DBGPKG
	u32 (*fwcmd_lb)(struct mac_adapter *adapter, u32 len, u8 burst);
	u32 (*mem_dump)(struct mac_adapter *adapter, enum mac_ax_mem_sel sel,
			u32 strt_addr, u8 *data, u32 size, u32 dbg_path);
	u32 (*get_mem_size)(struct mac_adapter *adapter,
			    enum mac_ax_mem_sel sel);
	void (*dbg_status_dump)(struct mac_adapter *adapter,
				struct mac_ax_dbgpkg *val,
				struct mac_ax_dbgpkg_en *en);
	u32 (*reg_dump)(struct mac_adapter *adapter,
			enum mac_ax_reg_sel sel);
	u32 (*rx_cnt)(struct mac_adapter *adapter,
		      struct mac_ax_rx_cnt *rxcnt);
	u32 (*dump_fw_rsvd_ple)(struct mac_adapter *adapter, u8 **buf);
	u32 (*fw_dbg_dump)(struct mac_adapter *adapter,
			   u8 **buf,
			   struct mac_ax_fwdbg_en *en);
#endif
#if MAC_AX_FEATURE_HV
	u32 (*ram_boot)(struct mac_adapter *adapter, u8 *fw, u32 len);
	/*fw offload related*/
	u32 (*clear_write_request)(struct mac_adapter *adapter);
	u32 (*add_write_request)(struct mac_adapter *adapter,
				 struct mac_ax_write_req *req,
				 u8 *value, u8 *mask);
	u32 (*write_ofld)(struct mac_adapter *adapter);
	u32 (*clear_conf_request)(struct mac_adapter *adapter);
	u32 (*add_conf_request)(struct mac_adapter *adapter,
				struct mac_ax_conf_ofld_req *req);
	u32 (*conf_ofld)(struct mac_adapter *adapter);
	u32 (*clear_read_request)(struct mac_adapter *adapter);
	u32 (*add_read_request)(struct mac_adapter *adapter,
				struct mac_ax_read_req *req);
	u32 (*read_ofld)(struct mac_adapter *adapter);
	u32 (*read_ofld_value)(struct mac_adapter *adapter,
			       u8 **val_buf, u16 *val_len);
#endif

#endif // if 0 NEO
};


#endif /* _MAC_DEF_H_ */
