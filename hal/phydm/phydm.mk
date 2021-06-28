EXTRA_CFLAGS += -I$(src)/hal/phydm

_PHYDM_FILES := hal/phydm/phydm_debug.o	\
								hal/phydm/phydm_antdiv.o\
								hal/phydm/phydm_smt_ant.o\
								hal/phydm/phydm_antdect.o\
								hal/phydm/phydm_interface.o\
								hal/phydm/phydm_phystatus.o\
								hal/phydm/phydm_hwconfig.o\
								hal/phydm/phydm.o\
								hal/phydm/phydm_dig.o\
								hal/phydm/phydm_pathdiv.o\
								hal/phydm/phydm_rainfo.o\
								hal/phydm/phydm_dynamictxpower.o\
								hal/phydm/phydm_adaptivity.o\
								hal/phydm/phydm_cfotracking.o\
								hal/phydm/phydm_noisemonitor.o\
								hal/phydm/phydm_direct_bf.o\
								hal/phydm/phydm_dfs.o\
								hal/phydm/txbf/phydm_hal_txbf_api.o\
								hal/phydm/phydm_adc_sampling.o\
								hal/phydm/phydm_ccx.o\
								hal/phydm/phydm_psd.o\
								hal/phydm/phydm_primary_cca.o\
								hal/phydm/phydm_cck_pd.o\
								hal/phydm/phydm_rssi_monitor.o\
								hal/phydm/phydm_auto_dbg.o\
								hal/phydm/phydm_math_lib.o\
								hal/phydm/phydm_api.o\
								hal/phydm/phydm_pow_train.o\
								hal/phydm/phydm_pmac_tx_setting.o\
								hal/phydm/phydm_mp.o\
								hal/phydm/phydm_cck_rx_pathdiv.o\
								hal/phydm/halrf/halrf.o\
								hal/phydm/halrf/halrf_debug.o\
								hal/phydm/halrf/halphyrf_ce.o\
								hal/phydm/halrf/halrf_powertracking_ce.o\
								hal/phydm/halrf/halrf_powertracking.o\
								hal/phydm/halrf/halrf_kfree.o\
								hal/phydm/halrf/halrf_psd.o
		
ifeq ($(CONFIG_RTL8822C), y)
RTL871X = rtl8822c
_PHYDM_FILES += hal/phydm/$(RTL871X)/halhwimg8822c_bb.o\
								hal/phydm/$(RTL871X)/phydm_hal_api8822c.o\
								hal/phydm/$(RTL871X)/phydm_regconfig8822c.o\
								hal/phydm/$(RTL871X)/phydm_rtl8822c.o\
								hal/phydm/halrf/$(RTL871X)/halrf_8822c.o\
								hal/phydm/halrf/$(RTL871X)/halrf_iqk_8822c.o\
								hal/phydm/halrf/$(RTL871X)/halrf_tssi_8822c.o\
								hal/phydm/halrf/$(RTL871X)/halrf_dpk_8822c.o\
								hal/phydm/halrf/$(RTL871X)/halrf_txgapk_8822c.o\
								hal/phydm/halrf/$(RTL871X)/halhwimg8822c_rf.o
endif
