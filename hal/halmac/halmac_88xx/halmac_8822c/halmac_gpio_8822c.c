/******************************************************************************
 *
 * Copyright(c) 2016 - 2019 Realtek Corporation. All rights reserved.
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

#include "halmac_gpio_8822c.h"
#include "../halmac_gpio_88xx.h"

#if HALMAC_8822C_SUPPORT
/* GPIO0 definition */
#define GPIO0_BT_GPIO0_8822C	\
	{HALMAC_BT_GPIO, HALMAC_GPIO0, HALMAC_GPIO_IN_OUT, \
	 0x66, BIT(2), BIT(2)}
#define GPIO0_BT_SDIO_INT_8822C	\
	{HALMAC_BT_SDIO_INT, HALMAC_GPIO0, HALMAC_GPIO_OUT, \
	 0x4F, BIT(5), BIT(5)}
#define GPIO0_USIN0_8822C	\
	{HALMAC_UART0, HALMAC_GPIO0, HALMAC_GPIO_IN, \
	 0x66, BIT(6), BIT(6)}
#define GPIO0_BT_ANT_SW_0_8822C	\
	{HALMAC_BT_RF, HALMAC_GPIO0, HALMAC_GPIO_OUT, \
	 0x4F, BIT(6), BIT(6)}
#define GPIO0_BT_ACT_8822C	\
	{HALMAC_BT_PTA, HALMAC_GPIO0, HALMAC_GPIO_IN_OUT, \
	 0x41, BIT(1), 0}
#define GPIO0_WL_ACT_8822C	\
	{HALMAC_WL_PTA, HALMAC_GPIO0, HALMAC_GPIO_IN_OUT, \
	 0x41, BIT(2), BIT(2)}
#define GPIO0_WLMAC_DBG_GPIO0_8822C	\
	{HALMAC_WLMAC_DBG, HALMAC_GPIO0, HALMAC_GPIO_OUT, \
	 0x40, BIT(1) | BIT(0), BIT(0)}
#define GPIO0_WLPHY_DBG_GPIO0_8822C	\
	{HALMAC_WLPHY_DBG, HALMAC_GPIO0, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(1) | BIT(0), BIT(1)}
#define GPIO0_BT_DBG_GPIO0_8822C	\
	{HALMAC_BT_DBG, HALMAC_GPIO0, HALMAC_GPIO_OUT, \
	 0x40, BIT(1) | BIT(0), BIT(1) | BIT(0)}
#define GPIO0_ANT_SW_GPIO0_8822C	\
	{HALMAC_SW_DPDT_SEL, HALMAC_GPIO0, HALMAC_GPIO_OUT, \
	 0x4E, BIT(7), BIT(7)}
#define GPIO0_BT_RFE_CTRL_1_8822C	\
	{HALMAC_BT_DPDT_SEL, HALMAC_GPIO0, HALMAC_GPIO_IN_OUT, \
	 0x42, BIT(1), BIT(1)}
#define GPIO0_WL_RFE_CTRL_9_8822C	\
	{HALMAC_WL_DPDT_SEL, HALMAC_GPIO0, HALMAC_GPIO_IN_OUT, \
	 0x42, BIT(1), BIT(1)}
#define GPIO0_RFE_CTRL_10_4_5_8822C	\
	{HALMAC_WLPHY_RFE_CTRL2GPIO, HALMAC_GPIO0, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(2), BIT(2)}
#define GPIO0_RFE_CTRL_10_6_7_8822C	\
	{HALMAC_WLPHY_RFE_CTRL2GPIO_2, HALMAC_GPIO0, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(3), BIT(3)}
#define GPIO0_SW_IO_8822C	\
	{HALMAC_SW_IO, HALMAC_GPIO0, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(1) | BIT(0), 0}

/* GPIO 1 definition */
#define GPIO1_BT_GPIO1_8822C	\
	{HALMAC_BT_GPIO, HALMAC_GPIO1, HALMAC_GPIO_IN_OUT, \
	 0x66, BIT(2), BIT(2)}
#define GPIO1_USOUT0_8822C	\
	{HALMAC_UART0, HALMAC_GPIO1, HALMAC_GPIO_OUT, \
	 0x66, BIT(6), BIT(6)}
#define GPIO1_BT_ANT_SW_1_8822C	\
	{HALMAC_BT_RF, HALMAC_GPIO1, HALMAC_GPIO_OUT, \
	 0x4F, BIT(6), BIT(6)}
#define GPIO1_WL_CK_8822C	\
	{HALMAC_BT_PTA, HALMAC_GPIO1, HALMAC_GPIO_OUT, \
	 0x41, BIT(1), 0}
#define GPIO1_BT_CK_8822C	\
	{HALMAC_WL_PTA, HALMAC_GPIO1, HALMAC_GPIO_OUT, \
	 0x41, BIT(2), BIT(2)}
#define GPIO1_WLMAC_DBG_GPIO1_8822C	\
	{HALMAC_WLMAC_DBG, HALMAC_GPIO1, HALMAC_GPIO_OUT, \
	 0x40, BIT(1) | BIT(0), BIT(0)}
#define GPIO1_WLPHY_DBG_GPIO1_8822C	\
	{HALMAC_WLPHY_DBG, HALMAC_GPIO1, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(1) | BIT(0), BIT(1)}
#define GPIO1_BT_DBG_GPIO1_8822C	\
	{HALMAC_BT_DBG, HALMAC_GPIO1, HALMAC_GPIO_OUT, \
	 0x40, BIT(1) | BIT(0), BIT(1) | BIT(0)}
#define GPIO1_ANT_SWB_GPIO1_8822C	\
	{HALMAC_SW_DPDT_SEL, HALMAC_GPIO1, HALMAC_GPIO_OUT, \
	 0x4E, BIT(7), BIT(7)}
#define GPIO1_BT_RFE_CTRL_0_8822C	\
	{HALMAC_BT_DPDT_SEL, HALMAC_GPIO1, HALMAC_GPIO_IN_OUT, \
	 0x42, BIT(1), BIT(1)}
#define GPIO1_WL_RFE_CTRL_8_8822C	\
	{HALMAC_WL_DPDT_SEL, HALMAC_GPIO1, HALMAC_GPIO_IN_OUT, \
	 0x42, BIT(1), BIT(1)}
#define GPIO1_RFE_CTRL_5_4_5_8822C	\
	{HALMAC_WLPHY_RFE_CTRL2GPIO, HALMAC_GPIO1, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(2), BIT(2)}
#define GPIO1_RFE_CTRL_5_6_7_8822C	\
	{HALMAC_WLPHY_RFE_CTRL2GPIO_2, HALMAC_GPIO1, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(3), BIT(3)}
#define GPIO1_SW_IO_8822C	\
	{HALMAC_SW_IO, HALMAC_GPIO1, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(1) | BIT(0), 0}

/* GPIO2 definition */
#define GPIO2_BT_GPIO2_8822C	\
	{HALMAC_BT_GPIO, HALMAC_GPIO2, HALMAC_GPIO_IN_OUT, \
	 0x66, BIT(2), BIT(2)}
#define GPIO2_BT_ANT_SW_2_8822C	\
	{HALMAC_BT_RF, HALMAC_GPIO2, HALMAC_GPIO_OUT, \
	 0x4F, BIT(6), BIT(6)}
#define GPIO2_WL_STATE_8822C	\
	{HALMAC_BT_PTA, HALMAC_GPIO2, HALMAC_GPIO_OUT, \
	 0x41, BIT(1), 0}
#define GPIO2_BT_STATE_8822C	\
	{HALMAC_WL_PTA, HALMAC_GPIO2, HALMAC_GPIO_OUT, \
	 0x41, BIT(2), BIT(2)}
#define GPIO2_WLMAC_DBG_GPIO2_8822C	\
	{HALMAC_WLMAC_DBG, HALMAC_GPIO2, HALMAC_GPIO_OUT, \
	 0x40, BIT(1) | BIT(0), BIT(0)}
#define GPIO2_WLPHY_DBG_GPIO2_8822C	\
	{HALMAC_WLPHY_DBG, HALMAC_GPIO2, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(1) | BIT(0), BIT(1)}
#define GPIO2_BT_DBG_GPIO2_8822C	\
	{HALMAC_BT_DBG, HALMAC_GPIO2, HALMAC_GPIO_OUT, \
	 0x40, BIT(1) | BIT(0), BIT(1) | BIT(0)}
#define GPIO2_RFE_CTRL_11_4_5_8822C	\
	{HALMAC_WLPHY_RFE_CTRL2GPIO, HALMAC_GPIO2, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(2), BIT(2)}
#define GPIO2_RFE_CTRL_11_6_7_8822C	\
	{HALMAC_WLPHY_RFE_CTRL2GPIO_2, HALMAC_GPIO2, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(3), BIT(3)}
#define GPIO2_SW_IO_8822C	\
	{HALMAC_SW_IO, HALMAC_GPIO2, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(1) | BIT(0), 0}

/* GPIO3 definition */
#define GPIO3_BT_GPIO3_8822C	\
	{HALMAC_BT_GPIO, HALMAC_GPIO3, HALMAC_GPIO_IN_OUT, \
	 0x66, BIT(2), BIT(2)}
#define GPIO3_BT_ANT_SW_3_8822C	\
	{HALMAC_BT_RF, HALMAC_GPIO3, HALMAC_GPIO_OUT, \
	 0x4F, BIT(6), BIT(6)}
#define GPIO3_WL_PRI_8822C	\
	{HALMAC_BT_PTA, HALMAC_GPIO3, HALMAC_GPIO_OUT, \
	 0x41, BIT(1), 0}
#define GPIO3_BT_PRI_8822C	\
	{HALMAC_WL_PTA, HALMAC_GPIO3, HALMAC_GPIO_OUT, \
	 0x41, BIT(2), BIT(2)}
#define GPIO3_WLMAC_DBG_GPIO3_8822C	\
	{HALMAC_WLMAC_DBG, HALMAC_GPIO3, HALMAC_GPIO_OUT, \
	 0x40, BIT(1) | BIT(0), BIT(0)}
#define GPIO3_WLPHY_DBG_GPIO3_8822C	\
	{HALMAC_WLPHY_DBG, HALMAC_GPIO3, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(1) | BIT(0), BIT(1)}
#define GPIO3_BT_DBG_GPIO3_8822C	\
	{HALMAC_BT_DBG, HALMAC_GPIO3, HALMAC_GPIO_OUT, \
	 0x40, BIT(1) | BIT(0), BIT(1) | BIT(0)}
#define GPIO3_LNAON_SEL_8822C	\
	{HALMAC_SW_LNAON_SET, HALMAC_GPIO3, HALMAC_GPIO_OUT, \
	 0x4F, BIT(2), BIT(2)}
#define GPIO3_BT_RFE_CTRL_5_8822C	\
	{HALMAC_BT_LNAON_SEL, HALMAC_GPIO3, HALMAC_GPIO_IN_OUT, \
	 0x42, BIT(0), BIT(0)}
#define GPIO3_RFE_CTRL_3_8822C	\
	{HALMAC_WL_LNAON_SEL, HALMAC_GPIO3, HALMAC_GPIO_IN_OUT, \
	 0x42, BIT(0), BIT(0)}
#define GPIO3_SW_IO_8822C	\
	{HALMAC_SW_IO, HALMAC_GPIO3, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(1) | BIT(0), 0}

/* GPIO4 definition */
#define GPIO4_BT_SPI_D0_8822C	\
	{HALMAC_BT_SFLASH, HALMAC_GPIO4, HALMAC_GPIO_IN_OUT, \
	 0x66, BIT(4), BIT(4)}
#define GPIO4_WL_SPI_D0_8822C	\
	{HALMAC_WL_SFLASH, HALMAC_GPIO4, HALMAC_GPIO_IN_OUT, \
	 0x42, BIT(3), BIT(3)}
#define GPIO4_BT_JTAG_TRST_8822C	\
	{HALMAC_BT_JTAG, HALMAC_GPIO4, HALMAC_GPIO_IN, \
	 0x67, BIT(0), BIT(0)}
#define GPIO4_WL_JTAG_TRST_8822C	\
	{HALMAC_WL_JTAG, HALMAC_GPIO4, HALMAC_GPIO_IN, \
	 0x65, BIT(7), BIT(7)}
#define GPIO4_DBG_GNT_WL_8822C	\
	{HALMAC_DBG_GNT_WL_BT, HALMAC_GPIO4, HALMAC_GPIO_OUT, \
	 0x73, BIT(3), BIT(3)}
#define GPIO4_WLMAC_DBG_GPIO4_8822C	\
	{HALMAC_WLMAC_DBG, HALMAC_GPIO4, HALMAC_GPIO_OUT, \
	 0x40, BIT(1) | BIT(0), BIT(0)}
#define GPIO4_WLPHY_DBG_GPIO4_8822C	\
	{HALMAC_WLPHY_DBG, HALMAC_GPIO4, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(1) | BIT(0), BIT(1)}
#define GPIO4_BT_DBG_GPIO4_8822C	\
	{HALMAC_BT_DBG, HALMAC_GPIO4, HALMAC_GPIO_OUT, \
	 0x40, BIT(1) | BIT(0), BIT(1) | BIT(0)}
#define GPIO4_ANT_SWB_GPIO4_8822C	\
	{HALMAC_SW_DPDT_SEL, HALMAC_GPIO4, HALMAC_GPIO_OUT, \
	 0x4E, BIT(7), BIT(7)}
#define GPIO4_BT_RFE_CTRL_0_8822C	\
	{HALMAC_BT_DPDT_SEL, HALMAC_GPIO4, HALMAC_GPIO_IN_OUT, \
	 0x42, BIT(1), BIT(1)}
#define GPIO4_WL_RFE_CTRL_8_8822C	\
	{HALMAC_WL_DPDT_SEL, HALMAC_GPIO4, HALMAC_GPIO_IN_OUT, \
	 0x42, BIT(1), BIT(1)}
#define GPIO4_SW_IO_8822C	\
	{HALMAC_SW_IO, HALMAC_GPIO4, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(1) | BIT(0), 0}

/* GPIO5 definition */
#define GPIO5_BT_SPI_D1_8822C	\
	{HALMAC_BT_SFLASH, HALMAC_GPIO5, HALMAC_GPIO_OUT, \
	 0x66, BIT(4), BIT(4)}
#define GPIO5_WL_SPI_D1_8822C	\
	{HALMAC_WL_SFLASH, HALMAC_GPIO5, HALMAC_GPIO_OUT, \
	 0x42, BIT(3), BIT(3)}
#define GPIO5_BT_JTAG_TDI_8822C	\
	{HALMAC_BT_JTAG, HALMAC_GPIO5, HALMAC_GPIO_IN, \
	 0x67, BIT(0), BIT(0)}
#define GPIO5_WL_JTAG_TDI_8822C	\
	{HALMAC_WL_JTAG, HALMAC_GPIO5, HALMAC_GPIO_IN, \
	 0x65, BIT(7), BIT(7)}
#define GPIO5_DBG_GNT_BT_8822C	\
	{HALMAC_DBG_GNT_WL_BT, HALMAC_GPIO5, HALMAC_GPIO_OUT, \
	 0x73, BIT(3), BIT(3)}
#define GPIO5_BT_GPIO18_8822C	\
	{HALMAC_BT_GPIO, HALMAC_GPIO5, HALMAC_GPIO_IN_OUT, \
	 0x67, BIT(1), BIT(1)}
#define GPIO5_SOUT_8822C	\
	{HALMAC_WL_UART, HALMAC_GPIO5, HALMAC_GPIO_OUT, \
	 0x41, BIT(0), BIT(0)}
#define GPIO5_WLMAC_DBG_GPIO5_8822C	\
	{HALMAC_WLMAC_DBG, HALMAC_GPIO5, HALMAC_GPIO_OUT, \
	 0x40, BIT(1) | BIT(0), BIT(0)}
#define GPIO5_WLPHY_DBG_GPIO5_8822C	\
	{HALMAC_WLPHY_DBG, HALMAC_GPIO5, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(1) | BIT(0), BIT(1)}
#define GPIO5_BT_DBG_GPIO5_8822C	\
	{HALMAC_BT_DBG, HALMAC_GPIO5, HALMAC_GPIO_OUT, \
	 0x40, BIT(1) | BIT(0), BIT(1) | BIT(0)}
#define GPIO5_I2C_INT_3_WIRE_8822C	\
	{HALMAC_MAILBOX_3W, HALMAC_GPIO5, HALMAC_GPIO_IN, \
	 0x4F, BIT(4), BIT(4)}
#define GPIO5_I2C_INT_1_WIRE_8822C	\
	{HALMAC_MAILBOX_1W, HALMAC_GPIO5, HALMAC_GPIO_IN, \
	 0x4F, BIT(7), BIT(7)}
#define GPIO5_SW_IO_8822C	\
	{HALMAC_SW_IO, HALMAC_GPIO5, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(1) | BIT(0), 0}

/* GPIO6 definition */
#define GPIO6_BT_SPI_D2_8822C	\
	{HALMAC_BT_SFLASH, HALMAC_GPIO6, HALMAC_GPIO_OUT, \
	 0x66, BIT(4), BIT(4)}
#define GPIO6_WL_SPI_D2_8822C	\
	{HALMAC_WL_SFLASH, HALMAC_GPIO6, HALMAC_GPIO_OUT, \
	 0x42, BIT(3), BIT(3)}
#define GPIO6_EEDO_8822C	\
	{HALMAC_EEPROM, HALMAC_GPIO6, HALMAC_GPIO_IN, \
	 0x40, BIT(4), BIT(4)}
#define GPIO6_BT_JTAG_TDO_8822C	\
	{HALMAC_BT_JTAG, HALMAC_GPIO6, HALMAC_GPIO_OUT, \
	 0x67, BIT(0), BIT(0)}
#define GPIO6_WL_JTAG_TDO_8822C	\
	{HALMAC_WL_JTAG, HALMAC_GPIO6, HALMAC_GPIO_OUT, \
	 0x65, BIT(0), BIT(0)}
#define GPIO6_BT_3DD_SYNC_B_8822C	\
	{HALMAC_BT_3DDLS_B, HALMAC_GPIO6, HALMAC_GPIO_IN, \
	 0x67, BIT(1), BIT(1)}
#define GPIO6_SIN_8822C	\
	{HALMAC_WL_UART, HALMAC_GPIO6, HALMAC_GPIO_IN, \
	 0x41, BIT(0), BIT(0)}
#define GPIO6_WLMAC_DBG_GPIO6_8822C	\
	{HALMAC_WLMAC_DBG, HALMAC_GPIO6, HALMAC_GPIO_OUT, \
	 0x40, BIT(1) | BIT(0), BIT(0)}
#define GPIO6_WLPHY_DBG_GPIO6_8822C	\
	{HALMAC_WLPHY_DBG, HALMAC_GPIO6, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(1) | BIT(0), BIT(1)}
#define GPIO6_BT_DBG_GPIO6_8822C	\
	{HALMAC_BT_DBG, HALMAC_GPIO6, HALMAC_GPIO_OUT, \
	 0x40, BIT(1) | BIT(0), BIT(1) | BIT(0)}
#define GPIO6_SW_PAPE_SEL_8822C	\
	{HALMAC_SW_PAPE_SEL, HALMAC_GPIO6, HALMAC_GPIO_OUT, \
	 0x4F, BIT(1), BIT(1)}
#define GPIO6_BT_RFE_CTRL_3_8822C	\
	{HALMAC_BT_PAPE_SEL, HALMAC_GPIO6, HALMAC_GPIO_IN_OUT, \
	 0x4F, BIT(3), BIT(3)}
#define GPIO6_RFE_CTRL_1_8822C	\
	{HALMAC_WL_PAPE_SEL, HALMAC_GPIO6, HALMAC_GPIO_IN_OUT, \
	 0x4F, BIT(3), BIT(3)}
#define GPIO6_EXT_SWR_CTRL_8822C	\
	{HALMAC_SWR_CTRL_EN, HALMAC_GPIO6, HALMAC_GPIO_IN, \
	 0x7F, BIT(7), BIT(7)}
#define GPIO6_SW_IO_8822C	\
	{HALMAC_SW_IO, HALMAC_GPIO6, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(1) | BIT(0), 0}

/* GPIO7 definition */
#define GPIO7_BT_SPI_D3_8822C	\
	{HALMAC_BT_SFLASH, HALMAC_GPIO7, HALMAC_GPIO_OUT, \
	 0x66, BIT(4), BIT(4)}
#define GPIO7_WL_SPI_D3_8822C	\
	{HALMAC_WL_SFLASH, HALMAC_GPIO7, HALMAC_GPIO_OUT, \
	 0x42, BIT(3), BIT(3)}
#define GPIO7_EEDI_8822C	\
	{HALMAC_EEPROM, HALMAC_GPIO7, HALMAC_GPIO_OUT, \
	 0x40, BIT(4), BIT(4)}
#define GPIO7_BT_JTAG_TMS_8822C	\
	{HALMAC_BT_JTAG, HALMAC_GPIO7, HALMAC_GPIO_IN, \
	 0x67, BIT(0), BIT(0)}
#define GPIO7_WL_JTAG_TMS_8822C	\
	{HALMAC_WL_JTAG, HALMAC_GPIO7, HALMAC_GPIO_IN, \
	 0x65, BIT(7), BIT(7)}
#define GPIO7_BT_GPIO16_8822C	\
	{HALMAC_BT_GPIO, HALMAC_GPIO7, HALMAC_GPIO_IN_OUT, \
	 0x67, BIT(2), BIT(2)}
#define GPIO7_SOUT_8822C	\
	{HALMAC_WL_UART, HALMAC_GPIO7, HALMAC_GPIO_OUT, \
	 0x41, BIT(0), BIT(0)}
#define GPIO7_WLMAC_DBG_GPIO7_8822C	\
	{HALMAC_WLMAC_DBG, HALMAC_GPIO7, HALMAC_GPIO_OUT, \
	 0x40, BIT(1) | BIT(0), BIT(0)}
#define GPIO7_WLPHY_DBG_GPIO7_8822C	\
	{HALMAC_WLPHY_DBG, HALMAC_GPIO7, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(1) | BIT(0), BIT(1)}
#define GPIO7_BT_DBG_GPIO7_8822C	\
	{HALMAC_BT_DBG, HALMAC_GPIO7, HALMAC_GPIO_OUT, \
	 0x40, BIT(1) | BIT(0), BIT(1) | BIT(0)}
#define GPIO7_LNAON_SEL_8822C	\
	{HALMAC_SW_LNAON_SET, HALMAC_GPIO7, HALMAC_GPIO_OUT, \
	 0x4F, BIT(2), BIT(2)}
#define GPIO7_BT_RFE_CTRL_4_8822C	\
	{HALMAC_BT_LNAON_SEL, HALMAC_GPIO7, HALMAC_GPIO_IN_OUT, \
	 0x42, BIT(0), BIT(0)}
#define GPIO7_RFE_CTRL_2_8822C	\
	{HALMAC_WL_LNAON_SEL, HALMAC_GPIO7, HALMAC_GPIO_IN_OUT, \
	 0x42, BIT(0), BIT(0)}
#define GPIO7_SW_IO_8822C	\
	{HALMAC_SW_IO, HALMAC_GPIO7, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(1) | BIT(0), 0}

/* GPIO8 definition */
#define GPIO8_WL_EXT_WOL_8822C	\
	{HALMAC_WL_HW_EXTWOL, HALMAC_GPIO8, HALMAC_GPIO_IN, \
	 0x4A, BIT(1) | BIT(0), BIT(1) | BIT(0)}
#define GPIO8_WL_LED_8822C	\
	{HALMAC_WL_LED, HALMAC_GPIO8, HALMAC_GPIO_OUT, \
	 0x4E, BIT(5), BIT(5)}
#define GPIO8_SOUT_8822C	\
	{HALMAC_WL_UART, HALMAC_GPIO8, HALMAC_GPIO_OUT, \
	 0x41, BIT(0), BIT(0)}
#define GPIO8_SW_IO_8822C	\
	{HALMAC_SW_IO, HALMAC_GPIO8, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(1) | BIT(0), 0}

/* GPIO9 definition */
#define GPIO9_DIS_WL_N_8822C	\
	{HALMAC_WL_HWPDN, HALMAC_GPIO9, HALMAC_GPIO_IN, \
	 0x68, BIT(3) | BIT(0), BIT(3) | BIT(0)}
#define GPIO9_WL_EXT_WOL_8822C	\
	{HALMAC_WL_HW_EXTWOL, HALMAC_GPIO9, HALMAC_GPIO_IN, \
	 0x4A, BIT(1) | BIT(0), BIT(0)}
#define GPIO9_USIN0_8822C	\
	{HALMAC_UART0, HALMAC_GPIO9, HALMAC_GPIO_IN, \
	 0x66, BIT(6), BIT(6)}
#define GPIO9_I2C_SD_8822C	\
	{HALMAC_MAILBOX_3W, HALMAC_GPIO9, HALMAC_GPIO_IN, \
	 0x4F, BIT(4), BIT(4)}
#define GPIO9_LNAON_SEL_8822C	\
	{HALMAC_SW_LNAON_SET, HALMAC_GPIO9, HALMAC_GPIO_OUT, \
	 0x4F, BIT(2), BIT(2)}
#define GPIO9_BT_RFE_CTRL_4_8822C	\
	{HALMAC_BT_LNAON_SEL, HALMAC_GPIO9, HALMAC_GPIO_IN_OUT, \
	 0x42, BIT(0), BIT(0)}
#define GPIO9_RFE_CTRL_2_8822C	\
	{HALMAC_WL_LNAON_SEL, HALMAC_GPIO9, HALMAC_GPIO_IN_OUT, \
	 0x42, BIT(0), BIT(0)}
#define GPIO9_SW_IO_8822C	\
	{HALMAC_SW_IO, HALMAC_GPIO9, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(1) | BIT(0), 0}

/* GPIO10 definition */
#define GPIO10_WL_SDIO_INT_8822C	\
	{HALMAC_SDIO_INT, HALMAC_GPIO10, HALMAC_GPIO_OUT, \
	 0x72, BIT(2), BIT(2)}
#define GPIO10_SW_IO_8822C	\
	{HALMAC_SW_IO, HALMAC_GPIO10, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(1) | BIT(0), 0}

/* GPIO11 definition */
#define GPIO11_DIS_BT_N_8822C	\
	{HALMAC_BT_HWPDN, HALMAC_GPIO11, HALMAC_GPIO_IN, \
	 0x6A, BIT(3) | BIT(0), BIT(3) | BIT(0)}
#define GPIO11_USOUT0_8822C	\
	{HALMAC_UART0, HALMAC_GPIO11, HALMAC_GPIO_OUT, \
	 0x66, BIT(6), BIT(6)}
#define GPIO11_RFE_CTRL_11_4_5_8822C	\
	{HALMAC_WLPHY_RFE_CTRL2GPIO, HALMAC_GPIO11, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(2), BIT(2)}
#define GPIO11_RFE_CTRL_11_6_7_8822C	\
	{HALMAC_WLPHY_RFE_CTRL2GPIO_2, HALMAC_GPIO11, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(3), BIT(3)}
#define GPIO11_SW_IO_8822C	\
	{HALMAC_SW_IO, HALMAC_GPIO11, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(1) | BIT(0), 0}

/* GPIO12 definition */
#define GPIO12_USCTS0_8822C	\
	{HALMAC_UART0, HALMAC_GPIO12, HALMAC_GPIO_IN, \
	 0x66, BIT(6), BIT(6)}
#define GPIO12_I2C_CLK_8822C	\
	{HALMAC_MAILBOX_3W, HALMAC_GPIO12, HALMAC_GPIO_IN, \
	 0x4F, BIT(4), BIT(4)}
#define GPIO12_ANT_SW_GPIO12_8822C	\
	{HALMAC_SW_DPDT_SEL, HALMAC_GPIO12, HALMAC_GPIO_OUT, \
	 0x4E, BIT(7), BIT(7)}
#define GPIO12_BT_RFE_CTRL_1_8822C	\
	{HALMAC_BT_DPDT_SEL, HALMAC_GPIO12, HALMAC_GPIO_IN_OUT, \
	 0x42, BIT(1), BIT(1)}
#define GPIO12_WL_RFE_CTRL_9_8822C	\
	{HALMAC_WL_DPDT_SEL, HALMAC_GPIO12, HALMAC_GPIO_IN_OUT, \
	 0x42, BIT(1), BIT(1)}
#define GPIO12_SW_IO_8822C	\
	{HALMAC_SW_IO, HALMAC_GPIO12, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(1) | BIT(0), 0}

/* GPIO13 definition */
#define GPIO13_BT_WAKE_8822C	\
	{HALMAC_GPIO13_14_WL_CTRL_EN, HALMAC_GPIO13, HALMAC_GPIO_IN, \
	 0x4E, BIT(6), BIT(6)}
#define GPIO13_BT_ANT_SW_0_8822C	\
	{HALMAC_BT_RF, HALMAC_GPIO13, HALMAC_GPIO_OUT, \
	 0x4F, BIT(6), BIT(6)}
#define GPIO13_ANT_SW_GPIO13_8822C	\
	{HALMAC_SW_DPDT_SEL, HALMAC_GPIO13, HALMAC_GPIO_OUT, \
	 0x4E, BIT(7), BIT(7)}
#define GPIO13_BT_RFE_CTRL_1_8822C	\
	{HALMAC_BT_DPDT_SEL, HALMAC_GPIO13, HALMAC_GPIO_IN_OUT, \
	 0x42, BIT(1), BIT(1)}
#define GPIO13_WL_RFE_CTRL_9_8822C	\
	{HALMAC_WL_DPDT_SEL, HALMAC_GPIO13, HALMAC_GPIO_IN_OUT, \
	 0x42, BIT(1), BIT(1)}
#define GPIO13_RFE_CTRL_10_4_5_8822C	\
	{HALMAC_WLPHY_RFE_CTRL2GPIO, HALMAC_GPIO13, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(2), BIT(2)}
#define GPIO13_RFE_CTRL_10_6_7_8822C	\
	{HALMAC_WLPHY_RFE_CTRL2GPIO_2, HALMAC_GPIO13, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(3), BIT(3)}
#define GPIO13_SW_IO_8822C	\
	{HALMAC_SW_IO, HALMAC_GPIO13, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(1) | BIT(0), 0}

/* GPIO14 definition */
#define GPIO14_UART_WAKE_8822C	\
	{HALMAC_GPIO13_14_WL_CTRL_EN, HALMAC_GPIO14, HALMAC_GPIO_OUT, \
	 0x4E, BIT(6), BIT(6)}
#define GPIO14_BT_ANT_SW_1_8822C	\
	{HALMAC_BT_RF, HALMAC_GPIO14, HALMAC_GPIO_OUT, \
	 0x4F, BIT(6), BIT(6)}
#define GPIO14_RFE_CTRL_5_4_5_8822C	\
	{HALMAC_WLPHY_RFE_CTRL2GPIO, HALMAC_GPIO14, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(2), BIT(2)}
#define GPIO14_RFE_CTRL_5_6_7_8822C	\
	{HALMAC_WLPHY_RFE_CTRL2GPIO_2, HALMAC_GPIO14, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(3), BIT(3)}
#define GPIO14_SW_IO_8822C	\
	{HALMAC_SW_IO, HALMAC_GPIO14, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(1) | BIT(0), 0}

/* GPIO15 definition */
#define GPIO15_EXT_XTAL_8822C	\
	{HALMAC_EXT_XTAL, HALMAC_GPIO15, HALMAC_GPIO_OUT, \
	 0x66, BIT(7), BIT(7)}
#define GPIO15_SW_IO_8822C	\
	{HALMAC_SW_IO, HALMAC_GPIO15, HALMAC_GPIO_IN_OUT, \
	 0x40, BIT(1) | BIT(0), 0}

static const struct halmac_gpio_pimux_list PINMUX_LIST_GPIO0_8822C[] = {
	GPIO0_BT_GPIO0_8822C,
	GPIO0_BT_SDIO_INT_8822C,
	GPIO0_USIN0_8822C,
	GPIO0_BT_ANT_SW_0_8822C,
	GPIO0_BT_ACT_8822C,
	GPIO0_WL_ACT_8822C,
	GPIO0_WLMAC_DBG_GPIO0_8822C,
	GPIO0_WLPHY_DBG_GPIO0_8822C,
	GPIO0_BT_DBG_GPIO0_8822C,
	GPIO0_ANT_SW_GPIO0_8822C,
	GPIO0_BT_RFE_CTRL_1_8822C,
	GPIO0_WL_RFE_CTRL_9_8822C,
	GPIO0_RFE_CTRL_10_4_5_8822C,
	GPIO0_RFE_CTRL_10_6_7_8822C,
	GPIO0_SW_IO_8822C
};

static const struct halmac_gpio_pimux_list PINMUX_LIST_GPIO1_8822C[] = {
	GPIO1_BT_GPIO1_8822C,
	GPIO1_USOUT0_8822C,
	GPIO1_BT_ANT_SW_1_8822C,
	GPIO1_WL_CK_8822C,
	GPIO1_BT_CK_8822C,
	GPIO1_WLMAC_DBG_GPIO1_8822C,
	GPIO1_WLPHY_DBG_GPIO1_8822C,
	GPIO1_BT_DBG_GPIO1_8822C,
	GPIO1_ANT_SWB_GPIO1_8822C,
	GPIO1_BT_RFE_CTRL_0_8822C,
	GPIO1_WL_RFE_CTRL_8_8822C,
	GPIO1_RFE_CTRL_5_4_5_8822C,
	GPIO1_RFE_CTRL_5_6_7_8822C,
	GPIO1_SW_IO_8822C
};

static const struct halmac_gpio_pimux_list PINMUX_LIST_GPIO2_8822C[] = {
	GPIO2_BT_GPIO2_8822C,
	GPIO2_BT_ANT_SW_2_8822C,
	GPIO2_WL_STATE_8822C,
	GPIO2_BT_STATE_8822C,
	GPIO2_WLMAC_DBG_GPIO2_8822C,
	GPIO2_WLPHY_DBG_GPIO2_8822C,
	GPIO2_BT_DBG_GPIO2_8822C,
	GPIO2_RFE_CTRL_11_4_5_8822C,
	GPIO2_RFE_CTRL_11_6_7_8822C,
	GPIO2_SW_IO_8822C
};

static const struct halmac_gpio_pimux_list PINMUX_LIST_GPIO3_8822C[] = {
	GPIO3_BT_GPIO3_8822C,
	GPIO3_BT_ANT_SW_3_8822C,
	GPIO3_WL_PRI_8822C,
	GPIO3_BT_PRI_8822C,
	GPIO3_WLMAC_DBG_GPIO3_8822C,
	GPIO3_WLPHY_DBG_GPIO3_8822C,
	GPIO3_BT_DBG_GPIO3_8822C,
	GPIO3_LNAON_SEL_8822C,
	GPIO3_BT_RFE_CTRL_5_8822C,
	GPIO3_RFE_CTRL_3_8822C,
	GPIO3_SW_IO_8822C
};

static const struct halmac_gpio_pimux_list PINMUX_LIST_GPIO4_8822C[] = {
	GPIO4_BT_SPI_D0_8822C,
	GPIO4_WL_SPI_D0_8822C,
	GPIO4_BT_JTAG_TRST_8822C,
	GPIO4_WL_JTAG_TRST_8822C,
	GPIO4_DBG_GNT_WL_8822C,
	GPIO4_WLMAC_DBG_GPIO4_8822C,
	GPIO4_WLPHY_DBG_GPIO4_8822C,
	GPIO4_BT_DBG_GPIO4_8822C,
	GPIO4_ANT_SWB_GPIO4_8822C,
	GPIO4_BT_RFE_CTRL_0_8822C,
	GPIO4_WL_RFE_CTRL_8_8822C,
	GPIO4_SW_IO_8822C
};

static const struct halmac_gpio_pimux_list PINMUX_LIST_GPIO5_8822C[] = {
	GPIO5_BT_SPI_D1_8822C,
	GPIO5_WL_SPI_D1_8822C,
	GPIO5_BT_JTAG_TDI_8822C,
	GPIO5_WL_JTAG_TDI_8822C,
	GPIO5_DBG_GNT_BT_8822C,
	GPIO5_BT_GPIO18_8822C,
	GPIO5_SOUT_8822C,
	GPIO5_WLMAC_DBG_GPIO5_8822C,
	GPIO5_WLPHY_DBG_GPIO5_8822C,
	GPIO5_BT_DBG_GPIO5_8822C,
	GPIO5_I2C_INT_3_WIRE_8822C,
	GPIO5_I2C_INT_1_WIRE_8822C,
	GPIO5_SW_IO_8822C
};

static const struct halmac_gpio_pimux_list PINMUX_LIST_GPIO6_8822C[] = {
	GPIO6_BT_SPI_D2_8822C,
	GPIO6_WL_SPI_D2_8822C,
	GPIO6_EEDO_8822C,
	GPIO6_BT_JTAG_TDO_8822C,
	GPIO6_WL_JTAG_TDO_8822C,
	GPIO6_BT_3DD_SYNC_B_8822C,
	GPIO6_SIN_8822C,
	GPIO6_WLMAC_DBG_GPIO6_8822C,
	GPIO6_WLPHY_DBG_GPIO6_8822C,
	GPIO6_BT_DBG_GPIO6_8822C,
	GPIO6_SW_PAPE_SEL_8822C,
	GPIO6_BT_RFE_CTRL_3_8822C,
	GPIO6_RFE_CTRL_1_8822C,
	GPIO6_EXT_SWR_CTRL_8822C,
	GPIO6_SW_IO_8822C
};

static const struct halmac_gpio_pimux_list PINMUX_LIST_GPIO7_8822C[] = {
	GPIO7_BT_SPI_D3_8822C,
	GPIO7_WL_SPI_D3_8822C,
	GPIO7_EEDI_8822C,
	GPIO7_BT_JTAG_TMS_8822C,
	GPIO7_WL_JTAG_TMS_8822C,
	GPIO7_BT_GPIO16_8822C,
	GPIO7_SOUT_8822C,
	GPIO7_WLMAC_DBG_GPIO7_8822C,
	GPIO7_WLPHY_DBG_GPIO7_8822C,
	GPIO7_BT_DBG_GPIO7_8822C,
	GPIO7_LNAON_SEL_8822C,
	GPIO7_BT_RFE_CTRL_4_8822C,
	GPIO7_RFE_CTRL_2_8822C,
	GPIO7_SW_IO_8822C
};

static const struct halmac_gpio_pimux_list PINMUX_LIST_GPIO8_8822C[] = {
	GPIO8_WL_EXT_WOL_8822C,
	GPIO8_WL_LED_8822C,
	GPIO8_SOUT_8822C,
	GPIO8_SW_IO_8822C
};

static const struct halmac_gpio_pimux_list PINMUX_LIST_GPIO9_8822C[] = {
	GPIO9_DIS_WL_N_8822C,
	GPIO9_WL_EXT_WOL_8822C,
	GPIO9_USIN0_8822C,
	GPIO9_I2C_SD_8822C,
	GPIO9_LNAON_SEL_8822C,
	GPIO9_BT_RFE_CTRL_4_8822C,
	GPIO9_RFE_CTRL_2_8822C,
	GPIO9_SW_IO_8822C
};

static const struct halmac_gpio_pimux_list PINMUX_LIST_GPIO10_8822C[] = {
	GPIO10_WL_SDIO_INT_8822C,
	GPIO10_SW_IO_8822C
};

static const struct halmac_gpio_pimux_list PINMUX_LIST_GPIO11_8822C[] = {
	GPIO11_DIS_BT_N_8822C,
	GPIO11_USOUT0_8822C,
	GPIO11_RFE_CTRL_11_4_5_8822C,
	GPIO11_RFE_CTRL_11_6_7_8822C,
	GPIO11_SW_IO_8822C
};

static const struct halmac_gpio_pimux_list PINMUX_LIST_GPIO12_8822C[] = {
	GPIO12_USCTS0_8822C,
	GPIO12_I2C_CLK_8822C,
	GPIO12_ANT_SW_GPIO12_8822C,
	GPIO12_BT_RFE_CTRL_1_8822C,
	GPIO12_WL_RFE_CTRL_9_8822C,
	GPIO12_SW_IO_8822C
};

static const struct halmac_gpio_pimux_list PINMUX_LIST_GPIO13_8822C[] = {
	GPIO13_BT_WAKE_8822C,
	GPIO13_BT_ANT_SW_0_8822C,
	GPIO13_ANT_SW_GPIO13_8822C,
	GPIO13_BT_RFE_CTRL_1_8822C,
	GPIO13_WL_RFE_CTRL_9_8822C,
	GPIO13_RFE_CTRL_10_4_5_8822C,
	GPIO13_RFE_CTRL_10_6_7_8822C,
	GPIO13_SW_IO_8822C
};

static const struct halmac_gpio_pimux_list PINMUX_LIST_GPIO14_8822C[] = {
	GPIO14_UART_WAKE_8822C,
	GPIO14_BT_ANT_SW_1_8822C,
	GPIO14_RFE_CTRL_5_4_5_8822C,
	GPIO14_RFE_CTRL_5_6_7_8822C,
	GPIO14_SW_IO_8822C
};

static const struct halmac_gpio_pimux_list PINMUX_LIST_GPIO15_8822C[] = {
	GPIO15_EXT_XTAL_8822C,
	GPIO15_SW_IO_8822C
};

static enum halmac_ret_status
get_pinmux_list_8822c(struct halmac_adapter *adapter,
		      enum halmac_gpio_func gpio_func,
		      const struct halmac_gpio_pimux_list **list,
		      u32 *list_size, u32 *gpio_id);

static enum halmac_ret_status
chk_pinmux_valid_8822c(struct halmac_adapter *adapter,
		       enum halmac_gpio_func gpio_func);

static enum halmac_ret_status
pinmux_switch_8822c(struct halmac_adapter *adapter,
		    const struct halmac_gpio_pimux_list *list, u32 size,
		    u32 gpio_id, enum halmac_gpio_func gpio_func);

static enum halmac_ret_status
pinmux_record_8822c(struct halmac_adapter *adapter,
		    enum halmac_gpio_func gpio_func, u8 val);

/**
 * pinmux_set_func_8822c() -set gpio function
 * @adapter : the adapter of halmac
 * @gpio_func : gpio function
 * Author : LIN YONG-CHING
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
pinmux_set_func_8822c(struct halmac_adapter *adapter,
		      enum halmac_gpio_func gpio_func)
{
	u32 list_size;
	u32 gpio_id;
	enum halmac_ret_status status;
	const struct halmac_gpio_pimux_list *list = NULL;

	PLTFM_MSG_TRACE("[TRACE]%s ===>\n", __func__);
	PLTFM_MSG_TRACE("[TRACE]func name : %d\n", gpio_func);

	status = chk_pinmux_valid_8822c(adapter, gpio_func);
	if (status != HALMAC_RET_SUCCESS)
		return status;

	status = get_pinmux_list_8822c(adapter, gpio_func, &list, &list_size,
				       &gpio_id);
	if (status != HALMAC_RET_SUCCESS)
		return status;

	status = pinmux_switch_8822c(adapter, list, list_size, gpio_id,
				     gpio_func);
	if (status != HALMAC_RET_SUCCESS)
		return status;

	status = pinmux_record_8822c(adapter, gpio_func, 1);
	if (status != HALMAC_RET_SUCCESS)
		return status;

	PLTFM_MSG_TRACE("[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

/**
 * pinmux_free_func_8822c() -free locked gpio function
 * @adapter : the adapter of halmac
 * @gpio_func : gpio function
 * Author : LIN YONG-CHING
 * Return : enum halmac_ret_status
 * More details of status code can be found in prototype document
 */
enum halmac_ret_status
pinmux_free_func_8822c(struct halmac_adapter *adapter,
		       enum halmac_gpio_func gpio_func)
{
	struct halmac_pinmux_info *info = &adapter->pinmux_info;

	PLTFM_MSG_TRACE("[TRACE]%s ===>\n", __func__);

	switch (gpio_func) {
	case HALMAC_GPIO_FUNC_SW_IO_0:
		info->sw_io_0 = 0;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_1:
	case HALMAC_GPIO_FUNC_ANTSWB:
		info->sw_io_1 = 0;
		info->antswb = 0;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_2:
	case HALMAC_GPIO_FUNC_S1_TRSW:
		info->sw_io_2 = 0;
		info->s1_trsw = 0;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_3:
	case HALMAC_GPIO_FUNC_S0_TRSW:
		info->sw_io_3 = 0;
		info->s0_trsw = 0;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_4:
		info->sw_io_4 = 0;
		info->sdio_int = 0;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_5:
		info->sw_io_5 = 0;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_6:
	case HALMAC_GPIO_FUNC_S0_PAPE:
		info->sw_io_6 = 0;
		info->s0_pape = 0;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_7:
	case HALMAC_GPIO_FUNC_S0_TRSWB:
		info->sw_io_7 = 0;
		info->s0_trswb = 0;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_8:
	case HALMAC_GPIO_FUNC_WL_LED:
		info->sw_io_8 = 0;
		info->wl_led = 0;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_9:
		info->sw_io_9 = 0;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_10:
	case HALMAC_GPIO_FUNC_SDIO_INT:
		info->sw_io_10 = 0;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_11:
		info->sw_io_11 = 0;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_12:
		info->sw_io_12 = 0;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_13:
	case HALMAC_GPIO_FUNC_BT_DEV_WAKE1:
	case HALMAC_GPIO_FUNC_ANTSW:
		info->bt_dev_wake = 0;
		info->sw_io_13 = 0;
		info->antsw = 0;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_14:
	case HALMAC_GPIO_FUNC_BT_HOST_WAKE1:
		info->bt_host_wake = 0;
		info->sw_io_14 = 0;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_15:
		info->sw_io_15 = 0;
		break;
	case HALMAC_GPIO_FUNC_S1_PAPE:
	case HALMAC_GPIO_FUNC_S1_TRSWB:
		return HALMAC_RET_PINMUX_NOT_SUPPORT;
	default:
		return HALMAC_RET_SWITCH_CASE_ERROR;
	}

	PLTFM_MSG_TRACE("[TRACE]func : %X\n", gpio_func);
	PLTFM_MSG_TRACE("[TRACE]%s <===\n", __func__);

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
get_pinmux_list_8822c(struct halmac_adapter *adapter,
		      enum halmac_gpio_func gpio_func,
		      const struct halmac_gpio_pimux_list **list,
		      u32 *list_size, u32 *gpio_id)
{
	switch (gpio_func) {
	case HALMAC_GPIO_FUNC_SW_IO_0:
		*list = PINMUX_LIST_GPIO0_8822C;
		*list_size = ARRAY_SIZE(PINMUX_LIST_GPIO0_8822C);
		*gpio_id = HALMAC_GPIO0;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_1:
	case HALMAC_GPIO_FUNC_ANTSWB:
		*list = PINMUX_LIST_GPIO1_8822C;
		*list_size = ARRAY_SIZE(PINMUX_LIST_GPIO1_8822C);
		*gpio_id = HALMAC_GPIO1;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_2:
	case HALMAC_GPIO_FUNC_S1_TRSW:
		*list = PINMUX_LIST_GPIO2_8822C;
		*list_size = ARRAY_SIZE(PINMUX_LIST_GPIO2_8822C);
		*gpio_id = HALMAC_GPIO2;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_3:
	case HALMAC_GPIO_FUNC_S0_TRSW:
		*list = PINMUX_LIST_GPIO3_8822C;
		*list_size = ARRAY_SIZE(PINMUX_LIST_GPIO3_8822C);
		*gpio_id = HALMAC_GPIO3;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_4:
		*list = PINMUX_LIST_GPIO4_8822C;
		*list_size = ARRAY_SIZE(PINMUX_LIST_GPIO4_8822C);
		*gpio_id = HALMAC_GPIO4;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_5:
		*list = PINMUX_LIST_GPIO5_8822C;
		*list_size = ARRAY_SIZE(PINMUX_LIST_GPIO5_8822C);
		*gpio_id = HALMAC_GPIO5;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_6:
	case HALMAC_GPIO_FUNC_S0_PAPE:
		*list = PINMUX_LIST_GPIO6_8822C;
		*list_size = ARRAY_SIZE(PINMUX_LIST_GPIO6_8822C);
		*gpio_id = HALMAC_GPIO6;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_7:
	case HALMAC_GPIO_FUNC_S0_TRSWB:
		*list = PINMUX_LIST_GPIO7_8822C;
		*list_size = ARRAY_SIZE(PINMUX_LIST_GPIO7_8822C);
		*gpio_id = HALMAC_GPIO7;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_8:
	case HALMAC_GPIO_FUNC_WL_LED:
		*list = PINMUX_LIST_GPIO8_8822C;
		*list_size = ARRAY_SIZE(PINMUX_LIST_GPIO8_8822C);
		*gpio_id = HALMAC_GPIO8;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_9:
		*list = PINMUX_LIST_GPIO9_8822C;
		*list_size = ARRAY_SIZE(PINMUX_LIST_GPIO9_8822C);
		*gpio_id = HALMAC_GPIO9;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_10:
	case HALMAC_GPIO_FUNC_SDIO_INT:
		*list = PINMUX_LIST_GPIO10_8822C;
		*list_size = ARRAY_SIZE(PINMUX_LIST_GPIO10_8822C);
		*gpio_id = HALMAC_GPIO10;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_11:
		*list = PINMUX_LIST_GPIO11_8822C;
		*list_size = ARRAY_SIZE(PINMUX_LIST_GPIO11_8822C);
		*gpio_id = HALMAC_GPIO11;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_12:
		*list = PINMUX_LIST_GPIO12_8822C;
		*list_size = ARRAY_SIZE(PINMUX_LIST_GPIO12_8822C);
		*gpio_id = HALMAC_GPIO12;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_13:
	case HALMAC_GPIO_FUNC_BT_DEV_WAKE1:
	case HALMAC_GPIO_FUNC_ANTSW:
		*list = PINMUX_LIST_GPIO13_8822C;
		*list_size = ARRAY_SIZE(PINMUX_LIST_GPIO13_8822C);
		*gpio_id = HALMAC_GPIO13;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_14:
	case HALMAC_GPIO_FUNC_BT_HOST_WAKE1:
		*list = PINMUX_LIST_GPIO14_8822C;
		*list_size = ARRAY_SIZE(PINMUX_LIST_GPIO14_8822C);
		*gpio_id = HALMAC_GPIO14;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_15:
		*list = PINMUX_LIST_GPIO15_8822C;
		*list_size = ARRAY_SIZE(PINMUX_LIST_GPIO15_8822C);
		*gpio_id = HALMAC_GPIO15;
		break;
	case HALMAC_GPIO_FUNC_S1_PAPE:
	case HALMAC_GPIO_FUNC_S1_TRSWB:
		return HALMAC_RET_PINMUX_NOT_SUPPORT;
	default:
		return HALMAC_RET_SWITCH_CASE_ERROR;
	}

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
chk_pinmux_valid_8822c(struct halmac_adapter *adapter,
		       enum halmac_gpio_func gpio_func)
{
	struct halmac_pinmux_info *info = &adapter->pinmux_info;
	enum halmac_ret_status status = HALMAC_RET_SUCCESS;

	switch (gpio_func) {
	case HALMAC_GPIO_FUNC_SW_IO_0:
		if (info->sw_io_0 == 1)
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_1:
	case HALMAC_GPIO_FUNC_ANTSWB:
		if (info->sw_io_1 == 1 || info->antswb == 1)
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_2:
	case HALMAC_GPIO_FUNC_S1_TRSW:
		if (info->sw_io_2 == 1 || info->s1_trsw == 1)
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_3:
	case HALMAC_GPIO_FUNC_S0_TRSW:
		if (info->sw_io_3 == 1 || info->s0_trsw == 1)
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_4:
		if (info->sw_io_4 == 1)
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_5:
		if (info->sw_io_5 == 1)
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_6:
	case HALMAC_GPIO_FUNC_S0_PAPE:
		if (info->sw_io_6 == 1 || info->s0_pape == 1)
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_7:
	case HALMAC_GPIO_FUNC_S0_TRSWB:
		if (info->sw_io_7 == 1)
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_8:
		if (info->sw_io_8 == 1 || info->wl_led == 1)
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_WL_LED:
		if (info->sw_io_8 == 1 || info->wl_led == 1 ||
		    info->bt_dev_wake == 1 || info->bt_host_wake == 1)
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_9:
		if (info->sw_io_9 == 1)
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_10:
	case HALMAC_GPIO_FUNC_SDIO_INT:
		if (info->sw_io_10 == 1 || info->sdio_int == 1)
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_11:
		if (info->sw_io_11 == 1)
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_12:
		if (info->sw_io_12 == 1)
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_13:
	case HALMAC_GPIO_FUNC_ANTSW:
		if (info->sw_io_13 == 1 || info->bt_dev_wake == 1 ||
		    info->antsw == 1)
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_BT_DEV_WAKE1:
		if (info->sw_io_13 == 1 || info->bt_dev_wake == 1 ||
		    info->wl_led == 1 || info->antsw == 1)
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_14:
		if (info->sw_io_14 == 1 || info->bt_host_wake == 1)
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_BT_HOST_WAKE1:
		if (info->sw_io_14 == 1 || info->bt_host_wake == 1 ||
		    info->wl_led == 1)
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_15:
		if (info->sw_io_15 == 1)
			status = HALMAC_RET_PINMUX_USED;
		break;
	case HALMAC_GPIO_FUNC_S1_PAPE:
	case HALMAC_GPIO_FUNC_S1_TRSWB:
		return HALMAC_RET_PINMUX_NOT_SUPPORT;
	default:
		return HALMAC_RET_SWITCH_CASE_ERROR;
	}

	PLTFM_MSG_TRACE("[TRACE]chk_pinmux_valid func : %X status : %X\n",
			gpio_func, status);

	return status;
}

static enum halmac_ret_status

pinmux_switch_8822c(struct halmac_adapter *adapter,
		    const struct halmac_gpio_pimux_list *list, u32 size,
		    u32 gpio_id, enum halmac_gpio_func gpio_func)
{
	u32 i;
	u8 value8;
	u16 switch_func;
	const struct halmac_gpio_pimux_list *cur_list = list;
	enum halmac_gpio_cfg_state *state;
	struct halmac_api *api = (struct halmac_api *)adapter->halmac_api;

	state = &adapter->halmac_state.gpio_cfg_state;

	if (*state == HALMAC_GPIO_CFG_STATE_BUSY)
		return HALMAC_RET_BUSY_STATE;

	switch (gpio_func) {
	case HALMAC_GPIO_FUNC_WL_LED:
		switch_func = HALMAC_WL_LED;
		break;
	case HALMAC_GPIO_FUNC_SDIO_INT:
		switch_func = HALMAC_SDIO_INT;
		break;
	case HALMAC_GPIO_FUNC_BT_HOST_WAKE1:
	case HALMAC_GPIO_FUNC_BT_DEV_WAKE1:
		switch_func = HALMAC_GPIO13_14_WL_CTRL_EN;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_0:
	case HALMAC_GPIO_FUNC_SW_IO_1:
	case HALMAC_GPIO_FUNC_SW_IO_2:
	case HALMAC_GPIO_FUNC_SW_IO_3:
	case HALMAC_GPIO_FUNC_SW_IO_4:
	case HALMAC_GPIO_FUNC_SW_IO_5:
	case HALMAC_GPIO_FUNC_SW_IO_6:
	case HALMAC_GPIO_FUNC_SW_IO_7:
	case HALMAC_GPIO_FUNC_SW_IO_8:
	case HALMAC_GPIO_FUNC_SW_IO_9:
	case HALMAC_GPIO_FUNC_SW_IO_10:
	case HALMAC_GPIO_FUNC_SW_IO_11:
	case HALMAC_GPIO_FUNC_SW_IO_12:
	case HALMAC_GPIO_FUNC_SW_IO_13:
	case HALMAC_GPIO_FUNC_SW_IO_14:
	case HALMAC_GPIO_FUNC_SW_IO_15:
		switch_func = HALMAC_SW_IO;
		break;
	case HALMAC_GPIO_FUNC_S1_TRSW:
	case HALMAC_GPIO_FUNC_S1_PAPE:
		switch_func = HALMAC_WLPHY_RFE_CTRL2GPIO;
		break;
	case HALMAC_GPIO_FUNC_S0_TRSW:
	case HALMAC_GPIO_FUNC_S0_TRSWB:
		switch_func = HALMAC_WL_LNAON_SEL;
		break;
	case HALMAC_GPIO_FUNC_S0_PAPE:
		switch_func = HALMAC_WL_PAPE_SEL;
		break;
	case HALMAC_GPIO_FUNC_ANTSW:
	case HALMAC_GPIO_FUNC_ANTSWB:
		switch_func = HALMAC_WL_DPDT_SEL;
		break;
	default:
		return HALMAC_RET_SWITCH_CASE_ERROR;
	}

	for (i = 0; i < size; i++) {
		if (gpio_id != cur_list->id) {
			PLTFM_MSG_ERR("[ERR]offset:%X, value:%X, func:%X\n",
				      cur_list->offset, cur_list->value,
				      cur_list->func);
			PLTFM_MSG_ERR("[ERR]id1 : %X, id2 : %X\n",
				      gpio_id, cur_list->id);
			return HALMAC_RET_GET_PINMUX_ERR;
		}

		if (switch_func == cur_list->func)
			break;

		cur_list++;
	}

	if (i == size) {
		PLTFM_MSG_ERR("[ERR]gpio func error:%X %X\n",
			      gpio_id, cur_list->id);
		return HALMAC_RET_GET_PINMUX_ERR;
	}

	*state = HALMAC_GPIO_CFG_STATE_BUSY;

	cur_list = list;
	for (i = 0; i < size; i++) {
		value8 = HALMAC_REG_R8(cur_list->offset);
		value8 &= ~(cur_list->msk);

		if (switch_func == cur_list->func) {
			value8 |= (cur_list->value & cur_list->msk);
			HALMAC_REG_W8(cur_list->offset, value8);
			break;
		}

		value8 |= (~cur_list->value & cur_list->msk);
		HALMAC_REG_W8(cur_list->offset, value8);

		cur_list++;
	}

	switch (switch_func) {
	case HALMAC_WL_DPDT_SEL:
		if (gpio_id == HALMAC_GPIO1) {
			HALMAC_REG_W8_CLR(REG_GPIO_MUXCFG + 3, BIT(2));
		} else if (gpio_id == HALMAC_GPIO13) {
			HALMAC_REG_W8_CLR(REG_GPIO_MUXCFG + 3, BIT(3));
			HALMAC_REG_W8_SET(REG_GPIO_MUXCFG + 3, BIT(4));
		}
		HALMAC_REG_W8_SET(REG_LED_CFG + 3, BIT(0));
		break;
	case HALMAC_WL_PAPE_SEL:
		HALMAC_REG_W8_SET(REG_PAD_CTRL1 + 3, BIT(5));
		break;
	case HALMAC_WL_LNAON_SEL:
		if (gpio_id == HALMAC_GPIO1)
			HALMAC_REG_W8_CLR(REG_GPIO_MUXCFG + 3, BIT(7));
		HALMAC_REG_W8_SET(REG_PAD_CTRL1 + 3, BIT(4));
		break;
	case HALMAC_WLPHY_RFE_CTRL2GPIO:
		if (gpio_id == HALMAC_GPIO2)
			HALMAC_REG_W8_CLR(REG_GPIO_MUXCFG_2, BIT(0));
		break;
	default:
		break;
	}

	*state = HALMAC_GPIO_CFG_STATE_IDLE;

	return HALMAC_RET_SUCCESS;
}

static enum halmac_ret_status
pinmux_record_8822c(struct halmac_adapter *adapter,
		    enum halmac_gpio_func gpio_func, u8 val)
{
	switch (gpio_func) {
	case HALMAC_GPIO_FUNC_WL_LED:
		adapter->pinmux_info.wl_led = val;
		break;
	case HALMAC_GPIO_FUNC_SDIO_INT:
		adapter->pinmux_info.sdio_int = val;
		break;
	case HALMAC_GPIO_FUNC_BT_HOST_WAKE1:
		adapter->pinmux_info.bt_host_wake = val;
		break;
	case HALMAC_GPIO_FUNC_BT_DEV_WAKE1:
		adapter->pinmux_info.bt_dev_wake = val;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_0:
		adapter->pinmux_info.sw_io_0 = val;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_1:
		adapter->pinmux_info.sw_io_1 = val;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_2:
		adapter->pinmux_info.sw_io_2 = val;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_3:
		adapter->pinmux_info.sw_io_3 = val;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_4:
		adapter->pinmux_info.sw_io_4 = val;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_5:
		adapter->pinmux_info.sw_io_5 = val;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_6:
		adapter->pinmux_info.sw_io_6 = val;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_7:
		adapter->pinmux_info.sw_io_7 = val;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_8:
		adapter->pinmux_info.sw_io_8 = val;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_9:
		adapter->pinmux_info.sw_io_9 = val;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_10:
		adapter->pinmux_info.sw_io_10 = val;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_11:
		adapter->pinmux_info.sw_io_11 = val;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_12:
		adapter->pinmux_info.sw_io_12 = val;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_13:
		adapter->pinmux_info.sw_io_13 = val;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_14:
		adapter->pinmux_info.sw_io_14 = val;
		break;
	case HALMAC_GPIO_FUNC_SW_IO_15:
		adapter->pinmux_info.sw_io_15 = val;
		break;
	case HALMAC_GPIO_FUNC_S0_PAPE:
		adapter->pinmux_info.s0_pape = val;
		break;
	case HALMAC_GPIO_FUNC_S0_TRSW:
		adapter->pinmux_info.s0_trsw = val;
		break;
	case HALMAC_GPIO_FUNC_S1_TRSW:
		adapter->pinmux_info.s1_trsw = val;
		break;
	case HALMAC_GPIO_FUNC_S0_TRSWB:
		adapter->pinmux_info.s0_trswb = val;
		break;
	case HALMAC_GPIO_FUNC_ANTSW:
		adapter->pinmux_info.antsw = val;
		break;
	case HALMAC_GPIO_FUNC_ANTSWB:
		adapter->pinmux_info.antswb = val;
		break;
	default:
		return HALMAC_RET_GET_PINMUX_ERR;
	}

	return HALMAC_RET_SUCCESS;
}
#endif /* HALMAC_8822C_SUPPORT */
