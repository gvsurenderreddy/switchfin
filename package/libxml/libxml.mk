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
# Primary Authors: mark@astfin.org, pawel@astfin.org, li770426@gmail.com
# Copyright @ 2010 SwitchFin <dpn@switchfin.org>
#########################################################################

#####################################################
# XML setting for uCLinux
#####################################################

LIBXML_SITE=ftp://xmlsoft.org/libxml2
LIBXML_VERSION=2.7.8
LIBXML_SOURCE=libxml2-sources-$(LIBXML_VERSION).tar.gz
LIBXML_UNZIP=zcat
LIBXML_CONFIGURE_OPTS=--host=$(CROSS_COMPILE) --without-python
LIBXML_DIR=$(BUILD_DIR)/libxml2-$(LIBXML_VERSION)

$(DL_DIR)/$(LIBXML_SOURCE):
	$(WGET) -P $(DL_DIR) $(LIBXML_SITE)/$(LIBXML_SOURCE)

libxml-source: $(DL_DIR)/$(LIBXML_SOURCE)

$(LIBXML_DIR)/.unpacked: $(DL_DIR)/$(LIBXML_SOURCE)
	$(LIBXML_UNZIP) $(DL_DIR)/$(LIBXML_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(LIBXML_DIR)/.unpacked

$(LIBXML_DIR)/.configured: $(LIBXML_DIR)/.unpacked
	cd $(LIBXML_DIR); ./configure $(LIBXML_CONFIGURE_OPTS)
	touch $(LIBXML_DIR)/.configured

libxml: $(LIBXML_DIR)/.configured
	$(MAKE) -C "$(LIBXML_DIR)" libxml2.la
	$(TARGET_STRIP) --strip-unneeded -o $(TARGET_DIR)/lib/libxml2.so $(LIBXML_DIR)/.libs/libxml2.so
	ln -sf libxml2.so $(TARGET_DIR)/lib/libxml2.so.2

libxml-clean:
	rm -rf $(LIBXML_DIR)/.configured
	 -$(MAKE) -C $(LIBXML_DIR) clean

libxml-dirclean:
	rm -rf $(LIBXML_DIR)
################################################
#
# Toplevel Makefile options
#
#################################################
ifeq ($(strip $(SF_PACKAGE_LIBXML)),y)
TARGETS+=libxml
endif
