#######################################################################
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# The Free Software Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# Copyright @ 2008 Astfin <mark@astfin.org>
# Primary Authors: mark@astfin.org
# Copyright @ 2010 SwitchFin <dpn@switchfin.org>
#######################################################################

##########################################
# Asterisk-h323 package for Astfin.org
##########################################

ASTERISK-H323_VERSION=1.4.6
ASTERISK-H323_NAME=asterisk-addons-$(ASTERISK-H323_VERSION)
ASTERISK-H323_DIR=$(BUILD_DIR)/$(ASTERISK-H323_NAME)
ASTERISK-H323_SOURCE=$(ASTERISK-H323_NAME).tar.gz
ASTERISK-H323_SITE=http://downloads.digium.com/pub/asterisk/releases
ASTERISK-H323_UNZIP=zcat

ASTERISK-H323_CPPFLAGS=-I$(STAGING_INC) -D__FIXED_PT__
ASTERISK-H323_CFLAGS=-g -mfast-fp -ffast-math -fno-jump-tables
ASTERISK-H323_LDFLAGS=-L$(STAGING_LIB)
ASTERISK-H323_DEP=
ASTERISK-H323_CONFIGURE_OPTS= --host=bfin-linux-uclibc --disable-largefile --without-pwlib --without-curl

$(DL_DIR)/$(ASTERISK-H323_SOURCE):
	$(WGET) -P $(DL_DIR) $(ASTERISK-H323_SITE)/$(ASTERISK-H323_SOURCE)

$(ASTERISK-H323_DIR)/.unpacked: $(DL_DIR)/$(ASTERISK-H323_SOURCE)
	$(ASTERISK-H323_UNZIP) $(DL_DIR)/$(ASTERISK-H323_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	cp -v package/asterisk-h323/menuselect.makeopts $(ASTERISK-H323_DIR)/menuselect.makeopts
	touch $(ASTERISK-H323_DIR)/.unpacked


$(ASTERISK-H323_DIR)/.configured: $(ASTERISK-H323_DIR)/.unpacked
	cd $(ASTERISK-H323_DIR); CPPFLAGS="$(ASTERISK-H323_CPPFLAGS)" CFLAGS="$(ASTERISK-H323_CFLAGS)" LDFLAGS="$(ASTERISK-H323_LDFLAGS)" ./configure $(ASTERISK-H323_CONFIGURE_OPTS)
	touch $(ASTERISK-H323_DIR)/.configured

asterisk-h323: asterisk $(ASTERISK-H323_DEP) $(ASTERISK-H323_DIR)/.configured

	OPTIMIZE="-Os" ASTCFLAGS="$(ASTERISK-H323_CPPFLAGS) $(ASTERISK-H323_CFLAGS)" ASTLDFLAGS="$(ASTERISK-H323_LDFLAGS)" \
	$(MAKE1) -C $(ASTERISK-H323_DIR) NOISY_BUILD=1

	$(TARGET_STRIP) $(ASTERISK-H323_DIR)/channels/chan_ooh323.so
	cp -f $(ASTERISK-H323_DIR)/channels/chan_ooh323.so $(TARGET_DIR)/usr/lib/asterisk/modules/

asterisk-h323-configure: $(ASTERISK-H323_DIR)/.configured

asterisk-h323-clean:
	rm -f $(ASTERISK-H323_DIR)/.configured
	 -$(MAKE) -C $(ASTERISK-H323_DIR) clean

asterisk-h323-dirclean:
	rm -rf $(ASTERISK-H323_DIR)

ifeq ($(strip $(SF_PACKAGE_ASTERISK-H323)),y)
asterisk-h323_: asterisk-h323
else
asterisk-h323_:
	rm -f $(TARGET_DIR)/usr/lib/asterisk/modules/chan_ooh323.so
endif
################################################
#
# Toplevel Makefile options
#
#################################################
TARGETS+=asterisk-h323_

