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

##########################################
# lighttpd package for Astfin.org
##########################################

LIGHTTPD_VERSION=1.4.19
LIGHTTPD_NAME=lighttpd-$(LIGHTTPD_VERSION)
LIGHTTPD_DIR=$(BUILD_DIR)/$(LIGHTTPD_NAME)
LIGHTTPD_DIR_LINK=$(BUILD_DIR)/lighttpd
LIGHTTPD_SOURCE=$(LIGHTTPD_NAME).tar.gz
LIGHTTPD_SITE=http://www.lighttpd.net/download
LIGHTTPD_UNZIP=zcat
LIGHTTPD_CONFIG=$(UCLINUX_KERNEL_SRC)/.config
LUA_SRC=$(shell find $(BUILD_DIR)/uClinux-dist/user/lua -type d | grep /src)

$(DL_DIR)/$(LIGHTTPD_SOURCE):
	$(WGET) -P $(DL_DIR) $(LIGHTTPD_SITE)/$(LIGHTTPD_SOURCE)

$(LIGHTTPD_DIR)/.unpacked: $(DL_DIR)/$(LIGHTTPD_SOURCE)
	$(LIGHTTPD_UNZIP) $(DL_DIR)/$(LIGHTTPD_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	patch -p0 -d $(LIGHTTPD_DIR) < package/lighttpd/lighttpd.patch;
	touch $(LIGHTTPD_DIR)/.unpacked

$(LIGHTTPD_DIR)/.configured: $(LIGHTTPD_DIR)/.unpacked
ifneq ($(strip $(SF_PACKAGE_LUA)),y)
	cd $(LIGHTTPD_DIR); \
		./configure \
                --host=bfin-linux-uclibc \
		--build=i686-linux \
		--disable-ipv6 \
		--without-zlib \
		--without-bzip2
else
	cd $(LIGHTTPD_DIR); \
                ./configure \
                --host=bfin-linux-uclibc \
                --build=i686-linux \
                --disable-ipv6 \
                --without-zlib \
                --without-bzip2 \
                --with-lua \
                --prefix=$(TARGET_DIR) \
                LUA_LIBS=$(LUA_SRC)/liblua.a LUA_CFLAGS=-I$(LUA_SRC)/

endif
	touch $(LIGHTTPD_DIR)/.configured

lighttpd: $(LIGHTTPD_DIR)/.configured
	$(MAKE) -C $(LIGHTTPD_DIR)
	cp -vf $(LIGHTTPD_DIR)/src/lighttpd $(TARGET_DIR)/bin
	cp -vf $(LIGHTTPD_DIR)/src/.libs/mod_access.so $(TARGET_DIR)/lib
	cp -vf $(LIGHTTPD_DIR)/src/.libs/mod_cgi.so $(TARGET_DIR)/lib
	-cp -vf $(LIGHTTPD_DIR)/src/.libs/mod_magnet.so $(TARGET_DIR)/lib
	cp -vf $(LIGHTTPD_DIR)/src/.libs/mod_indexfile.so $(TARGET_DIR)/lib
	cp -vf $(LIGHTTPD_DIR)/src/.libs/mod_dirlisting.so $(TARGET_DIR)/lib
	cp -vf $(LIGHTTPD_DIR)/src/.libs/mod_staticfile.so $(TARGET_DIR)/lib
	$(TARGET_STRIP) $(TARGET_DIR)/bin/lighttpd $(TARGET_DIR)/lib/mod_access.so \
	$(TARGET_DIR)/lib/mod_cgi.so $(TARGET_DIR)/lib/mod_magnet.so \
	$(TARGET_DIR)/lib/mod_indexfile.so $(TARGET_DIR)/lib/mod_dirlisting.so \
	$(TARGET_DIR)/lib/mod_staticfile.so
	-$(TARGET_STRIP) $(TARGET_DIR)/lib/mod_magnet.so
	cp -vf package/lighttpd/lighttpd.conf $(TARGET_DIR)/etc

lighttpd-configure: $(LIGHTTPD_DIR)/.configured

lighttpd-clean:
	rm -f $(LIGHTTPD_DIR)/.configured
	-$(MAKE) -C $(LIGHTTPD_DIR) clean

lighttpd-dirclean:
	rm -rf $(LIGHTTPD_DIR)
	rm -rf $(LIGHTTPD_DIR_LINK)
lighttpd-image:
	mkdir -p $(TARGET_DIR)/etc/autorun
	cp -fv package/lighttpd/lighttpd.sh $(TARGET_DIR)/etc/autorun
	

ifeq ($(strip $(SF_PACKAGE_LIGHTTPD)),y)
lighttpd_: lighttpd 
else
lighttpd_:
	rm -f $(TARGET_DIR)/bin/lighttpd
	rm -f $(TARGET_DIR)/lib/mod_access.so $(TARGET_DIR)/lib/mod_cgi.so \
	$(TARGET_DIR)/lib/mod_magnet.so $(TARGET_DIR)/lib/mod_indexfile.so \
        $(TARGET_DIR)/lib/mod_dirlisting.so $(TARGET_DIR)/lib/mod_staticfile.so
endif

################################################
#
# Toplevel Makefile options
#
#################################################
TARGETS+=lighttpd_
