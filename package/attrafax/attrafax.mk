#######################################################################
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# The Free Software Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# Copyright @ 2010 SwitchFin <dpn@switchfin.org>
#######################################################################

ATTRAFAX_PATCH=attrafax.patch
ATTRAFAX_SITE=http://www.zoiper.com/downloads/
ATTRAFAX_NAME=attrafax-0.9
ATTRAFAX_FILENAME=$(ATTRAFAX_NAME).tgz
ATTRAFAX_DIR=$(BUILD_DIR)/$(ATTRAFAX_NAME)
UNZIP=zcat

$(DL_DIR)/$(ATTRAFAX_FILENAME):
	$(WGET) -P $(DL_DIR) $(ATTRAFAX_SITE)/$(ATTRAFAX_FILENAME)

$(ATTRAFAX_DIR)/.unpacked: $(DL_DIR)/$(ATTRAFAX_FILENAME)
	$(UNZIP) $(DL_DIR)/$(ATTRAFAX_FILENAME) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	patch -p1 -d $(ATTRAFAX_DIR) < package/attrafax/attrafax.patch 
	touch $(ATTRAFAX_DIR)/.unpacked

attrafax: asterisk $(ATTRAFAX_DIR)/.unpacked
	#Compile t30 library
	cd $(ATTRAFAX_DIR); ./compile.sh $(STAGING_INC) $(STAGING_LIB)
	#Build Asterisk modules
	cd $(ATTRAFAX_DIR)/src/modules/; make STAGING_INC=$(STAGING_INC) ASTERISK_INC=$(BUILD_DIR)/asterisk/include/ \
	STAGING_LIB=${STAGING_LIB}
	#Copy to the rootfs
	mkdir -p $(TARGET_DIR)/lib/
	mkdir -p $(TARGET_DIR)/usr/lib/asterisk/modules 
	cp -f $(ATTRAFAX_DIR)/bin/libt30.so $(TARGET_DIR)/lib/
	find $(ATTRAFAX_DIR)/src/modules/ -name '*.so' -exec cp -v "{}" $(TARGET_DIR)/usr/lib/asterisk/modules/ \;
	$(TARGET_STRIP) $(TARGET_DIR)/usr/lib/asterisk/modules/*.so
	$(TARGET_STRIP) $(TARGET_DIR)/lib/libt30.so

ifeq ($(strip $(SF_PACKAGE_ATTRAFAX)),y)
attrafax_: attrafax
else
attrafax_:
	-cd $(TARGET_DIR)/usr/lib/asterisk/modules/; \
	rm -rf app_faxdetect.so app_sndrec.so app_t30rxfax.so app_t30txfax.so app_t38rxfax.so \
	app_t38rxfaxp.so app_t38txfax.so   app_t38txfaxp.so  gateway.so
endif

################################################
#
# Toplevel Makefile options
#
#################################################
TARGETS+=attrafax_
