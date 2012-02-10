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
# mk file by Kelvin Chua <kelvin@nextixsystems.com>
#########################################################################

##########################################
# ghostscript package for ecofax
##########################################

GHOSTSCRIPT_VERSION=8.64
GHOSTSCRIPT_NAME=ghostscript-$(GHOSTSCRIPT_VERSION)
GHOSTSCRIPT_DIR=$(BUILD_DIR)/$(GHOSTSCRIPT_NAME)
GHOSTSCRIPT_SOURCE=$(GHOSTSCRIPT_NAME).tar.gz
GHOSTSCRIPT_SITE=http://downloads.ghostscript.com/public
GHOSTSCRIPT_UNZIP=zcat

$(DL_DIR)/$(GHOSTSCRIPT_SOURCE):
	$(WGET) -P $(DL_DIR) $(GHOSTSCRIPT_SITE)/$(GHOSTSCRIPT_SOURCE)

$(GHOSTSCRIPT_DIR)/.unpacked: $(DL_DIR)/$(GHOSTSCRIPT_SOURCE)
	$(GHOSTSCRIPT_UNZIP) $(DL_DIR)/$(GHOSTSCRIPT_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(GHOSTSCRIPT_DIR)/.unpacked

$(GHOSTSCRIPT_DIR)/.configured: $(GHOSTSCRIPT_DIR)/.unpacked
	cd $(GHOSTSCRIPT_DIR); \
             ./configure --host=bfin-linux-uclibc --with-drivers=tiffg3 --disable-gtk --disable-cairo --disable-cups --disable-fontconfig --with-x=no --without-jasper --without-jbig2dec --without-ijs --without-omni --without-libpaper --disable-contrib --with-libiconv=no --disable-compile-inits --with-system-libtiff --without-pdftoraster --prefix=/usr
	touch $(GHOSTSCRIPT_DIR)/.configured

ghostscript: $(GHOSTSCRIPT_DIR)/.configured
	-mkdir $(GHOSTSCRIPT_DIR)/obj/
	cp $(SOURCES_DIR)/ghostscript/echogs $(GHOSTSCRIPT_DIR)/obj/
	cp $(SOURCES_DIR)/ghostscript/genarch $(GHOSTSCRIPT_DIR)/obj/
	cp $(SOURCES_DIR)/ghostscript/genconf $(GHOSTSCRIPT_DIR)/obj/
	$(MAKE) -C $(GHOSTSCRIPT_DIR) CAPOPT= GENOPT= AR=bfin-linux-uclibc-ar CC=bfin-linux-uclibc-gcc CCLD=bfin-linux-uclibc-gcc
	mkdir -p $(TARGET_DIR)/usr/share/ghostscript/$(GHOSTSCRIPT_VERSION)/Resource
	cp -r $(GHOSTSCRIPT_DIR)/Resource $(TARGET_DIR)/usr/share/ghostscript/$(GHOSTSCRIPT_VERSION)/
	cp -r $(GHOSTSCRIPT_DIR)/bin/gs $(TARGET_DIR)/bin/

	#add GUI and zoneinfo
	cp -r $(SOURCES_DIR)/ghostscript/www $(TARGET_DIR)/var/
	cp -r $(SOURCES_DIR)/ghostscript/zoneinfo $(TARGET_DIR)/usr/share/

ghostscript-configure: $(GHOSTSCRIPT_DIR)/.configured

ghostscript-clean:
	rm -f $(GHOSTSCRIPT_DIR)/.configured
	-$(MAKE) -C $(GHOSTSCRIPT_DIR) clean

ghostscript-dirclean:
	rm -rf $(GHOSTSCRIPT_DIR)

ifeq ($(strip $(SF_PACKAGE_GHOSTSCRIPT)),y)
ghostscript_: ghostscript 
else
ghostscript_:
	rm -f $(TARGET_DIR)/bin/gs
	rm -rf $(TARGET_DIR)/usr/share/ghostscript
endif

################################################
#
# Toplevel Makefile options
#
#################################################
TARGETS+=ghostscript_
