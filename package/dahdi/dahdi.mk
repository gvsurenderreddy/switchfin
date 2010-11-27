#########################################################################
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# The Free Software Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# Copyright @ 2007, 2009 Astfin
# Primary Authors: Dimitar Penev dpn@ucpbx.com
# Copyright @ 2010 SwitchFin dpn@switchfin.org
#########################################################################
#############################################
# DAHDI package for SwitchFin.org
#############################################

DAHDI_VERSION=2.3.0.1+2.3.0
DAHDI_NAME=dahdi-linux-complete-$(DAHDI_VERSION)
DAHDI_DIR=$(BUILD_DIR)/$(DAHDI_NAME)
DAHDI_SOURCE=$(DAHDI_NAME).tar.gz
DAHDI_SITE=http://downloads.asterisk.org/pub/telephony/dahdi-linux-complete/releases
DAHDI_UNZIP=zcat
DAHDI_SOURCES=$(SOURCES_DIR)/dahdi
LEC_SOURCES=$(SOURCES_DIR)/lec
DAHDI_MODULES_EXTRA=
TARGET_KERNEL_MODULES=$(shell ls $(TARGET_DIR)/lib/modules)

ifeq ($(strip $(SF_PR1_APPLIANCE)),y)
DAHDI_MODULES_EXTRA+= wpr1
DAHDI_EXTRA_CFLAGS+= -DCONFIG_PR1_CLOCK_10
ifeq ($(strip $(SF_PACKAGE_LEC)),y)
ZARLINK_LEC=lec
DAHDI_EXTRA_CFLAGS+= -DECHO_CAN_FROMENV #-DECHO_CAN_ZARLINK
endif
endif
ifeq ($(strip $(SF_IP04)),y)
DAHDI_MODULES_EXTRA+= wcfxs bfsi sport_interface
#Keep in mind that CONFIG_4FX_SPORT_INTERFACE macro is actually not used
#SPI/SPORT are switched by the definition of CONFIG_4FX_SPI_INTERFACE
DAHDI_EXTRA_CFLAGS+= -DCONFIG_4FX_SPORT_INTERFACE -DSF_IP04
endif
ifeq ($(strip $(SF_IP01)),y)
DAHDI_MODULES_EXTRA+= wcfxs bfsi sport_interface
DAHDI_EXTRA_CFLAGS+= -DCONFIG_4FX_SPI_INTERFACE -DSF_IP01
endif

$(DL_DIR)/$(DAHDI_SOURCE):
	mkdir -p $(TOPDIR)/$(DL_DIR)
	$(WGET) -P $(DL_DIR) $(DAHDI_SITE)/$(DAHDI_SOURCE)

$(DAHDI_DIR)/.unpacked: $(DL_DIR)/$(DAHDI_SOURCE) $(BASE_DIR)/.config
	$(DAHDI_UNZIP) $(DL_DIR)/$(DAHDI_SOURCE) | \
	tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	mkdir -p $(DAHDI_DIR)/linux/drivers/staging/
	cp -rf package/sources/oslec $(DAHDI_DIR)/linux/drivers/staging/echo
	cd $(DAHDI_DIR); patch -p0 < $(BASE_DIR)/package/dahdi/dahdi.patch
	cp -f $(LEC_SOURCES)/dahdi_echocan_zarlink.c $(DAHDI_DIR)/linux/drivers/dahdi/dahdi_echocan_zarlink.c

ifeq ($(strip $(SF_IP04)),y)
	cp -f $(DAHDI_SOURCES)/sport_interface.h $(DAHDI_DIR)/linux/drivers/dahdi/sport_interface.h
	cp -f $(DAHDI_SOURCES)/wcfxs.h $(DAHDI_DIR)/linux/drivers/dahdi/wcfxs.h
	cp -f $(DAHDI_SOURCES)/bfsi.h $(DAHDI_DIR)/linux/drivers/dahdi/bfsi.h
	cp -f $(DAHDI_SOURCES)/sport_interface.c $(DAHDI_DIR)/linux/drivers/dahdi/sport_interface.c
	cp -f $(DAHDI_SOURCES)/wcfxs.c $(DAHDI_DIR)/linux/drivers/dahdi/wcfxs.c
	cp -f $(DAHDI_SOURCES)/fx.c $(DAHDI_DIR)/linux/drivers/dahdi/fx.c
	cp -f $(DAHDI_SOURCES)/bfsi.c $(DAHDI_DIR)/linux/drivers/dahdi/bfsi.c
endif

ifeq ($(strip $(SF_IP01)),y)
	cp -f $(DAHDI_SOURCES)/sport_interface.h $(DAHDI_DIR)/linux/drivers/dahdi/sport_interface.h
	cp -f $(DAHDI_SOURCES)/wcfxs.h $(DAHDI_DIR)/linux/drivers/dahdi/wcfxs.h
	cp -f $(DAHDI_SOURCES)/bfsi.h $(DAHDI_DIR)/linux/drivers/dahdi/bfsi.h
	cp -f $(DAHDI_SOURCES)/sport_interface.c $(DAHDI_DIR)/linux/drivers/dahdi/sport_interface.c
	cp -f $(DAHDI_SOURCES)/wcfxs.c $(DAHDI_DIR)/linux/drivers/dahdi/wcfxs.c
	cp -f $(DAHDI_SOURCES)/fx-ip01.c $(DAHDI_DIR)/linux/drivers/dahdi/fx.c
	cp -f $(DAHDI_SOURCES)/bfsi.c $(DAHDI_DIR)/linux/drivers/dahdi/bfsi.c
endif

ifeq ($(strip $(SF_PACKAGE_DAHDI_GSM1)),y)
	patch -d $(DAHDI_DIR) -p1 < package/sources/dahdi/dahdi-gsm1.patch
	cp -f $(DAHDI_SOURCES)/wcfxs-gsm1.c $(DAHDI_DIR)/linux/drivers/dahdi/wcfxs.c
	cp -f $(DAHDI_SOURCES)/wcfxs-gsm1.h $(DAHDI_DIR)/linux/drivers/dahdi/wcfxs.h
	cp -f $(DAHDI_SOURCES)/gsm-sms.c $(DAHDI_DIR)/tools/

ifeq ($(strip $(SF_IP01)),y)
	cp -f $(DAHDI_SOURCES)/gsm_module-ip01.c $(DAHDI_DIR)/linux/drivers/dahdi/gsm_module.c
else
	cp -f $(DAHDI_SOURCES)/gsm_module.c $(DAHDI_DIR)/linux/drivers/dahdi/gsm_module.c
endif
endif

ifeq ($(strip $(SF_PR1_APPLIANCE)),y)
	cp -f $(DAHDI_SOURCES)/wpr1.c $(DAHDI_DIR)/linux/drivers/dahdi/wpr1.c
	cp -f $(DAHDI_SOURCES)/wpr1.h $(DAHDI_DIR)/linux/drivers/dahdi/wpr1.h
endif

	touch $(DAHDI_DIR)/.unpacked

$(DAHDI_DIR)/.linux: $(DAHDI_DIR)/.unpacked
        # build DAHDI kernel modules
	$(MAKE1) PWD=$(DAHDI_DIR)/linux \
		EXTRA_CFLAGS="$(DAHDI_EXTRA_CFLAGS)" \
		KSRC=$(UCLINUX_DIR)/linux-2.6.x \
		CONFIG_DAHDI_WCTDM="n" CONFIG_DAHDI_WCT1XXP="n" CONFIG_DAHDI_WCTE11XP="n" CONFIG_DAHDI_WCFXO="n" \
		CONFIG_DAHDI_PCIRADIO="n" CONFIG_DAHDI_VOICEBUS="n" CONFIG_DAHDI_WCB4XXP="n" CONFIG_DAHDI_WCT4XXP="n" \
		CONFIG_DAHDI_WCTC4XXP="n" CONFIG_DAHDI_XPP="n" CONFIG_DAHDI_TOR2="n" CONFIG_FW_LOADER="n" \
		MODULES_EXTRA="$(DAHDI_MODULES_EXTRA)" -C $(DAHDI_DIR)/linux modules
	touch $(DAHDI_DIR)/.linux

$(DAHDI_DIR)/.configured: $(DAHDI_DIR)/.linux

	cd $(DAHDI_DIR)/tools; ./configure --host=bfin-linux-uclibc --with-dahdi=$(DAHDI_DIR)/linux
	cp -rf $(DAHDI_DIR)/linux/include/dahdi/ $(STAGING_INC)
	touch $(DAHDI_DIR)/.configured


dahdi: $(ZARLINK_LEC) $(OSLEC_IN) $(DAHDI_DIR)/.configured
	mkdir -p $(TARGET_DIR)/lib/modules/$(TARGET_KERNEL_MODULES)/misc
	cp -f $(DAHDI_DIR)/linux/drivers/dahdi/dahdi.ko $(TARGET_DIR)/lib/modules/$(TARGET_KERNEL_MODULES)/misc

	cd $(DAHDI_DIR)/tools; make all

	cp $(DAHDI_DIR)/tools/tonezone.h $(STAGING_INC)/dahdi
	cp $(DAHDI_DIR)/tools/libtonezone.so $(STAGING_LIB)
	cp $(DAHDI_DIR)/tools/libtonezone.so $(TARGET_DIR)/lib
	$(TARGET_STRIP) $(TARGET_DIR)/lib/libtonezone.so

ifeq ($(strip $(SF_PR1_APPLIANCE)),y)
	cp -f $(DAHDI_DIR)/tools/dahdi_cfg $(DAHDI_DIR)/tools/dahdi_scan  $(TARGET_DIR)/bin
	cp -f $(DAHDI_DIR)/linux/drivers/dahdi/wpr1.ko $(TARGET_DIR)/lib/modules/$(TARGET_KERNEL_MODULES)/misc
	cp -f $(DAHDI_DIR)/linux/drivers/dahdi/dahdi_echocan_zarlink.ko $(TARGET_DIR)/lib/modules/$(TARGET_KERNEL_MODULES)/misc
endif
ifeq ($(strip $(SF_PACKAGE_DAHDI_TDMOE)),y)
	cp -f $(DAHDI_DIR)/linux/drivers/dahdi/dahdi_dynamic_eth.ko $(DAHDI_DIR)/linux/drivers/dahdi/dahdi_dynamic_loc.ko $(DAHDI_DIR)/linux/drivers/dahdi/dahdi_dynamic.ko \
	$(TARGET_DIR)/lib/modules/$(TARGET_KERNEL_MODULES)/misc
endif
ifeq ($(strip $(SF_BR4_APPLIANCE)),y)
	cp -f $(DAHDI_DIR)/linux/drivers/dahdi/dahdi_dummy.ko $(TARGET_DIR)/lib/modules/$(TARGET_KERNEL_MODULES)/misc
endif
ifeq ($(strip $(SF_IP04)),y)
	cp -f $(DAHDI_DIR)/tools/dahdi_cfg $(DAHDI_DIR)/tools/dahdi_scan  $(TARGET_DIR)/bin
	cp -f $(DAHDI_DIR)/linux/drivers/dahdi/wcfxs.ko $(DAHDI_DIR)/linux/drivers/dahdi/sport_interface.ko \
	$(DAHDI_DIR)/linux/drivers/dahdi/bfsi.ko $(TARGET_DIR)/lib/modules/$(TARGET_KERNEL_MODULES)/misc
	cp -f $(DAHDI_DIR)/linux/drivers/dahdi/dahdi_dummy.ko $(TARGET_DIR)/lib/modules/$(TARGET_KERNEL_MODULES)/misc
	cp -f $(DAHDI_DIR)/linux/drivers/staging/echo/echo.ko $(TARGET_DIR)/lib/modules/$(TARGET_KERNEL_MODULES)/misc
	cp -f $(DAHDI_DIR)/linux/drivers/dahdi/dahdi_echocan_oslec.ko $(TARGET_DIR)/lib/modules/$(TARGET_KERNEL_MODULES)/misc
ifeq ($(strip $(SF_PACKAGE_DAHDI_GSM1)),y)
	cp -f $(DAHDI_DIR)/tools/gsm-sms $(TARGET_DIR)/bin
else
	rm -f $(TARGET_DIR)/bin/gsm-sms
endif
endif	
ifeq ($(strip $(SF_IP01)),y)
	cp -f $(DAHDI_DIR)/tools/dahdi_cfg $(DAHDI_DIR)/tools/dahdi_scan  $(TARGET_DIR)/bin
	cp -f $(DAHDI_DIR)/linux/drivers/dahdi/wcfxs.ko $(DAHDI_DIR)/linux/drivers/dahdi/sport_interface.ko \
	$(DAHDI_DIR)/linux/drivers/dahdi/bfsi.ko $(TARGET_DIR)/lib/modules/$(TARGET_KERNEL_MODULES)/misc
	cp -f $(DAHDI_DIR)/linux/drivers/dahdi/dahdi_dummy.ko $(TARGET_DIR)/lib/modules/$(TARGET_KERNEL_MODULES)/misc
	cp -f $(DAHDI_DIR)/linux/drivers/staging/echo/echo.ko $(TARGET_DIR)/lib/modules/$(TARGET_KERNEL_MODULES)/misc
	cp -f $(DAHDI_DIR)/linux/drivers/dahdi/dahdi_echocan_oslec.ko $(TARGET_DIR)/lib/modules/$(TARGET_KERNEL_MODULES)/misc
ifeq ($(strip $(SF_PACKAGE_DAHDI_GSM1)),y)
	cp -f $(DAHDI_DIR)/tools/gsm-sms $(TARGET_DIR)/bin
else
	rm -f $(TARGET_DIR)/bin/gsm-sms
endif
endif
ifeq ($(strip $(SF_PACKAGE_DAHDI_EXTRATOOLS)),y)
	cp -f $(DAHDI_DIR)/tools/dahdi_maint $(DAHDI_DIR)/tools/dahdi_monitor $(DAHDI_DIR)/tools/dahdi_speed \
	$(DAHDI_DIR)/tools/dahdi_test $(DAHDI_DIR)/tools/fxotune $(TARGET_DIR)/bin
else
	rm -f $(TARGET_DIR)/bin/dahdi_maint $(TARGET_DIR)/bin/dahdi_monitor $(TARGET_DIR)/bin/dahdi_speed \
	$(TARGET_DIR)/bin/dahdi_test $(TARGET_DIR)/bin/fxotune
endif

dahdi-clean:
	make -C $(DAHDI_DIR) clean
################################################
#
# Toplevel Makefile options
#
#################################################
