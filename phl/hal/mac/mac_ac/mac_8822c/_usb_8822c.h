/** @file */
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

#ifndef _MAC_USB_8822C_H_
#define _MAC_USB_8822C_H_

#include "../../type.h"
#include "../_usb.h"


#if 0  //NEO

/**
 * @addtogroup HCI
 * @{
 * @addtogroup BasicIO
 * @{
 */
/**
 * @brief reg_read8_usb_8852a
 *
 * @param *adapter
 * @param addr
 * @return Please Place Description here.
 * @retval u8
 */
u8 reg_read8_usb_8852a(struct mac_ax_adapter *adapter, u32 addr);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup BasicIO
 * @{
 */

/**
 * @brief reg_write8_usb_8852a
 *
 * @param *adapter
 * @param addr
 * @param val
 * @return Please Place Description here.
 * @retval void
 */
void reg_write8_usb_8852a(struct mac_ax_adapter *adapter,
			  u32 addr, u8 val);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup BasicIO
 * @{
 */

/**
 * @brief reg_read16_usb_8852a
 *
 * @param *adapter
 * @param addr
 * @return Please Place Description here.
 * @retval u16
 */
u16 reg_read16_usb_8852a(struct mac_ax_adapter *adapter, u32 addr);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup BasicIO
 * @{
 */

/**
 * @brief reg_write16_usb_8852a
 *
 * @param *adapter
 * @param addr
 * @param val
 * @return Please Place Description here.
 * @retval void
 */
void reg_write16_usb_8852a(struct mac_ax_adapter *adapter, u32 addr, u16 val);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup BasicIO
 * @{
 */

/**
 * @brief reg_read32_usb_8852a
 *
 * @param *adapter
 * @param addr
 * @return Please Place Description here.
 * @retval u32
 */
u32 reg_read32_usb_8852a(struct mac_ax_adapter *adapter, u32 addr);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup BasicIO
 * @{
 */

/**
 * @brief reg_write32_usb_8852a
 *
 * @param *adapter
 * @param addr
 * @param val
 * @return Please Place Description here.
 * @retval void
 */
void reg_write32_usb_8852a(struct mac_ax_adapter *adapter, u32 addr, u32 val);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup USB
 * @{
 */

/**
 * @brief get_bulkout_id_8852a
 *
 * @param *adapter
 * @param ch_dma
 * @param mode
 * @return Please Place Description here.
 * @retval u8
 */
u8 get_bulkout_id_8852a(struct mac_ax_adapter *adapter, u8 ch_dma, u8 mode);
/**
 * @}
 * @}
 */

#endif //NEO

/**
 * @addtogroup HCI
 * @{
 * @addtogroup USB
 * @{
 */

/**
 * @brief usb_pre_init_8822c
 *
 * @param *adapter
 * @param *param
 * @return Please Place Description here.
 * @retval u32
 */
u32 usb_pre_init_8822c(struct mac_adapter *adapter, void *param);
/**
 * @}
 * @}
 */

#if 0 //NEO

/**
 * @addtogroup HCI
 * @{
 * @addtogroup USB
 * @{
 */

/**
 * @brief usb_init_8852a
 *
 * @param *adapter
 * @param *param
 * @return Please Place Description here.
 * @retval u32
 */
u32 usb_init_8852a(struct mac_ax_adapter *adapter, void *param);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup USB
 * @{
 */

/**
 * @brief usb_deinit_8852a
 *
 * @param *adapter
 * @param *param
 * @return Please Place Description here.
 * @retval u32
 */
u32 usb_deinit_8852a(struct mac_ax_adapter *adapter, void *param);

/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup USB
 * @{
 */

/**
 * @brief read_usb2phy_para_8852a
 *
 * @param *adapter
 * @param offset
 * @return Please Place Description here.
 * @retval u32
 */
u32 read_usb2phy_para_8852a(struct mac_ax_adapter *adapter, u16 offset);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup USB
 * @{
 */

/**
 * @brief write_usb2phy_para_8852a
 *
 * @param *adapter
 * @param offset
 * @param val
 * @return Please Place Description here.
 * @retval u32
 */
u32 write_usb2phy_para_8852a(struct mac_ax_adapter *adapter, u16 offset, u8 val);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup USB
 * @{
 */

/**
 * @brief read_usb3phy_para_8852a
 *
 * @param *adapter
 * @param offset
 * @param b_sel
 * @return Please Place Description here.
 * @retval u32
 */
u32 read_usb3phy_para_8852a(struct mac_ax_adapter *adapter,
			    u16 offset, u8 b_sel);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup USB
 * @{
 */

/**
 * @brief write_usb3phy_para_8852a
 *
 * @param *adapter
 * @param offset
 * @param b_sel
 * @param val
 * @return Please Place Description here.
 * @retval u32
 */
u32 write_usb3phy_para_8852a(struct mac_ax_adapter *adapter, u16 offset,
			     u8 b_sel, u8 val);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup USB
 * @{
 */

/**
 * @brief u2u3_switch_8852a
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 u2u3_switch_8852a(struct mac_ax_adapter *adapter);

 /**
  * @addtogroup HCI
  * @{
  * @addtogroup USB
  * @{
  */

/**
 * @brief get_usb_support_ability_8852a
 *
 * @param *adapter
 * @return Please Place Description here.
 * @retval u32
 */
u32 get_usb_support_ability_8852a(struct mac_ax_adapter *adapter);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup USB
 * @{
 */

/**
 * @brief usb_tx_agg_cfg_8852a
 *
 * @param *adapter
 * @param *agg
 * @return Please Place Description here.
 * @retval u32
 */
u32 usb_tx_agg_cfg_8852a(struct mac_ax_adapter *adapter,
			 struct mac_ax_usb_tx_agg_cfg *agg);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup USB
 * @{
 */

/**
 * @brief usb_rx_agg_cfg_8852a
 *
 * @param *adapter
 * @param *cfg
 * @return Please Place Description here.
 * @retval u32
 */
u32 usb_rx_agg_cfg_8852a(struct mac_ax_adapter *adapter,
			 struct mac_ax_rx_agg_cfg *cfg);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup USB
 * @{
 */

/**
 * @brief usb_pwr_switch_8852a
 *
 * @param *vadapter
 * @param pre_switch
 * @param on
 * @return Please Place Description here.
 * @retval u32
 */
u32 usb_pwr_switch_8852a(void *vadapter,
			 u8 pre_switch, u8 on);
/**
 * @}
 * @}
 */

/**
 * @addtogroup HCI
 * @{
 * @addtogroup USB
 * @{
 */

/**
 * @brief set_usb_wowlan_8852a
 *
 * @param *adapter
 * @param w_c
 * @return Please Place Description here.
 * @retval u32
 */
u32 set_usb_wowlan_8852a(struct mac_ax_adapter *adapter,
			 enum mac_ax_wow_ctrl w_c);
/**
 * @}
 * @}
 */

#endif //NEO
#endif /* __USB_8822C_H__ */
