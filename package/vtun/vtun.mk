#######################################################################
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# The Free Software Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# Copyright @ 2011 SwitchFin <xjimenez@capatres.com>
#######################################################################

##########################################
# Vtun package for Astfin.org
##########################################

VTUN_VERSION=3.0.2
VTUN_NAME=vtun-$(VTUN_VERSION)
VTUN_DIR=$(BUILD_DIR)/$(VTUN_NAME)
VTUN_DIR_LINK=$(BUILD_DIR)/vtun
VTUN_SOURCE=$(VTUN_NAME).tar.gz
VTUN_SITE=http://prdownloads.sourceforge.net/vtun
VTUN_UNZIP=zcat

VTUN_CFLAGS=-D__FIXED_PT__ -D__BLACKFIN__
VTUN_CFLAGS+= -I$(STAGING_INC)
VTUN_LDFLAGS=-L$(STAGING_LIB) -lz -lcrypto

VTUN_CONFIGURE_OPTS = --host=bfin-linux-uclibc \
                --disable-lzo \
                --libdir=$(STAGING_LIB) \
		--prefix=

VTUN_CONFIGURE_OPTS+= CFLAGS="$(VTUN_CFLAGS)" LDFLAGS="$(VTUN_LDFLAGS)"

GNU_LD=0

$(DL_DIR)/$(VTUN_SOURCE):
	$(WGET) -P $(DL_DIR) $(VTUN_SITE)/$(VTUN_SOURCE)

$(VTUN_DIR)/.unpacked: $(DL_DIR)/$(VTUN_SOURCE)
	$(VTUN_UNZIP) $(DL_DIR)/$(VTUN_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	ln -sf $(VTUN_DIR) $(VTUN_DIR_LINK)
	$(PATCH_KERNEL) $(VTUN_DIR_LINK) package/vtun vtun_fork.patch
	$(PATCH_KERNEL) $(VTUN_DIR_LINK) package/vtun vtun_config.patch

$(VTUN_DIR)/.configured: $(VTUN_DIR)/.unpacked
	cd $(VTUN_DIR); ./configure $(VTUN_CONFIGURE_OPTS)
	touch $(VTUN_DIR)/.configured

vtun: $(VTUN_DEP) $(VTUN_DIR)/.configured
	OPTIMIZE="-O4" VTUNCFLAGS="$(VTUN_CFLAGS)" VTUNLDFLAGS="$(VTUN_LDFLAGS)" \
	$(MAKE1) CFLAGS="$(VTUN_CFLAGS)" LDFLAGS="$(VTUN_LDFLAGS)" -C $(VTUN_DIR) NOISY_BUILD=1 GNU_LD=0
	cp -v $(VTUN_DIR)/vtund $(TARGET_DIR)/bin

vtun-configure: $(VTUN_DIR)/.configured

vtun-clean:
	rm -f $(VTUN_DIR)/.configured
	 -$(MAKE) -C $(VTUN_DIR) clean

vtun-dirclean:
	rm -rf $(VTUN_DIR)
	rm -rf $(VTUN_DIR_LINK)

################################################
#
# Toplevel Makefile options
#
#################################################
ifeq ($(strip $(SF_PACKAGE_VTUN)),y)
TARGETS+=vtun
endif
