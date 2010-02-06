#########################################################################
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# The Free Software Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# Copyright @ 2007, 2008 Astfin
# Primary Authors: mark@astfin.org, pawel@astfin.org, david@rowetel.com
# Update 27 July 2009, Dimitar Penev dpn@ucpbx.com
#        uClinux 2009R1-RC2 (kernel 2.6.28.10)
#        blackfin toolchain 2009R1-RC7
# Copyright @ 2010 SwitchFin <dpn@switchvoice.com>
#########################################################################
#############################################
# Zaptel package for Astfin.org
#############################################
ifeq ($(strip $(SF_CHUNKSIZE_1)),y)
SF_CHUNKSIZE=-DSF_CHUNKSIZE=1
endif
ifeq ($(strip $(SF_CHUNKSIZE_2)),y)
SF_CHUNKSIZE=-DSF_CHUNKSIZE=2
endif
ifeq ($(strip $(SF_CHUNKSIZE_5)),y)
SF_CHUNKSIZE=-DSF_CHUNKSIZE=5
endif
ifndef (SF_CHUNKSIZE)
SF_CHUNKSIZE=-DSF_CHUNKSIZE=1
endif

ifeq ($(strip $(SF_PACKAGE_ZAPTEL_OSLEC)),y)
ZAPTEL_VERSION=1.4.9.2
else
ZAPTEL_VERSION=1.4.10.1
ZAPTEL_EXTRA_CFLAGS+=$(SF_CHUNKSIZE)
endif
ZAPTEL_NAME=zaptel-$(ZAPTEL_VERSION)
ZAPTEL_DIR=$(BUILD_DIR)/$(ZAPTEL_NAME)
ZAPTEL_SOURCE=$(ZAPTEL_NAME).tar.gz
ZAPTEL_SITE=http://downloads.digium.com/pub/zaptel/releases
ZAPTEL_UNZIP=zcat
OSLEC_DIR=$(BUILD_DIR)/oslec
ZAPTEL_CONFIG=$(UCLINUX_KERNEL_SRC)/.config
ZAPTEL_LOCATION=$(UCLINUX_KERNEL_SRC)/drivers/zaptel
ZAPTEL_EXTRA_CFLAGS+=-DSTANDALONE_ZAPATA
ZAPTEL_SOURCES=$(SOURCES_DIR)/zaptel

ZAPTEL_TOPDIR_MODULES:="zaptel bfsi wcfxs sport_interface wpr1 ztdummy ztd-eth ztd-loc ztdynamic"
ifeq ($(strip $(SF_IP04)),y)
ZAPTEL_EXTRA_CFLAGS+=-DCONFIG_CALC_XLAW
endif
ifeq ($(strip $(SF_FX08)),y)
ZAPTEL_EXTRA_CFLAGS+=-DCONFIG_CALC_XLAW
endif
ifeq ($(strip $(SF_IP01)),y)
ZAPTEL_EXTRA_CFLAGS+=-DCONFIG_CALC_XLAW
endif
ifeq ($(strip $(SF_PACKAGE_ZAPTEL_IP04_SPI_SPI)),y)
ZAPTEL_EXTRA_CFLAGS+=-DCONFIG_4FX_SPI_INTERFACE
else
ifeq ($(strip $(SF_IP01)),y)
ZAPTEL_EXTRA_CFLAGS+=-DCONFIG_4FX_SPI_INTERFACE
else
ZAPTEL_EXTRA_CFLAGS+=-DCONFIG_4FX_SPORT_INTERFACE
endif
endif

ifeq ($(strip $(SF_PACKAGE_ZAPTEL_PR1_CLOCK_2)),y)
ZAPTEL_EXTRA_CFLAGS+=-DCONFIG_PR1_CLOCK_2
endif
ifeq ($(strip $(SF_PACKAGE_ZAPTEL_PR1_CLOCK_8)),y)
ZAPTEL_EXTRA_CFLAGS+=-DCONFIG_PR1_CLOCK_8
endif
ifeq ($(strip $(SF_PACKAGE_ZAPTEL_PR1_CLOCK_10)),y)
ZAPTEL_EXTRA_CFLAGS+=-DCONFIG_PR1_CLOCK_10
endif
ifneq ($(strip $(SF_PACKAGE_ZAPTEL_PR1)),y)
ZAPTEL_EXTRA_CFLAGS+=-DCONFIG_PR1_CLOCK_10
endif
ifeq ($(strip $(SF_PACKAGE_ZAPTEL_OSLEC)),y)
OSLEC_IN=oslec
endif
ifeq ($(strip $(SF_PR1_APPLIANCE)),y)
ifeq ($(strip $(SF_PACKAGE_LEC)),y)
ZARLINK_LEC=lec
ZAPTEL_EXTRA_CFLAGS+=-DECHO_CAN_FROMENV -DECHO_CAN_ZARLINK
endif
endif





$(DL_DIR)/$(ZAPTEL_SOURCE):
	mkdir -p $(TOPDIR)/$(DL_DIR)
	$(WGET) -P $(DL_DIR) $(ZAPTEL_SITE)/$(ZAPTEL_SOURCE)

$(ZAPTEL_DIR)/.unpacked: $(DL_DIR)/$(ZAPTEL_SOURCE) $(BASE_DIR)/.config
	$(ZAPTEL_UNZIP) $(DL_DIR)/$(ZAPTEL_SOURCE) | \
	tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(ZAPTEL_DIR)/.unpacked

$(ZAPTEL_DIR)/.configured: $(ZAPTEL_DIR)/.unpacked
	cd $(ZAPTEL_DIR); ./configure --host=bfin-linux-uclibc
ifeq ($(strip $(SF_PACKAGE_ZAPTEL_GSM)),y)
	ln -sf $(ZAPTEL_SOURCES)/gsm/sport_interface.c $(ZAPTEL_DIR)/kernel/sport_interface.c
	ln -sf $(ZAPTEL_SOURCES)/gsm/wcfxs.c $(ZAPTEL_DIR)/kernel/wcfxs.c
	ln -sf $(ZAPTEL_SOURCES)/gsm/fx.c $(ZAPTEL_DIR)/kernel/fx.c
	ln -sf $(ZAPTEL_SOURCES)/gsm/bfsi.c $(ZAPTEL_DIR)/kernel/bfsi.c
	ln -sf $(ZAPTEL_SOURCES)/gsm/gsm_module.c $(ZAPTEL_DIR)/kernel/gsm_module.c
	ln -sf $(ZAPTEL_SOURCES)/gsm/GSM_module_SPI.h $(ZAPTEL_DIR)/kernel/GSM_module_SPI.h
ifeq ($(strip $(SF_PACKAGE_ZAPTEL_OSLEC)),y)
	cp -f $(ZAPTEL_SOURCES)/gsm/zaptel-base.c-1.4.9.2 $(ZAPTEL_DIR)/kernel/zaptel-base.c
else
	cp -f $(ZAPTEL_SOURCES)/gsm/zaptel-base.c-1.4.10.1 $(ZAPTEL_DIR)/kernel/zaptel-base.c
endif
else
ifeq ($(strip $(SF_IP01)),y)
	ln -sf $(ZAPTEL_SOURCES)/sport_interface.c $(ZAPTEL_DIR)/kernel/sport_interface.c
	ln -sf $(ZAPTEL_SOURCES)/wcfxs.c-ip01 $(ZAPTEL_DIR)/kernel/wcfxs.c
	ln -sf $(ZAPTEL_SOURCES)/fx.c-ip01 $(ZAPTEL_DIR)/kernel/fx.c
	ln -sf 	$(ZAPTEL_SOURCES)/bfsi.c $(ZAPTEL_DIR)/kernel/bfsi.c
else
	ln -sf $(ZAPTEL_SOURCES)/sport_interface.c $(ZAPTEL_DIR)/kernel/sport_interface.c
	ln -sf $(ZAPTEL_SOURCES)/wcfxs.c $(ZAPTEL_DIR)/kernel/wcfxs.c
	ln -sf $(ZAPTEL_SOURCES)/fx.c $(ZAPTEL_DIR)/kernel/fx.c
	ln -sf $(ZAPTEL_SOURCES)/bfsi.c $(ZAPTEL_DIR)/kernel/bfsi.c
endif
endif
	ln -sf $(ZAPTEL_SOURCES)/sport_interface.h $(ZAPTEL_DIR)/kernel/sport_interface.h
	ln -sf $(ZAPTEL_SOURCES)/wcfxs.h $(ZAPTEL_DIR)/kernel/wcfxs.h
	ln -sf $(ZAPTEL_SOURCES)/bfsi.h $(ZAPTEL_DIR)/kernel/bfsi.h
	ln -sf $(ZAPTEL_SOURCES)/wpr1.c $(ZAPTEL_DIR)/kernel/wpr1.c
	ln -sf $(ZAPTEL_SOURCES)/wpr1.h $(ZAPTEL_DIR)/kernel/wpr1.h
	ln -sf $(ZAPTEL_SOURCES)/zl_wrap.h $(ZAPTEL_DIR)/kernel/zl_wrap.h	
	cd $(ZAPTEL_DIR); \
	patch -p0 < $(BASE_DIR)/package/zaptel/zaptel.patch
	cd $(ZAPTEL_DIR)/kernel; \
	patch -p0 < $(BASE_DIR)/package/zaptel/zaptel1.patch

ifneq ($(strip $(SF_PACKAGE_ZAPTEL_OSLEC)),y)
	cd $(ZAPTEL_DIR); \
#	patch -p0 < $(BASE_DIR)/package/zaptel/zaptel-chunksze-$(ZAPTEL_VERSION).patch Penev temporary removed
endif

ifeq ($(strip $(SF_PACKAGE_ZAPTEL_OSLEC)),y)
	cd $(ZAPTEL_DIR); \
	patch -p1 < $(OSLEC_DIR)/kernel/zaptel-$(ZAPTEL_VERSION).patch
endif

ifeq ($(strip $(SF_PR1_APPLIANCE)),y)
ifeq ($(strip $(SF_PACKAGE_LEC)),y)
	cd $(ZAPTEL_DIR); \
	patch -p2 < $(BASE_DIR)/package/lec/zaptel-$(ZAPTEL_VERSION).patch
	ln -sf $(ZAPTEL_SOURCES)/zl_wrap.c $(ZAPTEL_DIR)/kernel/zl_wrap.c
endif
endif
	touch $(ZAPTEL_DIR)/.configured


zaptel: $(ZARLINK_LEC) $(OSLEC_IN) $(ZAPTEL_DIR)/.configured
	# build libtonezone, and zapscan

	cd $(ZAPTEL_DIR); make libtonezone.so

	mkdir -p $(STAGING_INC)
	mkdir -p $(STAGING_INC)/zaptel
	mkdir -p $(STAGING_LIB)
	cp $(ZAPTEL_DIR)/tonezone.h $(STAGING_INC)/zaptel
	cp $(ZAPTEL_DIR)/ztcfg.h $(STAGING_INC)
	cp $(ZAPTEL_DIR)/kernel/zaptel.h $(STAGING_INC)/zaptel
	cp $(ZAPTEL_DIR)/libtonezone.so $(STAGING_LIB)
	cp $(ZAPTEL_DIR)/libtonezone.so $(TARGET_DIR)/lib
	$(TARGET_STRIP) $(TARGET_DIR)/lib/libtonezone.so
	cd $(ZAPTEL_DIR); make ztcfg ztscan
ifeq ($(strip $(SF_PACKAGE_ZAPTEL_TOOLS)),y)
	cd $(ZAPTEL_DIR); make patgen patlooptest pattest
endif
	$(TARGET_CC) -I$(STAGING_INC) $(ZAPTEL_SOURCES)/zapscan.c \
	-o $(ZAPTEL_DIR)/zapscan -Wall

	# build kernel modules
	cd $(ZAPTEL_DIR); \
	make version.h
	$(MAKE1) PWD=$(ZAPTEL_DIR) TOPDIR_MODULES=$(ZAPTEL_TOPDIR_MODULES) \
		EXTRA_CFLAGS="$(ZAPTEL_EXTRA_CFLAGS)" \
		KSRC=$(UCLINUX_DIR)/linux-2.6.x \
		-C $(ZAPTEL_DIR) modules
	$(MAKE1) PWD=$(ZAPTEL_DIR)  \
		EXTRA_CFLAGS="$(ZAPTEL_EXTRA_CFLAGS)" \
		KSRC=$(UCLINUX_DIR)/linux-2.6.x \
		-C $(ZAPTEL_DIR) fxstest

        # install
	mkdir -p $(TARGET_DIR)/lib/modules/$(shell ls $(TARGET_DIR)/lib/modules)/misc

ifeq ($(strip $(SF_PACKAGE_ZAPTEL_IP04_SPI_SPI)),y)
	cp -f $(ZAPTEL_DIR)/kernel/zaptel.ko $(ZAPTEL_DIR)/kernel/wcfxs.ko $(ZAPTEL_DIR)/kernel/bfsi.ko \
	$(TARGET_DIR)/lib/modules/$(shell ls $(TARGET_DIR)/lib/modules)/misc
endif
ifeq ($(strip $(SF_IP01)),y)
	cp -f $(ZAPTEL_DIR)/kernel/zaptel.ko $(ZAPTEL_DIR)/kernel/wcfxs.ko $(ZAPTEL_DIR)/kernel/bfsi.ko \
	$(TARGET_DIR)/lib/modules/$(shell ls $(TARGET_DIR)/lib/modules)/misc
endif
ifeq ($(strip $(SF_PACKAGE_ZAPTEL_IP04_SPI_SPORT)),y)
	cp -f $(ZAPTEL_DIR)/kernel/zaptel.ko $(ZAPTEL_DIR)/kernel/wcfxs.ko \
	$(ZAPTEL_DIR)/kernel/sport_interface.ko $(ZAPTEL_DIR)/kernel/bfsi.ko \
	$(TARGET_DIR)/lib/modules/$(shell ls $(TARGET_DIR)/lib/modules)/misc
endif
ifeq ($(strip $(SF_PACKAGE_ZAPTEL_PR1)),y)
	cp -f $(ZAPTEL_DIR)/kernel/zaptel.ko $(ZAPTEL_DIR)/kernel/wpr1.ko \
	$(TARGET_DIR)/lib/modules/$(shell ls $(TARGET_DIR)/lib/modules)/misc
endif
ifeq ($(strip $(SF_PACKAGE_ZAPTEL_TDMOE)),y)
	cp -f $(ZAPTEL_DIR)/kernel/ztd-eth.ko $(ZAPTEL_DIR)/kernel/ztd-loc.ko $(ZAPTEL_DIR)/kernel/ztdynamic.ko \
	$(TARGET_DIR)/lib/modules/$(shell ls $(TARGET_DIR)/lib/modules)/misc
endif
ifeq ($(strip $(SF_PACKAGE_ZAPTEL_ZTDUMMY)),y)
	cp -f $(ZAPTEL_DIR)/kernel/ztdummy.ko \
	$(TARGET_DIR)/lib/modules/$(shell ls $(TARGET_DIR)/lib/modules)/misc
endif
ifeq ($(strip $(SF_BR4_APPLIANCE)),y)
	cp -f $(ZAPTEL_DIR)/kernel/zaptel.ko $(ZAPTEL_DIR)/kernel/ztdummy.ko \
	$(TARGET_DIR)/lib/modules/$(shell ls $(TARGET_DIR)/lib/modules)/misc
endif

	cp -f $(ZAPTEL_DIR)/ztcfg $(ZAPTEL_DIR)/zapscan $(ZAPTEL_DIR)/ztscan $ $(ZAPTEL_DIR)/fxstest $(TARGET_DIR)/bin
ifeq ($(strip $(SF_PACKAGE_ZAPTEL_TOOLS)),y)
	cp -f $(ZAPTEL_DIR)/patgen $(ZAPTEL_DIR)/patlooptest $(ZAPTEL_DIR)/pattest $(TARGET_DIR)/bin
endif

zaptel-clean:
	make -C $(ZAPTEL_DIR) clean
################################################
#
# Toplevel Makefile options
#
#################################################
ifeq ($(strip $(SF_PACKAGE_ZAPTEL)),y)
TARGETS+=zaptel
endif
