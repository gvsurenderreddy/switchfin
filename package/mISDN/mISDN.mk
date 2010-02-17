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
# mISDN package for Astfin.org
##########################################
mISDN_NAME=mISDN
mISDN_REV=3
mISDN_DIR=$(BUILD_DIR)/$(mISDN_NAME)-$(mISDN_REV)
mISDN_SITE=https://switchfin.svn.sourceforge.net/svnroot/switchfin/mISDN/trunk
mISDN_UNZIP=zcat


$(DL_DIR)/$(mISDN_NAME)-$(mISDN_REV):
	$(SVN) -r $(mISDN_REV) $(mISDN_SITE) $(DL_DIR)/$(mISDN_NAME)-$(mISDN_REV)
	-find $(DL_DIR)/$(mISDN_NAME)-$(mISDN_REV) -type d -name .svn | xargs rm -rf

$(mISDN_DIR)/.unpacked: $(DL_DIR)/$(mISDN_NAME)-$(mISDN_REV)
	rm -rf $(mISDN_DIR)
	cp -R $(DL_DIR)/$(mISDN_NAME)-$(mISDN_REV) $(mISDN_DIR)
	touch $(mISDN_DIR)/.unpacked


$(mISDN_DIR)/.configured: $(mISDN_DIR)/.unpacked
	patch -d $(mISDN_DIR) -p0 < package/mISDN/mISDN.patch
	touch $(mISDN_DIR)/.configured


mISDN: $(mISDN_DIR)/.configured
ifeq ($(strip $(SF_BR4_APPLIANCE)),y)
ifeq ($(strip $(SF_PACKAGE_LEC)),y)
ifeq ($(strip $(SF_LEC_128)),y)
	$(MAKE)  LEC_ZL38065=y ENABLE_128MS=y CONFIG_BR4=y CROSS_COMPILE=CROSS_COMPILE=$(TARGET_CROSS) INSTALL_PREFIX=$(UCLINUX_DIR)/linux-2.6.x CC=$(TARGET_CC) LINUX="$(UCLINUX_DIR)/linux-2.6.x" -C $(mISDN_DIR) force
	$(MAKE)  LEC_ZL38065=y ENABLE_128MS=y CONFIG_BR4=y CROSS_COMPILE=CROSS_COMPILE=$(TARGET_CROSS) INSTALL_PREFIX=$(UCLINUX_DIR)/linux-2.6.x CC=$(TARGET_CC) LINUX="$(UCLINUX_DIR)/linux-2.6.x" -C $(mISDN_DIR) all
else
	$(MAKE)  LEC_ZL38065=y CONFIG_BR4=y CROSS_COMPILE=CROSS_COMPILE=$(TARGET_CROSS) INSTALL_PREFIX=$(UCLINUX_DIR)/linux-2.6.x CC=$(TARGET_CC) LINUX="$(UCLINUX_DIR)/linux-2.6.x" -C $(mISDN_DIR) force
	$(MAKE)  LEC_ZL38065=y CONFIG_BR4=y CROSS_COMPILE=CROSS_COMPILE=$(TARGET_CROSS) INSTALL_PREFIX=$(UCLINUX_DIR)/linux-2.6.x CC=$(TARGET_CC) LINUX="$(UCLINUX_DIR)/linux-2.6.x" -C $(mISDN_DIR) all      
endif
else
	$(MAKE)  CONFIG_BR4=y CROSS_COMPILE=CROSS_COMPILE=$(TARGET_CROSS) INSTALL_PREFIX=$(UCLINUX_DIR)/linux-2.6.x CC=$(TARGET_CC) LINUX="$(UCLINUX_DIR)/linux-2.6.x" -C $(mISDN_DIR) force
	$(MAKE)  CONFIG_BR4=y CROSS_COMPILE=CROSS_COMPILE=$(TARGET_CROSS) INSTALL_PREFIX=$(UCLINUX_DIR)/linux-2.6.x CC=$(TARGET_CC) LINUX="$(UCLINUX_DIR)/linux-2.6.x" -C $(mISDN_DIR) all
endif
else
	$(MAKE)  CROSS_COMPILE=CROSS_COMPILE=$(TARGET_CROSS) INSTALL_PREFIX=$(UCLINUX_DIR)/linux-2.6.x CC=$(TARGET_CC) LINUX="$(UCLINUX_DIR)/linux-2.6.x" -C $(mISDN_DIR) force
	$(MAKE)  CROSS_COMPILE=CROSS_COMPILE=$(TARGET_CROSS) INSTALL_PREFIX=$(UCLINUX_DIR)/linux-2.6.x CC=$(TARGET_CC) LINUX="$(UCLINUX_DIR)/linux-2.6.x" -C $(mISDN_DIR) all

endif

	mkdir -p $(TARGET_DIR)/lib/modules/$(shell ls $(TARGET_DIR)/lib/modules)/misc

	# Install 
	mkdir -p $(STAGING_INC)/linux
	cp -f $(mISDN_DIR)/include/linux/*.h $(STAGING_INC)/linux
	cp -f $(mISDN_DIR)/drivers/isdn/hardware/mISDN/*.ko \
        $(TARGET_DIR)/lib/modules/$(shell ls $(TARGET_DIR)/lib/modules)/misc


mISDN-configure: $(mISDN_DIR)/.configured

mISDN-clean:
	rm -f $(mISDN_DIR)/.configured
	 -$(MAKE) -C $(mISDN_DIR) clean

mISDN-config: $(mISDN_DIR)/.configured
	$(MAKE) -C $(mISDN_DIR) menuconfig

mISDN-dirclean:
	rm -rf $(mISDN_DIR)
	rm -rf $(mISDN_DIR_LINK)

################################################
#
# Toplevel Makefile options
#
#################################################
ifeq ($(strip $(SF_PACKAGE_MISDN)),y)
TARGETS+=mISDN
endif
