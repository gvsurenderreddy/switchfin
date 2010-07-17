#########################################################################
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# The Free Software Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# Copyright @ 2010 SwitchFin
#########################################################################

##########################################
# UW-IMAP package for SwitchFin
##########################################

UW-IMAP_VERSION=2007e
UW-IMAP_NAME=imap-$(UW-IMAP_VERSION)
UW-IMAP_DIR=$(BUILD_DIR)/$(UW-IMAP_NAME)
UW-IMAP_SOURCE=$(UW-IMAP_NAME).tar.gz
UW-IMAP_SITE=ftp://ftp.cac.washington.edu/imap
UW-IMAP_UNZIP=zcat
UW-IMAP_PATCH=uw-imap.patch

$(DL_DIR)/$(UW-IMAP_SOURCE):
	$(WGET) -P $(DL_DIR) $(UW-IMAP_SITE)/$(UW-IMAP_SOURCE)

$(UW-IMAP_DIR)/.unpacked: $(DL_DIR)/$(UW-IMAP_SOURCE)
	$(UW-IMAP_UNZIP) $(DL_DIR)/$(UW-IMAP_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(UW-IMAP_DIR)/.unpacked

$(UW-IMAP_DIR)/.configured: $(UW-IMAP_DIR)/.unpacked
	$(PATCH_KERNEL) $(UW-IMAP_DIR) package/uw-imap $(UW-IMAP_PATCH)
	touch $(UW-IMAP_DIR)/.configured

uw-imap: $(UW-IMAP_DIR)/.configured
	$(MAKE) -C $(UW-IMAP_DIR) lnx IP=4 SSLTYPE=none CC=$(CC) LDFLAGS=$(LDFLAGS)

uw-imap-clean:
	rm -f $(UW-IMAP_DIR)/.configured
	-$(MAKE) -C $(UW-IMAP_DIR) clean

uw-imap-dirclean:
	rm -rf $(UW-IMAP_DIR)

################################################
# 
# Toplevel Makefile options
#
#################################################
ifeq ($(strip $(SF_PACKAGE_UW-IMAP)),y)
TARGETS+=uw-imap
endif
