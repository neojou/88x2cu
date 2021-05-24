# All needed files would be added to _HAL_INTFS_FILES, and it would include
# hal/hal_halmac.c and all related files in directory hal/halmac/.
# Before include this makefile, be sure interface (CONFIG_*_HCI) and IC
# (CONFIG_RTL*) setting are all ready!

HAL = hal

ifeq ($(CONFIG_PHL_ARCH), y)
phl_path := phl/hal
phl_path_d1 := $(src)/phl/$(HAL)
else
phl_path := hal
phl_path_d1 := $(src)/$(HAL)
endif

# Base directory
path_hm := phl/hal/mac
# Level 1 directory
path_hm_ac := phl/hal/mac/mac_ac
path_fw_ac := phl/hal/mac/fw_ac

ifeq ($(CONFIG_PCI_HCI), y)
pci := y
endif
ifeq ($(CONFIG_SDIO_HCI), y)
sdio := y
endif
ifeq ($(CONFIG_USB_HCI), y)
usb := y
endif

halmac-y +=		$(path_hm_ac)/../mac.o

# Modify level 1 directory if needed
#			$(path_hm_d1)/fwdl.o

#halmac-y += $(path_hm_d1)/fwcmd.o
halmac-y += $(path_hm_ac)/fwcmd.o \
		$(path_hm_ac)/init.o \
		$(path_hm_ac)/trx_desc.o \
		$(path_hm_ac)/hw.o \
		$(path_hm_ac)/role.o \
		$(path_hm_ac)/security_cam.o \
		$(path_hm_ac)/efuse.o \
		$(path_hm_ac)/pwr.o \
		$(path_hm_ac)/fwdl.o \

#halmac-y +=		$(path_hm_d1)/addr_cam.o \
			$(path_hm_d1)/cmac_tx.o \
			$(path_hm_d1)/coex.o \
			$(path_hm_d1)/cpuio.o \
			$(path_hm_d1)/dbgpkg.o \
			$(path_hm_d1)/dle.o \
			$(path_hm_d1)/fwcmd.o \
			$(path_hm_d1)/fwofld.o \
			$(path_hm_d1)/gpio.o \
			$(path_hm_d1)/hci_fc.o \
			$(path_hm_d1)/hdr_conv.o \
			$(path_hm_d1)/hw_seq.o \
			$(path_hm_d1)/hwamsdu.o \
			$(path_hm_d1)/la_mode.o \
			$(path_hm_d1)/mcc.o \
			$(path_hm_d1)/mport.o \
			$(path_hm_d1)/phy_rpt.o \
			$(path_hm_d1)/power_saving.o \
			$(path_hm_d1)/p2p.o \
			$(path_hm_d1)/rx_filter.o \
			$(path_hm_d1)/rx_forwarding.o \
			$(path_hm_d1)/sounding.o \
			$(path_hm_d1)/status.o \
			$(path_hm_d1)/tblupd.o \
			$(path_hm_d1)/tcpip_checksum_offload.o \
			$(path_hm_d1)/trx_desc.o \
			$(path_hm_d1)/trxcfg.o \
			$(path_hm_d1)/twt.o \
			$(path_hm_d1)/wowlan.o 

halmac-$(pci) += 	$(path_hm_d1)/_pcie.o
halmac-$(usb) += 	$(path_hm_ac)/_usb.o
halmac-$(sdio) +=	$(path_hm_d1)/_sdio.o


ifeq ($(CONFIG_RTL8852A), y)
ic := 8852a

# Level 2 directory
path_hm_8852a := $(path_hm_d1)/mac_$(ic)

#halmac-y	+=	$(path_hm_8852a)/gpio_$(ic).o \
			$(path_hm_8852a)/init_$(ic).o \
			$(path_hm_8852a)/pwr_seq_$(ic).o

# fw files
path_fw_8852a := $(path_fw_d1)/rtl8852a

#halmac-y	+=	$(path_fw_8852a)/hal8852a_fw.o 
		
endif

ifeq ($(CONFIG_RTL8822C), y)
ic := 8822c

# Level 2 directory
path_hm_8822c := $(path_hm_ac)/mac_$(ic)

halmac-y	+=	$(path_hm_8822c)/init_$(ic).o \
			$(path_hm_8822c)/pwr_seq_$(ic).o

#halmac-y	+=	$(path_hm_8852b)/gpio_$(ic).o \
			$(path_hm_8852b)/init_$(ic).o \
			$(path_hm_8852b)/pwr_seq_$(ic).o


# fw files
path_fw_8822c := $(path_fw_ac)/rtl8822c

halmac-y	+=	$(path_fw_8822c)/hal8822c_fw.o 
		
halmac-$(usb)	+= 	$(path_hm_8822c)/_usb_8822c.o

endif

_HAL_MAC_FILES +=	$(halmac-y)
