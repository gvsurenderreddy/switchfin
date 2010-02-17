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
LIBXML_VERSION=2.6.31
LIBXML_SOURCE=libxml2-sources-$(LIBXML_VERSION).tar.gz
LIBXML_UNZIP=zcat
LIBXML_CONFIGURE_OPTS=--host=bfin-linux-uclibc --build=i686-linux --without-python --prefix=$(STAGING_DIR)
LIBXML_DIR=$(BUILD_DIR)/libxml2-$(LIBXML_VERSION)

export CC=bfin-linux-uclibc-gcc

$(DL_DIR)/$(LIBXML_SOURCE):
	$(WGET) -P $(DL_DIR) $(LIBXML_SITE)/$(LIBXML_SOURCE)

libxml-source: $(DL_DIR)/$(LIBXML_SOURCE)

$(LIBXML_DIR)/.unpacked: $(DL_DIR)/$(LIBXML_SOURCE)
	$(LIBXML_UNZIP) $(DL_DIR)/$(LIBXML_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(LIBXML_DIR)/.unpacked


$(LIBXML_DIR)/.configured: $(LIBXML_DIR)/.unpacked
	export CC=bfin-linux-uclibc-gcc
	cd $(LIBXML_DIR); ./configure $(LIBXML_CONFIGURE_OPTS)
	touch $(LIBXML_DIR)/.configured

libxml: $(LIBXML_DIR)/.configured
	$(MAKE) -C "$(LIBXML_DIR)" ARCH="bfin-linux-uclibc"  \
                LIBDIR="usr/lib"\
                CROSS="$(TARGET_CROSS)" \
                CFLAGS="$(LIBXML_CFLAGS)" \
                LDFLAGS="-L$(STAGING_LIB) -mfdpic -lpthread -ldl" \
		install
	cp -v $(LIBXML_DIR)/.libs/libxml2.so $(TARGET_DIR)/lib/libxml2.so
	cd $(TARGET_DIR)/lib; ln -sf libxml2.so libxml2.so.2



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
