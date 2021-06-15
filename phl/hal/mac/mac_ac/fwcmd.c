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

#include "fwcmd.h"
#include "../mac.h"
#if 0 // NEO mark off first

//#include "mcc.h"

#if MAC_AX_FEATURE_HV
#include "../hv_ax/dbgpkg_hv.h"
#endif

#define H2CB_CMD_HDR_SIZE	(FWCMD_HDR_LEN + WD_BODY_LEN)
#define H2CB_CMD_SIZE		(H2C_CMD_LEN - FWCMD_HDR_LEN)
#define H2CB_CMD_QLEN		8

#define H2CB_DATA_HDR_SIZE	(FWCMD_HDR_LEN + WD_BODY_LEN)
#define H2CB_DATA_SIZE		(H2C_DATA_LEN - FWCMD_HDR_LEN)
#define H2CB_DATA_QLEN		4

#define H2CB_LONG_DATA_HDR_SIZE	(FWCMD_HDR_LEN + WD_BODY_LEN)
#define H2CB_LONG_DATA_SIZE	(H2C_LONG_DATA_LEN - FWCMD_HDR_LEN)
#define H2CB_LONG_DATA_QLEN	1

#define FWCMD_WQ_MAX_JOB_NUM	5

#define FWCMD_LMT		12

#define MAC_AX_H2C_LMT_EN	0
#define C2HREG_HDR_LEN 2

#define BCN_GRPIE_OFST_EN BIT(7)

static struct h2c_buf_head h2cb_head[H2CB_CLASS_MAX];
static struct fwcmd_wkb_head fwcmd_wq_head;

struct fwcmd_outsrc_info {
#define MAX_OUTSRC_LEN 60 //need to extend if needed
	u32 dword0[MAX_OUTSRC_LEN];
};

static inline u32 h2cb_queue_len(struct h2c_buf_head *list)
{
	return list->qlen;
};

static inline void __h2cb_queue_head_init(struct h2c_buf_head *list)
{
	list->prev = (struct h2c_buf *)list;
	list->next = (struct h2c_buf *)list;
	list->qlen = 0;
	list->suspend = 0;
};

static inline void h2cb_queue_head_init(struct mac_ax_adapter *adapter,
					struct h2c_buf_head *list)
{
	PLTFM_MUTEX_INIT(&list->lock);
	__h2cb_queue_head_init(list);
}

static inline void __h2cb_insert(struct h2c_buf *new_h2cb, struct h2c_buf *prev,
				 struct h2c_buf *next,
				 struct h2c_buf_head *list)
{
	new_h2cb->next = next;
	new_h2cb->prev = prev;
	next->prev  = new_h2cb;
	prev->next = new_h2cb;
	list->qlen++;
}

static inline void __h2cb_queue_before(struct h2c_buf_head *list,
				       struct h2c_buf *next,
				       struct h2c_buf *new_h2cb)
{
	__h2cb_insert(new_h2cb, next->prev, next, list);
}

static inline void __h2cb_queue_tail(struct h2c_buf_head *list,
				     struct h2c_buf *new_h2cb)
{
	__h2cb_queue_before(list, (struct h2c_buf *)list, new_h2cb);
}

static inline void __h2cb_unlink(struct h2c_buf *h2cb,
				 struct h2c_buf_head *list)
{
	struct h2c_buf *next, *prev;

	list->qlen--;
	next = h2cb->next;
	prev = h2cb->prev;
	h2cb->prev = NULL;
	h2cb->next = NULL;
	next->prev = prev;
	prev->next = next;
}

#endif //NEO

static inline struct h2c_buf *h2cb_peek(struct h2c_buf_head *list)
{
	struct h2c_buf *h2cb = list->next;

	if (h2cb == (struct h2c_buf *)list)
		h2cb = NULL;
	return h2cb;
}

static inline u8 *h2cb_tail_pointer(const struct rtw_h2c_pkt *h2cb)
{
	return h2cb->vir_tail;
}

#if 0 //NEO

static inline struct h2c_buf *h2cb_dequeue(struct h2c_buf_head *list)
{
	struct h2c_buf *h2cb = h2cb_peek(list);

	if (h2cb)
		__h2cb_unlink(h2cb, list);
	return h2cb;
}

static u8 *__h2cb_alloc_buf_pool(struct mac_ax_adapter *adapter,
				 struct h2c_buf_head *list, u32 size, int num)
{
	u32 block_size = (size * num);
	u8 *ptr;

	ptr = (u8 *)PLTFM_MALLOC(block_size);
	list->pool = ptr;
	list->size = block_size;

	return ptr;
}

static struct h2c_buf *__h2cb_alloc(struct mac_ax_adapter *adapter,
				    enum h2c_buf_class buf_class,
				    u32 hdr_size, u8 *buf_ptr, int buf_size)
{
	struct h2c_buf *h2cb;

	//_ASSERT_(!buf_ptr);

	h2cb = (struct h2c_buf *)PLTFM_MALLOC(sizeof(struct h2c_buf));
	if (!h2cb)
		return NULL;
	PLTFM_MEMSET(h2cb, 0, sizeof(struct h2c_buf));

	h2cb->_class_ = buf_class;
	h2cb->id = 0;
	h2cb->master = 0;
	h2cb->len = 0;
	h2cb->head = buf_ptr;
	h2cb->end = h2cb->head + buf_size;
	h2cb->data = h2cb->head + hdr_size;
	h2cb->tail = h2cb->data;
	h2cb->hdr_len = hdr_size;
	h2cb->flags |= H2CB_FLAGS_FREED;

	return h2cb;
}

static u32 __h2cb_free(struct mac_ax_adapter *adapter,
		       enum h2c_buf_class buf_class)
{
	struct h2c_buf_head *list_head = &h2cb_head[buf_class];
	struct h2c_buf *h2cb;

	if (buf_class >= H2CB_CLASS_LAST)
		return MACNOITEM;

	if (!list_head->pool)
		return MACNPTR;

	if (!h2cb_queue_len(list_head))
		return MACSUCCESS;

	while ((h2cb = h2cb_dequeue(list_head)))
		PLTFM_FREE(h2cb, sizeof(struct h2c_buf));

	PLTFM_FREE(list_head->pool, list_head->size);
	list_head->pool = NULL;
	list_head->size = 0;
	PLTFM_MUTEX_DEINIT(&list_head->lock);

	return MACSUCCESS;
}

static u32 __h2cb_init(struct mac_ax_adapter *adapter,
		       enum h2c_buf_class buf_class, u32 num, u32 buf_size,
		       u32 hdr_size, u32 tailer_size)
{
	u32 i;
	u8 *ptr;
	struct h2c_buf_head *list_head = &h2cb_head[buf_class];
	u32 real_size = buf_size + hdr_size + tailer_size;
	struct h2c_buf *h2cb;

	if (buf_class >= H2CB_CLASS_LAST)
		return MACNOITEM;

	if (h2cb_queue_len(list_head))
		return MACBUFSZ;

	h2cb_queue_head_init(adapter, list_head);

	ptr = __h2cb_alloc_buf_pool(adapter, list_head, real_size, num);
	if (!ptr)
		return MACNPTR;

	for (i = 0; i < num; i++) {
		h2cb = __h2cb_alloc(adapter,
				    buf_class, hdr_size, ptr, real_size);
		if (!h2cb)
			goto h2cb_fail;
		__h2cb_queue_tail(list_head, h2cb);
		ptr += real_size;
	}

	return MACSUCCESS;
h2cb_fail:
	__h2cb_free(adapter, buf_class);

	return MACBUFALLOC;
}

static inline u32 fwcmd_wkb_queue_len(struct fwcmd_wkb_head *list)
{
	return list->qlen;
};

static inline void __fwcmd_wkb_queue_head_init(struct fwcmd_wkb_head *list)
{
	list->prev = (struct h2c_buf *)list;
	list->next = (struct h2c_buf *)list;
	list->qlen = 0;
};

static inline void fwcmd_wkb_queue_head_init(struct mac_ax_adapter *adapter,
					     struct fwcmd_wkb_head *list)
{
	PLTFM_MUTEX_INIT(&list->lock);
	__fwcmd_wkb_queue_head_init(list);
}

static u32 __fwcmd_wkb_init(struct mac_ax_adapter *adapter)
{
	struct fwcmd_wkb_head *list_head = &fwcmd_wq_head;

	if (fwcmd_wkb_queue_len(list_head))
		return MACBUFSZ;

	fwcmd_wkb_queue_head_init(adapter, list_head);

	return MACSUCCESS;
}

u32 h2cb_init(struct mac_ax_adapter *adapter)
{
	u32 ret;

	ret = __h2cb_init(adapter, H2CB_CLASS_CMD, H2CB_CMD_QLEN,
			  H2CB_CMD_SIZE, H2CB_CMD_HDR_SIZE, 0);
	if (ret)
		return ret;

	ret = __h2cb_init(adapter, H2CB_CLASS_DATA, H2CB_DATA_QLEN,
			  H2CB_DATA_SIZE, H2CB_DATA_HDR_SIZE, 0);
	if (ret)
		return ret;

	ret = __h2cb_init(adapter, H2CB_CLASS_LONG_DATA, H2CB_LONG_DATA_QLEN,
			  H2CB_LONG_DATA_SIZE, H2CB_LONG_DATA_HDR_SIZE, 0);
	if (ret)
		return ret;

	ret = __fwcmd_wkb_init(adapter);
	if (ret)
		return ret;

	return MACSUCCESS;
}

u32 h2cb_exit(struct mac_adapter *adapter)
{
	struct fwcmd_wkb_head *list_head = &fwcmd_wq_head;

	if (fwcmd_wkb_queue_len(list_head))
		return MACBUFSZ;

	__h2cb_free(adapter, H2CB_CLASS_CMD);
	__h2cb_free(adapter, H2CB_CLASS_DATA);
	__h2cb_free(adapter, H2CB_CLASS_LONG_DATA);

	return MACSUCCESS;
}

u32 h2c_end_flow(struct mac_ax_adapter *adapter)
{
	struct mac_fw_info *fwinfo = &adapter->fw_info;

	PLTFM_MUTEX_LOCK(&fwinfo->seq_lock);
	fwinfo->h2c_seq++;
	PLTFM_MUTEX_UNLOCK(&fwinfo->seq_lock);

	return MACSUCCESS;
}

#endif //NEO

struct rtw_h2c_pkt *h2cb_alloc(struct mac_adapter *adapter,
			       enum h2c_buf_class buf_class)
{
	struct rtw_h2c_pkt *h2cb;

	if (buf_class >= H2CB_CLASS_LAST) {
		PLTFM_MSG_ERR("[ERR]unknown class\n");
		return NULL;
	}


	h2cb = PLTFM_QUERY_H2C(buf_class);

	return h2cb;
}

void h2cb_free(struct mac_adapter *adapter, struct rtw_h2c_pkt *h2cb)
{
}

u8 *h2cb_push(struct rtw_h2c_pkt *h2cb, u32 len)
{
	pr_info("%s len: %u, vir_head:%u, vir_data:%u, data_len:%u\n",
		__func__, len, h2cb->vir_head, h2cb->vir_data, h2cb->data_len);

	h2cb->vir_data -= len;
	h2cb->data_len  += len;

	if (h2cb->vir_data < h2cb->vir_head)
		return NULL;

	pr_info("%s vir_data:%u, data_len:%u\n",
		__func__, h2cb->vir_data, h2cb->data_len);
	
	return h2cb->vir_data;
}

u8 *h2cb_pull(struct rtw_h2c_pkt *h2cb, u32 len)
{
	h2cb->vir_data += len;

	if (h2cb->vir_data > h2cb->vir_end)
		return NULL;

	if (h2cb->data_len < len)
		return NULL;

	h2cb->data_len -= len;

	return h2cb->vir_data;
}

u8 *h2cb_put(struct rtw_h2c_pkt *h2cb, u32 len)
{
	u8 *tmp = h2cb_tail_pointer(h2cb);

	pr_info("%s len: %u, vir_tail:%u, vir_end:%u, data_len:%u\n",
		__func__, len, h2cb->vir_tail, h2cb->vir_end, h2cb->data_len);
	h2cb->vir_tail += len;
	h2cb->data_len += len;

	if (h2cb->vir_tail > h2cb->vir_end)
		return NULL;

	pr_info("%s tmp:%p, vir_tail:%u, data_len:%u\n",
		__func__, tmp, h2cb->vir_tail, h2cb->vir_end, h2cb->data_len);
	return tmp;
}

#if 0 //NEO

u32 h2c_pkt_set_hdr(struct mac_ax_adapter *adapter, struct rtw_h2c_pkt *h2cb,
		    u8 type, u8 cat, u8 _class_, u8 func, u16 rack, u16 dack)
{
	struct fwcmd_hdr *hdr;
	struct mac_ax_fw_info *fwinfo = &adapter->fw_info;

	if (adapter->sm.fwdl != MAC_AX_FWDL_INIT_RDY)
		return MACFWNONRDY;

	hdr = (struct fwcmd_hdr *)h2cb_push(h2cb, FWCMD_HDR_LEN);
	if (!hdr)
		return MACNPTR;

	hdr->hdr0 = cpu_to_le32(SET_WORD(type, H2C_HDR_DEL_TYPE) |
				SET_WORD(cat, H2C_HDR_CAT) |
				SET_WORD(_class_, H2C_HDR_CLASS) |
				SET_WORD(func, H2C_HDR_FUNC) |
				SET_WORD(fwinfo->h2c_seq, H2C_HDR_H2C_SEQ));

	hdr->hdr1 = cpu_to_le32(SET_WORD(h2cb->data_len, H2C_HDR_TOTAL_LEN) |
				(rack || !(fwinfo->h2c_seq & 3) ?
				 H2C_HDR_REC_ACK : 0) |
				(dack ? H2C_HDR_DONE_ACK : 0));

	h2cb->id = SET_FWCMD_ID(type, cat, _class_, func);
	h2cb->h2c_seq = fwinfo->h2c_seq;

	return MACSUCCESS;
}

u32 h2c_pkt_set_hdr_fwdl(struct mac_ax_adapter *adapter,
			 struct rtw_h2c_pkt *h2cb, u8 type, u8 cat,
			 u8 _class_, u8 func, u16 rack, u16 dack)
{
	struct fwcmd_hdr *hdr;
	struct mac_ax_fw_info *fwinfo = &adapter->fw_info;

	hdr = (struct fwcmd_hdr *)h2cb_push(h2cb, FWCMD_HDR_LEN);
	if (!hdr)
		return MACNPTR;

	hdr->hdr0 = cpu_to_le32(SET_WORD(type, H2C_HDR_DEL_TYPE) |
				SET_WORD(cat, H2C_HDR_CAT) |
				SET_WORD(_class_, H2C_HDR_CLASS) |
				SET_WORD(func, H2C_HDR_FUNC) |
				SET_WORD(fwinfo->h2c_seq, H2C_HDR_H2C_SEQ));

	hdr->hdr1 = cpu_to_le32(SET_WORD(h2cb->data_len, H2C_HDR_TOTAL_LEN) |
				(rack ? H2C_HDR_REC_ACK : 0) |
				(dack ? H2C_HDR_DONE_ACK : 0));

	h2cb->id = SET_FWCMD_ID(type, cat, _class_, func);
	h2cb->h2c_seq = fwinfo->h2c_seq;

	return MACSUCCESS;
}

u32 h2c_pkt_set_cmd(struct mac_ax_adapter *adapter, struct rtw_h2c_pkt *h2cb,
		    u8 *cmd, u32 len)
{
	u8 *buf;

	buf = h2cb_put(h2cb, len);
	if (!buf)
		return MACNPTR;
	PLTFM_MEMCPY(buf, cmd, len);
	return MACSUCCESS;
}

#endif //NEO

u32 h2c_pkt_build_txd(struct mac_adapter *adapter, struct rtw_h2c_pkt *h2cb)
{
	struct mac_txpkt_info info = {0};
	struct mac_pkt_data *pkt_info = &info.u.data;
	u8 *buf;
	u32 txd_len;
	u32 ret;


	info.type = MAC_PKT_H2C;
	info.pktsize = h2cb->data_len;
	txd_len = mac_txdesc_len(adapter, &info);

	buf = h2cb_push(h2cb, txd_len);
	if (!buf)
		return MACNPTR;

	pkt_info->qsel = TX_DESC_QSEL_H2C;
	ret = mac_build_txdesc(adapter, &info, buf, 48);
	if (ret)
		return ret;

	return MACSUCCESS;
}

#if 0 //NEO

u32 fwcmd_wq_idle(struct mac_ax_adapter *adapter, u32 id)
{
	return MACSUCCESS;
}


static u32 c2h_fwi_cmd_log(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
			   struct rtw_c2h_info *info)
{
	if (buf[len - 1] != '\0')
		buf[len - 1] = '\0';
	PLTFM_MSG_WARN("C2H log: %s", (char *)(buf + FWCMD_HDR_LEN));

	return MACSUCCESS;
}

static u32 c2h_wow_rcv_ack_hdl(struct mac_ax_adapter *adapter,
			       struct rtw_c2h_info *info)
{
	u8 *state;

	switch (info->c2h_func) {
	case FWCMD_H2C_FUNC_AOAC_REPORT_REQ:
		state = &adapter->sm.aoac_rpt;
		break;

	default:
		return MACSUCCESS;
	}

	if (*state == MAC_AX_AOAC_RPT_H2C_SENDING)
		*state = MAC_AX_AOAC_RPT_H2C_RCVD;

	return MACSUCCESS;
}

static u32 c2h_fwofld_rcv_ack_hdl(struct mac_ax_adapter *adapter,
				  struct rtw_c2h_info *info)
{
	u8 *state;

	switch (info->c2h_func) {
	case FWCMD_H2C_FUNC_WRITE_OFLD:
		state = &adapter->sm.write_h2c;
		break;

	case FWCMD_H2C_FUNC_CONF_OFLD:
		state = &adapter->sm.conf_h2c;
		break;

	case FWCMD_H2C_FUNC_PACKET_OFLD:
		state = &adapter->sm.pkt_ofld;
		break;

	case FWCMD_H2C_FUNC_READ_OFLD:
		state = &adapter->sm.read_h2c;
		break;

	case FWCMD_H2C_FUNC_DUMP_EFUSE:
		state = &adapter->sm.efuse_ofld;
		break;

	default:
		return MACSUCCESS;
	}

	if (*state == MAC_AX_OFLD_H2C_SENDING)
		*state = MAC_AX_OFLD_H2C_RCVD;

	return MACSUCCESS;
}

static u32 c2h_fwi_rev_ack(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
			   struct rtw_c2h_info *info)
{
	u32 data = *(u32 *)(buf + FWCMD_HDR_LEN);
	u32 ret;
	u32 cat;

	data = le32_to_cpu(data);

	cat = GET_FIELD(data, FWCMD_C2H_REC_ACK_CAT);
	if (cat == FWCMD_H2C_CAT_OUTSRC)
		return MACSUCCESS;

	info->c2h_cat = GET_FIELD(data, FWCMD_C2H_REC_ACK_CAT);
	info->c2h_class = GET_FIELD(data, FWCMD_C2H_REC_ACK_CLASS);
	info->c2h_func = GET_FIELD(data, FWCMD_C2H_REC_ACK_FUNC);
	info->h2c_seq = GET_FIELD(data, FWCMD_C2H_REC_ACK_H2C_SEQ);
	adapter->fw_info.rec_seq = info->h2c_seq;
	info->type_rec_ack = 1;

	if (info->c2h_cat == FWCMD_H2C_CAT_MAC) {
		switch (info->c2h_class) {
		case FWCMD_H2C_CL_WOW:
			ret = c2h_wow_rcv_ack_hdl(adapter, info);
			if (ret)
				return ret;
			break;

		case FWCMD_H2C_CL_FW_OFLD:
			ret = c2h_fwofld_rcv_ack_hdl(adapter, info);
			if (ret)
				return ret;
			break;

		default:
			return MACSUCCESS;
		}
	}

	return MACSUCCESS;
}

static u32 c2h_fwofld_done_ack_hdl(struct mac_ax_adapter *adapter,
				   struct rtw_c2h_info *info)
{
	struct mac_ax_state_mach *sm = &adapter->sm;
	struct mac_ax_pkt_ofld_info *ofld_info = &adapter->pkt_ofld_info;

	switch (info->c2h_func) {
	case FWCMD_H2C_FUNC_WRITE_OFLD:
		if (sm->write_h2c == MAC_AX_OFLD_H2C_RCVD) {
			if (info->h2c_return == MACSUCCESS)
				sm->write_h2c = MAC_AX_OFLD_H2C_IDLE;
			else
				sm->write_h2c = MAC_AX_OFLD_H2C_ERROR;
		}
		break;

	case FWCMD_H2C_FUNC_CONF_OFLD:
		if (sm->conf_h2c == MAC_AX_OFLD_H2C_RCVD) {
			if (info->h2c_return == MACSUCCESS)
				sm->conf_h2c = MAC_AX_OFLD_H2C_IDLE;
			else
				sm->conf_h2c = MAC_AX_OFLD_H2C_ERROR;
		}
		break;

	case FWCMD_H2C_FUNC_PACKET_OFLD:
		if (sm->pkt_ofld == MAC_AX_OFLD_H2C_RCVD) {
			if (info->h2c_return == MACSUCCESS) {
				if (ofld_info->last_op == PKT_OFLD_OP_READ)
					sm->pkt_ofld = MAC_AX_OFLD_H2C_DONE;
				else
					sm->pkt_ofld = MAC_AX_OFLD_H2C_IDLE;
			} else {
				sm->pkt_ofld = MAC_AX_OFLD_H2C_ERROR;
			}
		}
		break;

	case FWCMD_H2C_FUNC_READ_OFLD:
		if (sm->read_h2c == MAC_AX_OFLD_H2C_RCVD) {
			if (info->h2c_return == MACSUCCESS)
				sm->read_h2c = MAC_AX_OFLD_H2C_DONE;
			else
				sm->read_h2c = MAC_AX_OFLD_H2C_ERROR;
		}
		break;

	case FWCMD_H2C_FUNC_DUMP_EFUSE:
		if (sm->efuse_ofld == MAC_AX_OFLD_H2C_RCVD) {
			if (info->h2c_return == MACSUCCESS)
				sm->efuse_ofld = MAC_AX_OFLD_H2C_DONE;
			else
				sm->efuse_ofld = MAC_AX_OFLD_H2C_ERROR;
		}
		break;

	default:
		break;
	}

	return MACSUCCESS;
}

static u32 c2h_ps_done_ack_hdl(struct mac_ax_adapter *adapter,
				   struct rtw_c2h_info *info)
{
	struct mac_ax_state_mach *sm = &adapter->sm;

	switch (info->c2h_func) {
	case FWCMD_H2C_FUNC_P2P_ACT:
		if (sm->p2p_stat == MAC_AX_P2P_ACT_BUSY) {
			if (info->h2c_return == MACSUCCESS)
				sm->p2p_stat = MAC_AX_P2P_ACT_IDLE;
			else
				sm->p2p_stat = MAC_AX_P2P_ACT_FAIL;
		} else {
			PLTFM_MSG_ERR("[ERR]c2h ps done ack p2p stat err %d\n",
				      sm->p2p_stat);
			return MACPROCERR;
		}
		break;
	default:
		break;
	}

	return MACSUCCESS;
}

static u32 c2h_fwi_done_ack(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
			    struct rtw_c2h_info *info)
{
	u32 data = *(u32 *)(buf + FWCMD_HDR_LEN);
	u32 ret;
	u32 cat;

	data = le32_to_cpu(data);

	cat = GET_FIELD(data, FWCMD_C2H_REC_ACK_CAT);
	if (cat == FWCMD_H2C_CAT_OUTSRC)
		return MACSUCCESS;

	info->c2h_cat = GET_FIELD(data, FWCMD_C2H_DONE_ACK_CAT);
	info->c2h_class = GET_FIELD(data, FWCMD_C2H_DONE_ACK_CLASS);
	info->c2h_func = GET_FIELD(data, FWCMD_C2H_DONE_ACK_FUNC);
	info->h2c_return = GET_FIELD(data, FWCMD_C2H_DONE_ACK_H2C_RETURN);
	info->h2c_seq = GET_FIELD(data, FWCMD_C2H_DONE_ACK_H2C_SEQ);
	info->type_done_ack = 1;

	if (info->c2h_cat == FWCMD_H2C_CAT_MAC) {
		if (info->c2h_class == FWCMD_H2C_CL_FW_OFLD) {
			ret = c2h_fwofld_done_ack_hdl(adapter, info);
			if (ret != MACSUCCESS)
				return ret;
		} else if (info->c2h_class == FWCMD_H2C_CL_PS) {
			ret = c2h_ps_done_ack_hdl(adapter, info);
			if (ret != MACSUCCESS)
				return ret;
		}
	}

	return MACSUCCESS;
}

static u32 c2h_fwi_bcn_stats(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
			     struct rtw_c2h_info *info)
{
	return MACSUCCESS;
}

static struct c2h_proc_func c2h_proc_fw_info_cmd[] = {
	{FWCMD_C2H_FUNC_REC_ACK, c2h_fwi_rev_ack},
	{FWCMD_C2H_FUNC_DONE_ACK, c2h_fwi_done_ack},
	{FWCMD_C2H_FUNC_C2H_LOG, c2h_fwi_cmd_log},
	{FWCMD_C2H_FUNC_BCN_CNT, c2h_fwi_bcn_stats},
	{FWCMD_C2H_FUNC_NULL, NULL},
};

u32 c2h_fw_info(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		struct rtw_c2h_info *info)
{
	struct c2h_proc_func *proc = c2h_proc_fw_info_cmd;
	u32 (*handler)(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		       struct rtw_c2h_info *info) = NULL;
	u32 hdr0;
	u32 func;

	hdr0 = ((struct fwcmd_hdr *)buf)->hdr0;
	hdr0 = le32_to_cpu(hdr0);

	func = GET_FIELD(hdr0, C2H_HDR_FUNC);

	while (proc->id != FWCMD_C2H_FUNC_NULL) {
		if (func == proc->id) {
			handler = proc->handler;
			break;
		}
		proc++;
	}

	if (!handler) {
		PLTFM_MSG_ERR("[ERR]null func handler id: %X", func);
		return MACNOITEM;
	}

	return handler(adapter, buf, len, info);
}

static u32 c2h_dump_efuse_hdl(struct mac_ax_adapter *adapter, u8 *buf,
			      u32 len, struct rtw_c2h_info *info)
{
	struct mac_ax_efuse_ofld_info *ofld_info = &adapter->efuse_ofld_info;
	u32 size;

	if (adapter->sm.efuse_ofld != MAC_AX_OFLD_H2C_RCVD) {
		PLTFM_MSG_ERR("[ERR]not cmd sending\n");
		return MACPROCERR;
	}

	size = adapter->hw_info->efuse_size;

	if (!ofld_info->buf) {
		ofld_info->buf = (u8 *)PLTFM_MALLOC(size);
		if (!ofld_info->buf) {
			adapter->sm.efuse = MAC_AX_EFUSE_IDLE;
			return MACBUFALLOC;
		}
	}

	PLTFM_MEMCPY(ofld_info->buf, buf + FWCMD_HDR_LEN, size);

	adapter->sm.efuse_ofld = MAC_AX_OFLD_H2C_DONE;

	return MACSUCCESS;
}

static u32 c2h_read_rsp_hdl(struct mac_ax_adapter *adapter, u8 *buf,
			    u32 len, struct rtw_c2h_info *info)
{
	struct mac_ax_read_ofld_value *value_info = &adapter->read_ofld_value;
	u32 hdr1;
	u16 read_len;
	u8 *read_buff;

	if (value_info->buf)
		PLTFM_FREE(value_info->buf, value_info->len);

	hdr1 = ((struct fwcmd_hdr *)buf)->hdr1;
	hdr1 = le32_to_cpu(hdr1);

	read_len = GET_FIELD(hdr1, C2H_HDR_TOTAL_LEN) - FWCMD_HDR_LEN;

	read_buff = (u8 *)PLTFM_MALLOC(read_len);
	if (!read_buff)
		return MACBUFALLOC;

	PLTFM_MEMCPY(read_buff, buf + FWCMD_HDR_LEN, read_len);

	value_info->len = read_len;
	value_info->buf = read_buff;

	return MACSUCCESS;
}

static u32 c2h_pkt_ofld_rsp_hdl(struct mac_ax_adapter *adapter, u8 *buf,
				u32 len, struct rtw_c2h_info *info)
{
	struct mac_ax_pkt_ofld_info *ofld_info = &adapter->pkt_ofld_info;
	struct mac_ax_pkt_ofld_pkt *ofld_pkt = &adapter->pkt_ofld_pkt;
	u32 c2h_content = *(u32 *)(buf + FWCMD_HDR_LEN);
	u16 pkt_len;
	u8 id, pkt_op;
	u8 *pkt_buff;
	u8 *pkt_content;

	c2h_content = le32_to_cpu(c2h_content);

	pkt_op = GET_FIELD(c2h_content, FWCMD_C2H_PKT_OFLD_RSP_PKT_OP);
	pkt_len = GET_FIELD(c2h_content, FWCMD_C2H_PKT_OFLD_RSP_PKT_LENGTH);
	id = GET_FIELD(c2h_content, FWCMD_C2H_PKT_OFLD_RSP_PKT_ID);

	PLTFM_MSG_ERR("pkt_op: %d, pkt_len: %d, id: %d\n", pkt_op, pkt_len, id);

	switch (pkt_op) {
	case PKT_OFLD_OP_ADD:
		if (pkt_len != 0) {
			ofld_info->id_bitmap[id >> 3] |= (1 << (id & 7));
			ofld_info->free_id_count--;
			ofld_info->used_id_count++;
		}

		break;

	case PKT_OFLD_OP_DEL:
		if (pkt_len != 0) {
			ofld_info->id_bitmap[id >> 3] &= ~(1 << (id & 7));
			ofld_info->free_id_count++;
			ofld_info->used_id_count--;
		}

		break;

	case PKT_OFLD_OP_READ:
		if (pkt_len != 0) {
			if (ofld_pkt->pkt)
				PLTFM_FREE(ofld_pkt->pkt, ofld_pkt->pkt_len);

			pkt_buff = (u8 *)PLTFM_MALLOC(pkt_len);
			if (!pkt_buff)
				return MACBUFALLOC;

			pkt_content = buf + FWCMD_HDR_LEN;
			pkt_content += sizeof(struct mac_ax_pkt_ofld_hdr);
			PLTFM_MEMCPY(pkt_buff, pkt_content, pkt_len);
			ofld_pkt->pkt_id = id;
			ofld_pkt->pkt_len = pkt_len;
			ofld_pkt->pkt = pkt_buff;
		}
		break;

	default:
		PLTFM_MSG_ERR("[ERR]invalid packet offload op: %d", pkt_op);
		break;
	}

	return MACSUCCESS;
}

static u32 c2h_beacon_resend_hdl(struct mac_ax_adapter *adapter, u8 *buf,
				 u32 len, struct rtw_c2h_info *info)
{
	return MACSUCCESS;
}

static u32 c2h_macid_pause_hdl(struct mac_ax_adapter *adapter, u8 *buf,
			       u32 len, struct rtw_c2h_info *info)
{
	return MACSUCCESS;
}

static struct c2h_proc_func c2h_proc_fw_ofld_cmd[] = {
	{FWCMD_C2H_FUNC_EFUSE_DUMP, c2h_dump_efuse_hdl},
	{FWCMD_C2H_FUNC_READ_RSP, c2h_read_rsp_hdl},
	{FWCMD_C2H_FUNC_PKT_OFLD_RSP, c2h_pkt_ofld_rsp_hdl},
	{FWCMD_C2H_FUNC_BEACON_RESEND, c2h_beacon_resend_hdl},
	{FWCMD_C2H_FUNC_MACID_PAUSE, c2h_macid_pause_hdl},
	{FWCMD_C2H_FUNC_NULL, NULL},
};

u32 c2h_fw_ofld(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		struct rtw_c2h_info *info)
{
	struct c2h_proc_func *proc = c2h_proc_fw_ofld_cmd;
	u32 (*handler)(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		       struct rtw_c2h_info *info) = NULL;
	u32 hdr0;
	u32 func;

	hdr0 = ((struct fwcmd_hdr *)buf)->hdr0;
	hdr0 = le32_to_cpu(hdr0);

	func = GET_FIELD(hdr0, C2H_HDR_FUNC);

	while (proc->id != FWCMD_C2H_FUNC_NULL) {
		if (func == proc->id) {
			handler = proc->handler;
			break;
		}
		proc++;
	}

	if (!handler) {
		PLTFM_MSG_ERR("[ERR]null func handler id: %X", func);
		return MACNOITEM;
	}

	return handler(adapter, buf, len, info);
}

u32 c2h_twt(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
	    struct rtw_c2h_info *info)
{
	return MACSUCCESS;
}

u32 c2h_wow_aoac_report_hdl(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
			    struct rtw_c2h_info *info)
{
	struct mac_ax_wowlan_info *wowlan_info = &adapter->wowlan_info;
	u8 *c2h_content = buf + FWCMD_HDR_LEN;
	u8 *rpt_buff;

	if (adapter->sm.aoac_rpt != MAC_AX_AOAC_RPT_H2C_RCVD)
		return MACPROCERR;

	if (wowlan_info->aoac_report) {
		PLTFM_FREE(wowlan_info->aoac_report,
			   sizeof(struct mac_ax_aoac_report));
	}

	rpt_buff = (u8 *)PLTFM_MALLOC(sizeof(struct mac_ax_aoac_report));
	if (!rpt_buff) {
		adapter->sm.aoac_rpt = MAC_AX_AOAC_RPT_ERROR;
		return MACBUFALLOC;
	}

	PLTFM_MEMCPY(rpt_buff, c2h_content, sizeof(struct mac_ax_aoac_report));
	wowlan_info->aoac_report = rpt_buff;

	adapter->sm.aoac_rpt = MAC_AX_AOAC_RPT_H2C_DONE;

	return MACSUCCESS;
}

static struct c2h_proc_func c2h_proc_wow_cmd[] = {
	{FWCMD_C2H_FUNC_AOAC_REPORT, c2h_wow_aoac_report_hdl},
	{FWCMD_C2H_FUNC_NULL, NULL},
};

u32 c2h_wow(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
	    struct rtw_c2h_info *info)
{
	struct c2h_proc_func *proc = c2h_proc_wow_cmd;
	u32 (*handler)(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		       struct rtw_c2h_info *info) = NULL;
	u32 hdr0;
	u32 func;

	hdr0 = ((struct fwcmd_hdr *)buf)->hdr0;
	hdr0 = le32_to_cpu(hdr0);

	func = GET_FIELD(hdr0, C2H_HDR_FUNC);

	while (proc->id != FWCMD_C2H_FUNC_NULL) {
		if (func == proc->id) {
			handler = proc->handler;
			break;
		}
		proc++;
	}

	if (!handler) {
		PLTFM_MSG_ERR("[ERR]null func handler id: %X", func);
		return MACNOITEM;
	}

	return handler(adapter, buf, len, info);
}

u32 c2h_mcc_rcv_ack_hdl(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
			struct rtw_c2h_info *info)
{
	struct mac_ax_state_mach *sm = &adapter->sm;
	u32 c2h_content = *(u32 *)(buf + FWCMD_HDR_LEN);
	u8 group, h2c_func;

	c2h_content = le32_to_cpu(c2h_content);
	group = GET_FIELD(c2h_content, FWCMD_C2H_MCC_RCV_ACK_GROUP);
	h2c_func = GET_FIELD(c2h_content, FWCMD_C2H_MCC_RCV_ACK_H2C_FUNC);

	if (h2c_func <= FWCMD_H2C_FUNC_RESET_MCC_GROUP) {
		PLTFM_MSG_TRACE("[TRACE]%s: MCC group H2C rcv ack\n",
				__func__);

		if (sm->mcc_group[group] == MAC_AX_MCC_STATE_H2C_SENT) {
			sm->mcc_group[group] = MAC_AX_MCC_STATE_H2C_RCVD;

			PLTFM_MSG_TRACE("[TRACE]%s: MCC group %d state: %d\n",
					__func__, group,
					MAC_AX_MCC_STATE_H2C_RCVD);
		}
	} else if (h2c_func <= FWCMD_H2C_FUNC_MCC_SET_DURATION) {
		PLTFM_MSG_TRACE("[TRACE]%s: MCC request H2C rcv ack\n",
				__func__);

		if (sm->mcc_request[group] == MAC_AX_MCC_REQ_H2C_SENT) {
			sm->mcc_request[group] = MAC_AX_MCC_REQ_H2C_RCVD;

			PLTFM_MSG_TRACE("[TRACE]%s: MCC group %d state: %d\n",
					__func__, group,
					MAC_AX_MCC_REQ_H2C_RCVD);
		}
	} else {
		PLTFM_MSG_ERR("[ERR]%s: invalid MCC H2C func %d\n",
			      __func__, h2c_func);
		return MACNOITEM;
	}

	return MACSUCCESS;
}

u32 c2h_mcc_req_ack_hdl(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
			struct rtw_c2h_info *info)
{
	struct mac_ax_state_mach *sm = &adapter->sm;
	u32 c2h_content = *(u32 *)(buf + FWCMD_HDR_LEN);
	u8 group, h2c_func, h2c_return;

	c2h_content = le32_to_cpu(c2h_content);
	group = GET_FIELD(c2h_content, FWCMD_C2H_MCC_REQ_ACK_GROUP);
	h2c_func = GET_FIELD(c2h_content, FWCMD_C2H_MCC_REQ_ACK_H2C_FUNC);
	h2c_return = GET_FIELD(c2h_content, FWCMD_C2H_MCC_REQ_ACK_H2C_RETURN);

	PLTFM_MSG_TRACE("[TRACE]%s: group: %d, h2c_func: %d, h2c_return: %d\n",
			__func__, group, h2c_func, h2c_return);

	if (h2c_func < FWCMD_H2C_FUNC_MCC_REQ_TSF) {
		PLTFM_MSG_ERR("[ERR]%s: invalid MCC H2C func: %d\n",
			      __func__, h2c_func);
		return MACNOITEM;
	}

	PLTFM_MSG_TRACE("[TRACE]%s: group %d curr req state: %d\n",
			__func__, group, sm->mcc_request[group]);

	if (sm->mcc_request[group] == MAC_AX_MCC_REQ_H2C_RCVD) {
		if (h2c_return == 0) {
			if (h2c_func == FWCMD_H2C_FUNC_MCC_REQ_TSF)
				sm->mcc_request[group] = MAC_AX_MCC_REQ_DONE;
			else
				sm->mcc_request[group] = MAC_AX_MCC_REQ_IDLE;
		} else {
			sm->mcc_request[group] = MAC_AX_MCC_REQ_FAIL;
			PLTFM_MSG_ERR("[ERR]%s: MCC H2C func %d fail: %d\n",
				      __func__, h2c_func, h2c_return);
		}
	}

	return MACSUCCESS;
}

u32 c2h_mcc_tsf_rpt_hdl(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
			struct rtw_c2h_info *info)
{
	struct mac_ax_mcc_group_info *mcc_info = &adapter->mcc_group_info;
	struct fwcmd_mcc_tsf_rpt *tsf_rpt;
	u32 c2h_content;
	u32 tsf;
	u8 macid_x, macid_y, group;

	PLTFM_MSG_TRACE("[TRACE]%s: mcc tsf report received\n", __func__);

	tsf_rpt = (struct fwcmd_mcc_tsf_rpt *)(buf + FWCMD_HDR_LEN);

	c2h_content = tsf_rpt->dword0;
	c2h_content = le32_to_cpu(c2h_content);
	group = GET_FIELD(c2h_content, FWCMD_C2H_MCC_TSF_RPT_GROUP);
	macid_x = GET_FIELD(c2h_content, FWCMD_C2H_MCC_TSF_RPT_MACID_X);
	macid_y = GET_FIELD(c2h_content, FWCMD_C2H_MCC_TSF_RPT_MACID_Y);

	PLTFM_MSG_TRACE("[TRACE]%s: group: %d, macid_x: %d, macid_y: %d\n",
			__func__, group, macid_x, macid_y);

	mcc_info->groups[group].macid_x = macid_x;
	mcc_info->groups[group].macid_y = macid_y;

	tsf = tsf_rpt->dword1;
	tsf = le32_to_cpu(tsf);
	mcc_info->groups[group].tsf_x_low = tsf;

	tsf = tsf_rpt->dword2;
	tsf = le32_to_cpu(tsf);
	mcc_info->groups[group].tsf_x_high = tsf;

	tsf = tsf_rpt->dword3;
	tsf = le32_to_cpu(tsf);
	mcc_info->groups[group].tsf_y_low = tsf;

	tsf = tsf_rpt->dword4;
	tsf = le32_to_cpu(tsf);
	mcc_info->groups[group].tsf_y_high = tsf;

	PLTFM_MSG_TRACE("[TRACE]%s: tsf_x_high: 0x%x, tsf_x_low: 0x%x\n",
			__func__, mcc_info->groups[group].tsf_x_high,
			mcc_info->groups[group].tsf_x_low);

	PLTFM_MSG_TRACE("[TRACE]%s: tsf_y_high: 0x%x, tsf_y_low: 0x%x\n",
			__func__, mcc_info->groups[group].tsf_y_high,
			mcc_info->groups[group].tsf_y_low);

	return MACSUCCESS;
}

u32 c2h_mcc_status_rpt_hdl(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
			   struct rtw_c2h_info *info)
{
	struct mac_ax_mcc_group_info *mcc_info = &adapter->mcc_group_info;
	struct mac_ax_state_mach *sm = &adapter->sm;
	struct fwcmd_mcc_status_rpt *mcc_rpt;
	u32 c2h_content;
	u32 tsf_low;
	u32 tsf_high;
	u8 group, status, macid;

	PLTFM_MSG_TRACE("[TRACE]%s: mcc status report received\n", __func__);

	mcc_rpt = (struct fwcmd_mcc_status_rpt *)(buf + FWCMD_HDR_LEN);

	c2h_content = mcc_rpt->dword0;
	tsf_low = mcc_rpt->dword1;
	tsf_high = mcc_rpt->dword2;

	c2h_content = le32_to_cpu(c2h_content);
	group = GET_FIELD(c2h_content, FWCMD_C2H_MCC_STATUS_RPT_GROUP);
	macid = GET_FIELD(c2h_content, FWCMD_C2H_MCC_STATUS_RPT_MACID);
	status = GET_FIELD(c2h_content, FWCMD_C2H_MCC_STATUS_RPT_STATUS);

	PLTFM_MSG_TRACE("[TRACE]%s: mcc group: %d, macid: %d, status: %d\n",
			__func__, group, macid, status);

	switch (status) {
	case MAC_AX_MCC_ADD_ROLE_OK:
		if (sm->mcc_group[group] == MAC_AX_MCC_STATE_H2C_RCVD) {
			sm->mcc_group[group] = MAC_AX_MCC_ADD_DONE;
			PLTFM_MSG_TRACE("[TRACE]%s: mcc group %d add done\n",
					__func__, group);
		}
		break;

	case MAC_AX_MCC_START_GROUP_OK:
		if (sm->mcc_group[group] == MAC_AX_MCC_STATE_H2C_RCVD) {
			sm->mcc_group[group] = MAC_AX_MCC_START_DONE;
			PLTFM_MSG_TRACE("[TRACE]%s: mcc group %d start done\n",
					__func__, group);
		}
		break;

	case MAC_AX_MCC_STOP_GROUP_OK:
		if (sm->mcc_group[group] == MAC_AX_MCC_STATE_H2C_RCVD) {
			sm->mcc_group[group] = MAC_AX_MCC_STOP_DONE;
			PLTFM_MSG_TRACE("[TRACE]%s: mcc group %d stop done\n",
					__func__, group);
		}
		break;

	case MAC_AX_MCC_DEL_GROUP_OK:
	case MAC_AX_MCC_RESET_GROUP_OK:
		if (sm->mcc_group[group] == MAC_AX_MCC_STATE_H2C_RCVD) {
			sm->mcc_group[group] = MAC_AX_MCC_EMPTY;
			PLTFM_MSG_TRACE("[TRACE]%s: mcc group %d empty\n",
					__func__, group);
		}
		break;

	case MAC_AX_MCC_ADD_ROLE_FAIL:
	case MAC_AX_MCC_START_GROUP_FAIL:
	case MAC_AX_MCC_STOP_GROUP_FAIL:
	case MAC_AX_MCC_DEL_GROUP_FAIL:
	case MAC_AX_MCC_RESET_GROUP_FAIL:
		if (sm->mcc_group[group] == MAC_AX_MCC_STATE_H2C_RCVD) {
			PLTFM_MSG_ERR("[ERR]%s: mcc group %d fail status: %d\n",
				      __func__, group, status);
			sm->mcc_group[group] = MAC_AX_MCC_STATE_ERROR;
		}
		break;

	default:
		break;
	}

	tsf_low = le32_to_cpu(tsf_low);
	tsf_high = le32_to_cpu(tsf_high);

	mcc_info->groups[group].rpt_status = status;
	mcc_info->groups[group].rpt_macid = macid;
	mcc_info->groups[group].rpt_tsf_low = tsf_low;
	mcc_info->groups[group].rpt_tsf_high = tsf_high;

	PLTFM_MSG_TRACE("[TRACE]%s: tsf_high: 0x%x, tsf_low: 0x%x\n",
			__func__, tsf_high, tsf_low);

	return MACSUCCESS;
}

static struct c2h_proc_func c2h_proc_mcc_cmd[] = {
	{FWCMD_C2H_FUNC_MCC_RCV_ACK, c2h_mcc_rcv_ack_hdl},
	{FWCMD_C2H_FUNC_MCC_REQ_ACK, c2h_mcc_req_ack_hdl},
	{FWCMD_C2H_FUNC_MCC_TSF_RPT, c2h_mcc_tsf_rpt_hdl},
	{FWCMD_C2H_FUNC_MCC_STATUS_RPT, c2h_mcc_status_rpt_hdl},
	{FWCMD_C2H_FUNC_NULL, NULL},
};

u32 c2h_mcc(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
	    struct rtw_c2h_info *info)
{
	struct c2h_proc_func *proc = c2h_proc_mcc_cmd;
	u32 (*handler)(struct mac_ax_adapter *adapter, u8 *buf, u32 len,
		       struct rtw_c2h_info *info) = NULL;
	u32 hdr0;
	u32 func;

	hdr0 = ((struct fwcmd_hdr *)buf)->hdr0;
	hdr0 = le32_to_cpu(hdr0);

	func = GET_FIELD(hdr0, C2H_HDR_FUNC);

	PLTFM_MSG_TRACE("[TRACE]%s: func: %d\n", __func__, func);

	while (proc->id != FWCMD_C2H_FUNC_NULL) {
		if (func == proc->id) {
			handler = proc->handler;
			break;
		}
		proc++;
	}

	if (!handler) {
		PLTFM_MSG_ERR("[ERR]%s: null func handler id: %X",
			      __func__, func);
		return MACNOITEM;
	}

	return handler(adapter, buf, len, info);
}

static struct c2h_proc_class c2h_proc_sys[] = {
#if MAC_AX_FEATURE_DBGPKG
	{FWCMD_C2H_CL_CMD_PATH, c2h_sys_cmd_path},
	{FWCMD_H2C_CL_PLAT_AUTO_TEST, c2h_sys_plat_autotest},
#if MAC_AX_FEATURE_HV
	{FWCMD_C2H_CL_FW_AUTO, c2h_sys_fw_autotest},
#endif
#endif
	{FWCMD_C2H_CL_NULL, NULL},
};

static struct c2h_proc_class c2h_proc_mac[] = {
#if 0 // NEO
	{FWCMD_C2H_CL_FW_INFO, c2h_fw_info},
	{FWCMD_C2H_CL_FW_OFLD, c2h_fw_ofld},
	{FWCMD_C2H_CL_TWT, c2h_twt},
	{FWCMD_C2H_CL_WOW, c2h_wow},
	{FWCMD_C2H_CL_MCC, c2h_mcc},
#endif // if 0
	{FWCMD_C2H_CL_NULL, NULL},
};

static inline struct c2h_proc_class *c2h_proc_sel(u8 cat)
{
	struct c2h_proc_class *proc;

	switch (cat) {
	case FWCMD_C2H_CAT_TEST:
		proc = c2h_proc_sys;
		break;
	case FWCMD_C2H_CAT_MAC:
		proc = c2h_proc_mac;
		break;
	default:
		proc = NULL;
		break;
	}

	return proc;
}


u8 c2h_field_parsing(u8 *hdr, struct rtw_c2h_info *info)
{
	u32 val;

	val = le32_to_cpu(hdr->hdr0);
	info->c2h_cat = GET_FIELD(val, C2H_HDR_CAT);
	info->c2h_class = GET_FIELD(val, C2H_HDR_CLASS);
	info->c2h_func = GET_FIELD(val, C2H_HDR_FUNC);

	val = le32_to_cpu(hdr->hdr1);
	info->content_len = GET_FIELD(val, C2H_HDR_TOTAL_LEN) -
				FWCMD_HDR_LEN;
	info->content = (u8 *)(hdr + 1);
	return MACSUCCESS;
}
#endif // if 0 NEO

void rtl8822c_c2h_handler_no_io(_adapter *adapter, u8 *pbuf, u16 len);

u32 mac_process_c2h(struct mac_adapter *adapter, u8 *buf, u32 len,
		    u8 *ret)
{
	struct rtw_hal_com_t *hal_com = (struct rtw_hal_com_t *)adapter->drv_adapter;
	struct dvobj_priv *dvobj = hal_com->drv_priv;
	_adapter *tadapter = dvobj_get_primary_adapter(dvobj);

	rtl8822c_c2h_handler_no_io(tadapter, buf, len);

#if 0 // NEO
	u8 _class_, result;
	struct c2h_proc_class *proc;
	//struct fwcmd_hdr *hdr;
	struct rtw_c2h_info *info;
	u32 (*handler)(struct mac_adapter *adapter, u8 *buf, u32 len,
		       struct rtw_c2h_info *info) = NULL;
	u8 cat;
	u32 val;


	//hdr = (struct fwcmd_hdr *)buf;

	info = (struct rtw_c2h_info *)ret;
	//val = le32_to_cpu(hdr->hdr0);

	result = c2h_field_parsing(buf, info);
	if (result) {
		RTW_INFO("%s test PLTFM_MSG_ERR\n", __func__);
		PLTFM_MSG_ERR("[ERR]parsing c2h hdr error: %X\n", val);
		return MACNOITEM;
	}

	if (GET_FIELD(val, C2H_HDR_DEL_TYPE) != FWCMD_TYPE_C2H) {
		PLTFM_MSG_ERR("[ERR]wrong fwcmd type: %X\n", val);
		return MACNOITEM;
	}

	cat = (u8)GET_FIELD(val, C2H_HDR_CAT);

	PLTFM_MSG_TRACE("[TRACE]%s: cat: %d\n", __func__, cat);

	if (cat == FWCMD_C2H_CAT_OUTSRC)
		return MACSUCCESS;

	proc = c2h_proc_sel(cat);
	if (!proc) {
		PLTFM_MSG_ERR("[ERR]wrong fwcmd cat: %X\n", val);
		return MACNOITEM;
	}

	_class_ = GET_FIELD(val, C2H_HDR_CLASS);

	PLTFM_MSG_TRACE("[TRACE]%s: class: %d\n", __func__, _class_);

	for (; proc->id != FWCMD_C2H_CL_NULL; proc++) {
		if (_class_ == proc->id) {
			handler = proc->handler;
			break;
		}
	}

	if (!handler) {
		PLTFM_MSG_ERR("[ERR]null class handler id: %X", proc->id);
		return MACNOITEM;
	}

	return handler(adapter, buf, len, info);
#endif // if 0
	return MACNOITEM;

}

#if 0 // NEO

u32 mac_outsrc_h2c_common(struct mac_ax_adapter *adapter,
			  struct rtw_g6_h2c_hdr *hdr, u32 *pvalue)
{
	u32 ret = 0;
	u8 *buf;
	#if MAC_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_outsrc_info *info;

	h2cb = h2cb_alloc(adapter, (enum h2c_buf_class)hdr->type);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, hdr->content_len);
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}
	info = (struct fwcmd_outsrc_info *)buf;
	PLTFM_MEMCPY(info->dword0, pvalue, hdr->content_len);

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_OUTSRC,
			      hdr->h2c_class,
			      hdr->h2c_func,
			      hdr->rec_ack,
			      hdr->done_ack);
	if (ret)
		goto fail;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret)
		goto fail;

	#if MAC_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret)
		goto fail;

	h2cb_free(adapter, h2cb);

	h2c_end_flow(adapter);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_fw_log_cfg(struct mac_ax_adapter *adapter,
		   struct mac_ax_fw_log *log_cfg)
{
	u32 ret = 0;
	u8 *buf;
	#if MAC_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	struct fwcmd_log_cfg *log;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_CMD);
	if (!h2cb)
		return MACNPTR;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_log_cfg));
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}
	PLTFM_MEMSET(buf, 0, sizeof(struct fwcmd_log_cfg));

	log = (struct fwcmd_log_cfg *)buf;

	log->dword0 = cpu_to_le32(SET_WORD(log_cfg->level,
					   FWCMD_H2C_LOG_CFG_DBG_LV) |
				  SET_WORD(log_cfg->output,
					   FWCMD_H2C_LOG_CFG_PATH));
	log->dword1 = cpu_to_le32(log_cfg->comp);

	log->dword2 = cpu_to_le32(log_cfg->comp_ext);

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FW_INFO,
			      FWCMD_H2C_FUNC_LOG_CFG,
			      0,
			      1);
	if (ret)
		goto fail;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret)
		goto fail;

	#if MAC_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret)
		goto fail;

	h2cb_free(adapter, h2cb);

	h2c_end_flow(adapter);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_send_bcn_h2c(struct mac_ax_adapter *adapter,
		     struct mac_ax_bcn_info *info)
{
	#if MAC_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	u8 *buf;
	struct fwcmd_bcn_upd *hdr;
	u32 ret = MACSUCCESS;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_LONG_DATA);
	if (!h2cb)
		return MACNPTR;

	hdr = (struct fwcmd_bcn_upd *)
		h2cb_put(h2cb, sizeof(struct fwcmd_bcn_upd));
	if (!hdr) {
		ret = MACNOBUF;
		goto fail;
	}

	info->grp_ie_ofst |= info->grp_ie_ofst ? BCN_GRPIE_OFST_EN : 0;

	hdr->dword0 =
		cpu_to_le32(SET_WORD(info->port,
				     FWCMD_H2C_BCN_UPD_PORT) |
			    SET_WORD(info->mbssid,
				     FWCMD_H2C_BCN_UPD_MBSSID) |
			    SET_WORD(info->band,
				     FWCMD_H2C_BCN_UPD_BAND) |
			    SET_WORD(info->grp_ie_ofst,
				     FWCMD_H2C_BCN_UPD_GRP_IE_OFST));

	hdr->dword1 =
		cpu_to_le32(SET_WORD(info->macid,
				     FWCMD_H2C_BCN_UPD_MACID) |
			    SET_WORD(info->ssn_sel,
				     FWCMD_H2C_BCN_UPD_SSN_SEL) |
			    SET_WORD(info->ssn_mode,
				     FWCMD_H2C_BCN_UPD_SSN_MODE) |
			    SET_WORD(info->rate_sel,
				     FWCMD_H2C_BCN_UPD_RATE) |
			    SET_WORD(info->txpwr,
				     FWCMD_H2C_BCN_UPD_TXPWR));

	hdr->dword2 =
		cpu_to_le32((info->txinfo_ctrl_en ?
			     FWCMD_H2C_BCN_UPD_TXINFO_CTRL_EN : 0) |
			    SET_WORD(info->ntx_path_en,
				     FWCMD_H2C_BCN_UPD_NTX_PATH_EN) |
			    SET_WORD(info->path_map_a,
				     FWCMD_H2C_BCN_UPD_PATH_MAP_A) |
			    SET_WORD(info->path_map_b,
				     FWCMD_H2C_BCN_UPD_PATH_MAP_B) |
			    SET_WORD(info->path_map_c,
				     FWCMD_H2C_BCN_UPD_PATH_MAP_C) |
			    SET_WORD(info->path_map_d,
				     FWCMD_H2C_BCN_UPD_PATH_MAP_D) |
			    (info->antsel_a ?
			     FWCMD_H2C_BCN_UPD_ANTSEL_A : 0) |
			    (info->antsel_b ?
			     FWCMD_H2C_BCN_UPD_ANTSEL_B : 0) |
			    (info->antsel_c ?
			     FWCMD_H2C_BCN_UPD_ANTSEL_C : 0) |
			    (info->antsel_d ?
			     FWCMD_H2C_BCN_UPD_ANTSEL_D : 0));

	buf = h2cb_put(h2cb, info->pld_len);
	if (!buf) {
		ret = MACNOBUF;
		goto fail;
	}

	PLTFM_MEMCPY(buf, info->pld_buf, info->pld_len);

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_FR_EXCHG,
			      FWCMD_H2C_FUNC_BCN_UPD,
			      0,
			      0);
	if (ret)
		goto fail;

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret)
		goto fail;

	#if MAC_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret)
		goto fail;

	h2cb_free(adapter, h2cb);

	h2c_end_flow(adapter);

	return MACSUCCESS;
fail:
	h2cb_free(adapter, h2cb);

	return ret;
}

u32 mac_host_getpkt_h2c(struct mac_ax_adapter *adapter, u8 macid, u8 pkttype)
{
	//temp patch, wait for h2creg api
	u32 content = 0;
	u32 ret;

	content |= FWCMD_H2CREG_FUNC_GETPKT_INFORM;
	content |= (1 << 8);
	content |= (macid << 16);
	content |= (pkttype << 24);

	ret = mac_send_h2creg(adapter, &content, 1);

	return ret;
}

#if MAC_PHL_H2C
u32 __ie_cam_set_cmd(struct mac_ax_adapter *adapter, struct rtw_h2c_pkt *h2cb,
		     struct mac_ax_ie_cam_cmd_info *info)
{
	struct fwcmd_ie_cam *cmd;
	u8 *buf;
	u32 ret = MACSUCCESS;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_ie_cam));
	if (!buf) {
		ret = MACNOBUF;
		return ret;
	}

	cmd = (struct fwcmd_ie_cam *)buf;
	cmd->dword0 =
		cpu_to_le32((info->en ? FWCMD_H2C_IE_CAM_CAM_EN : 0) |
			    (info->band ? FWCMD_H2C_IE_CAM_BAND : 0) |
			    (info->hit_en ? FWCMD_H2C_IE_CAM_HIT_FRWD_EN : 0) |
			    (info->miss_en ?
			     FWCMD_H2C_IE_CAM_MISS_FRWD_EN : 0) |
			    (info->rst ? FWCMD_H2C_IE_CAM_RST : 0) |
			    SET_WORD(info->port, FWCMD_H2C_IE_CAM_PORT) |
			    SET_WORD(info->hit_sel, FWCMD_H2C_IE_CAM_HIT_FRWD) |
			    SET_WORD(info->miss_sel,
				     FWCMD_H2C_IE_CAM_MISS_FRWD) |
			    SET_WORD(info->num, FWCMD_H2C_IE_CAM_UPD_NUM));

	buf = h2cb_put(h2cb, info->buf_len);
	if (!buf) {
		ret = MACNOBUF;
		return ret;
	}

	PLTFM_MEMCPY(buf, info->buf, info->buf_len);

	return ret;
}

#else
u32 __ie_cam_set_cmd(struct mac_ax_adapter *adapter, struct h2c_buf *h2cb,
		     struct mac_ax_ie_cam_cmd_info *info)
{
	struct fwcmd_ie_cam *cmd;
	u8 *buf;
	u32 ret = MACSUCCESS;

	buf = h2cb_put(h2cb, sizeof(struct fwcmd_ie_cam));
	if (!buf) {
		ret = MACNOBUF;
		return ret;
	}

	cmd = (struct fwcmd_ie_cam *)buf;
	cmd->dword0 =
		cpu_to_le32((info->en ? FWCMD_H2C_IE_CAM_CAM_EN : 0) |
			    (info->band ? FWCMD_H2C_IE_CAM_BAND : 0) |
			    (info->hit_en ? FWCMD_H2C_IE_CAM_HIT_FRWD_EN : 0) |
			    (info->miss_en ?
			     FWCMD_H2C_IE_CAM_MISS_FRWD_EN : 0) |
			    (info->rst ? FWCMD_H2C_IE_CAM_RST : 0) |
			    SET_WORD(info->port, FWCMD_H2C_IE_CAM_PORT) |
			    SET_WORD(info->hit_sel, FWCMD_H2C_IE_CAM_HIT_FRWD) |
			    SET_WORD(info->miss_sel,
				     FWCMD_H2C_IE_CAM_MISS_FRWD) |
			    SET_WORD(info->num, FWCMD_H2C_IE_CAM_UPD_NUM));

	buf = h2cb_put(h2cb, info->buf_len);
	if (!buf) {
		ret = MACNOBUF;
		return ret;
	}

	PLTFM_MEMCPY(buf, info->buf, info->buf_len);

	return ret;
}
#endif
u32 mac_ie_cam_upd(struct mac_ax_adapter *adapter,
		   struct mac_ax_ie_cam_cmd_info *info)
{
	#if MAC_PHL_H2C
	struct rtw_h2c_pkt *h2cb;
	#else
	struct h2c_buf *h2cb;
	#endif
	u32 ret;

	h2cb = h2cb_alloc(adapter, H2CB_CLASS_DATA);
	if (!h2cb)
		return MACNPTR;

	ret = __ie_cam_set_cmd(adapter, h2cb, info);
	if (ret) {
		PLTFM_MSG_ERR("H2C IE CAM set cmd fail %d\n", ret);
		goto fail;
	}

	ret = h2c_pkt_set_hdr(adapter, h2cb,
			      FWCMD_TYPE_H2C,
			      FWCMD_H2C_CAT_MAC,
			      FWCMD_H2C_CL_IE_CAM,
			      FWCMD_H2C_FUNC_IE_CAM, 0, 1);
	if (ret) {
		PLTFM_MSG_ERR("H2C IE CAM set hdr fail %d\n", ret);
		goto fail;
	}

	ret = h2c_pkt_build_txd(adapter, h2cb);
	if (ret) {
		PLTFM_MSG_ERR("H2C IE CAM build txd fail %d\n", ret);
		goto fail;
	}

	#if MAC_PHL_H2C
	ret = PLTFM_TX(h2cb);
	#else
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	#endif
	if (ret) {
		PLTFM_MSG_ERR("H2C IE CAM tx fail %d\n", ret);
		goto fail;
	}

fail:
	h2cb_free(adapter, h2cb);

	if (!ret)
		h2c_end_flow(adapter);

	return ret;
}

u32 mac_send_h2creg(struct mac_ax_adapter *adapter, u32 *content, u8 len)
{
#define MAC_AX_H2CREG_PCNT 5
	u32 cnt = MAC_AX_H2CREG_PCNT;
	u8 i;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	do {
		if (!MAC_REG_R8(R_AX_H2CREG_CTRL))
			break;
		PLTFM_DELAY_US(1000);
		cnt--;
	} while (cnt);

	if (!cnt) {
		PLTFM_MSG_ERR("FW does not process H2CREG\n");
		return MACPOLLTO;
	}

	for (i = 0; i < len; i++)
		MAC_REG_W32(R_AX_H2CREG_DATA0 + i * 4, content[i]);

	MAC_REG_W8(R_AX_H2CREG_CTRL, 1);

	return MACSUCCESS;
}

u32 mac_recv_c2hreg(struct mac_ax_adapter *adapter,
		    u8 *buf)
{
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u8 i;

	if (!MAC_REG_R8(R_AX_C2HREG_CTRL)) {
		PLTFM_MSG_WARN("FW does not send C2H reg\n");
		return MACSUCCESS;
	}

	for (i = 0; i < C2HREG_LEN; i = i + 4)
		*(u32 *)(buf + i) = MAC_REG_R32(R_AX_C2HREG_DATA0 + i);

	MAC_REG_W8(R_AX_C2HREG_CTRL, 0);

	return MACSUCCESS;
}

u32 mac_process_c2hreg(struct mac_ax_adapter *adapter,
		       struct mac_ax_c2hreg_info *info)
{
#define MAC_AX_C2HREG_LEN 16
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ret;
	struct fwcmd_c2hreg_hdr *hdr;

	info->id = FWCMD_C2H_FUNC_NULL;
	if (!MAC_REG_R8(R_AX_C2HREG_CTRL))
		return MACSUCCESS;

	ret = mac_recv_c2hreg(adapter, info->c2hreg);
	if (ret) {
		PLTFM_MSG_ERR("Get C2H REG fail\n");
		return ret;
	}

	hdr = (struct fwcmd_c2hreg_hdr *)info->c2hreg;
	info->id = GET_FIELD(hdr->dword0, FWCMD_C2HREG_C2HREG_HDR_FUNC);
	info->content = info->c2hreg + C2HREG_HDR_LEN;

	return MACSUCCESS;
}

#endif // if 0 NEO


