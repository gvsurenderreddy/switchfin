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
# Primary Author: mark@astfin.org
# Copyright @ 2010 SwitchFin <dpn@switchfin.org>
#########################################################################

CGIC_MAJ_VERSION=2
CGIC_MIN_VERSION=05
CGIC_VER=$(CGIC_MAJ_VERSION)$(CGIC_MIN_VERSION)
CGIC_DIRNAME=cgic$(CGIC_VER)
CGIC_DIR=$(BUILD_DIR)/$(CGIC_DIRNAME)
CGIC_SOURCE=cgic$(CGIC_VER).tar.gz
CGIC_SITE=http://www.boutell.com/cgic
CGIC_CONFIGURE_OPTS = --host=bfin-linux-uclibc \
		--prefix=$(STAGING_DIR)/usr \
		--libdir=$(STAGING_LIB)

CGIC_CFLAGS=-mfdpic -Os -I$(STAGING_INC)
CGIC_LDFLAGS=-L$(STAGING_LIB)

$(DL_DIR)/$(CGIC_SOURCE):
	mkdir -p dl
	wget -P $(DL_DIR) $(CGIC_SITE)/$(CGIC_SOURCE)

$(CGIC_DIR)/.unpacked: $(DL_DIR)/$(CGIC_SOURCE)
	zcat $(DL_DIR)/$(CGIC_SOURCE) | tar -C $(BUILD_DIR) -xf -
	$(PATCH_KERNEL) $(CGIC_DIR) package/cgic cgic.patch
	touch $(CGIC_DIR)/.unpacked

cgic: $(CGIC_DIR)/.unpacked
	 make CC="$(TARGET_CC)" CFLAGS="$(CGIC_CFLAGS)" LDFLAGS="$(CGIC_LDFLAGS)" \
	AR="$(TARGET_AR)" RANLIB="$(TARGET_RANLIB)" PREFIX="$(STAGING_DIR)/usr" \
	-C $(CGIC_DIR) all
	$(TARGET_STRIP) $(CGIC_DIR)/*.cgi
	 make CC="$(TARGET_CC)" CFLAGS="$(CGIC_CFLAGS)" LDFLAGS="$(CGIC_LDFLAGS)" \
	AR="$(TARGET_AR)" RANLIB="$(TARGET_RANLIB)" PREFIX="$(STAGING_DIR)/usr" \
	-C $(CGIC_DIR) install


cgic-dirclean:
	rm -rf $(CGIC_DIR)

################################################
#
# Toplevel Makefile options
#
#################################################
ifeq ($(strip $(SF_PACKAGE_CGIC)),y)
TARGETS+=cgic
endif


