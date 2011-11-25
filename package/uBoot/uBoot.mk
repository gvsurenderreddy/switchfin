#########################################################################
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# The Free Software Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# Copyright @ 2008 Astfin <mark@astfin.org>
# Primary Authors: mark@astfin.org, pawel@astfin.org
# Copyright @ 2010 SwitchFin <dpn@switchfin.org>
#########################################################################

##########################################
# uBoot package for Astfin.org
##########################################

UBOOT_SOURCES_DIR=$(SOURCES_DIR)/uBoot

ifeq ($(strip $(SF_IP04)),y)
UBOOT_DIRNAME=u-boot-1.1.5-bf1
UBOOT_DIR=$(BUILD_DIR)/$(UBOOT_DIRNAME)
UBOOT_SOURCE=u-boot-1.1.5-bf1-061210.tar.bz2
UBOOT_SITE=http://blackfin.uclinux.org/gf/download/frsrelease/330/2208
UBOOT_UNZIP=bzcat
PATCHNAME=uBoot-ip04
UCONFIG=ip04
endif

ifeq ($(strip $(SF_IP01)),y)
UBOOT_DIRNAME=u-boot-1.1.5-bf1
UBOOT_DIR=$(BUILD_DIR)/$(UBOOT_DIRNAME)
UBOOT_SOURCE=u-boot-1.1.5-bf1-061210.tar.bz2
UBOOT_SITE=http://blackfin.uclinux.org/gf/download/frsrelease/330/2208
UBOOT_UNZIP=bzcat
PATCHNAME=uBoot-ip04
UCONFIG=ip04
endif

ifeq ($(strip $(SF_PR1_APPLIANCE)),y)
UBOOT_DIRNAME=u-boot-dc1aeeb
UBOOT_DIR=$(BUILD_DIR)/$(UBOOT_DIRNAME)
UBOOT_SOURCE=u-boot-dc1aeeb.tar.gz
UBOOT_SITE='http://blackfin.uclinux.org/git/?p=u-boot;a=snapshot;h=dc1aeebd486bc0408902b96402cd6455cc93eb1a;sf=tgz'
UBOOT_UNZIP=zcat
PATCHNAME=uBoot-pr1
UCONFIG=pr1
endif

ifeq ($(strip $(SF_BR4_APPLIANCE)),y)
UBOOT_DIRNAME=u-boot-dc1aeeb
UBOOT_DIR=$(BUILD_DIR)/$(UBOOT_DIRNAME)
UBOOT_SOURCE=u-boot-dc1aeeb.tar.gz
UBOOT_SITE='http://blackfin.uclinux.org/git/?p=u-boot;a=snapshot;h=dc1aeebd486bc0408902b96402cd6455cc93eb1a;sf=tgz'
UBOOT_UNZIP=zcat
PATCHNAME=uBoot-br4
UCONFIG=br4
endif

ifeq ($(strip $(SF_SDRAM_128)),y)
MEM_FLAGS=SDRAM_128MB
endif
ifeq ($(strip $(SF_SDRAM_64)),y)
ifeq ($(strip $(SF_CAS_3)),y)
MEM_FLAGS=SDRAM_64MB_SLOW
endif
ifeq ($(strip $(SF_CAS_2)),y)
MEM_FLAGS=SDRAM_64MB_FAST
endif
endif

ifeq ($(strip $(SF_CPU_300)),y)
CPU_FLAGS=CPU_300
endif
ifeq ($(strip $(SF_CPU_500)),y)
CPU_FLAGS=CPU_500
endif
ifeq ($(strip $(SF_CPU_600)),y)
CPU_FLAGS=CPU_600
endif

$(DL_DIR)/$(UBOOT_SOURCE):

ifeq ($(strip $(SF_PR1_APPLIANCE)),y)
ifeq ($(strip $(SF_BR4_APPLIANCE)),y)
	$(WGET) -O $(DL_DIR)/$(UBOOT_SOURCE) $(UBOOT_SITE)
else
	$(WGET) -O $(DL_DIR)/$(UBOOT_SOURCE) $(UBOOT_SITE)
endif
else
	$(WGET) -P $(DL_DIR)  $(UBOOT_SITE)/$(UBOOT_SOURCE)
endif

$(UBOOT_DIR)/.unpacked: $(DL_DIR)/$(UBOOT_SOURCE)
	mkdir -p $(BUILD_DIR)
	$(UBOOT_UNZIP) $(DL_DIR)/$(UBOOT_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH_KERNEL) $(UBOOT_DIR) package/uBoot $(PATCHNAME).patch

ifeq ($(strip $(SF_PR1_APPLIANCE)),y)
#	$(PATCH_KERNEL) $(UBOOT_DIR) package/uBoot uBoot-pr1-post.patch
ifeq ($(strip $(SF_SDRAM_64)),y)
	sed -i  -e's/^#define CONFIG_MEM_ADD_WDTH.*/#define CONFIG_MEM_ADD_WDTH     10/' $(UBOOT_DIR)/include/configs/pr1.h
	sed -i  -e's/^#define CONFIG_MEM_SIZE.*/#define CONFIG_MEM_SIZE         64/' $(UBOOT_DIR)/include/configs/pr1.h
else
	sed -i  -e's/^#define CONFIG_MEM_ADD_WDTH.*/#define CONFIG_MEM_ADD_WDTH     11/' $(UBOOT_DIR)/include/configs/pr1.h
	sed -i  -e's/^#define CONFIG_MEM_SIZE.*/#define CONFIG_MEM_SIZE         128/' $(UBOOT_DIR)/include/configs/pr1.h
endif
ifeq ($(strip $(SF_CPU_REV_2)),y)
	sed -i  -e's/^#define CONFIG_BFIN_CPU.*/#define CONFIG_BFIN_CPU             bf537-0.2/' $(UBOOT_DIR)/include/configs/pr1.h
else
	sed -i  -e's/^#define CONFIG_BFIN_CPU.*/#define CONFIG_BFIN_CPU             bf537-0.3/' $(UBOOT_DIR)/include/configs/pr1.h
endif

endif


ifeq ($(strip $(SF_BR4_APPLIANCE)),y)
#	$(PATCH_KERNEL) $(UBOOT_DIR) package/uBoot uBoot-br4-post.patch
ifeq ($(strip $(SF_SDRAM_64)),y)
	sed -i  -e's/^#define CONFIG_MEM_ADD_WDTH.*/#define CONFIG_MEM_ADD_WDTH     10/' $(UBOOT_DIR)/include/configs/br4.h
	sed -i  -e's/^#define CONFIG_MEM_SIZE.*/#define CONFIG_MEM_SIZE         64/' $(UBOOT_DIR)/include/configs/br4.h
else
	sed -i  -e's/^#define CONFIG_MEM_ADD_WDTH.*/#define CONFIG_MEM_ADD_WDTH     11/' $(UBOOT_DIR)/include/configs/br4.h
	sed -i  -e's/^#define CONFIG_MEM_SIZE.*/#define CONFIG_MEM_SIZE         128/' $(UBOOT_DIR)/include/configs/br4.h
endif
ifeq ($(strip $(SF_CPU_REV_2)),y)
	sed -i  -e's/^#define CONFIG_BFIN_CPU.*/#define CONFIG_BFIN_CPU             bf537-0.2/' $(UBOOT_DIR)/include/configs/br4.h
else
	sed -i  -e's/^#define CONFIG_BFIN_CPU.*/#define CONFIG_BFIN_CPU             bf537-0.3/' $(UBOOT_DIR)/include/configs/br4.h
endif

endif

ifneq ($(strip $(SF_PR1_APPLIANCE)),y)
ifneq ($(strip $(SF_BR4_APPLIANCE)),y)
	patch -N -p1 -d $(UBOOT_DIR) < package/uBoot/nand.patch
endif
endif
	touch $(UBOOT_DIR)/.unpacked

$(UBOOT_DIR)/.configured: $(UBOOT_DIR)/.unpacked
	-$(MAKE) -C $(UBOOT_DIR) UBOOT_FLAGS="$(MEM_FLAGS)" UBOOT_FLAGS2="$(CPU_FLAGS)" $(UCONFIG)_config
	touch $(UBOOT_DIR)/.configured

uBoot: $(UBOOT_DIR)/.configured
	mkdir -p $(IMAGE_DIR)
	$(MAKE)  -C $(UBOOT_DIR)
ifneq ($(strip $(SF_PR1_APPLIANCE)),y)
ifneq ($(strip $(SF_BR4_APPLIANCE)),y)
	cd $(UBOOT_DIR)/tools/bin2ldr; ./runme.sh
endif
endif
	cp -v $(UBOOT_DIR)/u-boot.ldr $(IMAGE_DIR)

uBoot-configure: $(UBOOT_DIR)/.configured

uBoot-clean:
	rm -f $(UBOOT_DIR)/.configured
	$(MAKE) -C $(UBOOT_DIR) clean

uBoot-config: $(UBOOT_DIR)/.configured
	$(MAKE) -C $(UBOOT_DIR) menuconfig

uBoot-dirclean:
	rm -rf $(UBOOT_DIR)

################################################
#
# Toplevel Makefile options
#
#################################################
ifneq ($(strip $(SF_FX08)),y)
ifeq ($(strip $(SF_PACKAGE_UBOOT)),y)
TARGETS+=uBoot
endif
endif

