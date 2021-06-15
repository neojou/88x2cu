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
#ifndef __USB_OPS_LINUX_H__
#define __USB_OPS_LINUX_H__

#define VENDOR_CMD_MAX_DATA_LEN	254
#define FW_START_ADDRESS	0x1000

#define RTW_USB_CONTROL_MSG_TIMEOUT_TEST	10/* ms */
#define RTW_USB_CONTROL_MSG_TIMEOUT	500/* ms */

#define RECV_BULK_IN_ADDR		0x80/* assign by drv, not real address */
#define RECV_INT_IN_ADDR		0x81/* assign by drv, not real address */

#define INTERRUPT_MSG_FORMAT_LEN 60

#if defined(CONFIG_VENDOR_REQ_RETRY) && defined(CONFIG_USB_VENDOR_REQ_MUTEX)
	/* vendor req retry should be in the situation when each vendor req is atomically submitted from others */
	#define MAX_USBCTRL_VENDORREQ_TIMES	10
#else
	#define MAX_USBCTRL_VENDORREQ_TIMES	1
#endif

#define RTW_USB_BULKOUT_TIMEOUT	5000/* ms */

#if (LINUX_VERSION_CODE < KERNEL_VERSION(2, 5, 0)) || (LINUX_VERSION_CODE > KERNEL_VERSION(2, 6, 18))
#define _usbctrl_vendorreq_async_callback(urb, regs)	_usbctrl_vendorreq_async_callback(urb)
#define usb_bulkout_zero_complete(purb, regs)	usb_bulkout_zero_complete(purb)
#define usb_write_mem_complete(purb, regs)	usb_write_mem_complete(purb)
#define usb_write_port_complete(purb, regs)	usb_write_port_complete(purb)
#define usb_read_interrupt_complete(purb, regs)	usb_read_interrupt_complete(purb)
#endif

#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 12))
#define rtw_usb_control_msg(dev, pipe, request, requesttype, value, index, data, size, timeout_ms) \
	usb_control_msg((dev), (pipe), (request), (requesttype), (value), (index), (data), (size), (timeout_ms))
#define rtw_usb_bulk_msg(usb_dev, pipe, data, len, actual_length, timeout_ms) \
	usb_bulk_msg((usb_dev), (pipe), (data), (len), (actual_length), (timeout_ms))
#else
#define rtw_usb_control_msg(dev, pipe, request, requesttype, value, index, data, size, timeout_ms) \
	usb_control_msg((dev), (pipe), (request), (requesttype), (value), (index), (data), (size), \
		((timeout_ms) == 0) || ((timeout_ms) * HZ / 1000 > 0) ? ((timeout_ms) * HZ / 1000) : 1)
#define rtw_usb_bulk_msg(usb_dev, pipe, data, len, actual_length, timeout_ms) \
	usb_bulk_msg((usb_dev), (pipe), (data), (len), (actual_length), \
		((timeout_ms) == 0) || ((timeout_ms) * HZ / 1000 > 0) ? ((timeout_ms) * HZ / 1000) : 1)
#endif

unsigned int bulkid2pipe(struct dvobj_priv *pdvobj, u32 addr, u8 bulk_out);


int rtw_os_urb_resource_alloc(struct data_urb *dataurb);
void rtw_os_urb_resource_free(struct data_urb *dataurb);

int usbctrl_vendorreq(struct dvobj_priv *pdvobjpriv, u8 request, u16 value, u16 index,
			 void *pdata, u16 len, u8 requesttype);

#ifdef CONFIG_USB_SUPPORT_ASYNC_VDN_REQ
int _usbctrl_vendorreq_async_write(struct usb_device *udev, u8 request,
		u16 value, u16 index, void *pdata, u16 len, u8 requesttype);
#endif /* CONFIG_USB_SUPPORT_ASYNC_VDN_REQ */

void usb_recv_tasklet(void *priv);

#ifdef CONFIG_USB_INTERRUPT_IN_PIPE
void usb_read_interrupt_complete(struct urb *purb, struct pt_regs *regs);
#endif

u32 rtw_usb_write_port(void *d, u8 *phl_tx_buf_ptr,
	u8 bulk_id, u32 len, u8 *pkt_data_buf);

u32 rtw_usb_write_data(void *d, u8 bulk_id, struct sk_buff *skb);

void rtw_usb_write_port_cancel(void *d);

u32 rtw_usb_read_port(void *d, void *rxobj,
	u8 *inbuf, u32 inbuf_len, u8 bulk_id, u8 minlen);

void rtw_usb_read_port_cancel(void *d);


#endif
