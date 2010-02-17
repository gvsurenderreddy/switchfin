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
# Primary Authors: mark@astfin.org, pawel@astfin.org, david@rowetel.com
# Copyright @ 2010 SwitchFin <dpn@switchfin.org>
#########################################################################

##############################################
# sqlite3 package for Astfin.org based on BAPS
##############################################

SQLITE3_VERSION=3.6.11
SQLITE3_DIRNAME=sqlite-$(SQLITE3_VERSION)
SQLITE3_DIR=$(BUILD_DIR)/$(SQLITE3_DIRNAME)
SQLITE3_SITE=http://www.sqlite.org
SQLITE3_SOURCE=sqlite-$(SQLITE3_VERSION).tar.gz
SQLITE3_CONFIGURE_OPTS = --host=bfin-linux-uclibc \
		--prefix=$(STAGING_DIR)/usr \
                --enable-shared \
		--disable-static \
                --disable-tcl \
		--libdir=$(STAGING_LIB)

SQLITE3_CFLAGS=-mfdpic -Os -I$(STAGING_INC)
SQLITE3_LDFLAGS=-L$(STAGING_LIB)

$(DL_DIR)/$(SQLITE3_SOURCE):
	mkdir -p dl
	wget -P $(DL_DIR) $(SQLITE3_SITE)/$(SQLITE3_SOURCE)

$(SQLITE3_DIR)/.unpacked: $(DL_DIR)/$(SQLITE3_SOURCE)
	zcat $(DL_DIR)/$(SQLITE3_SOURCE) | tar -C $(BUILD_DIR) -xf -
	$(PATCH_KERNEL) $(SQLITE3_DIR) package/sqlite3 sqlite3.patch
	touch $(SQLITE3_DIR)/.unpacked

$(SQLITE3_DIR)/.configured: $(SQLITE3_DIR)/.unpacked
	cd $(SQLITE3_DIR); ./configure $(SQLITE3_CONFIGURE_OPTS)
	touch $(SQLITE3_DIR)/.configured

sqlite3: $(SQLITE3_DIR)/.configured
	 make CFLAGS="$(SQLITE3_CFLAGS)" LDFLAGS="$(SQLITE3_LDFLAGS)" \
	-C $(SQLITE3_DIR) install

	mkdir -p $(TARGET_DIR)/bin
	mkdir -p $(TARGET_DIR)/lib
	cp $(STAGING_LIB)/libsqlite3.so.0.8.6 $(TARGET_DIR)/lib
	ln -sf libsqlite3.so.0.8.6 $(TARGET_DIR)/lib/libsqlite3.so 
	ln -sf libsqlite3.so.0.8.6 $(TARGET_DIR)/lib/libsqlite3.so.0
	$(TARGET_STRIP) $(TARGET_DIR)/lib/libsqlite3.so.0.8.6

sqlite3-dirclean:
	rm -rf $(SQLITE3_DIR)

################################################
#
# Toplevel Makefile options
#
#################################################
ifeq ($(strip $(SF_PACKAGE_SQLITE3)),y)
TARGETS+=sqlite3
endif


