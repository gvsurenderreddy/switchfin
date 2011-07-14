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
# Primary Author: Dimitar Penev dpn@astfin.org
# Copyright @ 2011 SwitchFin <dpn@switchfin.org>
#########################################################################
#############################################
# Home Automation Package for switchfin.org
#############################################

HA_WORKING=$(BUILD_DIR)/home-auto
LEC_EXTRA_CFLAGS=

$(HA_WORKING)/.unpacked:
	mkdir -p $(HA_WORKING)
	cp -f package/sources/home-auto/* $(HA_WORKING)
	cp -f package/sources/dahdi/sport_interface* $(HA_WORKING)
	touch $(HA_WORKING)/.unpacked

home-auto: $(HA_WORKING)/.unpacked
	cd $(HA_WORKING); \
	$(MAKE) EXTRA_CFLAGS="$(LEC_EXTRA_CFLAGS)" -C $(UCLINUX_DIR)/linux-2.6.x SUBDIRS=$(HA_WORKING)
	make -f Makefile.1 CC=$(TARGET_CC) -C $(HA_WORKING)
	
	# install
	mkdir -p $(shell ls -d $(TARGET_DIR)/lib/modules/*switchfin | tail -n1)/misc

	cp -f $(HA_WORKING)/wltc.ko $(shell ls -d $(TARGET_DIR)/lib/modules/*switchfin | tail -n1)/misc
	cp -f $(HA_WORKING)/home-auto $(TARGET_DIR)/bin/
	$(TARGET_STRIP) $(TARGET_DIR)/bin/home-auto

home-auto-clean:
	rm -rf $(HA_WORKING) 
################################################
#
# Toplevel Makefile options
#
#################################################
ifeq ($(strip $(SF_PACKAGE_HOME_AUTO)),y)	 
TARGETS+=home-auto	 
endif
