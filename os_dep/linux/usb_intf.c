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
 *****************************************************************************/
#define _HCI_INTF_C_

#include <drv_types.h>
#include <hal_data.h>

#include <platform_ops.h>
#include <rtw_trx_usb.h>

#ifndef CONFIG_USB_HCI
#error "CONFIG_USB_HCI shall be on!\n"
#endif

#ifdef CONFIG_80211N_HT
extern int rtw_ht_enable;
extern int rtw_bw_mode;
extern int rtw_ampdu_enable;/* for enable tx_ampdu */
#endif

#ifdef CONFIG_GLOBAL_UI_PID
int ui_pid[3] = {0, 0, 0};
#endif


static int rtw_dev_suspend(struct usb_interface *intf, pm_message_t message);
static int rtw_dev_resume(struct usb_interface *intf);


static int rtw_dev_probe(struct usb_interface *pusb_intf, const struct usb_device_id *pdid);
static void rtw_dev_remove(struct usb_interface *pusb_intf);

static void rtw_dev_shutdown(struct device *dev)
{
	struct usb_interface *usb_intf = container_of(dev, struct usb_interface, dev);
	struct dvobj_priv *dvobj = NULL;
	_adapter *adapter = NULL;

	RTW_INFO("%s\n", __func__);

	if (usb_intf) {
		dvobj = usb_get_intfdata(usb_intf);
		if (dvobj) {
			adapter = dvobj_get_primary_adapter(dvobj);
			if (adapter) {
				if (!dev_is_surprise_removed(dvobj)) {
					#ifdef CONFIG_WOWLAN
					struct pwrctrl_priv *pwrctl = adapter_to_pwrctl(adapter);

					#ifdef CONFIG_GPIO_WAKEUP
					/*default wake up pin change to BT*/
					RTW_INFO("%s:default wake up pin change to BT\n", __FUNCTION__);
					rtw_hal_switch_gpio_wl_ctrl(adapter, pwrctl->wowlan_gpio_index, _FALSE);
					#endif /* CONFIG_GPIO_WAKEUP */

					if (pwrctl->wowlan_mode == _TRUE)
						RTW_PRINT("%s wowlan_mode ==_TRUE do not run rtk_hal_deinit()\n", __FUNCTION__);
					else
					#endif
					{
						#ifdef CONFIG_BT_COEXIST
						RTW_INFO("%s call halt notify\n", __FUNCTION__);
						rtw_btcoex_HaltNotify(adapter);
						#endif
						rtw_hw_stop(dvobj);
						rtk_hal_deinit(adapter);
						dev_set_surprise_removed(dvobj);
					}
				}
			}
			ATOMIC_SET(&dvobj->continual_io_error, MAX_CONTINUAL_IO_ERR + 1);
		}
	}
}

#if (LINUX_VERSION_CODE <= KERNEL_VERSION(2, 6, 23))
/* Some useful macros to use to create struct usb_device_id */
#define USB_DEVICE_ID_MATCH_VENDOR			 0x0001
#define USB_DEVICE_ID_MATCH_PRODUCT			 0x0002
#define USB_DEVICE_ID_MATCH_DEV_LO			 0x0004
#define USB_DEVICE_ID_MATCH_DEV_HI			 0x0008
#define USB_DEVICE_ID_MATCH_DEV_CLASS			 0x0010
#define USB_DEVICE_ID_MATCH_DEV_SUBCLASS		 0x0020
#define USB_DEVICE_ID_MATCH_DEV_PROTOCOL		 0x0040
#define USB_DEVICE_ID_MATCH_INT_CLASS			 0x0080
#define USB_DEVICE_ID_MATCH_INT_SUBCLASS		 0x0100
#define USB_DEVICE_ID_MATCH_INT_PROTOCOL		 0x0200
#define USB_DEVICE_ID_MATCH_INT_NUMBER		 0x0400


#define USB_DEVICE_ID_MATCH_INT_INFO \
	(USB_DEVICE_ID_MATCH_INT_CLASS | \
	 USB_DEVICE_ID_MATCH_INT_SUBCLASS | \
	 USB_DEVICE_ID_MATCH_INT_PROTOCOL)


#define USB_DEVICE_AND_INTERFACE_INFO(vend, prod, cl, sc, pr) \
	.match_flags = USB_DEVICE_ID_MATCH_INT_INFO \
		       | USB_DEVICE_ID_MATCH_DEVICE, \
		       .idVendor = (vend), \
				   .idProduct = (prod), \
						.bInterfaceClass = (cl), \
						.bInterfaceSubClass = (sc), \
						.bInterfaceProtocol = (pr)

/**
 * USB_VENDOR_AND_INTERFACE_INFO - describe a specific usb vendor with a class of usb interfaces
 * @vend: the 16 bit USB Vendor ID
 * @cl: bInterfaceClass value
 * @sc: bInterfaceSubClass value
 * @pr: bInterfaceProtocol value
 *
 * This macro is used to create a struct usb_device_id that matches a
 * specific vendor with a specific class of interfaces.
 *
 * This is especially useful when explicitly matching devices that have
 * vendor specific bDeviceClass values, but standards-compliant interfaces.
 */
#define USB_VENDOR_AND_INTERFACE_INFO(vend, cl, sc, pr) \
	.match_flags = USB_DEVICE_ID_MATCH_INT_INFO \
		       | USB_DEVICE_ID_MATCH_VENDOR, \
		       .idVendor = (vend), \
				   .bInterfaceClass = (cl), \
						   .bInterfaceSubClass = (sc), \
						   .bInterfaceProtocol = (pr)

/* ----------------------------------------------------------------------- */
#endif


#define USB_VENDER_ID_REALTEK		0x0BDA


/* DID_USB_v916_20130116 */
static struct usb_device_id rtw_usb_id_tbl[] = {
#ifdef CONFIG_RTL8188E
	/*=== Realtek demoboard ===*/
	{USB_DEVICE(USB_VENDER_ID_REALTEK, 0x8179), .driver_info = RTL8188E}, /* 8188EUS */
	{USB_DEVICE(USB_VENDER_ID_REALTEK, 0x0179), .driver_info = RTL8188E}, /* 8188ETV */
	/*=== Customer ID ===*/
	/****** 8188EUS ********/
	{USB_DEVICE(0x07B8, 0x8179), .driver_info = RTL8188E}, /* Abocom - Abocom */
#endif

#ifdef CONFIG_RTL8812A
	/*=== Realtek demoboard ===*/
	{USB_DEVICE(USB_VENDER_ID_REALTEK, 0x8812), .driver_info = RTL8812}, /* Default ID */
	{USB_DEVICE(USB_VENDER_ID_REALTEK, 0x881A), .driver_info = RTL8812}, /* Default ID */
	{USB_DEVICE(USB_VENDER_ID_REALTEK, 0x881B), .driver_info = RTL8812}, /* Default ID */
	{USB_DEVICE(USB_VENDER_ID_REALTEK, 0x881C), .driver_info = RTL8812}, /* Default ID */
	/*=== Customer ID ===*/
	{USB_DEVICE(0x050D, 0x1106), .driver_info = RTL8812}, /* Belkin - sercomm */
	{USB_DEVICE(0x2001, 0x330E), .driver_info = RTL8812}, /* D-Link - ALPHA */
	{USB_DEVICE(0x7392, 0xA822), .driver_info = RTL8812}, /* Edimax - Edimax */
	{USB_DEVICE(0x0DF6, 0x0074), .driver_info = RTL8812}, /* Sitecom - Edimax */
	{USB_DEVICE(0x04BB, 0x0952), .driver_info = RTL8812}, /* I-O DATA - Edimax */
	{USB_DEVICE(0x0789, 0x016E), .driver_info = RTL8812}, /* Logitec - Edimax */
	{USB_DEVICE(0x0409, 0x0408), .driver_info = RTL8812}, /* NEC - */
	{USB_DEVICE(0x0B05, 0x17D2), .driver_info = RTL8812}, /* ASUS - Edimax */
	{USB_DEVICE(0x0E66, 0x0022), .driver_info = RTL8812}, /* HAWKING - Edimax */
	{USB_DEVICE(0x0586, 0x3426), .driver_info = RTL8812}, /* ZyXEL - */
	{USB_DEVICE(0x2001, 0x3313), .driver_info = RTL8812}, /* D-Link - ALPHA */
	{USB_DEVICE(0x1058, 0x0632), .driver_info = RTL8812}, /* WD - Cybertan*/
	{USB_DEVICE(0x1740, 0x0100), .driver_info = RTL8812}, /* EnGenius - EnGenius */
	{USB_DEVICE(0x2019, 0xAB30), .driver_info = RTL8812}, /* Planex - Abocom */
	{USB_DEVICE(0x07B8, 0x8812), .driver_info = RTL8812}, /* Abocom - Abocom */
	{USB_DEVICE(0x2001, 0x3315), .driver_info = RTL8812}, /* D-Link - Cameo */
	{USB_DEVICE(0x2001, 0x3316), .driver_info = RTL8812}, /* D-Link - Cameo */
#endif

#ifdef CONFIG_RTL8821A
	/*=== Realtek demoboard ===*/
	{USB_DEVICE(USB_VENDER_ID_REALTEK, 0x0811), .driver_info = RTL8821}, /* Default ID */
	{USB_DEVICE(USB_VENDER_ID_REALTEK, 0x0821), .driver_info = RTL8821}, /* Default ID */
	{USB_DEVICE(USB_VENDER_ID_REALTEK, 0x8822), .driver_info = RTL8821}, /* Default ID */
	{USB_DEVICE(USB_VENDER_ID_REALTEK, 0xA811) , .driver_info = RTL8821},/* Default ID */
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0x0820, 0xff, 0xff, 0xff), .driver_info = RTL8821}, /* 8821AU */
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0x0823, 0xff, 0xff, 0xff), .driver_info = RTL8821}, /* 8821AU */
	/*=== Customer ID ===*/
	{USB_DEVICE(0x7392, 0xA811), .driver_info = RTL8821}, /* Edimax - Edimax */
	{USB_DEVICE(0x04BB, 0x0953), .driver_info = RTL8821}, /* I-O DATA - Edimax */
	{USB_DEVICE(0x2001, 0x3314), .driver_info = RTL8821}, /* D-Link - Cameo */
	{USB_DEVICE(0x2001, 0x3318), .driver_info = RTL8821}, /* D-Link - Cameo */
	{USB_DEVICE(0x0E66, 0x0023), .driver_info = RTL8821}, /* HAWKING - Edimax */
	{USB_DEVICE(0x056E, 0x400E) , .driver_info = RTL8821}, /* ELECOM -  ELECOM */
	{USB_DEVICE(0x056E, 0x400F) , .driver_info = RTL8821}, /* ELECOM -  ELECOM */
	{USB_DEVICE(0x20f4, 0x804b), .driver_info = RTL8821}, /* TRENDnet  */
#endif

#ifdef CONFIG_RTL8192E
	/*=== Realtek demoboard ===*/
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0x818B, 0xff, 0xff, 0xff), .driver_info = RTL8192E}, /* Default ID */
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0x818C, 0xff, 0xff, 0xff), .driver_info = RTL8192E}, /* Default ID */
#endif

#ifdef CONFIG_RTL8723B
	/* === Realtek demoboard === */
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xB720, 0xff, 0xff, 0xff), .driver_info = RTL8723B}, /* 8723BU 1*1 */
	/* {USB_DEVICE(USB_VENDER_ID_REALTEK, 0xB720),.driver_info = RTL8723B},  8723BU */
#endif

#ifdef CONFIG_RTL8703B
	/*=== Realtek demoboard ===*/
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xB703, 0xff, 0xff, 0xff), .driver_info = RTL8703B}, /* 8723CU 1*1 */
	/* {USB_DEVICE(USB_VENDER_ID_REALTEK, 0xB703), .driver_info = RTL723C}, */ /* 8723CU 1*1 */
#endif /* CONFIG_RTL8703B */

#ifdef CONFIG_RTL8814A

	{USB_DEVICE(USB_VENDER_ID_REALTEK, 0x8813), .driver_info = RTL8814A},
	{USB_DEVICE(0x2001, 0x331a), .driver_info = RTL8814A}, /* D-Link - D-Link */
	{USB_DEVICE(0x0b05, 0x1817), .driver_info = RTL8814A}, /* ASUS - ASUSTeK */
	{USB_DEVICE(0x056E, 0x400B), .driver_info = RTL8814A}, /* ELECOM - ELECOM */
	{USB_DEVICE(0x056E, 0x400D), .driver_info = RTL8814A}, /* ELECOM - ELECOM */
	{USB_DEVICE(0x7392, 0xA834), .driver_info = RTL8814A}, /* Edimax - Edimax */
#endif /* CONFIG_RTL8814A */

#ifdef CONFIG_RTL8188F
	/*=== Realtek demoboard ===*/
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xF179, 0xff, 0xff, 0xff), .driver_info = RTL8188F}, /* 8188FU 1*1 */
#endif

#ifdef CONFIG_RTL8188GTV
	/*=== Realtek demoboard ===*/
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0x018C, 0xff, 0xff, 0xff), .driver_info = RTL8188GTV}, /* 8188GTV 1*1 */
#endif

#ifdef CONFIG_RTL8822B
	/*=== Realtek demoboard ===*/
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xB82C, 0xff, 0xff, 0xff), .driver_info = RTL8822B}, /* Default ID for USB multi-function */
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xB812, 0xff, 0xff, 0xff), .driver_info = RTL8822B}, /* Default ID for USB Single-function, WiFi only */
	/*=== Customer ID ===*/
	{USB_DEVICE_AND_INTERFACE_INFO(0x13b1, 0x0043, 0xff, 0xff, 0xff), .driver_info = RTL8822B}, /* Alpha - Alpha*/
#endif /* CONFIG_RTL8822B */

#ifdef CONFIG_RTL8723D
	/*=== Realtek demoboard ===*/
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xD723, 0xff, 0xff, 0xff), .driver_info = RTL8723D}, /* 8723DU 1*1 */
#endif

#ifdef CONFIG_RTL8192F
	/*=== Realtek demoboard ===*/
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xF192, 0xff, 0xff, 0xff), .driver_info = RTL8192F}, /* 8192FU 2*2 */
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xA725, 0xff, 0xff, 0xff), .driver_info = RTL8192F}, /* 8725AU 2*2 */
#endif

#ifdef CONFIG_RTL8821C
	/*=== Realtek demoboard ===*/
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xb82b, 0xff, 0xff, 0xff), .driver_info = RTL8821C}, /* 8821CU */
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xb820, 0xff, 0xff, 0xff), .driver_info = RTL8821C}, /* 8821CU */
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xC821, 0xff, 0xff, 0xff), .driver_info = RTL8821C}, /* 8821CU */
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xC820, 0xff, 0xff, 0xff), .driver_info = RTL8821C}, /* 8821CU */
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xC82A, 0xff, 0xff, 0xff), .driver_info = RTL8821C}, /* 8821CU */
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xC82B, 0xff, 0xff, 0xff), .driver_info = RTL8821C}, /* 8821CU */
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xC811, 0xff, 0xff, 0xff), .driver_info = RTL8821C}, /* 8811CU */
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0x8811, 0xff, 0xff, 0xff), .driver_info = RTL8821C}, /* 8811CU */
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0x8731, 0xff, 0xff, 0xff), .driver_info = RTL8821C}, /* 8731AU */
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xC80C, 0xff, 0xff, 0xff), .driver_info = RTL8821C}, /* 8821CUH */
	/*=== Customer ID ===*/
#endif

#ifdef CONFIG_RTL8710B
	/*=== Realtek dongle ===*/
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xB711, 0xff, 0xff, 0xff), .driver_info = RTL8710B}, /* 8710B = 8188GU 1*1 */
#endif

#ifdef CONFIG_RTL8822C
	/*=== Realtek demoboard ===*/
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xC82C, 0xff, 0xff, 0xff), .driver_info = RTL8822C}, /* Default ID for USB multi-function */
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xC82E, 0xff, 0xff, 0xff), .driver_info = RTL8822C}, /* Default ID for USB multi-function */
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xC812, 0xff, 0xff, 0xff), .driver_info = RTL8822C}, /* Default ID for USB Single-function, WiFi only */
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xD820, 0xff, 0xff, 0xff), .driver_info = RTL8822C}, /* 21D USB multi-fuction*/
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xD82B, 0xff, 0xff, 0xff), .driver_info = RTL8822C}, /* 21D USB Single-fuction, WiFi only*/
	/*=== Customer ID ===*/
	{USB_DEVICE_AND_INTERFACE_INFO(0x13b1, 0x0043, 0xff, 0xff, 0xff), .driver_info = RTL8822C}, /* Alpha - Alpha*/
#endif /* CONFIG_RTL8822C */

#ifdef CONFIG_RTL8814B
	/*=== Realtek demoboard ===*/
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xB814, 0xff, 0xff, 0xff), .driver_info = RTL8814B}, /* Default ID for USB multi-function */
#endif /* CONFIG_RTL8814B */
#ifdef CONFIG_RTL8723F
	/*=== Realtek IC ===*/
	{USB_DEVICE_AND_INTERFACE_INFO(USB_VENDER_ID_REALTEK, 0xB733, 0xff, 0xff, 0xff), .driver_info = RTL8723F}, 
#endif

	{}	/* Terminating entry */
};

MODULE_DEVICE_TABLE(usb, rtw_usb_id_tbl);

int const rtw_usb_id_len = sizeof(rtw_usb_id_tbl) / sizeof(struct usb_device_id);

static struct specific_device_id specific_device_id_tbl[] = {
	{.idVendor = USB_VENDER_ID_REALTEK, .idProduct = 0x8177, .flags = SPEC_DEV_ID_DISABLE_HT}, /* 8188cu 1*1 dongole, (b/g mode only) */
	{.idVendor = USB_VENDER_ID_REALTEK, .idProduct = 0x817E, .flags = SPEC_DEV_ID_DISABLE_HT}, /* 8188CE-VAU USB minCard (b/g mode only) */
	{.idVendor = 0x0b05, .idProduct = 0x1791, .flags = SPEC_DEV_ID_DISABLE_HT},
	{.idVendor = 0x13D3, .idProduct = 0x3311, .flags = SPEC_DEV_ID_DISABLE_HT},
	{.idVendor = 0x13D3, .idProduct = 0x3359, .flags = SPEC_DEV_ID_DISABLE_HT}, /* Russian customer -Azwave (8188CE-VAU  g mode) */
#ifdef RTK_DMP_PLATFORM
	{.idVendor = USB_VENDER_ID_REALTEK, .idProduct = 0x8111, .flags = SPEC_DEV_ID_ASSIGN_IFNAME}, /* Realtek 5G dongle for WiFi Display */
	{.idVendor = 0x2019, .idProduct = 0xAB2D, .flags = SPEC_DEV_ID_ASSIGN_IFNAME}, /* PCI-Abocom 5G dongle for WiFi Display */
#endif /* RTK_DMP_PLATFORM */
	{}
};

struct rtw_usb_drv {
	struct usb_driver usbdrv;
	int drv_registered;
};

struct rtw_usb_drv usb_drv = {
	.usbdrv.name = (char *)DRV_NAME,
	.usbdrv.probe = rtw_dev_probe,
	.usbdrv.disconnect = rtw_dev_remove,
	.usbdrv.id_table = rtw_usb_id_tbl,
	.usbdrv.suspend =  rtw_dev_suspend,
	.usbdrv.resume = rtw_dev_resume,
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 22))
	.usbdrv.reset_resume   = rtw_dev_resume,
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 19))
	.usbdrv.drvwrap.driver.shutdown = rtw_dev_shutdown,
#else
	.usbdrv.driver.shutdown = rtw_dev_shutdown,
#endif
};

static inline int RT_usb_endpoint_dir_in(const struct usb_endpoint_descriptor *epd)
{
	return (epd->bEndpointAddress & USB_ENDPOINT_DIR_MASK) == USB_DIR_IN;
}

static inline int RT_usb_endpoint_dir_out(const struct usb_endpoint_descriptor *epd)
{
	return (epd->bEndpointAddress & USB_ENDPOINT_DIR_MASK) == USB_DIR_OUT;
}

static inline int RT_usb_endpoint_xfer_int(const struct usb_endpoint_descriptor *epd)
{
	return (epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_INT;
}

static inline int RT_usb_endpoint_xfer_bulk(const struct usb_endpoint_descriptor *epd)
{
	return (epd->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK) == USB_ENDPOINT_XFER_BULK;
}

static inline int RT_usb_endpoint_is_bulk_in(const struct usb_endpoint_descriptor *epd)
{
	return RT_usb_endpoint_xfer_bulk(epd) && RT_usb_endpoint_dir_in(epd);
}

static inline int RT_usb_endpoint_is_bulk_out(const struct usb_endpoint_descriptor *epd)
{
	return RT_usb_endpoint_xfer_bulk(epd) && RT_usb_endpoint_dir_out(epd);
}

static inline int RT_usb_endpoint_is_int_in(const struct usb_endpoint_descriptor *epd)
{
	return RT_usb_endpoint_xfer_int(epd) && RT_usb_endpoint_dir_in(epd);
}

static inline int RT_usb_endpoint_num(const struct usb_endpoint_descriptor *epd)
{
	return epd->bEndpointAddress & USB_ENDPOINT_NUMBER_MASK;
}

static u8 rtw_init_intf_priv(struct dvobj_priv *dvobj)
{
	u8 rst = _SUCCESS;
	PUSB_DATA pusb_data = dvobj_to_usb(dvobj);

#ifdef CONFIG_USB_VENDOR_REQ_MUTEX
	_rtw_mutex_init(&pusb_data->usb_vendor_req_mutex);
#endif


#ifdef CONFIG_USB_VENDOR_REQ_BUFFER_PREALLOC
	pusb_data->usb_alloc_vendor_req_buf = rtw_zmalloc(MAX_USB_IO_CTL_SIZE);
	if (pusb_data->usb_alloc_vendor_req_buf == NULL) {
		RTW_INFO("alloc usb_vendor_req_buf failed... /n");
		rst = _FAIL;
		goto exit;
	}
	pusb_data->usb_vendor_req_buf =
		(u8 *)N_BYTE_ALIGMENT((SIZE_PTR)(pusb_data->usb_alloc_vendor_req_buf), ALIGNMENT_UNIT);
exit:
#endif

	return rst;

}

static u8 rtw_deinit_intf_priv(struct dvobj_priv *dvobj)
{
	u8 rst = _SUCCESS;
	PUSB_DATA pusb_data = dvobj_to_usb(dvobj);

#ifdef CONFIG_USB_VENDOR_REQ_BUFFER_PREALLOC
	if (pusb_data->usb_vendor_req_buf)
		rtw_mfree(pusb_data->usb_alloc_vendor_req_buf, MAX_USB_IO_CTL_SIZE);
#endif

#ifdef CONFIG_USB_VENDOR_REQ_MUTEX
	_rtw_mutex_free(&pusb_data->usb_vendor_req_mutex);
#endif

	return rst;
}

static unsigned int rtw_endpoint_max_bpi(struct usb_device *dev,
					 struct usb_host_endpoint *ep)
{
	u16 psize;
	u16 mult = 1;
	int max_size_1 = 0, max_size_2 = 0;

	switch (dev->speed) {
	case USB_SPEED_SUPER:
#if ((LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 20) && LINUX_VERSION_CODE < KERNEL_VERSION(4, 5, 0)) ||\
		LINUX_VERSION_CODE >= KERNEL_VERSION(4, 6, 0))
	case USB_SPEED_SUPER_PLUS:
		max_size_1 = le16_to_cpu(ep->ss_ep_comp.wBytesPerInterval);
		max_size_2 = usb_endpoint_maxp(&ep->desc);
#endif
		break;
	case USB_SPEED_HIGH:
		psize = usb_endpoint_maxp(&ep->desc);
		#if (LINUX_VERSION_CODE >= KERNEL_VERSION(4, 9, 71))
		mult = usb_endpoint_maxp_mult(&ep->desc);
		max_size_1 = psize * mult;
		#endif
		max_size_2 = usb_endpoint_maxp(&ep->desc);
		break;
	case USB_SPEED_WIRELESS:
		max_size_1 = max_size_2 = usb_endpoint_maxp(&ep->desc);
		break;
	default:
		max_size_1 = max_size_2 = usb_endpoint_maxp(&ep->desc);
		break;

	}
	RTW_INFO("USB EP MAX_PKT_SZ:%d-%d\n",max_size_1, max_size_2);
	return max_size_1;
}

static void rtw_decide_chip_type_by_usb_info(struct dvobj_priv *pdvobjpriv, const struct usb_device_id *pdid)
{
	pdvobjpriv->chip_type = pdid->driver_info;

#ifdef CONFIG_RTL8188E
	if (pdvobjpriv->chip_type == RTL8188E)
		rtl8188eu_set_hw_type(pdvobjpriv);
#endif

#if defined(CONFIG_RTL8812A) || defined(CONFIG_RTL8821A)
	if (pdvobjpriv->chip_type == RTL8812 || pdvobjpriv->chip_type == RTL8821)
		rtl8812au_set_hw_type(pdvobjpriv);
#endif

#ifdef CONFIG_RTL8192E
	if (pdvobjpriv->chip_type == RTL8192E)
		rtl8192eu_set_hw_type(pdvobjpriv);
#endif

#ifdef CONFIG_RTL8723B
	if (pdvobjpriv->chip_type == RTL8723B)
		rtl8723bu_set_hw_type(pdvobjpriv);
#endif

#ifdef CONFIG_RTL8814A
	if (pdvobjpriv->chip_type == RTL8814A)
		rtl8814au_set_hw_type(pdvobjpriv);
#endif /* CONFIG_RTL8814A */

#ifdef CONFIG_RTL8188F
	if (pdvobjpriv->chip_type == RTL8188F)
		rtl8188fu_set_hw_type(pdvobjpriv);
#endif

#ifdef CONFIG_RTL8188GTV
	if (pdvobjpriv->chip_type == RTL8188GTV)
		rtl8188gtvu_set_hw_type(pdvobjpriv);
#endif

#ifdef CONFIG_RTL8703B
	if (pdvobjpriv->chip_type == RTL8703B)
		rtl8703bu_set_hw_type(pdvobjpriv);
#endif /* CONFIG_RTL8703B */

#ifdef CONFIG_RTL8822B
	if (pdvobjpriv->chip_type == RTL8822B)
		rtl8822bu_set_hw_type(pdvobjpriv);
#endif /* CONFIG_RTL8822B */

#ifdef CONFIG_RTL8723D
	if (pdvobjpriv->chip_type == RTL8723D)
		rtl8723du_set_hw_type(pdvobjpriv);
#endif /* CONFIG_RTL8723D */

#ifdef CONFIG_RTL8821C
	if (pdvobjpriv->chip_type == RTL8821C)
		rtl8821cu_set_hw_type(pdvobjpriv);
#endif /* CONFIG_RTL8821C */

#ifdef CONFIG_RTL8710B
	if (pdvobjpriv->chip_type == RTL8710B)
		rtl8710bu_set_hw_type(pdvobjpriv);
#endif /* CONFIG_RTL8710B */

#ifdef CONFIG_RTL8192F
	if (pdvobjpriv->chip_type == RTL8192F)
		rtl8192fu_set_hw_type(pdvobjpriv);
#endif /* CONFIG_RTL8192F */

#ifdef CONFIG_RTL8822C
	if (pdvobjpriv->chip_type == RTL8822C)
		rtl8822cu_set_hw_type(pdvobjpriv);
#endif /* CONFIG_RTL8822C */

#ifdef CONFIG_RTL8814B
	if (pdvobjpriv->chip_type == RTL8814B)
		rtl8814bu_set_hw_type(pdvobjpriv);
#endif /* CONFIG_RTL8814B */

#ifdef CONFIG_RTL8723F
	if (pdvobjpriv->chip_type == RTL8723F)
		rtl8723fu_set_hw_type(pdvobjpriv);
#endif /* CONFIG_RTL8723F */
}

static struct dvobj_priv *usb_dvobj_init(struct usb_interface *usb_intf,
					 const struct usb_device_id *pdid)
{
	int	i;
	int	status = _FAIL;
	struct dvobj_priv *dvobj;
	struct usb_device_descriptor	*pdev_desc;
	struct usb_host_config			*phost_conf;
	struct usb_config_descriptor		*pconf_desc;
	struct usb_host_interface		*phost_iface;
	struct usb_interface_descriptor	*piface_desc;
	struct usb_host_endpoint		*phost_endp;
	struct usb_endpoint_descriptor	*pendp_desc;
	struct usb_device				*pusbd;
	PUSB_DATA pusb_data;

	dvobj = devobj_init();
	if (dvobj == NULL)
		goto exit;

	pusb_data = dvobj_to_usb(dvobj);

	pusb_data->pusbintf = usb_intf ;
	pusbd = pusb_data->pusbdev = interface_to_usbdev(usb_intf);
	usb_set_intfdata(usb_intf, dvobj);

	pusb_data->RtNumInPipes = 0;
	pusb_data->RtNumOutPipes = 0;

	pdev_desc = &pusbd->descriptor;

#if 0
	RTW_INFO("\n8712_usb_device_descriptor:\n");
	RTW_INFO("bLength=%x\n", pdev_desc->bLength);
	RTW_INFO("bDescriptorType=%x\n", pdev_desc->bDescriptorType);
	RTW_INFO("bcdUSB=%x\n", pdev_desc->bcdUSB);
	RTW_INFO("bDeviceClass=%x\n", pdev_desc->bDeviceClass);
	RTW_INFO("bDeviceSubClass=%x\n", pdev_desc->bDeviceSubClass);
	RTW_INFO("bDeviceProtocol=%x\n", pdev_desc->bDeviceProtocol);
	RTW_INFO("bMaxPacketSize0=%x\n", pdev_desc->bMaxPacketSize0);
	RTW_INFO("idVendor=%x\n", pdev_desc->idVendor);
	RTW_INFO("idProduct=%x\n", pdev_desc->idProduct);
	RTW_INFO("bcdDevice=%x\n", pdev_desc->bcdDevice);
	RTW_INFO("iManufacturer=%x\n", pdev_desc->iManufacturer);
	RTW_INFO("iProduct=%x\n", pdev_desc->iProduct);
	RTW_INFO("iSerialNumber=%x\n", pdev_desc->iSerialNumber);
	RTW_INFO("bNumConfigurations=%x\n", pdev_desc->bNumConfigurations);
#endif

	phost_conf = pusbd->actconfig;
	pconf_desc = &phost_conf->desc;

#if 0
	RTW_INFO("\n[USB] configuration_descriptor:\n");
	RTW_INFO("bNumInterfaces=%x\n", pconf_desc->bNumInterfaces);
	RTW_INFO("bLength=%x\n", pconf_desc->bLength);
	RTW_INFO("bDescriptorType=%x\n", pconf_desc->bDescriptorType);
	RTW_INFO("wTotalLength=%x\n", pconf_desc->wTotalLength);
	RTW_INFO("bConfigurationValue=%x\n", pconf_desc->bConfigurationValue);
	RTW_INFO("iConfiguration=%x\n", pconf_desc->iConfiguration);
	RTW_INFO("bmAttributes=%x\n", pconf_desc->bmAttributes);
	RTW_INFO("bMaxPower=%x\n", pconf_desc->bMaxPower);
#endif

	phost_iface = &usb_intf->altsetting[0];
	piface_desc = &phost_iface->desc;
#if 0
	RTW_INFO("\n[USB] usb_interface_descriptor:\n");
	RTW_INFO("bInterfaceNumber=%x\n", piface_desc->bInterfaceNumber);
	RTW_INFO("bAlternateSetting=%x\n", piface_desc->bAlternateSetting);
	RTW_INFO("bLength=%x\n", piface_desc->bLength);
	RTW_INFO("bDescriptorType=%x\n", piface_desc->bDescriptorType);
	RTW_INFO("bNumEndpoints=%x\n", piface_desc->bNumEndpoints);
	RTW_INFO("bInterfaceClass=%x\n", piface_desc->bInterfaceClass);
	RTW_INFO("bInterfaceSubClass=%x\n", piface_desc->bInterfaceSubClass);
	RTW_INFO("bInterfaceProtocol=%x\n", piface_desc->bInterfaceProtocol);
	RTW_INFO("iInterface=%x\n", piface_desc->iInterface);
#endif

	pusb_data->nr_endpoint = piface_desc->bNumEndpoints;
	if (pusb_data->nr_endpoint > MAX_ENDPOINT_NUM) {
		RTW_ERR("USB EP_Number : %d > RT DEF-MAX_EP_NUM :%d\n",
				pusb_data->nr_endpoint, MAX_ENDPOINT_NUM);
		rtw_warn_on(1);
	}

	/* RTW_INFO("\ndump usb_endpoint_descriptor:\n"); */

	for (i = 0; i < pusb_data->nr_endpoint; i++) {
		phost_endp = phost_iface->endpoint + i;
		if (phost_endp) {
			pendp_desc = &phost_endp->desc;

			RTW_INFO("\nusb_endpoint_descriptor(%d):\n", i);
			RTW_INFO("bLength=%x\n", pendp_desc->bLength);
			RTW_INFO("bDescriptorType=%x\n", pendp_desc->bDescriptorType);
			RTW_INFO("bEndpointAddress=%x\n", pendp_desc->bEndpointAddress);
			/* RTW_INFO("bmAttributes=%x\n",pendp_desc->bmAttributes); */
			RTW_INFO("wMaxPacketSize=%d\n", le16_to_cpu(pendp_desc->wMaxPacketSize));
			RTW_INFO("bInterval=%x\n", pendp_desc->bInterval);
			/* RTW_INFO("bRefresh=%x\n",pendp_desc->bRefresh); */
			/* RTW_INFO("bSynchAddress=%x\n",pendp_desc->bSynchAddress); */

			if (RT_usb_endpoint_is_bulk_in(pendp_desc)) {
				RTW_INFO("RT_usb_endpoint_is_bulk_in = %x\n", RT_usb_endpoint_num(pendp_desc));
				pusb_data->RtInPipe[pusb_data->RtNumInPipes] = RT_usb_endpoint_num(pendp_desc);
				pusb_data->inpipe_type[pusb_data->RtNumInPipes] = REALTEK_USB_BULK_IN_EP_IDX;
				pusb_data->RtNumInPipes++;
				RTW_INFO("USB#%d bulkin size:%d", pusb_data->RtNumInPipes,
					rtw_endpoint_max_bpi(pusbd, phost_endp));
			} else if (RT_usb_endpoint_is_int_in(pendp_desc)) {
				RTW_INFO("RT_usb_endpoint_is_int_in = %x, Interval = %x\n", RT_usb_endpoint_num(pendp_desc), pendp_desc->bInterval);
				pusb_data->RtInPipe[pusb_data->RtNumInPipes] = RT_usb_endpoint_num(pendp_desc);
				pusb_data->inpipe_type[pusb_data->RtNumInPipes] = REALTEK_USB_IN_INT_EP_IDX;
				pusb_data->RtNumInPipes++;
			} else if (RT_usb_endpoint_is_bulk_out(pendp_desc)) {
				RTW_INFO("RT_usb_endpoint_is_bulk_out = %x\n", RT_usb_endpoint_num(pendp_desc));
				pusb_data->RtOutPipe[pusb_data->RtNumOutPipes] = RT_usb_endpoint_num(pendp_desc);
				RTW_INFO("USB#%d bulkout size:%d", pusb_data->RtNumOutPipes,
					rtw_endpoint_max_bpi(pusbd, phost_endp));
				pusb_data->RtNumOutPipes++;
			}
			/*pusb_data->ep_num[i] = RT_usb_endpoint_num(pendp_desc);*/
		}
	}

	RTW_INFO("nr_endpoint=%d, in_num=%d, out_num=%d\n\n",
		pusb_data->nr_endpoint, pusb_data->RtNumInPipes, pusb_data->RtNumOutPipes);

	switch (pusbd->speed) {
	case USB_SPEED_LOW:
	case USB_SPEED_FULL:
		RTW_INFO("USB_SPEED_FULL\n");
		pusb_data->usb_speed = RTW_USB_SPEED_FULL;/*U2- 1.1 - 1.5MBs*/
		pusb_data->usb_bulkout_size = USB_FULL_SPEED_BULK_SIZE;
		break;
	case USB_SPEED_HIGH:
		RTW_INFO("USB_SPEED_HIGH\n");
		pusb_data->usb_speed = RTW_USB_SPEED_HIGH;/*U2- 2.1 - 60MBs*/
		pusb_data->usb_bulkout_size = USB_HIGH_SPEED_BULK_SIZE;
		break;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 31))
	case USB_SPEED_SUPER:
		RTW_INFO("USB_SPEED_SUPER\n");
		pusb_data->usb_speed = RTW_USB_SPEED_SUPER;/*U3- 3.0 - 640MBs*/
		pusb_data->usb_bulkout_size = USB_SUPER_SPEED_BULK_SIZE;
		break;
#if ((LINUX_VERSION_CODE >= KERNEL_VERSION(4, 4, 20) && LINUX_VERSION_CODE < KERNEL_VERSION(4, 5, 0)) ||\
		LINUX_VERSION_CODE >= KERNEL_VERSION(4, 6, 0))
	case USB_SPEED_SUPER_PLUS:
		RTW_INFO("USB_SPEED_SUPER_PLUS\n");
		pusb_data->usb_speed = RTW_USB_SPEED_SUPER_10G;/*U3- 3.1 - 1280MBs*/
		pusb_data->usb_bulkout_size = USB_SUPER_SPEED_BULK_SIZE;
		break;
#endif
#endif
	default:
		RTW_INFO("USB_SPEED_UNKNOWN(%d)\n", pusbd->speed);
		pusb_data->usb_speed = RTW_USB_SPEED_UNKNOWN;
		break;
	}

	if (pusb_data->usb_speed == RTW_USB_SPEED_UNKNOWN) {
		RTW_INFO("UNKNOWN USB SPEED MODE, ERROR !!!\n");
		goto free_dvobj;
	}

	if (rtw_init_intf_priv(dvobj) == _FAIL) {
		goto free_dvobj;
	}

	/*step 1-1., decide the chip_type via driver_info*/
	dvobj->interface_type = RTW_HCI_USB;
	// NEO : TODO : take off hardware type
	rtw_decide_chip_type_by_usb_info(dvobj, pdid);
	dvobj->ic_id = pdid->driver_info;
	dvobj->intf_ops = &usb_ops;

	/* .3 misc */
	rtw_reset_continual_io_error(dvobj);

	usb_get_dev(pusbd);

	status = _SUCCESS;

free_dvobj:
	if (status != _SUCCESS && dvobj) {
		usb_set_intfdata(usb_intf, NULL);
		devobj_deinit(dvobj);
		dvobj = NULL;
	}
exit:
	return dvobj;
}

static void usb_dvobj_deinit(struct usb_interface *usb_intf)
{
	struct dvobj_priv *dvobj = usb_get_intfdata(usb_intf);


	usb_set_intfdata(usb_intf, NULL);
	if (dvobj) {
		rtw_deinit_intf_priv(dvobj);
		devobj_deinit(dvobj);
	}

	/* RTW_INFO("%s %d\n", __func__, ATOMIC_READ(&usb_intf->dev.kobj.kref.refcount)); */
	usb_put_dev(interface_to_usbdev(usb_intf));

}

static int usb_reprobe_switch_usb_mode(_adapter *adapter)
{
	struct registry_priv *registry_par = &adapter->registrypriv;
	struct dvobj_priv *dvobj = adapter_to_dvobj(adapter);
	PUSB_DATA pusb_data = dvobj_to_usb(dvobj);
	HAL_DATA_TYPE *pHalData = GET_HAL_DATA(adapter);
	u8 ret = _FALSE;

	RTW_INFO("%s NEO : changed \n", __func__);
	/* efuse not allow driver to switch usb mode */
	if (pHalData->EEPROMUsbSwitch == _FALSE)
		goto exit;

	/* registry not allow driver to switch usb mode */
	if (registry_par->switch_usb_mode == 0)
		goto exit;
	else if (registry_par->switch_usb_mode == 1 && pusb_data->usb_speed < RTW_USB_SPEED_SUPER)
		ret = rtw_phl_force_usb_switch(dvobj->phl, RTW_USB_SPEED_SUPER);
	else if (registry_par->switch_usb_mode == 2 && pusb_data->usb_speed >= RTW_USB_SPEED_SUPER)
		ret = rtw_phl_force_usb_switch(dvobj->phl, RTW_USB_SPEED_HIGH);

exit:
	return ret;
}

u8 rtw_set_hal_ops(_adapter *padapter)
{
	/* alloc memory for HAL DATA */
	if (rtw_hal_data_init(padapter) == _FAIL)
		return _FAIL;

	if (rtw_get_chip_type(padapter) == RTL8822C)
		rtl8822cu_set_hal_ops(padapter);

	if (_FAIL == rtw_hal_ops_check(padapter))
		return _FAIL;

	if (hal_spec_init(padapter) == _FAIL)
		return _FAIL;

	return _SUCCESS;
}

static void usb_intf_start(_adapter *padapter)
{
	PHAL_DATA_TYPE hal = GET_HAL_DATA(padapter);

	rtw_hal_inirp_init(padapter);
	hal->usb_intf_start = _TRUE;


}

static void usb_intf_stop(_adapter *padapter)
{
	PHAL_DATA_TYPE hal = GET_HAL_DATA(padapter);

	/* disabel_hw_interrupt */
	if (!rtw_is_surprise_removed(padapter)) {
		/* device still exists, so driver can do i/o operation */
		/* TODO: */
	}

	/* cancel in irp */
	rtw_hal_inirp_deinit(padapter);

	/* cancel out irp */
	rtw_write_port_cancel(padapter);

	/* todo:cancel other irps */

	hal->usb_intf_start = _FALSE;

}

static void process_spec_devid(const struct usb_device_id *pdid)
{
	u16 vid, pid;
	u32 flags;
	int i;
	int num = sizeof(specific_device_id_tbl) / sizeof(struct specific_device_id);

	for (i = 0; i < num; i++) {
		vid = specific_device_id_tbl[i].idVendor;
		pid = specific_device_id_tbl[i].idProduct;
		flags = specific_device_id_tbl[i].flags;

#ifdef CONFIG_80211N_HT
		if ((pdid->idVendor == vid) && (pdid->idProduct == pid) && (flags & SPEC_DEV_ID_DISABLE_HT)) {
			rtw_ht_enable = 0;
			rtw_bw_mode = 0;
			rtw_ampdu_enable = 0;
		}
#endif

#ifdef RTK_DMP_PLATFORM
		/* Change the ifname to wlan10 when PC side WFD dongle plugin on DMP platform. */
		/* It is used to distinguish between normal and PC-side wifi dongle/module. */
		if ((pdid->idVendor == vid) && (pdid->idProduct == pid) && (flags & SPEC_DEV_ID_ASSIGN_IFNAME)) {
			extern char *ifname;
			strncpy(ifname, "wlan10", 6);
			/* RTW_INFO("%s()-%d: ifname=%s, vid=%04X, pid=%04X\n", __FUNCTION__, __LINE__, ifname, vid, pid); */
		}
#endif /* RTK_DMP_PLATFORM */

	}
}

#ifdef SUPPORT_HW_RFOFF_DETECTED
int rtw_hw_suspend(_adapter *padapter)
{
	struct pwrctrl_priv *pwrpriv;
	struct usb_interface *pusb_intf;
	struct net_device *pnetdev;

	if (NULL == padapter)
		goto error_exit;

	if ((_FALSE == padapter->netif_up) || RTW_CANNOT_RUN(adapter_to_dvobj(padapter))) {
		RTW_INFO("padapter->netif_up=%d bDriverStopped=%s bSurpriseRemoved = %s\n"
			 , padapter->netif_up
			 , rtw_is_drv_stopped(padapter) ? "True" : "False"
			, rtw_is_surprise_removed(padapter) ? "True" : "False");
		goto error_exit;
	}

	pwrpriv = adapter_to_pwrctl(padapter);
	pusb_intf = adapter_to_dvobj(padapter)->pusbintf;
	pnetdev = padapter->pnetdev;

	LeaveAllPowerSaveMode(padapter);

	RTW_INFO("==> rtw_hw_suspend\n");
	_enter_pwrlock(&pwrpriv->lock);
	pwrpriv->bips_processing = _TRUE;
	/* padapter->net_closed = _TRUE; */
	/* s1. */
	if (pnetdev) {
		rtw_netif_carrier_off(pnetdev);
		rtw_netif_stop_queue(pnetdev);
	}

	/* s2. */
	rtw_disassoc_cmd(padapter, 500, RTW_CMDF_DIRECTLY);

	/* s2-2.  indicate disconnect to os */
	/* rtw_indicate_disconnect(padapter); */
	{
		struct	mlme_priv *pmlmepriv = &padapter->mlmepriv;
		if (check_fwstate(pmlmepriv, WIFI_ASOC_STATE)) {
			_clr_fwstate_(pmlmepriv, WIFI_ASOC_STATE);
			rtw_led_control(padapter, LED_CTL_NO_LINK);

			rtw_os_indicate_disconnect(padapter, 0, _FALSE);

#ifdef CONFIG_LPS
			/* donnot enqueue cmd */
			rtw_lps_ctrl_wk_cmd(padapter, LPS_CTRL_DISCONNECT, RTW_CMDF_DIRECTLY);
#endif
		}
	}
	/* s2-3. */
	rtw_free_assoc_resources(padapter, _TRUE);

	/* s2-4. */
	rtw_free_network_queue(padapter, _TRUE);
#ifdef CONFIG_IPS
	rtw_ips_dev_unload(padapter);
#endif
	pwrpriv->rf_pwrstate = rf_off;
	pwrpriv->bips_processing = _FALSE;
	_exit_pwrlock(&pwrpriv->lock);

	return 0;

error_exit:
	RTW_INFO("%s, failed\n", __FUNCTION__);
	return -1;

}

int rtw_hw_resume(_adapter *padapter)
{
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);
	struct usb_interface *pusb_intf = adapter_to_dvobj(padapter)->pusbintf;
	struct net_device *pnetdev = padapter->pnetdev;

	RTW_INFO("==> rtw_hw_resume\n");
	_enter_pwrlock(&pwrpriv->lock);
	pwrpriv->bips_processing = _TRUE;
	rtw_reset_drv_sw(padapter);

	if (pm_netdev_open(pnetdev, _FALSE) != 0) {
		_exit_pwrlock(&pwrpriv->lock);
		goto error_exit;
	}
	rtw_netif_device_attach(pnetdev);
	rtw_netif_carrier_on(pnetdev);

	rtw_netif_wake_queue(pnetdev);

	pwrpriv->bkeepfwalive = _FALSE;
	pwrpriv->brfoffbyhw = _FALSE;

	pwrpriv->rf_pwrstate = rf_on;
	pwrpriv->bips_processing = _FALSE;
	_exit_pwrlock(&pwrpriv->lock);


	return 0;
error_exit:
	RTW_INFO("%s, Open net dev failed\n", __FUNCTION__);
	return -1;
}
#endif

static int rtw_dev_suspend(struct usb_interface *pusb_intf, pm_message_t message)
{
	struct dvobj_priv *dvobj;
	struct pwrctrl_priv *pwrpriv;
	struct debug_priv *pdbgpriv;
	_adapter *padapter;
	int ret = 0;


	dvobj = usb_get_intfdata(pusb_intf);
	pwrpriv = dvobj_to_pwrctl(dvobj);
	pdbgpriv = &dvobj->drv_dbg;
	padapter = dvobj_get_primary_adapter(dvobj);

	if (pwrpriv->bInSuspend == _TRUE) {
		RTW_INFO("%s bInSuspend = %d\n", __FUNCTION__, pwrpriv->bInSuspend);
		pdbgpriv->dbg_suspend_error_cnt++;
		goto exit;
	}

	ret =  rtw_suspend_common(padapter);

exit:
	return ret;
}

static int rtw_resume_process(_adapter *padapter)
{
	int ret;
	struct pwrctrl_priv *pwrpriv = adapter_to_pwrctl(padapter);
	struct dvobj_priv *pdvobj = padapter->dvobj;
	struct debug_priv *pdbgpriv = &pdvobj->drv_dbg;


	if (pwrpriv->bInSuspend == _FALSE) {
		pdbgpriv->dbg_resume_error_cnt++;
		RTW_INFO("%s bInSuspend = %d\n", __FUNCTION__, pwrpriv->bInSuspend);
		return -1;
	}

	/*
	 * Due to usb wow suspend flow will cancel read/write port via intf_stop and
	 * bReadPortCancel and bWritePortCancel are set _TRUE in intf_stop.
	 * But they will not be clear in intf_start during wow resume flow.
	 * It should move to os_intf in the feature.
	 */
	RTW_ENABLE_FUNC(pdvobj, DF_RX_BIT);
	RTW_ENABLE_FUNC(pdvobj, DF_TX_BIT);

	ret =  rtw_resume_common(padapter);

	return ret;
}

static int rtw_dev_resume(struct usb_interface *pusb_intf)
{
	struct dvobj_priv *dvobj;
	struct pwrctrl_priv *pwrpriv;
	struct debug_priv *pdbgpriv;
	_adapter *padapter;
	struct mlme_ext_priv *pmlmeext;
	int ret = 0;


	dvobj = usb_get_intfdata(pusb_intf);
	pwrpriv = dvobj_to_pwrctl(dvobj);
	pdbgpriv = &dvobj->drv_dbg;
	padapter = dvobj_get_primary_adapter(dvobj);
	pmlmeext = &padapter->mlmeextpriv;

	RTW_INFO("==> %s (%s:%d)\n", __FUNCTION__, current->comm, current->pid);
	pdbgpriv->dbg_resume_cnt++;


	if (pwrpriv->wowlan_mode || pwrpriv->wowlan_ap_mode) {
		rtw_resume_lock_suspend();
		ret = rtw_resume_process(padapter);
		rtw_resume_unlock_suspend();
	} else {
#ifdef CONFIG_RESUME_IN_WORKQUEUE
		rtw_resume_in_workqueue(pwrpriv);
#else
		if (rtw_is_earlysuspend_registered(pwrpriv)) {
			/* jeff: bypass resume here, do in late_resume */
			rtw_set_do_late_resume(pwrpriv, _TRUE);
		} else {
			rtw_resume_lock_suspend();
			ret = rtw_resume_process(padapter);
			rtw_resume_unlock_suspend();
		}
#endif
	}

	pmlmeext->last_scan_time = rtw_get_current_time();
	RTW_INFO("<========  %s return %d\n", __FUNCTION__, ret);

	return ret;
}


/*
 * drv_init() - a device potentially for us
 *
 * notes: drv_init() is called when the bus driver has located a card for us to support.
 *        We accept the new device by returning 0.
*/
_adapter *rtw_usb_primary_adapter_init(struct dvobj_priv *dvobj,
	struct usb_interface *pusb_intf)
{
	_adapter *padapter = NULL;
	int status = _FAIL;
	u8 hw_mac_addr[ETH_ALEN] = {0};

	padapter = (_adapter *)rtw_zvmalloc(sizeof(*padapter));
	if (padapter == NULL)
		goto exit;

	padapter->dvobj = dvobj;

	if (rtw_load_registry(padapter) != _SUCCESS)
		goto free_adapter;

	dvobj->padapters[dvobj->iface_nums++] = padapter;
	padapter->iface_id = IFACE_ID0;

	/* set adapter_type/iface type for primary padapter */
	padapter->isprimary = _TRUE;
	padapter->adapter_type = PRIMARY_ADAPTER;
#ifdef CONFIG_MI_WITH_MBSSID_CAM/*Configure all IFACE to PORT0-MBSSID*/
	padapter->hw_port = HW_PORT0;
#else
	padapter->hw_port = HW_PORT0;
#endif

	/* step 2. hook HalFunc, allocate HalData */
	if (rtw_set_hal_ops(padapter) == _FAIL)
		goto free_hal_data;


	padapter->intf_start = &usb_intf_start;
	padapter->intf_stop = &usb_intf_stop;

	/* step usb endpoint mapping */
	rtw_hal_chip_configure(padapter);

#ifdef CONFIG_BT_COEXIST
	rtw_btcoex_Initialize(padapter);
#endif
	rtw_btcoex_wifionly_initialize(padapter);

	/* LED */
	{
	
	struct led_priv *ledpriv = adapter_to_led(padapter);
	PHAL_DATA_TYPE hal;

	hal = GET_HAL_DATA(padapter);
	ledpriv->bRegUseLed = _TRUE;
	hal->CustomerID = RT_CID_DEFAULT;
	ledpriv->LedStrategy = 12;
	}

	/* step 5. */
	if (rtw_init_drv_sw(padapter) == _FAIL) {
		goto free_hal_data;
	}

#ifdef CONFIG_PM
#if (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 18))
	if (dvobj_to_pwrctl(dvobj)->bSupportRemoteWakeup) {
		dvobj_to_usb(dvobj)->pusbdev->do_remote_wakeup = 1;
		pusb_intf->needs_remote_wakeup = 1;
		device_init_wakeup(&pusb_intf->dev, 1);
		RTW_INFO("pwrctrlpriv.bSupportRemoteWakeup~~~~~~\n");
		RTW_INFO("pwrctrlpriv.bSupportRemoteWakeup~~~[%d]~~~\n", device_may_wakeup(&pusb_intf->dev));
	}
#endif
#endif
	/* 2012-07-11 Move here to prevent the 8723AS-VAU BT auto suspend influence */
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 33))
	if (usb_autopm_get_interface(pusb_intf) < 0)
		RTW_INFO("can't get autopm:\n");
#endif
#ifdef CONFIG_BT_COEXIST
	dvobj_to_pwrctl(dvobj)->autopm_cnt = 1;
#endif

	/* get mac addr */
	rtw_hw_get_mac_addr(dvobj, hw_mac_addr);
	/* set mac addr */
	rtw_macaddr_cfg(adapter_mac_addr(padapter), hw_mac_addr);
#ifdef CONFIG_MI_WITH_MBSSID_CAM
	rtw_mbid_camid_alloc(padapter, adapter_mac_addr(padapter));
#endif

#ifdef CONFIG_P2P
	rtw_init_wifidirect_addrs(padapter, adapter_mac_addr(padapter), adapter_mac_addr(padapter));
#endif /* CONFIG_P2P */
	RTW_INFO("bDriverStopped:%s, bSurpriseRemoved:%s, netif_up:%d, hw_init_completed:%d\n"
		 , rtw_is_drv_stopped(padapter) ? "True" : "False"
		 , rtw_is_surprise_removed(padapter) ? "True" : "False"
		 , padapter->netif_up
		 , rtw_get_hw_init_completed(padapter)
		);

	status = _SUCCESS;

free_hal_data:
	if (status != _SUCCESS && padapter->HalData)
		rtw_hal_free_data(padapter);
free_adapter:
	if (status != _SUCCESS && padapter) {
		rtw_halmac_deinit_adapter(dvobj);
		rtw_vmfree((u8 *)padapter, sizeof(*padapter));
		padapter = NULL;
	}
exit:
	return padapter;
}

static void rtw_usb_primary_adapter_deinit(_adapter *padapter)
{
	RTW_INFO(FUNC_ADPT_FMT"\n", FUNC_ADPT_ARG(padapter));

#ifdef CONFIG_BTC
	if (1 == adapter_to_pwrctl(padapter)->autopm_cnt) {
		struct mlme_priv *pmlmepriv = &padapter->mlmepriv;
		struct dvobj_priv *dvobj = adapter_to_dvobj(padapter);
		PUSB_DATA usb_data = dvobj_to_usb(dvobj);

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 33))
		usb_autopm_put_interface(usb_data->pusbintf);
#elif (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 20))
		usb_autopm_enable(usb_data->pusbintf);
#else
		usb_autosuspend_device(usb_data->pusbdev, 1);
#endif
		adapter_to_pwrctl(padapter)->autopm_cnt--;
	}
#endif

	rtw_free_drv_sw(padapter);

	/* TODO: use rtw_os_ndevs_deinit instead at the first stage of driver's dev deinit function */
	rtw_os_ndev_free(padapter);

	rtw_halmac_deinit_adapter(adapter_to_dvobj(padapter));

	rtw_vmfree((u8 *)padapter, sizeof(_adapter));
}

static void rtw_usb_drop_all_phl_rx_pkt(struct dvobj_priv *dvobj)
{
	u16 rx_pkt_num = 0;
	struct rtw_recv_pkt *rx_req = NULL;

	rx_pkt_num = rtw_phl_query_new_rx_num(GET_PHL_INFO(dvobj));

	if (rx_pkt_num) {
		RTW_INFO("%s, rx_pkt not empty !!(%d)\n", __func__, rx_pkt_num);

		while (rx_pkt_num--) {
			rx_req = rtw_phl_query_rx_pkt(GET_PHL_INFO(dvobj));
			if (rx_req) {
				struct sk_buff *skb = rx_req->os_priv;

				rtw_phl_return_rxbuf(GET_PHL_INFO(dvobj), (u8 *)rx_req);
				rtw_skb_free(skb);
			}
		}
	}
}

static int rtw_dev_probe(struct usb_interface *pusb_intf, const struct usb_device_id *pdid)
{
	_adapter *padapter = NULL;
	struct dvobj_priv *dvobj;

	RTW_INFO("+%s\n", __func__);

	/* step 0. */
	process_spec_devid(pdid);

	/* Initialize dvobj_priv */
	dvobj = usb_dvobj_init(pusb_intf, pdid);
	if (dvobj == NULL) {
		RTW_ERR("usb_dvobj_init Failed!\n");
		goto exit;
	}

	if (devobj_trx_resource_init(dvobj) == _FAIL)
		goto free_dvobj;

	/*init hw - register and get chip-info */
	if (rtw_hw_init(dvobj) == _FAIL) {
		RTW_ERR("rtw_hw_init Failed!\n");
		goto free_trx_reso;
	}

	padapter = rtw_usb_primary_adapter_init(dvobj, pusb_intf);
	if (padapter == NULL) {
		RTW_ERR("rtw_usb_primary_adapter_init Failed!\n");
		goto free_hw;
	}

	if (usb_reprobe_switch_usb_mode(padapter) == _TRUE) {
		RTW_ERR("usb_reprobe_switch_usb_mode Failed!\n");
		goto free_if_prim;
	}

#ifdef CONFIG_CONCURRENT_MODE
	if (rtw_drv_add_vir_ifaces(dvobj) == _FAIL)
		goto free_if_vir;
#endif

	/*init data of dvobj from registary and ic spec*/
	if (devobj_data_init(dvobj) == _FAIL) {
		RTW_ERR("devobj_data_init Failed!\n");
		goto free_devobj_data;
	}

#ifdef CONFIG_GLOBAL_UI_PID
	if (ui_pid[1] != 0) {
		RTW_INFO("ui_pid[1]:%d\n", ui_pid[1]);
		rtw_signal_process(ui_pid[1], SIGUSR2);
	}
#endif

	/* dev_alloc_name && register_netdev */
	if (rtw_os_ndevs_init(dvobj) != _SUCCESS) {
		RTW_ERR("rtw_os_ndevs_init Failed!\n");
		goto free_devobj_data;
	}

#ifdef CONFIG_HOSTAPD_MLME
	hostapd_mode_init(padapter);
#endif

	RTW_INFO("-%s success\n", __func__);
	return 0;

free_devobj_data:
	devobj_data_deinit(dvobj);

#ifdef CONFIG_CONCURRENT_MODE
free_if_vir:
	rtw_drv_stop_vir_ifaces(dvobj);
	rtw_drv_free_vir_ifaces(dvobj);
#endif

free_if_prim:
	if (padapter)
		rtw_usb_primary_adapter_deinit(padapter);
free_hw:
	rtw_hw_deinit(dvobj);

free_trx_reso:
	devobj_trx_resource_deinit(dvobj);

free_dvobj:
	usb_dvobj_deinit(pusb_intf);
exit:
	return -ENODEV;
}

/*
 * dev_remove() - our device is being removed
*/
/* rmmod module & unplug(SurpriseRemoved) will call r871xu_dev_remove() => how to recognize both */
static void rtw_dev_remove(struct usb_interface *pusb_intf)
{
	struct dvobj_priv *dvobj = usb_get_intfdata(pusb_intf);
#if defined(CONFIG_HAS_EARLYSUSPEND) || defined(CONFIG_ANDROID_POWER)
	struct pwrctrl_priv *pwrctl = dvobj_to_pwrctl(dvobj);
#endif
	_adapter *padapter = dvobj_get_primary_adapter(dvobj);

	RTW_INFO("+%s\n", __func__);

	dvobj->processing_dev_remove = _TRUE;

	/* TODO: use rtw_os_ndevs_deinit instead at the first stage of driver's dev deinit function */
	rtw_os_ndevs_unregister(dvobj);

	if (usb_drv.drv_registered == _TRUE) {
		/* RTW_INFO("r871xu_dev_remove():padapter->bSurpriseRemoved == _TRUE\n"); */
		dev_set_surprise_removed(dvobj);
	}

#if defined(CONFIG_HAS_EARLYSUSPEND) || defined(CONFIG_ANDROID_POWER)
	rtw_unregister_early_suspend(pwrctl);
#endif
#if 0 /*GEORGIA_TODO_FIXIT*/
	if (GET_PHL_COM(dvobj)->fw_ready == _TRUE) {
		rtw_pm_set_ips(padapter, IPS_NONE);
		rtw_pm_set_lps(padapter, PM_PS_MODE_ACTIVE);

		LeaveAllPowerSaveMode(padapter);
	}
#endif
	dev_set_drv_stopped(adapter_to_dvobj(padapter));	/*for stop thread*/
#if 0 /*ifdef CONFIG_CORE_CMD_THREAD*/
	rtw_stop_cmd_thread(padapter);
#endif
#ifdef CONFIG_CONCURRENT_MODE
	rtw_drv_stop_vir_ifaces(dvobj);
#endif /* CONFIG_CONCURRENT_MODE */

	rtw_drv_stop_prim_iface(padapter);

	if (rtw_hw_is_init_completed(dvobj))
		rtw_hw_stop(dvobj);
	dev_set_surprise_removed(dvobj);

	rtw_usb_drop_all_phl_rx_pkt(dvobj);

	rtw_usb_primary_adapter_deinit(padapter);

#ifdef CONFIG_CONCURRENT_MODE
	rtw_drv_free_vir_ifaces(dvobj);
#endif /* CONFIG_CONCURRENT_MODE */

	rtw_hw_deinit(dvobj);
	devobj_data_deinit(dvobj);
	devobj_trx_resource_deinit(dvobj);
	usb_dvobj_deinit(pusb_intf);

	RTW_INFO("-%s done\n", __func__);

	return;

}
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24))
extern int console_suspend_enabled;
#endif

static int __init rtw_drv_entry(void)
{
	int ret = 0;

	RTW_PRINT("module init start\n");
	dump_drv_version(RTW_DBGDUMP);
#ifdef BTCOEXVERSION
	RTW_PRINT(DRV_NAME" BT-Coex version = %s\n", BTCOEXVERSION);
#endif /* BTCOEXVERSION */

	ret = platform_wifi_power_on();
	if (ret != 0) {
		RTW_INFO("%s: power on failed!!(%d)\n", __FUNCTION__, ret);
		ret = -1;
		goto exit;
	}
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 24))
	/* console_suspend_enabled=0; */
#endif

	usb_drv.drv_registered = _TRUE;
	rtw_suspend_lock_init();
	rtw_drv_proc_init();
	rtw_nlrtw_init();
#ifdef CONFIG_PLATFORM_CMAP_INTFS
	cmap_intfs_init();
#endif
	rtw_ndev_notifier_register();
	rtw_inetaddr_notifier_register();

	ret = usb_register(&usb_drv.usbdrv);

	if (ret != 0) {
		usb_drv.drv_registered = _FALSE;
		rtw_suspend_lock_uninit();
		rtw_drv_proc_deinit();
		rtw_nlrtw_deinit();
#ifdef CONFIG_PLATFORM_CMAP_INTFS
		cmap_intfs_deinit();
#endif
		rtw_ndev_notifier_unregister();
		rtw_inetaddr_notifier_unregister();
		goto exit;
	}

exit:
	RTW_PRINT("module init ret=%d\n", ret);
	return ret;
}

static void __exit rtw_drv_halt(void)
{
	RTW_PRINT("module exit start\n");

	usb_drv.drv_registered = _FALSE;

	usb_deregister(&usb_drv.usbdrv);
	platform_wifi_power_off();

	rtw_suspend_lock_uninit();
	rtw_drv_proc_deinit();
	rtw_nlrtw_deinit();
#ifdef CONFIG_PLATFORM_CMAP_INTFS
	cmap_intfs_deinit();
#endif
	rtw_ndev_notifier_unregister();
	rtw_inetaddr_notifier_unregister();

	RTW_PRINT("module exit success\n");

	rtw_mstat_dump(RTW_DBGDUMP);
}

module_init(rtw_drv_entry);
module_exit(rtw_drv_halt);
