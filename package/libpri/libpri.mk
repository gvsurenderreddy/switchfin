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

#####################################################
# libpri setting for uCLinux and zaptel
#####################################################

LIBPRI_SITE=http://downloads.digium.com/pub/libpri/releases
LIBPRI_VERSION=1.4.3
LIBPRI_SOURCE=libpri-$(LIBPRI_VERSION).tar.gz
LIBPRI_UNZIP=zcat
LIBPRI_DIR=$(BUILD_DIR)/libpri-$(LIBPRI_VERSION)

$(DL_DIR)/$(LIBPRI_SOURCE):
	$(WGET) -P $(DL_DIR) $(LIBPRI_SITE)/$(LIBPRI_SOURCE)

libpri-source: $(DL_DIR)/$(LIBPRI_SOURCE)

$(LIBPRI_DIR)/.unpacked: $(DL_DIR)/$(LIBPRI_SOURCE)
	$(LIBPRI_UNZIP) $(DL_DIR)/$(LIBPRI_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(LIBPRI_DIR)/.unpacked


$(LIBPRI_DIR)/.configured: $(LIBPRI_DIR)/.unpacked
	cd $(LIBPRI_DIR); \
	patch -p0 < $(BASE_DIR)/package/libpri/libpri.patch
	touch $(LIBPRI_DIR)/.configured

libpri: $(LIBPRI_DIR)/.configured
	$(MAKE) CC=$(TARGET_CC) DESTDIR=$(TARGET_DIR) STAGEDIR=$(STAGING_DIR) \
	INSTALL_BASE="/"  -C $(LIBPRI_DIR)
	cp -f $(LIBPRI_DIR)/libpri.so.1.0 $(TARGET_DIR)/lib/
	$(TARGET_STRIP) $(TARGET_DIR)/lib/libpri.so.1.0
	cd $(TARGET_DIR)/lib/; \
	ln -sf libpri.so.1.0 libpri.so.1; \
	ln -sf libpri.so.1.0 libpri.so

libpri-clean:
	$(MAKE) -C $(LIBPRI_DIR) clean

libpri-dirclean:
	rm -rf $(LIBPRI_DIR)
################################################
#
# Toplevel Makefile options
#
#################################################
ifeq ($(strip $(SF_PR1_APPLIANCE)),y)
#TARGETS+=libpri
endif
