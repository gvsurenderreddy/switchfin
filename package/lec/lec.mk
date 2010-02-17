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
# Primary Author: mark@astfin.org, dpn@astfin.org
# Copyright @ 2010 SwitchFin <dpn@switchfin.org>
#########################################################################
#############################################
# Echo Cancelleation package for Astfin.org
#############################################

LEC_REV=5
LEC_SOURCE=lec-$(LEC_REV)
LEC_DIR=$(DL_DIR)/$(LEC_SOURCE)
LEC_SITE=https://switchfin.svn.sourceforge.net/svnroot/switchfin/lec/trunk
LEC_WORKING=$(BUILD_DIR)/$(LEC_SOURCE)

ifeq ($(strip $(SF_BR4_APPLIANCE)),y)	 
ifneq ($(strip $(STFIN_LEC_128)),y)	 
LEC_EXTRA_CFLAGS=-DCHANNELS_8
else
LEC_EXTRA_CFLAGS=-DCHANNELS_16
endif
endif

ifeq ($(strip $(SF_PR1_APPLIANCE)),y)
LEC_EXTRA_CFLAGS=-DCHANNELS_32
endif



$(LEC_DIR):
	$(SVN) -r $(LEC_REV) $(LEC_SITE) $(LEC_DIR)

$(LEC_WORKING)/.unpacked: $(LEC_DIR)
	rm -rf $(LEC_WORKING)
	cp -R $(LEC_DIR) $(LEC_WORKING)
	patch -d $(LEC_WORKING) -p0 < package/lec/lec.patch
	touch $(LEC_WORKING)/.unpacked

$(LEC_WORKING)/.configured: $(LEC_WORKING)/.unpacked
	-find $(LEC_WORKING) -type d -name .svn | xargs rm -rf
	touch $(LEC_DIR)/.configured


lec: $(LEC_WORKING)/.configured
	cd $(LEC_WORKING); \
	$(MAKE) EXTRA_CFLAGS="$(LEC_EXTRA_CFLAGS)" -C $(UCLINUX_DIR)/linux-2.6.x SUBDIRS=$(LEC_WORKING)

        # install
	mkdir -p $(TARGET_DIR)/lib/modules/$(shell ls $(TARGET_DIR)/lib/modules)/misc

	cp -f $(LEC_WORKING)/ec_module.ko \
        	$(TARGET_DIR)/lib/modules/$(shell ls $(TARGET_DIR)/lib/modules)/misc




lec-clean:
	rm -rf $(LEC_WORKING) 
################################################
#
# Toplevel Makefile options
#
#################################################
ifeq ($(strip $(SF_PACKAGE_LEC)),y)	 
 TARGETS+=lec	 
 endif
