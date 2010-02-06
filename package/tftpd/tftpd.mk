######################################################################
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# The Free Software Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# Copyright @ 2008 Astfin <mark@astfin.org>
# Primary Authors: mark@astfin.org, Jeff Knighton
# Copyright @ 2010 SwitchFin <dpn@switchvoice.com>
#######################################################################

##########################################
# Tftpd package for Astfin.org
##########################################

TFTPD_SITE=http://downloads.openwrt.org/sources
TFTPD_VERSION=0.7
TFTPD_SOURCE=atftp-$(TFTPD_VERSION).tar.gz
TFTPD_UNZIP=zcat
TFTPD_DIR=$(BUILD_DIR)/atftp-$(TFTPD_VERSION)
TFTPD_CONFIGURE_OPTS=--host=bfin-linux-uclibc 


$(DL_DIR)/$(TFTPD_SOURCE):
	$(WGET) -P $(DL_DIR) $(TFTPD_SITE)/$(TFTPD_SOURCE)

tftpd-source: $(DL_DIR)/$(TFTPD_SOURCE)

$(TFTPD_DIR)/.unpacked: $(DL_DIR)/$(TFTPD_SOURCE)
	$(TFTPD_UNZIP) $(DL_DIR)/$(TFTPD_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(TFTPD_DIR)/.unpacked

$(TFTPD_DIR)/.configured: $(TFTPD_DIR)/.unpacked
	chmod a+x $(TFTPD_DIR)/configure
	cp -vf package/tftpd/config.sub-atftp $(TFTPD_DIR)/config.sub
	$(PATCH_KERNEL) $(TFTPD_DIR) package/tftpd tftpd.patch
	cd $(TFTPD_DIR); ./configure $(TFTPD_CONFIGURE_OPTS)
	touch $(TFTPD_DIR)/.configured

	# setup directories for package
	mkdir -p $(TARGET_DIR)/bin


tftpd: $(TFTPD_DIR)/.configured
	make -C $(TFTPD_DIR)/ STAGEDIR=$(STAGING_DIR)
	#copy to the package location
	cp -f $(TFTPD_DIR)/atftpd $(TARGET_DIR)/bin/in.tftpd

tftpd-dirclean:
	rm -rf $(TFTPD_DIR)

tftpd-clean:
	rm -rf $(TFTPD_DIR)


################################################
#
# Toplevel Makefile options
#
#################################################
ifeq ($(strip $(SF_PACKAGE_TFTPD)),y)
TARGETS+=tftpd
endif

