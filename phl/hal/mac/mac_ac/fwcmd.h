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

#ifndef _MAC_AX_FW_CMD_H_
#define _MAC_AX_FW_CMD_H_

#include "../type.h"


//#include "fwcmd_intf.h"
//#include "fwcmd_intf_f2p.h"
#include "trx_desc.h"
//#include "fwofld.h"

#define FWCMD_HDR_LEN		48

#define H2C_CMD_LEN		32
#define H2C_DATA_LEN		256
#define H2C_LONG_DATA_LEN	8192

//NEO
// H2C, C2H : need to move to G6's H2C, fwcmd.c/fwcmd.h, and rewrite to common style
/* PKT H2C */
#define H2C_PKT_CMD_ID 0xFF
#define H2C_PKT_CATEGORY 0x01

#define H2C_PKT_GENERAL_INFO 0x0D
#define H2C_PKT_PHYDM_INFO 0x11
#define H2C_PKT_IQK 0x0E

#define H2C_PKT_CH_SWITCH 0x02
#define H2C_PKT_UPDATE_PKT 0x0C

#define H2C_PKT_CH_SWITCH_LEN 0x20
#define H2C_PKT_UPDATE_PKT_LEN 0x4
#define C2H_PKT_BUF		256
#define H2C_PKT_SIZE		32
#define H2C_PKT_HDR_SIZE	8

#define H2C_PKT_SIZE		32
#define H2C_PKT_HDR_SIZE	8

#define C2H_GET_CMD_ID(ch2_pkt) LE_BITS_TO_4BYTE(ch2_pkt + 0x00, 0, 8)
#define C2H_GET_SEQ(c2h_pkt) LE_BITS_TO_4BYTE(c2h_pkt + 0x00, 8, 8)

#define SET_FWCMD_ID(_t, _ca, _cl, _f)                                         \
		(SET_WORD(_t, H2C_HDR_DEL_TYPE) | SET_WORD(_ca, H2C_HDR_CAT) | \
		 SET_WORD(_cl, H2C_HDR_CLASS) | SET_WORD(_f, H2C_HDR_FUNC))
#define GET_FWCMD_TYPE(id)	(GET_FIELD(id, C2H_HDR_DEL_TYPE))
#define GET_FWCMD_CAT(id)	(GET_FIELD(id, C2H_HDR_CAT))
#define GET_FWCMD_CLASS(id)	(GET_FIELD(id, C2H_HDR_CLASS))
#define GET_FWCMD_FUNC(id)	(GET_FIELD(id, C2H_HDR_FUNC))

#define FWCMD_TYPE_H2C	0
#define FWCMD_TYPE_C2H	1

#define FWCMD_C2H_CL_NULL		0xFF
#define FWCMD_C2H_FUNC_NULL		0xFF


enum h2c_buf_class {
	H2CB_CLASS_CMD,		/* FW command */
	H2CB_CLASS_DATA,	/* FW command + data */
	H2CB_CLASS_LONG_DATA,	/* FW command + long data */

	/* keep last */
	H2CB_CLASS_LAST,
	H2CB_CLASS_MAX = H2CB_CLASS_LAST,
	H2CB_CLASS_INVALID = H2CB_CLASS_LAST,
};

struct h2c_buf_head {
	/* keep first */
	struct h2c_buf *next;
	struct h2c_buf *prev;
	u8 *pool;
	u32 size;
	u32 qlen;
	u8 suspend;
	mac_mutex lock;
};

struct fwcmd_wkb_head {
	/* keep first */
	struct h2c_buf *next;
	struct h2c_buf *prev;
	u32 qlen;
	mac_mutex lock;
};

struct h2c_buf {
	/* keep first */
	struct h2c_buf *next;
	struct h2c_buf *prev;
	enum h2c_buf_class _class_;
	u32 id;
	u8 master;
	u32 len;
	u8 *head;
	u8 *end;
	u8 *data;
	u8 *tail;
	u32 hdr_len;
#define H2CB_FLAGS_FREED	BIT(0)
	u32 flags;
	u8 h2c_seq;
};

struct c2h_proc_class {
	u16 id;
	u32 (*handler)(struct mac_adapter *adapter, u8 *buf, u32 len,
		       struct rtw_c2h_info *info);
};

struct c2h_proc_func {
	u16 id;
	u32 (*handler)(struct mac_adapter *adapter, u8 *buf, u32 len,
		       struct rtw_c2h_info *info);
};

struct mac_ax_c2hreg_info {
#define C2HREG_LEN 16
	u8 id;
	u8 total_len;
	u8 *content;
	u8 c2hreg[C2HREG_LEN];
};

u32 h2cb_init(struct mac_adapter *adapter);
u32 h2cb_exit(struct mac_adapter *adapter);
#if MAC_PHL_H2C
struct rtw_h2c_pkt *h2cb_alloc(struct mac_adapter *adapter,
			       enum h2c_buf_class buf_class);
void h2cb_free(struct mac_adapter *adapter, struct rtw_h2c_pkt *h2cb);
u8 *h2cb_push(struct rtw_h2c_pkt *h2cb, u32 len);
u8 *h2cb_pull(struct rtw_h2c_pkt *h2cb, u32 len);
u8 *h2cb_put(struct rtw_h2c_pkt *h2cb, u32 len);
u32 h2c_pkt_set_hdr(struct mac_adapter *adapter, struct rtw_h2c_pkt *h2cb,
		    u8 type, u8 cat, u8 _class_, u8 func, u16 rack, u16 dack);
u32 h2c_pkt_set_hdr_fwdl(struct mac_adapter *adapter,
			 struct rtw_h2c_pkt *h2cb, u8 type, u8 cat, u8 _class_,
			 u8 func, u16 rack, u16 dack);
u32 h2c_pkt_set_cmd(struct mac_adapter *adapter, struct rtw_h2c_pkt *h2cb,
		    u8 *cmd, u32 len);
u32 h2c_pkt_build_txd(struct mac_adapter *adapter, struct rtw_h2c_pkt *h2cb);
#else
struct h2c_buf *h2cb_alloc(struct mac_adapter *adapter,
			   enum h2c_buf_class buf_class);
void h2cb_free(struct mac_adapter *adapter, struct h2c_buf *h2cb);
u8 *h2cb_push(struct h2c_buf *h2cb, u32 len);
u8 *h2cb_pull(struct h2c_buf *h2cb, u32 len);
u8 *h2cb_put(struct h2c_buf *h2cb, u32 len);
u32 h2c_pkt_set_hdr(struct mac_adapter *adapter, struct h2c_buf *h2cb,
		    u8 type, u8 cat, u8 _class_, u8 func, u16 rack, u16 dack);
u32 h2c_pkt_set_hdr_fwdl(struct mac_adapter *adapter, struct h2c_buf *h2cb,
			 u8 type, u8 cat, u8 _class_, u8 func, u16 rack,
			 u16 dack);
u32 h2c_pkt_set_cmd(struct mac_adapter *adapter, struct h2c_buf *h2cb,
		    u8 *cmd, u32 len);
u32 h2c_pkt_build_txd(struct mac_adapter *adapter, struct h2c_buf *h2cb);
#endif
u32 fwcmd_wq_enqueue(struct mac_adapter *adapter, struct h2c_buf *h2cb);
struct h2c_buf *fwcmd_wq_dequeue(struct mac_adapter *adapter, u32 id);
u32 fwcmd_wq_idle(struct mac_adapter *adapter, u32 id);

u32 mac_process_c2h(struct mac_adapter *adapter, u8 *buf, u32 len,
		    u8 *ret);

u8 c2h_field_parsing(struct fwcmd_hdr *hdr, struct rtw_c2h_info *info);
u32 mac_fw_log_cfg(struct mac_adapter *adapter,
		   struct mac_fw_log *log_cfg);
u32 mac_send_general_info_h2c(struct mac_adapter *adapter);
u32 mac_send_bcn_h2c(struct mac_adapter *adapter,
		     struct mac_bcn_info *info);
u32 mac_host_getpkt_h2c(struct mac_adapter *adapter, u8 macid, u8 pkttype);
u32 mac_outsrc_h2c_common(struct mac_adapter *adapter,
			  struct rtw_g6_h2c_hdr *hdr, u32 *pvalue);
u32 mac_ie_cam_upd(struct mac_adapter *adapter,
		   struct mac_ie_cam_cmd_info *info);
u32 h2c_end_flow(struct mac_adapter *adapter);
u32 mac_send_h2creg(struct mac_adapter *adapter, u32 *content, u8 len);
u32 mac_process_c2hreg(struct mac_adapter *adapter,
		       struct mac_c2hreg_info *info);


#endif
