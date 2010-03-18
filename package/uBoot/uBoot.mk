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
UBOOT_SITE=http://blackfin.uclinux.org/gf/download/frsrelease/330/2208/
UBOOT_UNZIP=bzcat
PATCHNAME=uBoot-ip04
UCONFIG=ip04
endif

ifeq ($(strip $(SF_IP01)),y)
UBOOT_DIRNAME=u-boot-1.1.5-bf1
UBOOT_DIR=$(BUILD_DIR)/$(UBOOT_DIRNAME)
UBOOT_SOURCE=u-boot-1.1.5-bf1-061210.tar.bz2
UBOOT_SITE=http://blackfin.uclinux.org/gf/download/frsrelease/330/2208/
UBOOT_UNZIP=bzcat
PATCHNAME=uBoot-ip04
UCONFIG=ip04
endif

ifeq ($(strip $(SF_PR1_APPLIANCE)),y)
UBOOT_DIRNAME=u-boot-1.1.6
UBOOT_DIR=$(BUILD_DIR)/$(UBOOT_DIRNAME)
UBOOT_SOURCE=u-boot-1.1.6.tar.bz2
UBOOT_SITE=http://blackfin.uclinux.org/gf/download/frsrelease/40/3028
UBOOT_UNZIP=bzcat
PATCHNAME=uBoot
UCONFIG=pr1
endif

ifeq ($(strip $(SF_BR4_APPLIANCE)),y)
UBOOT_DIRNAME=u-boot-1.1.6
UBOOT_DIR=$(BUILD_DIR)/$(UBOOT_DIRNAME)
UBOOT_SOURCE=u-boot-1.1.6.tar.bz2
UBOOT_SITE=http://blackfin.uclinux.org/gf/download/frsrelease/40/3028
UBOOT_UNZIP=bzcat
PATCHNAME=uBoot
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
	$(WGET) -P $(DL_DIR) $(UBOOT_SITE)/$(UBOOT_SOURCE)

uBoot-source: $(DL_DIR)/$(UBOOT_SOURCE)


$(UBOOT_DIR)/.unpacked: $(DL_DIR)/$(UBOOT_SOURCE)
	mkdir -p $(BUILD_DIR)
	$(UBOOT_UNZIP) $(DL_DIR)/$(UBOOT_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	$(PATCH_KERNEL) $(UBOOT_DIR) package/uBoot $(PATCHNAME).patch
ifeq ($(strip $(SF_BR4_APPLIANCE)),y)
	$(PATCH_KERNEL) $(UBOOT_DIR) package/uBoot $(PATCHNAME).patch.br4
	mkdir -p $(UBOOT_DIR)/board/SwitchVoice/br4
	touch $(UBOOT_DIR)/board/SwitchVoice/br4/memory.c
	ln -sf $(UBOOT_SOURCES_DIR)/br4.h $(UBOOT_DIR)/include/configs/
	ln -sf $(UBOOT_SOURCES_DIR)/boot_progress.c $(UBOOT_DIR)/board/SwitchVoice/br4/
	ln -sf $(UBOOT_SOURCES_DIR)/br4.c $(UBOOT_DIR)/board/SwitchVoice/br4/
	ln -sf $(UBOOT_SOURCES_DIR)/cmd_bf537led.c $(UBOOT_DIR)/board/SwitchVoice/br4/
	ln -sf $(UBOOT_SOURCES_DIR)/config.mk.uboot $(UBOOT_DIR)/board/SwitchVoice/br4/config.mk
	ln -sf $(UBOOT_SOURCES_DIR)/Makefile.uboot.br4 $(UBOOT_DIR)/board/SwitchVoice/br4/Makefile
	ln -sf $(UBOOT_SOURCES_DIR)/ether_bf537.c $(UBOOT_DIR)/board/SwitchVoice/br4/
	ln -sf $(UBOOT_SOURCES_DIR)/ether_bf537.h $(UBOOT_DIR)/board/SwitchVoice/br4/
	ln -sf $(UBOOT_SOURCES_DIR)/flash-defines.h $(UBOOT_DIR)/board/SwitchVoice/br4/
	ln -sf $(UBOOT_SOURCES_DIR)/i2c.c $(UBOOT_DIR)/board/SwitchVoice/br4/
	ln -sf $(UBOOT_SOURCES_DIR)/nand.c $(UBOOT_DIR)/board/SwitchVoice/br4/
	ln -sf $(UBOOT_SOURCES_DIR)/post-memory.c.br4 $(UBOOT_DIR)/board/SwitchVoice/br4/post-memory.c
	ln -sf $(UBOOT_SOURCES_DIR)/stm_m25p40.c $(UBOOT_DIR)/board/SwitchVoice/br4/
	ln -sf $(UBOOT_SOURCES_DIR)/u-boot.lds.S.br4 $(UBOOT_DIR)/board/SwitchVoice/br4/u-boot.lds.S
	ln -sf $(UBOOT_SOURCES_DIR)/tftp.c $(UBOOT_DIR)/net/
	patch -N -p1 -d $(UBOOT_DIR) < package/uBoot/defragnet.patch
	patch -N -p1 -d $(UBOOT_DIR) < package/uBoot/defragtftp.patch
endif
ifeq ($(strip $(SF_PR1_APPLIANCE)),y)
	mkdir -p $(UBOOT_DIR)/board/SwitchVoice/pr1
	ln -sf $(UBOOT_SOURCES_DIR)/pr1.h $(UBOOT_DIR)/include/configs/
	ln -sf $(UBOOT_SOURCES_DIR)/pr1.c $(UBOOT_DIR)/board/SwitchVoice/pr1/
	ln -sf $(UBOOT_SOURCES_DIR)/cmd_bf537led.c $(UBOOT_DIR)/board/SwitchVoice/pr1/
	ln -sf $(UBOOT_SOURCES_DIR)/config.mk.uboot $(UBOOT_DIR)/board/SwitchVoice/pr1/config.mk
	ln -sf $(UBOOT_SOURCES_DIR)/Makefile.uboot.pr1 $(UBOOT_DIR)/board/SwitchVoice/pr1/Makefile
	ln -sf $(UBOOT_SOURCES_DIR)/ether_bf537.c $(UBOOT_DIR)/board/SwitchVoice/pr1/
	ln -sf $(UBOOT_SOURCES_DIR)/ether_bf537.h $(UBOOT_DIR)/board/SwitchVoice/pr1/
	ln -sf $(UBOOT_SOURCES_DIR)/flash-defines.h $(UBOOT_DIR)/board/SwitchVoice/pr1/
	ln -sf $(UBOOT_SOURCES_DIR)/nand.c $(UBOOT_DIR)/board/SwitchVoice/pr1/
	ln -sf $(UBOOT_SOURCES_DIR)/post-memory.c.pr1 $(UBOOT_DIR)/board/SwitchVoice/pr1/post-memory.c
	ln -sf $(UBOOT_SOURCES_DIR)/stm_m25p40.c $(UBOOT_DIR)/board/SwitchVoice/pr1/
	ln -sf $(UBOOT_SOURCES_DIR)/u-boot.lds.S.pr1 $(UBOOT_DIR)/board/SwitchVoice/pr1/u-boot.lds.S
	ln -sf $(UBOOT_SOURCES_DIR)/tftp.c $(UBOOT_DIR)/net/
	patch -N -p1 -d $(UBOOT_DIR) < package/uBoot/defragnet.patch
	patch -N -p1 -d $(UBOOT_DIR) < package/uBoot/defragtftp.patch
endif

	touch $(UBOOT_DIR)/.unpacked

$(UBOOT_DIR)/.configured: $(UBOOT_DIR)/.unpacked

	-$(MAKE) -C $(UBOOT_DIR) UBOOT_FLAGS="$(MEM_FLAGS)" UBOOT_FLAGS2="$(CPU_FLAGS)" $(UCONFIG)_config
	touch $(UBOOT_DIR)/.configured

uBoot: $(UBOOT_DIR)/.configured
	mkdir -p $(IMAGE_DIR)
	$(MAKE)  -C $(UBOOT_DIR)
	cd $(UBOOT_DIR)/tools/bin2ldr; ./runme.sh
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

