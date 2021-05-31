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

#include "fwdl.h"

#define FWDL_WAIT_CNT 400000
#define FWDL_SECTION_MAX_NUM 10
#define FWDL_SECTION_CHKSUM_LEN	8
#define FWDL_SECTION_PER_PKT_LEN 0x1000

/* NEO: halmac_fw_info.h */
/* FW bin information */
#define WLAN_FW_HDR_SIZE			64
#define WLAN_FW_HDR_CHKSUM_SIZE			8

#define WLAN_FW_HDR_VERSION			4
#define WLAN_FW_HDR_SUBVERSION			6
#define WLAN_FW_HDR_SUBINDEX			7
#define WLAN_FW_HDR_MONTH			16
#define WLAN_FW_HDR_DATE			17
#define WLAN_FW_HDR_HOUR			18
#define WLAN_FW_HDR_MIN				19
#define WLAN_FW_HDR_YEAR			20
#define WLAN_FW_HDR_MEM_USAGE			24
#define WLAN_FW_HDR_H2C_FMT_VER			28
#define WLAN_FW_HDR_DMEM_ADDR			32
#define WLAN_FW_HDR_DMEM_SIZE			36
#define WLAN_FW_HDR_IMEM_SIZE			48
#define WLAN_FW_HDR_EMEM_SIZE			52
#define WLAN_FW_HDR_EMEM_ADDR			56
#define WLAN_FW_HDR_IMEM_ADDR			60

#define TX_DESC_SIZE_88XX		48
#define PKT_OFFSET_SZ			8
#define HALMAC_DDMA_POLLING_COUNT	1000
#define BIT_MASK_DDMACH0_DLEN		0x3ffff


#define OCPBASE_TXBUF_88XX		0x18780000
#define OCPBASE_DMEM_88XX		0x00200000
#define OCPBASE_EMEM_88XX		0x00100000

#define BIT_DDMACH0_OWN			BIT(31)
#define BIT_DDMACH0_CHKSUM_EN		BIT(29)
#define BIT_DDMACH0_DA_W_DISABLE	BIT(28)
#define BIT_DDMACH0_CHKSUM_STS		BIT(27)
#define BIT_DDMACH0_DDMA_MODE		BIT(26)
#define BIT_DDMACH0_RESET_CHKSUM_STS	BIT(25)
#define BIT_DDMACH0_CHKSUM_CONT		BIT(24)

#define BIT_IMEM_DW_OK		BIT(3)
#define BIT_IMEM_CHKSUM_OK	BIT(4)
#define BIT_DMEM_DW_OK		BIT(5)
#define BIT_DMEM_CHKSUM_OK	BIT(6)

#define C2H_DEFEATURE_RSVD	0xFD

struct halmac_backup_info {
	u32 mac_register;
	u32 value;
	u8 length;
};

struct hw_info {
	u8 chip;
	u8 cut;
	u8 category;
};

struct fwld_info {
	u32 len;
	u8 *fw;
};

static u32
dl_rsvd_page_88xx(struct mac_adapter *adapter, u16 pg_addr, struct sk_buff *skb)
{
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);
	u8 restore[2];
	u8 value8;
	u8 *buf = skb->data;
	u16 rsvd_pg_head;
	u32 cnt;
	u32 size = skb->len;
	u32 ret = MACSUCCESS;

	if (size == 0) {
		PLTFM_MSG_TRACE("[TRACE]pkt size = 0\n");
		return MACBUFSZ;
	}

	pg_addr &= 0xfff;
	MAC_REG_W16(REG_FIFOPAGE_CTRL_2, (u16)(pg_addr | BIT(15)));

	value8 = MAC_REG_R8(REG_CR + 1);
	restore[0] = value8;
	value8 = (u8)(value8 | BIT(0));
	MAC_REG_W8(REG_CR + 1, value8);

	value8 = MAC_REG_R8(REG_FWHW_TXQ_CTRL + 2);
	restore[1] = value8;
	value8 = (u8)(value8 & ~(BIT(6)));
	MAC_REG_W8(REG_FWHW_TXQ_CTRL + 2, value8);

	ret = PLTFM_SEND_RSVD_PAGE(skb);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]send rvsd pg(pltfm)!!\n");
		ret = MACDLELINK;
		goto DL_RSVD_PG_END;
	}

	cnt = 1000;
	while (!(MAC_REG_R8(REG_FIFOPAGE_CTRL_2 + 1) & BIT(7))) {
		PLTFM_DELAY_US(10);
		cnt--;
		if (cnt == 0) {
			PLTFM_MSG_ERR("[ERR]bcn valid!!\n");
			ret = MACPOLLTO;
			break;
		}
	}

DL_RSVD_PG_END:
	//rsvd_pg_head = adapter->txff_alloc.rsvd_boundary;
	rsvd_pg_head = 0;
	MAC_REG_W16(REG_FIFOPAGE_CTRL_2, rsvd_pg_head | BIT(15));
	MAC_REG_W8(REG_FWHW_TXQ_CTRL + 2, restore[1]);
	MAC_REG_W8(REG_CR + 1, restore[0]);
	return ret;
}

static u32 __section_push(struct rtw_h2c_pkt *h2cb)
{
#define section_push_len 8
	h2cb->vir_data -= section_push_len;
	h2cb->vir_tail -= section_push_len;

	return MACSUCCESS;
}


static u32
send_fwpkt_88xx(struct mac_adapter *adapter, u16 pg_addr, u8 *fw_bin, u32 size)
{
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct mac_ops *mac_ops = adapter_to_mac_ops(adapter);
	struct mac_txpkt_info info;
	struct mac_pkt_data *pkt_info = &info.u.data;
	//struct rtw_h2c_pkt *h2cb;
	struct sk_buff *skb;
	u32 headsize;
	u32 pkt_len = size;
	u32 ret = MACSUCCESS;
	u8 add_pkt_offset = 0;
	int desclen;
	int len;

	/* __section_build_txd */
	memset(&info, 0, sizeof(info));
	info.type = MAC_PKT_FWDL;
	info.pktsize = size;
	desclen = mac_ops->txdesc_len(adapter, &info);
	headsize = desclen;

	// USB2 - 512 , USB3 - 1024 ? 
	if (!((size + desclen) & (512 - 1)))
		size += 1;

	if ((size + desclen) % 512 == 0)
		add_pkt_offset = 1;

	if (add_pkt_offset == 1) {
		headsize += PKT_OFFSET_SZ;
		pkt_info->offset = desclen + 8;
		pkt_info->pkt_offset = 1;
	} else {
		pkt_info->offset = desclen;
	}
	pkt_info->qsel = 0x10; // TX_DESC_QSEL_BEACON

	len = headsize + size;

	skb = dev_alloc_skb(len);
	if (unlikely(!skb))
		return -ENOMEM;

	skb_reserve(skb, headsize);
	skb_put_data(skb, fw_bin, size);
	skb_push(skb, headsize);
	memset(skb->data, 0, headsize);

	ret = mac_ops->build_txdesc(adapter, &info, skb->data, desclen);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]build txdesc!!\n");
		goto out;
	}

	ret = dl_rsvd_page_88xx(adapter, pg_addr, skb);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]dl rsvd page!!\n");
		goto out;
	}

#if 0 //NEO
	h2cb = h2cb_alloc(adapter, H2CB_CLASS_LONG_DATA);
	if (!h2cb) {
		PLTFM_MSG_ERR("[ERR]%s: ", __func__);
		PLTFM_MSG_ERR("h2cb_alloc fail\n");
		return MACNPTR;
	}

	if (!((size + TX_DESC_SIZE_88XX) & (512 - 1)))
		pkt_len += 1;

	__section_push(h2cb);
	buf = h2cb_put(h2cb, pkt_len);
	if (!buf) {
		PLTFM_MSG_ERR("[ERR]%s: ", __func__);
		PLTFM_MSG_ERR("h2cb_put fail\n");
		ret = MACNOBUF;
		goto out;
	}

	PLTFM_MEMCPY(buf, fw_bin, size);
	if (!((size + TX_DESC_SIZE_88XX) & (512 - 1)))
		buf[size] = 0;

	ret = __sections_build_txd(adapter, h2cb);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: ", __func__);
		PLTFM_MSG_ERR("__sections_build_txd fail\n");
		goto fail;
	}
	ret = PLTFM_TX(h2cb->data, h2cb->len);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: PLTFM_TX fail\n", __func__);
		goto fail;
	}
#endif //NEO

	return MACSUCCESS;
out:
	//h2cb_free(adapter, h2cb);
	dev_kfree_skb_any(skb);
	return ret;
}

static u32
iddma_en_88xx(struct mac_adapter *adapter, u32 src, u32 dest, u32 ctrl)
{
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 cnt = HALMAC_DDMA_POLLING_COUNT;

	MAC_REG_W32(REG_DDMA_CH0SA, src);
	MAC_REG_W32(REG_DDMA_CH0DA, dest);
	MAC_REG_W32(REG_DDMA_CH0CTRL, ctrl);

	while (MAC_REG_R32(REG_DDMA_CH0CTRL) & BIT_DDMACH0_OWN) {
		cnt--;
		if (cnt == 0)
			return MACDLELINK;
	}

	return MACSUCCESS;
}

static u32
iddma_dlfw_88xx(struct mac_adapter *adapter, u32 src, u32 dest, u32 len,
		u8 first)
{
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 ch0_ctrl = (u32)(BIT_DDMACH0_CHKSUM_EN | BIT_DDMACH0_OWN);
	u32 cnt;
	u32 ret;

	cnt = HALMAC_DDMA_POLLING_COUNT;
	while (MAC_REG_R32(REG_DDMA_CH0CTRL) & BIT_DDMACH0_OWN) {
		cnt--;
		if (cnt == 0) {
			PLTFM_MSG_ERR("[ERR]ch0 ready!!\n");
			return MACDLELINK;
		}
	}

	ch0_ctrl |= (len & BIT_MASK_DDMACH0_DLEN);
	if (first == 0)
		ch0_ctrl |= BIT_DDMACH0_CHKSUM_CONT;

	ret = iddma_en_88xx(adapter, src, dest, ch0_ctrl);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]iddma en!!\n");
		return MACDLELINK;
	}

	return MACSUCCESS;
}


static u32
check_fw_rdy(struct mac_adapter *adapter, u32 mem_addr)
{
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);
	u8 fw_ctrl;

	fw_ctrl = MAC_REG_R8(REG_MCUFW_CTRL);

	if (MAC_REG_R32(REG_DDMA_CH0CTRL) & BIT_DDMACH0_CHKSUM_STS) {
		if (mem_addr < OCPBASE_DMEM_88XX) {
			fw_ctrl |= BIT_IMEM_DW_OK;
			fw_ctrl &= ~BIT_IMEM_CHKSUM_OK;
			MAC_REG_W8(REG_MCUFW_CTRL, fw_ctrl);
		} else {
			fw_ctrl |= BIT_DMEM_DW_OK;
			fw_ctrl &= ~BIT_DMEM_CHKSUM_OK;
			MAC_REG_W8(REG_MCUFW_CTRL, fw_ctrl);
		}

		PLTFM_MSG_ERR("[ERR]fw chksum!!\n");

		return MACFWCHKSUM;
	}

	if (mem_addr < OCPBASE_DMEM_88XX) {
		fw_ctrl |= (BIT_IMEM_DW_OK | BIT_IMEM_CHKSUM_OK);
		MAC_REG_W8(REG_MCUFW_CTRL, fw_ctrl);
	} else {
		fw_ctrl |= (BIT_DMEM_DW_OK | BIT_DMEM_CHKSUM_OK);
		MAC_REG_W8(REG_MCUFW_CTRL, fw_ctrl);
	}

	adapter->sm.fwdl = MAC_FWDL_INIT_RDY;

	return MACSUCCESS;
}

static u32 __sections_download(struct mac_adapter *adapter, u8 *fw_bin, u32 src, u32 dest, u32 size)
{
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);
	u32 residue_len;
	u32 mem_offset;
	u32 pkt_len;
	u8 first_part;
	u32 ret;

	mem_offset = 0;
	first_part = 1;
	residue_len = size;

	while (residue_len) {
		if (residue_len >= FWDL_SECTION_PER_PKT_LEN)
			pkt_len = FWDL_SECTION_PER_PKT_LEN;
		else
			pkt_len = residue_len;

		ret = send_fwpkt_88xx(adapter, (u16)(src >> 7),
					 fw_bin + mem_offset, pkt_len);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]send fw pkt!!\n");
			goto fail;
		}

		ret = iddma_dlfw_88xx(adapter,
					 OCPBASE_TXBUF_88XX +
					 src + TX_DESC_SIZE_88XX,
					 dest + mem_offset, pkt_len,
					 first_part);
		if (ret) {
			PLTFM_MSG_ERR("[ERR]iddma dlfw!!\n");
			goto fail;
		}

		first_part = 0;
		mem_offset += pkt_len;
		residue_len -= pkt_len;
	}

	ret = check_fw_rdy(adapter, dest);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]chk fw chksum!!\n");
		goto fail;
	}

	return MACSUCCESS;
fail:
	PLTFM_MSG_ERR("[ERR]%s ret: %d\n", __func__, ret);

	return ret;
}

static u32 
start_dlfw_88xx(struct mac_adapter *adapter, u8 *fw_bin, u32 size)
{
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);
	u8 *cur_fw;
	u16 value16;
	u32 real_size;
	u32 imem_size;
	u32 dmem_size;
	u32 emem_size = 0;
	u32 addr;
	u32 ret;

	dmem_size = *((__le32 *)(fw_bin + WLAN_FW_HDR_DMEM_SIZE));
	imem_size = *((__le32 *)(fw_bin + WLAN_FW_HDR_IMEM_SIZE));
	if (0 != ((*(fw_bin + WLAN_FW_HDR_MEM_USAGE)) & BIT(4)))
		emem_size =*((__le32 *)(fw_bin + WLAN_FW_HDR_EMEM_SIZE));

	dmem_size += WLAN_FW_HDR_CHKSUM_SIZE;
	imem_size += WLAN_FW_HDR_CHKSUM_SIZE;
	if (emem_size != 0)
		emem_size += WLAN_FW_HDR_CHKSUM_SIZE;

	real_size = WLAN_FW_HDR_SIZE + dmem_size + imem_size + emem_size;
	if (size != real_size) {
		PLTFM_MSG_ERR("[ERR]size != real size!\n");
		return MACBUFSZ;
	}

	value16 = (u16)(MAC_REG_R16(REG_MCUFW_CTRL) & 0x3800);
	value16 |= BIT(0);
	MAC_REG_W16(REG_MCUFW_CTRL, value16);

	cur_fw = fw_bin + WLAN_FW_HDR_SIZE;
	addr = *((__le32 *)(fw_bin + WLAN_FW_HDR_DMEM_ADDR));
	addr &= ~BIT(31);
	ret = __sections_download(adapter, cur_fw, 0, addr, dmem_size);
	if (ret)
		return ret;

	cur_fw = fw_bin + WLAN_FW_HDR_SIZE + dmem_size;
	addr = *((__le32 *)(fw_bin + WLAN_FW_HDR_IMEM_ADDR));
	addr &= ~BIT(31);
	ret = __sections_download(adapter, cur_fw, 0, addr, imem_size);
	if (ret)
		return ret;

DLFW_EMEM:
	if (emem_size) {
		cur_fw = fw_bin + WLAN_FW_HDR_SIZE + dmem_size + imem_size;
		addr = *((__le32 *)(fw_bin + WLAN_FW_HDR_EMEM_ADDR));
		addr &= ~BIT(31);
		ret = __sections_download(adapter, cur_fw, 0, addr, emem_size);
		if (ret)
			return ret;
	}

#if 0 //NEO
	update_fw_info_88xx(adapter, fw_bin);
#endif //NEO

	return ret;
}

u32 mac_fwdl(struct mac_adapter *adapter)
{
	u8 *fw = array_8822c_nic;
	u32 len = array_length_8822c_nic;
	u32 ret;

	if (len < WLAN_FW_HDR_SIZE) {
		PLTFM_MSG_ERR("[ERR]FW size error!\n");
		return MACBUFSZ;
	}

	ret = start_dlfw_88xx(adapter, fw, len);
	if (ret) {
		PLTFM_MSG_ERR("[ERR] start_dlfw_88xx failed\n");
		goto fwdl_err;
	}

	pr_info("%s FW DL success\n", __func__);
	return MACSUCCESS;

fwdl_err:
	return ret;
}

u32 mac_enable_cpu(struct mac_adapter *adapter, u8 boot_reason, u8 dlfw)
{
	u32 val32, ret;
	u16 val16;
	u8 val8;
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);

	/* cpu io interface enable or disable */
	val8 = MAC_REG_R8(REG_RSV_CTRL + 1);
	val8 |= BIT(0);
	MAC_REG_W8(REG_RSV_CTRL + 1, val8);
	
	/* cpu enable or disable */
	val8 = MAC_REG_R8(REG_SYS_FUNC_EN + 1);
	val8 |= BIT(2);
	MAC_REG_W8(REG_SYS_FUNC_EN + 1, val8);

	adapter->sm.fwdl = MAC_FWDL_CPU_ON;

	return MACSUCCESS;
}

u32 mac_disable_cpu(struct mac_adapter *adapter)
{
	u32 val32, ret;
	u8 val8;
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);

	adapter->sm.fwdl = MAC_FWDL_IDLE;
	//todo: need to check cpu in safe state before reset CPU

	/* cpu enable or disable */
	val8 = MAC_REG_R8(REG_SYS_FUNC_EN + 1);
	val8 &= ~BIT(2);
	MAC_REG_W8(REG_SYS_FUNC_EN + 1, val8);

	/* cpu io interface enable or disable */
	val8 = MAC_REG_R8(REG_RSV_CTRL + 1);
	val8 &= ~BIT(0);
	MAC_REG_W8(REG_RSV_CTRL + 1, val8);

	return MACSUCCESS;
}

#if 0 //NEO

u32 mac_romdl(struct mac_ax_adapter *adapter, u8 *ROM, u32 ROM_addr, u32 len)
{
	u8 *content = NULL;
	u32 val32, ret;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	ret = mac_disable_cpu(adapter);
	if (ret)
		return ret;

	if (!ROM)
		return MACNOITEM;

	val32 = MAC_REG_R32(R_AX_SEC_CTRL);

	if (val32 & BIT(0)) {
		ret = __write_memory(adapter, ROM, ROM_addr, len);
		if (ret)
			return ret;
	} else {
		PLTFM_MSG_ERR("[ERR]%s: __write_memory fail\n", __func__);
		return MACSECUREON;
	}

	PLTFM_FREE(content, len);

	return MACSUCCESS;
}

u32 mac_ram_boot(struct mac_ax_adapter *adapter, u8 *fw, u32 len)
{
	u32 addr;
	u32 ret, section_num;
	struct fw_bin_info info;
	struct fwhdr_section_info *section_info;
	struct mac_ax_intf_ops *ops = adapter_to_intf_ops(adapter);

	ret = mac_disable_cpu(adapter);
	if (ret)
		goto fwdl_err;

	ret = fwhdr_parser(adapter, fw, len, &info);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: fwhdr_parser fail\n", __func__);
		goto fwdl_err;
	}

	ret = update_fw_ver(adapter, (struct fwhdr_hdr_t *)fw);
	if (ret)
		goto fwdl_err;

	section_num = info.section_num;
	section_info = info.section_info;

	while (section_num > 0) {
		ret = __write_memory(adapter, section_info->addr,
				     section_info->dladdr, section_info->len);
		if (ret)
			goto fwdl_err;

		section_info++;
		section_num--;
	}

	addr = (0xb8003000 + R_AX_CPU_BOOT_ADDR) & 0x1FFFFFFF;
	PLTFM_MSG_ERR("%s ind access 0x%X start\n", __func__, addr);
	PLTFM_MUTEX_LOCK(&adapter->hw_info->ind_access_lock);
	adapter->hw_info->ind_aces_cnt++;
	MAC_REG_W32(R_AX_FILTER_MODEL_ADDR, addr);
	MAC_REG_W32(R_AX_INDIR_ACCESS_ENTRY, (info.section_info[0].dladdr) |
					      0xA0000000);
	adapter->hw_info->ind_aces_cnt--;
	PLTFM_MUTEX_UNLOCK(&adapter->hw_info->ind_access_lock);
	PLTFM_MSG_ERR("%s ind access 0x%X end\n", __func__, addr);

	ret = mac_enable_cpu(adapter, AX_BOOT_REASON_PWR_ON, 0);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: mac_enable_cpu fail\n", __func__);
		goto fwdl_err;
	}

	PLTFM_DELAY_MS(10);

	ret = check_fw_rdy(adapter);
	if (ret) {
		PLTFM_MSG_ERR("[ERR]%s: check_fw_rdy fail\n", __func__);
		goto fwdl_err;
	}
	return MACSUCCESS;

fwdl_err:
	fwdl_fail_dump(adapter, &info, ret);

	return ret;
}

#endif //NEO


static void
pltfm_reset_88xx(struct mac_adapter *adapter)
{
	u8 value8;
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);


	value8 = MAC_REG_R8(REG_CPU_DMEM_CON + 2) & ~BIT(0);
	MAC_REG_W8(REG_CPU_DMEM_CON + 2, value8);

	value8 = MAC_REG_R8(REG_CPU_DMEM_CON + 2) | BIT(0);
	MAC_REG_W8(REG_CPU_DMEM_CON + 2, value8);
}


#define DLFW_RESTORE_REG_NUM		6
#define HALMAC_DMA_MAPPING_HIGH		3
#define BIT_FW_DW_RDY			BIT(14)
#define ILLEGAL_KEY_GROUP		0xFAAAAA00

static void
restore_mac_reg(struct mac_adapter *adapter,
		struct halmac_backup_info *info, u32 num)
{
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct halmac_backup_info *curr_info = info;
	u8 len;
	u32 i;
	u32 reg;
	u32 value;

	for (i = 0; i < num; i++) {
		reg = curr_info->mac_register;
		value = curr_info->value;
		len = curr_info->length;

		if (len == 1)
			MAC_REG_W8(reg, (u8)value);
		else if (len == 2)
			MAC_REG_W16(reg, (u16)value);
		else if (len == 4)
			MAC_REG_W32(reg, value);

		curr_info++;
	}
}


static u32
dlfw_end_flow(struct mac_adapter *adapter)
{
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);
	u16 fw_ctrl;
	u32 cnt;
	u32 ret;

	MAC_REG_W32(REG_TXDMA_STATUS, BIT(2));

	/* Check IMEM & DMEM checksum is OK or not */
	fw_ctrl = MAC_REG_R16(REG_MCUFW_CTRL);
	if ((fw_ctrl & 0x50) != 0x50)
		return MACFWCHKSUM;

	MAC_REG_W16(REG_MCUFW_CTRL, (fw_ctrl | BIT_FW_DW_RDY) & ~BIT(0));

	ret = mac_enable_cpu(adapter, BOOT_REASON_PWR_ON, 1);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]%s: mac_enable_cpu fail\n", __func__);
		return ret;
	}

	cnt = 5000;
	while (MAC_REG_R16(REG_MCUFW_CTRL) != 0xC078) {
		if (cnt == 0) {
			PLTFM_MSG_ERR("[ERR]Check 0x80 = 0xC078 fail\n");
			if ((MAC_REG_R32(REG_FW_DBG7) & 0xFFFFFF00) ==
			    ILLEGAL_KEY_GROUP) {
				PLTFM_MSG_ERR("[ERR]Key!!\n");
				return MACFWCHKSUM;
			}
			return MACFWCHKSUM;
		}
		cnt--;
		PLTFM_DELAY_US(50);
	}

	PLTFM_MSG_TRACE("[TRACE]0x80=0xC078, cnt=%d\n", cnt);
	pr_info(" %s NEO [TRACE]0x80=0xC078, cnt=%d\n", __func__, cnt);

	return MACSUCCESS;
}

u32 mac_enable_fw(struct mac_adapter *adapter)
{
	struct mac_intf_ops *ops = adapter_to_intf_ops(adapter);
	struct halmac_backup_info bckp[DLFW_RESTORE_REG_NUM];
	u8 value8;
	u32 chip_id, cv;
	u32 bckp_idx = 0;
	u32 ret = MACSUCCESS;

	/* for efuse hidden rpt */
	MAC_REG_W8(REG_C2HEVT, C2H_DEFEATURE_RSVD);

	ret = mac_disable_cpu(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]%s: mac_disable_cpu fail\n", __func__);
		return ret;
	}

	/* set HIQ to hi priority */
	bckp[bckp_idx].length = 1;
	bckp[bckp_idx].mac_register = REG_TXDMA_PQ_MAP + 1;
	bckp[bckp_idx].value = MAC_REG_R8(REG_TXDMA_PQ_MAP + 1);
	bckp_idx++;
	value8 = HALMAC_DMA_MAPPING_HIGH << 6;
	MAC_REG_W8(REG_TXDMA_PQ_MAP + 1, value8);

	/* DLFW only use HIQ, map HIQ to hi priority */
	bckp[bckp_idx].length = 1;
	bckp[bckp_idx].mac_register = REG_CR;
	bckp[bckp_idx].value = MAC_REG_R8(REG_CR);
	bckp_idx++;
	bckp[bckp_idx].length = 4;
	bckp[bckp_idx].mac_register = REG_H2CQ_CSR;
	bckp[bckp_idx].value = BIT(31);
	bckp_idx++;
	value8 = BIT(0) | BIT(2);
	MAC_REG_W8(REG_CR, value8);
	MAC_REG_W32(REG_H2CQ_CSR, BIT(31));

	/* Config hi priority queue and public priority queue page number */
	bckp[bckp_idx].length = 2;
	bckp[bckp_idx].mac_register = REG_FIFOPAGE_INFO_1;
	bckp[bckp_idx].value = MAC_REG_R16(REG_FIFOPAGE_INFO_1);
	bckp_idx++;
	bckp[bckp_idx].length = 4;
	bckp[bckp_idx].mac_register = REG_RQPN_CTRL_2;
	bckp[bckp_idx].value = MAC_REG_R32(REG_RQPN_CTRL_2) | BIT(31);
	bckp_idx++;
	MAC_REG_W16(REG_FIFOPAGE_INFO_1, 0x200);
	MAC_REG_W32(REG_RQPN_CTRL_2, bckp[bckp_idx - 1].value);

	/* Disable beacon related functions */
	value8 = MAC_REG_R8(REG_BCN_CTRL);
	bckp[bckp_idx].length = 1;
	bckp[bckp_idx].mac_register = REG_BCN_CTRL;
	bckp[bckp_idx].value = value8;
	bckp_idx++;
	value8 = (u8)((value8 & (~BIT(3))) | BIT(4));
	MAC_REG_W8(REG_BCN_CTRL, value8);

	pltfm_reset_88xx(adapter);

	ret = mac_fwdl(adapter);
	restore_mac_reg(adapter, bckp, DLFW_RESTORE_REG_NUM);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]%s: mac_enable_cpu fail\n", __func__);
		return ret;
	}

	ret = dlfw_end_flow(adapter);
	if (ret != MACSUCCESS) {
		PLTFM_MSG_ERR("[ERR]%s: dlfw_end_flow fail\n", __func__);
		return ret;
	}

	return ret;
}

