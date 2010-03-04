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

DAHDI_VERSION=2.2.0.2+2.2.0
DAHDI_NAME=dahdi-linux-complete-$(DAHDI_VERSION)
DAHDI_DIR=$(BUILD_DIR)/$(DAHDI_NAME)
DAHDI_SOURCE=$(DAHDI_NAME).tar.gz
DAHDI_SITE=http://downloads.asterisk.org/pub/telephony/dahdi-linux-complete/releases
DAHDI_UNZIP=zcat
DAHDI_SOURCES=$(SOURCES_DIR)/dahdi
LEC_SOURCES=$(SOURCES_DIR)/lec
DAHDI_MODULES_EXTRA=

ifeq ($(strip $(SF_PR1_APPLIANCE)),y)
DAHDI_MODULES_EXTRA+= wpr1
DAHDI_EXTRA_CFLAGS+= -DCONFIG_PR1_CLOCK_10
ifeq ($(strip $(SF_PACKAGE_LEC)),y)
ZARLINK_LEC=lec
ZAPTEL_EXTRA_CFLAGS+= -DECHO_CAN_FROMENV -DECHO_CAN_ZARLINK
endif
endif
ifeq ($(strip $(SF_IP04)),y)
DAHDI_MODULES_EXTRA+= wcfxs bfsi sport_interface
DAHDI_EXTRA_CFLAGS+= -DCONFIG_4FX_SPORT_INTERFACE
endif

$(DL_DIR)/$(DAHDI_SOURCE):
	mkdir -p $(TOPDIR)/$(DL_DIR)
	$(WGET) -P $(DL_DIR) $(DAHDI_SITE)/$(DAHDI_SOURCE)

$(DAHDI_DIR)/.unpacked: $(DL_DIR)/$(DAHDI_SOURCE) $(BASE_DIR)/.config
	$(DAHDI_UNZIP) $(DL_DIR)/$(DAHDI_SOURCE) | \
	tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	cd $(DAHDI_DIR); patch -p0 < $(BASE_DIR)/package/dahdi/dahdi.patch
ifeq ($(strip $(SF_IP04)),y)
	ln -sf $(DAHDI_SOURCES)/sport_interface.h $(DAHDI_DIR)/linux/drivers/dahdi/sport_interface.h
	ln -sf $(DAHDI_SOURCES)/wcfxs.h $(DAHDI_DIR)/linux/drivers/dahdi/wcfxs.h
	ln -sf $(DAHDI_SOURCES)/bfsi.h $(DAHDI_DIR)/linux/drivers/dahdi/bfsi.h
	ln -sf $(DAHDI_SOURCES)/sport_interface.c $(DAHDI_DIR)/linux/drivers/dahdi/sport_interface.c
	ln -sf $(DAHDI_SOURCES)/wcfxs.c $(DAHDI_DIR)/linux/drivers/dahdi/wcfxs.c
	ln -sf $(DAHDI_SOURCES)/fx.c $(DAHDI_DIR)/linux/drivers/dahdi/fx.c
	ln -sf $(DAHDI_SOURCES)/bfsi.c $(DAHDI_DIR)/linux/drivers/dahdi/bfsi.c
endif
ifeq ($(strip $(SF_PR1_APPLIANCE)),y)
	ln -sf $(LEC_SOURCES)/zl_wrap.h $(DAHDI_DIR)/linux/drivers/dahdi/zl_wrap.h	
	ln -sf $(DAHDI_SOURCES)/wpr1.c $(DAHDI_DIR)/linux/drivers/dahdi/wpr1.c
	ln -sf $(DAHDI_SOURCES)/wpr1.h $(DAHDI_DIR)/linux/drivers/dahdi/wpr1.h
endif
ifeq ($(strip $(SF_PACKAGE_LEC)),y)
	ln -sf $(LEC_SOURCES)/zl_wrap.c $(DAHDI_DIR)/linux/drivers/dahdi/zl_wrap.c
endif
	touch $(DAHDI_DIR)/.unpacked

$(DAHDI_DIR)/.linux: $(DAHDI_DIR)/.unpacked
        # build DAHDI kernel modules
	$(MAKE1) PWD=$(DAHDI_DIR)/linux \
		EXTRA_CFLAGS="$(DAHDI_EXTRA_CFLAGS)" \
		KSRC=$(UCLINUX_DIR)/linux-2.6.x \
		CONFIG_DAHDI_WCTDM="n" CONFIG_DAHDI_WCT1XXP="n" CONFIG_DAHDI_WCTE11XP="n" CONFIG_DAHDI_WCFXO="n" \
		CONFIG_DAHDI_PCIRADIO="n" CONFIG_DAHDI_VOICEBUS="n" CONFIG_DAHDI_WCB4XXP="n" CONFIG_DAHDI_WCT4XXP="n" \
		CONFIG_DAHDI_WCTC4XXP="n" CONFIG_DAHDI_XPP="n" CONFIG_DAHDI_TOR2="n" MODULES_EXTRA="$(DAHDI_MODULES_EXTRA)" \
		-C $(DAHDI_DIR)/linux modules
	touch $(DAHDI_DIR)/.linux

$(DAHDI_DIR)/.configured: $(DAHDI_DIR)/.linux

	cd $(DAHDI_DIR)/tools; ./configure --host=bfin-linux-uclibc --with-dahdi=$(DAHDI_DIR)/linux
	cp -rf $(DAHDI_DIR)/linux/include/dahdi/ $(STAGING_INC)
	touch $(DAHDI_DIR)/.configured


dahdi: $(ZARLINK_LEC) $(OSLEC_IN) $(DAHDI_DIR)/.configured
	mkdir -p $(TARGET_DIR)/lib/modules/$(shell ls $(TARGET_DIR)/lib/modules)/misc
	cp -f $(DAHDI_DIR)/linux/drivers/dahdi/dahdi.ko $(TARGET_DIR)/lib/modules/$(shell ls $(TARGET_DIR)/lib/modules)/misc

	cd $(DAHDI_DIR)/tools; make all

	cp $(DAHDI_DIR)/tools/tonezone.h $(STAGING_INC)/dahdi
	cp $(DAHDI_DIR)/tools/libtonezone.so $(STAGING_LIB)
	cp $(DAHDI_DIR)/tools/libtonezone.so $(TARGET_DIR)/lib
	$(TARGET_STRIP) $(TARGET_DIR)/lib/libtonezone.so
ifeq ($(strip $(SF_PR1_APPLIANCE)),y)

	cp -f $(DAHDI_DIR)/tools/dahdi_cfg $(DAHDI_DIR)/tools/dahdi_scan  $(TARGET_DIR)/bin
	cp -f $(DAHDI_DIR)/linux/drivers/dahdi/wpr1.ko \
	$(TARGET_DIR)/lib/modules/$(shell ls $(TARGET_DIR)/lib/modules)/misc
endif
ifeq ($(strip $(SF_PACKAGE_DAHDI_TDMOE)),y)
	cp -f $(DAHDI_DIR)/linux/drivers/dahdi/dahdi_dynamic_eth.ko $(DAHDI_DIR)/linux/drivers/dahdi/dahdi_dynamic_loc.ko $(DAHDI_DIR)/linux/drivers/dahdi/dahdi_dynamic.ko \
	$(TARGET_DIR)/lib/modules/$(shell ls $(TARGET_DIR)/lib/modules)/misc
endif
ifeq ($(strip $(SF_BR4_APPLIANCE)),y)
	cp -f $(DAHDI_DIR)/linux/drivers/dahdi/dahdi_dummy.ko $(TARGET_DIR)/lib/modules/$(shell ls $(TARGET_DIR)/lib/modules)/misc
endif
ifeq ($(strip $(SF_IP04)),y)
	cp -f $(DAHDI_DIR)/tools/dahdi_cfg $(DAHDI_DIR)/tools/dahdi_scan  $(TARGET_DIR)/bin
	cp -f $(DAHDI_DIR)/linux/drivers/dahdi/wcfxs.ko $(DAHDI_DIR)/linux/drivers/dahdi/sport_interface.ko \
	$(DAHDI_DIR)/linux/drivers/dahdi/bfsi.ko $(TARGET_DIR)/lib/modules/$(shell ls $(TARGET_DIR)/lib/modules)/misc
endif	

dahdi-clean:
	make -C $(DAHDI_DIR) clean
################################################
#
# Toplevel Makefile options
#
#################################################
ifeq ($(strip $(SF_PACKAGE_DAHDI)),y)
TARGETS+=dahdi
endif
