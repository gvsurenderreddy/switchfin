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
# Primary Authors: mark@astfin.org, pawel@astfin.org, Diego Searfin
# Primary Authors: David@rowetel.com, Jeff Knighton
# Copyright @ 2010 SwitchFin <dpn@switchvoice.com>
#######################################################################

##########################################
# Asterisk package for Astfin.org
##########################################

ASTERISK_VERSION=1.4.21.2
ASTERISK_NAME=asterisk-$(ASTERISK_VERSION)
ASTERISK_DIR=$(BUILD_DIR)/$(ASTERISK_NAME)
ASTERISK_DIR_LINK=$(BUILD_DIR)/asterisk
ASTERISK_SOURCE=$(ASTERISK_NAME).tar.gz
ASTERISK_SITE=http://downloads.digium.com/pub/asterisk/releases
ASTERISK_UNZIP=zcat

ASTERISK_CFLAGS=-g -mfdpic -mfast-fp -ffast-math -D__FIXED_PT__ -D__BLACKFIN__
ASTERISK_CFLAGS+= -I$(STAGING_INC) -fno-jump-tables
ASTERISK_LDFLAGS=-mfdpic -L$(STAGING_LIB) -lpthread -ldl -ltonezone -lsqlite3 -lspeexdsp
ifeq ($(strip $(SF_SPANDSP_CALLERID)),y)
ASTERISK_CFLAGS+= -DUSE_SPANDSP_CALLERID
ASTERISK_LDFLAGS+= -lspandsp -ltiff
endif
ASTERISK_CONFIGURE_OPTS= --host=bfin-linux-uclibc --disable-largefile --without-pwlib
ASTERISK_CONFIGURE_OPTS+= --without-curl
ASTERISK_DEP=""

ifeq ($(strip $(SF_PACKAGE_MISDNUSER)),y)
ASTERISK_DEP= mISDNuser
ASTERISK_CFLAGS+= -I$(STAGING_INC)/include
ASTERISK_CONFIGURE_OPTS+= --with-misdn
endif
ASTERISK_CONFIGURE_OPTS+= CFLAGS="$(ASTERISK_CFLAGS)" LDFLAGS="$(ASTERISK_LDFLAGS)"

$(DL_DIR)/$(ASTERISK_SOURCE):
	$(WGET) -P $(DL_DIR) $(ASTERISK_SITE)/$(ASTERISK_SOURCE)

$(ASTERISK_DIR)/.unpacked: $(DL_DIR)/$(ASTERISK_SOURCE)
	$(ASTERISK_UNZIP) $(DL_DIR)/$(ASTERISK_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	ln -sf $(ASTERISK_DIR) $(ASTERISK_DIR_LINK)
	$(PATCH_KERNEL) $(ASTERISK_DIR_LINK) package/asterisk asterisk.patch
	$(PATCH_KERNEL) $(ASTERISK_DIR_LINK) package/asterisk cid.patch
	rm -f $(ASTERISK_DIR)/channels/chan_misdn/channels/chan_misdn.c 
	rm -f $(ASTERISK_DIR)/channels/chan_misdn/channels/misdn_config.c
	rm -f $(ASTERISK_DIR)/channels/chan_misdn/channels/misdn/chan_misdn_config.h
	rm -f $(ASTERISK_DIR)/channels/chan_misdn/channels/misdn/isdn_lib.c
	rm -f $(ASTERISK_DIR)/channels/chan_misdn/channels/misdn/isdn_lib.h
	ln -sf $(SOURCES_DIR)/chan_misdn/channels/chan_misdn.c $(ASTERISK_DIR_LINK)/channels/
	ln -sf $(SOURCES_DIR)/chan_misdn/channels/misdn_config.c $(ASTERISK_DIR_LINK)/channels/
	ln -sf $(SOURCES_DIR)/chan_misdn/channels/misdn/chan_misdn_config.h $(ASTERISK_DIR_LINK)/channels/misdn/
	ln -sf $(SOURCES_DIR)/chan_misdn/channels/misdn/isdn_lib.c $(ASTERISK_DIR_LINK)/channels/misdn/
	ln -sf $(SOURCES_DIR)/chan_misdn/channels/misdn/isdn_lib.h $(ASTERISK_DIR_LINK)/channels/misdn/
ifeq ($(strip $(SF_PACKAGE_ASTERISK_G729)),y)
	ln -sf $(SOURCES_DIR)/codec_g729.c $(ASTERISK_DIR)/codecs
	ln -sf $(SOURCES_DIR)/g729ab_codec.h $(ASTERISK_DIR)/codecs
endif   
	ln -sf $(SOURCES_DIR)/codec_speex.c $(ASTERISK_DIR)/codecs
#	ln -sf $(SOURCES_DIR)/cdr_sqlite3.c $(ASTERISK_DIR)/cdr
	touch $(ASTERISK_DIR)/.unpacked


$(ASTERISK_DIR)/.configured: $(ASTERISK_DIR)/.unpacked
ifeq ($(strip $(SF_PACKAGE_MISDNUSER)),y)
	cp -v package/asterisk/embedded_template_misdn $(ASTERISK_DIR)/menuselect.makeopts
else
	cp -v package/asterisk/embedded_template $(ASTERISK_DIR)/menuselect.makeopts
endif
#	ln -sf $(SOURCES_DIR)/res_sqlite3.c $(ASTERISK_DIR)/res/
	cd $(ASTERISK_DIR); ./configure $(ASTERISK_CONFIGURE_OPTS)
	sed -i -e "s/^# CONFIG_ZAPTEL is not set/CONFIG_ZAPTEL=$(SF_PACKAGE_ZAPTEL)/" $(ZAPTEL_CONFIG);
	touch $(ASTERISK_DIR)/.configured

$(STAGING_LIB)/libgsm.a:
	make -C $(UCLINUX_DIST)/lib/blackfin-gsm \
	CC=$(TARGET_CC) AR=bfin-linux-uclibc-ar
	ln -sf $(UCLINUX_DIST)/lib/blackfin-gsm/gsm/lib/libgsm.a $(STAGING_LIB)
	ln -sf $(UCLINUX_DIST)/lib/blackfin-gsm/gsm/inc/gsm.h $(STAGING_INC)


ifeq ($(strip $(SF_PR1_APPLIANCE)),y)
ifeq ($(strip $(SF_SPANDSP_CALLERID)),y)
asterisk: sqlite3 zaptel spandsp libpri $(ASTERISK_DEP) $(ASTERISK_DIR)/.configured
else
asterisk: sqlite3 zaptel libpri $(ASTERISK_DEP) $(ASTERISK_DIR)/.configured
endif
endif
ifeq ($(strip $(SF_BR4_APPLIANCE)),y)
ifeq ($(strip $(SF_SPANDSP_CALLERID)),y)
asterisk: sqlite3 zaptel spandsp mISDNuser $(ASTERISK_DEP) $(ASTERISK_DIR)/.configured
else
asterisk: sqlite3 zaptel mISDNuser $(ASTERISK_DEP) $(ASTERISK_DIR)/.configured
endif
endif
ifeq ($(strip $(SF_IP04)),y)
ifeq ($(strip $(SF_SPANDSP_CALLERID)),y)
asterisk: sqlite3 zaptel spandsp $(ASTERISK_DEP) $(ASTERISK_DIR)/.configured
else
asterisk: sqlite3 zaptel $(ASTERISK_DEP) $(ASTERISK_DIR)/.configured
endif
endif
ifeq ($(strip $(SF_IP01)),y)
ifeq ($(strip $(SF_SPANDSP_CALLERID)),y)
asterisk: sqlite3 zaptel spandsp $(ASTERISK_DEP) $(ASTERISK_DIR)/.configured
else
asterisk: sqlite3 zaptel $(ASTERISK_DEP) $(ASTERISK_DIR)/.configured
endif
endif

ifeq ($(strip $(SF_FX08)),y)
ifeq ($(strip $(SF_SPANDSP_CALLERID)),y)
asterisk: sqlite3 zaptel spandsp $(ASTERISK_DEP) $(ASTERISK_DIR)/.configured
else
asterisk: sqlite3 zaptel $(ASTERISK_DEP) $(ASTERISK_DIR)/.configured
endif
endif

ifeq ($(strip $(SF_PACKAGE_ASTERISK_VERBOSE)),y)
	-$(MAKE1) -C $(ASTERISK_DIR) menuselect
endif
	OPTIMIZE="-O4" ASTCFLAGS="$(ASTERISK_CFLAGS)" ASTLDFLAGS="$(ASTERISK_LDFLAGS)" \
	$(MAKE1) -C $(ASTERISK_DIR) codecs NOISY_BUILD=1

	OPTIMIZE="-Os" ASTCFLAGS="$(ASTERISK_CFLAGS)" ASTLDFLAGS="$(ASTERISK_LDFLAGS)" \
	$(MAKE1) -C $(ASTERISK_DIR) NOISY_BUILD=1

	mkdir -p $(TARGET_DIR)/bin/
	mkdir -p $(TARGET_DIR)/var/lib/asterisk/sounds
	mkdir -p $(TARGET_DIR)/var/lib/asterisk/sounds/moh
	mkdir -p $(TARGET_DIR)/var/lib/asterisk/sounds/meetme
	mkdir -p $(TARGET_DIR)/var/lib/asterisk/sounds/voicemail
	mkdir -p $(TARGET_DIR)/var/spool/asterisk
	mkdir -p $(TARGET_DIR)/usr/lib/asterisk/modules
	ln -sf /var/lib/asterisk/sounds/moh $(TARGET_DIR)/var/lib/asterisk/
	ln -sf /var/lib/asterisk/sounds/meetme $(TARGET_DIR)/var/spool/asterisk/
	ln -sf /var/lib/asterisk/sounds/voicemail $(TARGET_DIR)/var/spool/asterisk/
	cp -v $(ASTERISK_DIR)/main/asterisk $(TARGET_DIR)/bin/
	ln -sf /bin/asterisk $(TARGET_DIR)/bin/rasterisk
	find $(ASTERISK_DIR) -name '*.so' -exec cp -v "{}" $(TARGET_DIR)/usr/lib/asterisk/modules/ \;
	$(TARGET_STRIP) $(TARGET_DIR)/bin/asterisk
	$(TARGET_STRIP) $(TARGET_DIR)/usr/lib/asterisk/modules/*.so



asterisk-configure: $(ASTERISK_DIR)/.configured

asterisk-clean:
	rm -f $(ASTERISK_DIR)/.configured
	 -$(MAKE) -C $(ASTERISK_DIR) clean

asterisk-config: $(ASTERISK_DIR)/.configured
	$(MAKE) -C $(ASTERISK_DIR) menuconfig

asterisk-dirclean:
	rm -rf $(ASTERISK_DIR)
	rm -rf $(ASTERISK_DIR_LINK)

################################################
#
# Toplevel Makefile options
#
#################################################
ifeq ($(strip $(SF_PACKAGE_ASTERISK)),y)
TARGETS+=asterisk
endif
ifeq ($(strip $(SF_PACKAGE_ASTERISK-H323)),y)
TARGETS+=asterisk-h323
endif
