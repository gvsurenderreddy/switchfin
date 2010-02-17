#########################################################################
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# The Free Software Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# Copyright @ 2008 Astfin <mark@astfin.org> and <Li@astfin.org>
# Primary Authors: mark@astfin.org, li@astfin.org
# Copyright @ 2010 SwitchFin <dpn@switchfin.org>
#########################################################################

##########################################
# Asterisk GUI package for Astfin.org
##########################################

ASTERISKGUI_TAG=2.0
ASTERISKGUI_DIR=$(BUILD_DIR)/asterisk-gui-$(ASTERISKGUI_TAG)
ASTERISKGUI_UNPACKED=asterisk-gui-$(ASTERISKGUI_TAG)
ASTERISKGUI_SITE=https://switchfin.svn.sourceforge.net/svnroot/switchfin/asterisk-gui/tags/$(ASTERISKGUI_TAG)

#
# Target not currently used as statically keeping the code locally in svn
# in the dl directory
#

$(DL_DIR)/$(ASTERISKGUI_UNPACKED):
	$(SVN)  $(ASTERISKGUI_SITE) $(DL_DIR)/$(ASTERISKGUI_UNPACKED)

asterisk-gui-source: $(DL_DIR)/asterisk-gui


$(ASTERISKGUI_DIR)/.unpacked: $(DL_DIR)/$(ASTERISKGUI_UNPACKED)
	cp -R $(DL_DIR)/$(ASTERISKGUI_UNPACKED) $(ASTERISKGUI_DIR)
	touch $(ASTERISKGUI_DIR)/.unpacked


$(ASTERISKGUI_DIR)/.configured: $(ASTERISKGUI_DIR)/.unpacked
	touch $(ASTERISKGUI_DIR)/.configured

asterisk-gui:  $(ASTERISKGUI_DIR)/.configured
	-find $(ASTERISKGUI_DIR) -type d -name .svn | xargs rm -rf
	#mkdir -p $(TARGET_DIR)/etc/scripts
	mkdir -p $(TARGET_DIR)/var/lib/asterisk
	mkdir -p $(TARGET_DIR)/var/lib/asterisk/scripts
	mkdir -p $(TARGET_DIR)/var/lib/asterisk/static-http
	mkdir -p $(TARGET_DIR)/var/lib/asterisk/static-http/config
	cp -Rv $(ASTERISKGUI_DIR)/config/* $(TARGET_DIR)/var/lib/asterisk/static-http/config/
	cp -v $(ASTERISKGUI_DIR)/scripts/* $(TARGET_DIR)/var/lib/asterisk/scripts/
ifeq ($(strip $(SF_PR1_APPLIANCE)),y)
	cp -v $(BASE_DIR)/package/asterisk-gui/editzap.sh $(TARGET_DIR)/var/lib/asterisk/scripts/
endif
	chmod +x  $(TARGET_DIR)/var/lib/asterisk/scripts/*


asterisk-gui-configure: $(ASTERISKGUI_DIR)/.configured

asterisk-gui-clean:
	rm -f $(ASTERISKGUI_DIR)/.configured
	 -$(MAKE) -C $(ASTERISKGUI_DIR) clean

asterisk-gui-config: $(ASTERISKGUI_DIR)/.configured
	$(MAKE) -C $(ASTERISKGUI_DIR) menuconfig

asterisk-gui-dirclean:
	rm -rf $(ASTERISKGUI_DIR)

################################################
# 
# Toplevel Makefile options
#
#################################################
#ifeq ($(strip $(SF_PACKAGE_ASTERISKGUI)),y)
TARGETS+=asterisk-gui
#endif
