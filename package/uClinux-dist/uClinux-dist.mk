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
# Update 27 July 2009, Dimitar Penev dpn@ucpbx.com
#	 uClinux 2009R1-RC2 (kernel 2.6.28.10)
#        blackfin toolchain 2009R1-RC7 	 
#	 Big thanks to Jeff Palmer jpalmer@gsat.us
# Copyright @ 2010 SwitchFin <dpn@switchfin.org>
#########################################################################

##########################################
# uClinux-dist package for Astfin.org
##########################################
UCLINUX_KERNEL_SRC=$(BUILD_DIR)/uClinux-dist/linux-2.6.x
UCLINUX_SOURCE=uClinux-dist-2009R1.1-RC4.tar.bz2
UCLINUX_SITE=http://blackfin.uclinux.org/gf/download/frsrelease/473/7285/
UCLINUX_UNZIP=bzcat
UCLINUX_HOME=$(BUILD_DIR)/uClinux-dist
TOOLCHAIN_BUILD=$(BASE_DIR)/toolchain
VARIABLE_CONFIG_FILE=$(BUILD_DIR)/config_tmp
LIBS_CONFIG=$(BUILD_DIR)/uClinux-dist/config/.config
ifeq ($(strip $(SF_IP04)),y)
SF_ANALOG=y
endif
ifeq ($(strip $(SF_IP01)),y)
SF_ANALOG=y
endif
ifeq ($(strip $(SF_FX08)),y)
SF_ANALOG=y
endif

################################################
# uClinux Dep....
################################################


UCLINUX_CONFIG_FILE=$(subst ",, $(strip $(SF_PACKAGE_UCLINUX_CONFIG)))#"

$(DL_DIR)/$(UCLINUX_SOURCE):
	$(WGET) -P $(DL_DIR) $(UCLINUX_SITE)/$(UCLINUX_SOURCE)

uClinux-source: $(DL_DIR)/$(UCLINUX_SOURCE) $(UCLINUX_CONFIG_FILE)

#---------------------------------------------------------------------------
#                              .unpacked Target
#---------------------------------------------------------------------------

$(UCLINUX_DIR)/.unpacked: $(DL_DIR)/$(UCLINUX_SOURCE)
	tar xjf $(DL_DIR)/$(UCLINUX_SOURCE) -C $(BUILD_DIR);
	patch -d $(UCLINUX_DIR) -p1 < package/uClinux-dist/common/ncpu.patch
	patch -d $(UCLINUX_DIR) -p1 < package/uClinux-dist/common/curl.patch

ifeq ($(strip $(SF_PR1_APPLIANCE)),y)
	patch -d $(UCLINUX_DIR) -p1 < package/uClinux-dist/vendors/SwitchVoice/PR1-APPLIANCE/pre_config/mem.patch
	patch -d $(UCLINUX_DIR) -p1 < package/uClinux-dist/vendors/SwitchVoice/PR1-APPLIANCE/pre_config/flash.patch
	patch -d $(UCLINUX_DIR) -p1 < package/uClinux-dist/vendors/SwitchVoice/PR1-APPLIANCE/pre_config/Kconfig_arch.patch
	patch -d $(UCLINUX_DIR) -p1 < package/uClinux-dist/vendors/SwitchVoice/PR1-APPLIANCE/pre_config/boards.patch

endif

ifeq ($(strip $(SF_IP04)),y)
	patch -d $(UCLINUX_DIR) -p1 < package/uClinux-dist/vendors/Rowetel/IP04/pre_config/ip04.patch
endif

ifeq ($(strip $(SF_IP01)),y)
	patch -d $(UCLINUX_DIR) -p1 < package/uClinux-dist/vendors/Rowetel/IP01/pre_config/ip01.patch
endif


ifeq ($(strip $(SF_FX08)),y)
	patch -d $(UCLINUX_DIR) -p1 < package/uClinux-dist/vendors/SwitchVoice/FX08/pre_config/fx08.patch
endif

ifeq ($(strip $(SF_BR4_APPLIANCE)),y)
	patch -d $(UCLINUX_DIR) -p1 < package/uClinux-dist/vendors/SwitchVoice/BR4-APPLIANCE/pre_config/mem.patch
	patch -d $(UCLINUX_DIR) -p1 < package/uClinux-dist/vendors/SwitchVoice/BR4-APPLIANCE/pre_config/flash.patch
	patch -d $(UCLINUX_DIR) -p1 < package/uClinux-dist/vendors/SwitchVoice/BR4-APPLIANCE/pre_config/Kconfig_arch.patch
	patch -d $(UCLINUX_DIR) -p1 < package/uClinux-dist/vendors/SwitchVoice/BR4-APPLIANCE/pre_config/boards.patch
	patch -d $(UCLINUX_DIR) -p1 < package/uClinux-dist/common/i2c.patch
endif

	patch -d $(UCLINUX_DIR) -u -p1 < package/uClinux-dist/common/gsm.patch
	patch -d $(UCLINUX_DIR) -u -p0 < package/uClinux-dist/common/mkuclinux.patch
	patch -d $(UCLINUX_DIR) -p1 < package/uClinux-dist/common/dropbear.patch
#	patch -d $(UCLINUX_DIR) -p1 < package/uClinux-dist/common/u-boot-tools.patch	     Penev: I think not needed

	touch $(UCLINUX_DIR)/.unpacked


#---------------------------------------------------------------------------
#                              .configured Target
#---------------------------------------------------------------------------

$(UCLINUX_DIR)/.configured: $(UCLINUX_DIR)/.unpacked

ifeq ($(strip $(SF_ANALOG)),y)
	echo "No need to do anything for analog boxes...."
else
ifeq ($(strip $(SF_CPU_REV_3)),y)
	echo "CONFIG_BF_REV_0_3=y" > $(VARIABLE_CONFIG_FILE)
endif
ifeq ($(strip $(SF_CPU_REV_2)),y)
	echo "CONFIG_BF_REV_0_2=y" > $(VARIABLE_CONFIG_FILE)
endif
ifeq ($(strip $(SF_SDRAM_128)),y)
	echo "CONFIG_MEM_MT48LC64M8A2_75=y" >> $(VARIABLE_CONFIG_FILE)
	echo "CONFIG_MEM_SIZE=128" >> $(VARIABLE_CONFIG_FILE)
	echo "CONFIG_MEM_ADD_WIDTH=11" >> $(VARIABLE_CONFIG_FILE)
endif
ifeq ($(strip $(SF_SDRAM_64)),y)
	echo "CONFIG_MEM_SIZE=64" >> $(VARIABLE_CONFIG_FILE)
	echo "CONFIG_MEM_ADD_WIDTH=10" >> $(VARIABLE_CONFIG_FILE)
ifeq ($(strip $(SF_CAS_3)),y)
	echo "CONFIG_MEM_MT48LC32M8A2_75=y" >> $(VARIABLE_CONFIG_FILE)
endif
ifeq ($(strip $(SF_CAS_2)),y)
	echo "CONFIG_MEM_MT48LC32M8A2_7E=y" >> $(VARIABLE_CONFIG_FILE)
endif
endif
ifeq ($(strip $(SF_NAND_256MB)),y)
	echo "CONFIG_BFIN_NAND_SIZE=0x10000000" >> $(VARIABLE_CONFIG_FILE)
endif
ifeq ($(strip $(SF_NAND_512MB)),y)
	echo "CONFIG_BFIN_NAND_SIZE=0x20000000" >> $(VARIABLE_CONFIG_FILE)
endif
ifeq ($(strip $(SF_NAND_1GB)),y)
	echo "CONFIG_BFIN_NAND_SIZE=0x40000000" >> $(VARIABLE_CONFIG_FILE)
endif
endif
ifeq ($(strip $(SF_PACKAGE_IPTABLES)),y)
	rm -rf $(UCLINUX_DIR)/user/iptables/;
	svn checkout --revision 9614 svn://sources.blackfin.uclinux.org/uclinux-dist/trunk/user/iptables $(UCLINUX_DIR)/user/iptables
#	patch -d $(UCLINUX_DIR) -p1 < package/uClinux-dist/common/iptables.patch

	cat package/iptables/config.iptables >> $(VARIABLE_CONFIG_FILE)
else
	echo "# CONFIG_NETFILTER is not set" >> $(VARIABLE_CONFIG_FILE)
endif

	if [ ! -d $(UCLINUX_DIR)/user/busybox.original ]; then \
                mv -u $(UCLINUX_DIR)/user/busybox $(UCLINUX_DIR)/user/busybox.original; \
        else \
		rm -rf $(UCLINUX_DIR)/user/busybox; \
	fi
	svn checkout --revision 9685 svn://sources.blackfin.uclinux.org/uclinux-dist/trunk/user/busybox $(UCLINUX_DIR)/user/busybox
	patch -d $(UCLINUX_DIR) -p1 < package/uClinux-dist/common/dhcpd.patch
	

ifeq ($(strip $(SF_PR1_APPLIANCE)),y)
	mkdir -p $(UCLINUX_DIR)/vendors/SwitchVoice/PR1-APPLIANCE/
	mkdir -p $(UCLINUX_DIR)/vendors/SwitchVoice/common/
	cp -af package/uClinux-dist/vendors/SwitchVoice/PR1-APPLIANCE/* $(UCLINUX_DIR)/vendors/SwitchVoice/PR1-APPLIANCE
	cp -af package/uClinux-dist/vendors/SwitchVoice/common/* $(UCLINUX_DIR)/vendors/SwitchVoice/common
	cp -af package/uClinux-dist/vendors/SwitchVoice/vendor.mak $(UCLINUX_DIR)/vendors/SwitchVoice/
	cat $(VARIABLE_CONFIG_FILE) >> $(UCLINUX_DIR)/vendors/SwitchVoice/PR1-APPLIANCE/config.linux-2.6.x
	cp -af package/uClinux-dist/vendors/SwitchVoice/PR1-APPLIANCE/pre_config/pr1_appliance.c $(UCLINUX_DIR)/linux-2.6.x/arch/blackfin/mach-bf537/boards
	ln -sf $(UCLINUX_DIR)/vendors/SwitchVoice/PR1-APPLIANCE/config.linux-2.6.x $(UCLINUX_DIR)/linux-2.6.x/arch/blackfin/configs/PR1-APPLIANCE_defconfig
	$(MAKE) -C $(UCLINUX_DIR) SwitchVoice/PR1-APPLIANCE_defconfig
endif

ifeq ($(strip $(SF_IP04)),y)
	mkdir -p $(UCLINUX_DIR)/vendors/Rowetel/IP04/
	mkdir -p $(UCLINUX_DIR)/vendors/Rowetel/common/
	cp -af package/uClinux-dist/vendors/Rowetel/IP04/* $(UCLINUX_DIR)/vendors/Rowetel/IP04/
	cp -af package/uClinux-dist/vendors/Rowetel/common/* $(UCLINUX_DIR)/vendors/Rowetel/common
	cp -af package/uClinux-dist/vendors/Rowetel/vendor.mak $(UCLINUX_DIR)/vendors/Rowetel/
	cp -af package/uClinux-dist/vendors/Rowetel/IP04/pre_config/ip04.c $(UCLINUX_DIR)/linux-2.6.x/arch/blackfin/mach-bf533/boards/ip0x.c
	ln -sf $(UCLINUX_DIR)/vendors/Rowetel/IP04/config.linux-2.6.x $(UCLINUX_DIR)/linux-2.6.x/arch/blackfin/configs/IP04_defconfig
	
	$(MAKE) -C $(UCLINUX_DIR) Rowetel/IP04_defconfig
endif

ifeq ($(strip $(SF_IP01)),y)
	mkdir -p $(UCLINUX_DIR)/vendors/Rowetel/IP01/
	mkdir -p $(UCLINUX_DIR)/vendors/Rowetel/common/
	cp -af package/uClinux-dist/vendors/Rowetel/common/* $(UCLINUX_DIR)/vendors/Rowetel/common
	cp -af package/uClinux-dist/vendors/Rowetel/vendor.mak $(UCLINUX_DIR)/vendors/Rowetel/
	cp -af package/uClinux-dist/vendors/Rowetel/IP01/* $(UCLINUX_DIR)/vendors/Rowetel/IP01
	cp -af package/uClinux-dist/vendors/Rowetel/IP01/pre_config/ip01.c $(UCLINUX_DIR)/linux-2.6.x/arch/blackfin/mach-bf533/boards/ip0x.c
	ln -sf $(UCLINUX_DIR)/vendors/Rowetel/IP01/config.linux-2.6.x $(UCLINUX_DIR)/linux-2.6.x/arch/blackfin/configs/IP01_defconfig
	
	$(MAKE) -C $(UCLINUX_DIR) Rowetel/IP01_defconfig
endif

ifeq ($(strip $(SF_FX08)),y)
	mkdir -p $(UCLINUX_DIR)/vendors/SwitchVoice/FX08/
	cp -af package/uClinux-dist/vendors/SwitchVoice/FX08/* $(UCLINUX_DIR)/vendors/SwitchVoice/FX08
	cp -af package/uClinux-dist/vendors/SwitchVoice/FX08/pre_config/fx0x.c $(UCLINUX_DIR)/linux-2.6.x/arch/blackfin/mach-bf533/boards
	cp -af package/uClinux-dist/vendors/SwitchVoice/FX08/config.linux-2.6.x $(UCLINUX_DIR)/linux-2.6.x/arch/blackfin/configs/FX08_defconfig
	cp -af package/uClinux-dist/common/vendor.mak $(UCLINUX_DIR)/vendors/SwitchVoice
	$(MAKE) -C $(UCLINUX_DIR) SwitchVoice/FX08_config
endif

ifeq ($(strip $(SF_BR4_APPLIANCE)),y)
	mkdir -p $(UCLINUX_DIR)/vendors/SwitchVoice/BR4-APPLIANCE
	mkdir -p $(UCLINUX_DIR)/vendors/SwitchVoice/common/
	cp -af package/uClinux-dist/vendors/SwitchVoice/BR4-APPLIANCE/* $(UCLINUX_DIR)/vendors/SwitchVoice/BR4-APPLIANCE
	cp -af package/uClinux-dist/vendors/SwitchVoice/common/* $(UCLINUX_DIR)/vendors/SwitchVoice/common
	cp -af package/uClinux-dist/vendors/SwitchVoice/vendor.mak $(UCLINUX_DIR)/vendors/SwitchVoice/
	cat $(VARIABLE_CONFIG_FILE) >> $(UCLINUX_DIR)/vendors/SwitchVoice/BR4-APPLIANCE/config.linux-2.6.x
	cp -af package/uClinux-dist/vendors/SwitchVoice/BR4-APPLIANCE/pre_config/br4_appliance.c $(UCLINUX_DIR)/linux-2.6.x/arch/blackfin/mach-bf537/boards
	ln -sf $(SOURCES_DIR)/linux/i2c-pca9539.h $(UCLINUX_DIR)/linux-2.6.x/include/linux/
	ln -sf $(UCLINUX_DIR)/vendors/SwitchVoice/BR4-APPLIANCE/config.linux-2.6.x $(UCLINUX_DIR)/linux-2.6.x/arch/blackfin/configs/BR4-APPLIANCE_defconfig
	$(MAKE) -C $(UCLINUX_DIR) SwitchVoice/BR4-APPLIANCE_defconfig
endif

	touch $(UCLINUX_DIR)/.configured

#---------------------------------------------------------------------------
#                              uClinux Target
#---------------------------------------------------------------------------

uClinux: $(UCLINUX_DEP) $(UCLINUX_DIR)/.configured
ifeq ($(strip $(SF_TARGET_CUSTOM)),y)
	-$(MAKE) -C $(UCLINUX_DIR) ROMFSDIR=$(TARGET_DIR) menuconfig
endif
	$(MAKE) -C $(UCLINUX_DIR) ROMFSDIR=$(TARGET_DIR)
	cp -af $(SOURCES_DIR)/target/groups $(TARGET_DIR)/bin
ifeq ($(strip $(SF_PACKAGE_CURL)),y)
	find $(UCLINUX_DIR)/lib/libcurl/ -type f -name curl -print0 | xargs -0 file | grep ELF | cut -d: -f1 | xargs -i cp {} $(TARGET_DIR)/usr/bin/curl
else
	rm -f $(TARGET_DIR)/usr/bin/curl
endif

ifneq ($(strip $(SF_PACKAGE_IPTABLES)),y)
	rm -f $(TARGET_DIR)/usr/bin/iptables
endif

	
	touch $(UCLINUX_DIR)/.built

uClinux-unpacked: $(UCLINUX_DIR)/.unpacked
	@echo "---- uClinux Unpacked ------"

uClinux-configure: $(UCLINUX_DIR)/.configured
	@echo "------ uClinux Configure -------"

uClinux-clean:
	@echo "------ uClinux Make clean -------"
	 -$(MAKE) -C $(UCLINUX_DIR) clean

uClinux-config: $(UCLINUX_DIR)/.configured
	$(MAKE) -C $(UCLINUX_DIR) menuconfig

uClinux-dirclean:
	rm -rf $(UCLINUX_DIR)
