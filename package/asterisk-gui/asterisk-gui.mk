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


ifeq ($(strip $(SF_ASTERISK_GUI_3_0)),y)
ASTERISKGUI_DIR=$(BUILD_DIR)/asterisk-gui-3.0
ASTERISKGUI_UNPACKED=asterisk-gui-3.0
ASTERISKGUI_SITE=https://switchfin.svn.sourceforge.net/svnroot/switchfin/asterisk-gui/tags/3.0/
else
ASTERISKGUI_DIR=$(BUILD_DIR)/asterisk-gui-4.0
ASTERISKGUI_UNPACKED=asterisk-gui-4.0
ASTERISKGUI_SITE=https://switchfin.svn.sourceforge.net/svnroot/switchfin/asterisk-gui/trunk/
endif
#
# Target not currently used as statically keeping the code locally in svn
# in the dl directory
#

$(DL_DIR)/$(ASTERISKGUI_UNPACKED):
	$(SVN)  $(ASTERISKGUI_SITE) $(DL_DIR)/$(ASTERISKGUI_UNPACKED)

asterisk-gui-source: $(DL_DIR)/$(ASTERISKGUI_UNPACKED)


$(ASTERISKGUI_DIR)/.unpacked: $(DL_DIR)/$(ASTERISKGUI_UNPACKED)
	cp -R $(DL_DIR)/$(ASTERISKGUI_UNPACKED) $(ASTERISKGUI_DIR)
	touch $(ASTERISKGUI_DIR)/.unpacked

$(ASTERISKGUI_DIR)/.configured: $(ASTERISKGUI_DIR)/.unpacked
	touch $(ASTERISKGUI_DIR)/.configured

gui_check_prev_ver:
ifeq ($(strip $(SF_ASTERISK_GUI_3_0)),y)
	if test -d $(BUILD_DIR)/asterisk-gui-4.0; then \
		rm -rf $(BUILD_DIR)/asterisk-gui-4.0/; \
	fi
	rm -rf $(TARGET_DIR)/var/lib/asterisk/
	rm -rf $(BUILD_DIR)/asterisk-gui-3.0/
endif
ifeq ($(strip $(SF_ASTERISK_GUI_4_0)),y)
	if test -d $(BUILD_DIR)/asterisk-gui-3.0; then \
		rm -rf $(BUILD_DIR)/asterisk-gui-3.0/; \
	fi
	rm -rf $(TARGET_DIR)/var/lib/asterisk/
	rm -rf $(BUILD_DIR)/asterisk-gui-4.0/
endif

asterisk-gui: gui_check_prev_ver $(ASTERISKGUI_DIR)/.configured
	svn up $(DL_DIR)/$(ASTERISKGUI_UNPACKED)
	-find $(ASTERISKGUI_DIR) -type d -name .svn | xargs rm -rf
	#mkdir -p $(TARGET_DIR)/etc/scripts
	mkdir -p $(TARGET_DIR)/var/lib/asterisk
	mkdir -p $(TARGET_DIR)/var/lib/asterisk/scripts
	mkdir -p $(TARGET_DIR)/var/lib/asterisk/static-http
	mkdir -p $(TARGET_DIR)/var/lib/asterisk/static-http/config
	cp -Rv $(ASTERISKGUI_DIR)/config/* $(TARGET_DIR)/var/lib/asterisk/static-http/config/
	cp -v $(ASTERISKGUI_DIR)/scripts/* $(TARGET_DIR)/var/lib/asterisk/scripts/
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
ifeq ($(strip $(SF_PACKAGE_ASTERISKGUI)),y)
TARGETS+=asterisk-gui
endif

