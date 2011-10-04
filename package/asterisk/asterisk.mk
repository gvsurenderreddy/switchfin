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
# Dimitar Penev: 3.Dec. 2009  Upgrade to Asterisk 1.4.28 and DAHDI 
# Dimitar Penev: 18.Apr. 2010  Asterisk 1.6 support is added. (Some work 
# taken from BAPS, thank you David!)
#
# Copyright @ 2010 SwitchFin <dpn@switchfin.org>
#######################################################################

##########################################
# Asterisk package for Astfin.org
##########################################

ifeq ($(strip $(SF_ASTERISK_1_4)),y)
	ASTERISK_VERSION=1.4.42
	ASTERISK_SERIES=1.4
	ATTRAFAX_DIR=$(BUILD_DIR)/attrafax-0.9
else
	ASTERISK_VERSION=1.6.2.6
	ASTERISK_SERIES=1.6
	ATTRAFAX_DIR=$(BUILD_DIR)/asterisk-$(ASTERISK_VERSION)
endif

ASTERISK_NAME=asterisk-$(ASTERISK_VERSION)
ASTERISK_DIR=$(BUILD_DIR)/$(ASTERISK_NAME)
ASTERISK_DIR_LINK=$(BUILD_DIR)/asterisk
ASTERISK_SOURCE=$(ASTERISK_NAME).tar.gz
ASTERISK_SITE=http://downloads.asterisk.org/pub/telephony/asterisk/releases
ASTERISK_UNZIP=zcat

APP_FAX_SITE=https://agx-ast-addons.svn.sourceforge.net/svnroot/agx-ast-addons/trunk
APP_FAX_REV=69

ASTERISK_CFLAGS=-g -mfdpic -mfast-fp -ffast-math -D__FIXED_PT__ -D__BLACKFIN__
ASTERISK_CFLAGS+= -I$(STAGING_INC) -fno-jump-tables
ASTERISK_LDFLAGS=-mfdpic -L$(STAGING_LIB) -lpthread -ldl -ltonezone -lsqlite3 -lspeexdsp
ASTERISK_DEP=sqlite3 $(if $(filter $(SF_PR1_APPLIANCE),y), libpri) $(if $(filter $(SF_PACKAGE_LUA),y), _lua)
ASTERISK_CONFIGURE_OPTS= --host=bfin-linux-uclibc --disable-largefile --without-pwlib --without-sdl
ASTERISK_CONFIGURE_OPTS+= --without-curl --disable-xmldoc --with-dahdi=$(DAHDI_DIR)/linux

ifeq ($(filter $(SF_SPANDSP_FAX) $(SF_SPANDSP_CALLERID),y),y)
ASTERISK_CFLAGS+= $(if $(filter $(SF_SPANDSP_CALLERID),y), -DUSE_SPANDSP_CALLERID)
ASTERISK_LDFLAGS+= -lspandsp -ltiff
ASTERISK_DEP+= spandsp
endif

ifeq ($(strip $(SF_PACKAGE_MISDNUSER)),y)
ASTERISK_DEP+= mISDNuser dahdi
ASTERISK_CONFIGURE_OPTS+= --with-misdn
else
ASTERISK_DEP+= dahdi
endif

ifeq ($(strip $(SF_PACKAGE_UW-IMAP)),y)
ASTERISK_DEP+= uw-imap
ASTERISK_CONFIGURE_OPTS+= --with-imap=$(BUILD_DIR)/imap-2007e
endif

$(DL_DIR)/$(ASTERISK_SOURCE):
	$(WGET) -P $(DL_DIR) $(ASTERISK_SITE)/$(ASTERISK_SOURCE)

$(ASTERISK_DIR)/.unpacked: $(DL_DIR)/$(ASTERISK_SOURCE)
	$(ASTERISK_UNZIP) $(DL_DIR)/$(ASTERISK_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	ln -sf $(ASTERISK_DIR) $(ASTERISK_DIR_LINK)
ifeq ($(strip $(SF_ASTERISK_1_4)),y)
ifeq ($(strip $(SF_PACKAGE_OPENR2)),y)
	$(PATCH_KERNEL) $(ASTERISK_DIR_LINK) package/OpenR2/ openr2-asterisk-1.4.42-p1.patch
endif
endif
	$(PATCH_KERNEL) $(ASTERISK_DIR_LINK) package/asterisk asterisk-$(ASTERISK_SERIES).patch
ifeq ($(strip $(SF_PACKAGE_ASTERISK_G729)),y)
	ln -sf $(SOURCES_DIR)/asterisk/codec_g729$(if $(filter $(SF_ASTERISK_1_6),y),_ast1_6).c $(ASTERISK_DIR)/codecs/codec_g729.c
	ln -sf $(SOURCES_DIR)/asterisk/g729ab_codec.h $(ASTERISK_DIR)/codecs
ifeq ($(strip $(SF_ASTERISK_1_4)),y)
ifeq ($(strip $(SF_PACKAGE_DAHDI_GSM1)),y)
	$(PATCH_KERNEL) $(ASTERISK_DIR_LINK) package/asterisk asterisk-1.4-gsm1.patch
endif
ifeq ($(strip $(SF_PACKAGE_LUA)),y)
	ln -sf $(SOURCES_DIR)/lua/pbx_lua.c $(ASTERISK_DIR)/pbx
	ln -sf $(SOURCES_DIR)/lua/hashtab.h $(ASTERISK_DIR)/include/asterisk/
endif
ifeq ($(strip $(SF_PACKAGE_DEVSTATE)),y)
	ln -sf $(SOURCES_DIR)/asterisk/func_devstate.c $(ASTERISK_DIR)/funcs		
endif
	ln -sf $(SOURCES_DIR)/asterisk/codec_speex.c $(ASTERISK_DIR)/codecs
endif
	touch $(ASTERISK_DIR)/.asterisk.$(ASTERISK_SERIES)
	touch $(ASTERISK_DIR)/.unpacked

$(ASTERISK_DIR_LINK)/.configured: $(ASTERISK_DEP) $(ASTERISK_DIR)/.unpacked $(if $(filter $(SF_PACKAGE_ATTRAFAX),y), $(ATTRAFAX_DIR)/.unpacked)
	cp -v package/asterisk/asterisk-$(ASTERISK_SERIES).makeopts $(ASTERISK_DIR)/menuselect.makeopts
ifeq ($(strip $(SF_PACKAGE_MISDNUSER)),y)
	sed -i 's/\b\S*misdn\S*\b//g' $(ASTERISK_DIR)/menuselect.makeopts
else
	sed -i 's/\b\S*dahdi\S*\b//g' $(ASTERISK_DIR)/menuselect.makeopts
endif
ifeq ($(strip $(SF_PACKAGE_UW-IMAP)),y)
	sed -i 's/^MENUSELECT_OPTS_app_voicemail=/\0IMAP_STORAGE/' $(ASTERISK_DIR)/menuselect.makeopts
endif
	cd $(ASTERISK_DIR); CFLAGS="$(ASTERISK_CFLAGS)" LDFLAGS="$(ASTERISK_LDFLAGS)" GNU_LD=0 ./configure $(ASTERISK_CONFIGURE_OPTS)
	#The config doesn't detect the fork properly. We know fork is properly emulated under uClinux
	sed -i 's/WORKING_FORK=/WORKING_FORK=1/' $(ASTERISK_DIR)/build_tools/menuselect-deps
	echo LUA=1 >> $(ASTERISK_DIR)/build_tools/menuselect-deps
ifeq ($(strip $(SF_SPANDSP_FAX)),y)	
ifeq ($(strip $(SF_ASTERISK_1_4)),y)
	cd $(ASTERISK_DIR)/apps/; svn -r$(APP_FAX_REV) export $(APP_FAX_SITE)/app-spandsp/app_fax.c 
	cd $(ASTERISK_DIR)/; svn -r$(APP_FAX_REV) export $(APP_FAX_SITE)/addon_version.h
endif
	sed -i '/MENUSELECT_APPS=/s/\bapp_fax\b//g' $(ASTERISK_DIR)/menuselect.makeopts
endif
	cp -v package/sources/nvfax/app_nv_faxdetect.c $(ASTERISK_DIR)/apps/app_nvfaxdetect.c
ifeq ($(strip $(SF_PACKAGE_ATTRAFAX)),y)
        #Patch Asterisk for Attrafax if not already patched
	patch -p1 -d $(ASTERISK_DIR) < package/asterisk/asterisk-attrafax.patch; \
	ln -sf ${ATTRAFAX_DIR}/src/asterisk/include/attractel/ $(ASTERISK_DIR)/include/
	ln -sf ${ATTRAFAX_DIR}/src/asterisk/main/attrlic.c $(ASTERISK_DIR)/main/
endif
	touch $(ASTERISK_DIR)/.configured

$(STAGING_LIB)/libgsm.a:
	make -C $(UCLINUX_DIST)/lib/blackfin-gsm CC=$(TARGET_CC) AR=bfin-linux-uclibc-ar
	ln -sf $(UCLINUX_DIST)/lib/blackfin-gsm/gsm/lib/libgsm.a $(STAGING_LIB)
	ln -sf $(UCLINUX_DIST)/lib/blackfin-gsm/gsm/inc/gsm.h $(STAGING_INC)


check_prev_ver:
ifeq ($(strip $(SF_ASTERISK_1_4)),y)
	if test -f $(ASTERISK_DIR_LINK)/.asterisk.1.6; then \
		rm -rf $(BUILD_DIR)/asterisk-1.6*; \
		rm -rf $(ASTERISK_DIR_LINK); \
		rm -rf $(TARGET_DIR)/usr/lib/asterisk/modules/; \
		rm -rf $(TARGET_DIR)/bin/asterisk; \
	fi
endif
ifeq ($(strip $(SF_ASTERISK_1_6)),y)
	if test -f $(ASTERISK_DIR_LINK)/.asterisk.1.4; then \
		rm -rf $(BUILD_DIR)/asterisk-1.4*; \
		rm -rf $(ASTERISK_DIR_LINK); \
		rm -rf $(TARGET_DIR)/usr/lib/asterisk/modules/; \
		rm -rf $(TARGET_DIR)/bin/asterisk; \
	fi
endif

asterisk: check_prev_ver $(ASTERISK_DIR_LINK)/.configured

ifeq ($(strip $(SF_PACKAGE_ASTERISK_VERBOSE)),y)
#	-$(MAKE1) -C $(ASTERISK_DIR) menuselect
endif
#	OPTIMIZE="-O4" ASTCFLAGS="$(ASTERISK_CFLAGS)" ASTLDFLAGS="$(ASTERISK_LDFLAGS)" \
#	$(MAKE1) -C $(ASTERISK_DIR) codecs NOISY_BUILD=1 GNU_LD=0

	OPTIMIZE="-O4" ASTCFLAGS="$(ASTERISK_CFLAGS)" ASTLDFLAGS="$(ASTERISK_LDFLAGS)" \
	$(MAKE1) -C $(ASTERISK_DIR) NOISY_BUILD=1 GNU_LD=0

	mkdir -p $(TARGET_DIR)/bin/
	mkdir -p $(TARGET_DIR)/var/lib/asterisk/sounds
	mkdir -p $(TARGET_DIR)/var/lib/asterisk/sounds/moh
	mkdir -p $(TARGET_DIR)/var/lib/asterisk/sounds/meetme
	mkdir -p $(TARGET_DIR)/var/lib/asterisk/sounds/voicemail
	mkdir -p $(TARGET_DIR)/var/spool/asterisk
	mkdir -p $(TARGET_DIR)/usr/lib/asterisk/modules
	cp -v $(ASTERISK_DIR)/main/asterisk $(TARGET_DIR)/bin/
	ln -sf /bin/asterisk $(TARGET_DIR)/bin/rasterisk
	find $(ASTERISK_DIR) -name '*.so' -exec cp -v "{}" $(TARGET_DIR)/usr/lib/asterisk/modules/ \;
	$(TARGET_STRIP)  $(TARGET_DIR)/bin/asterisk
	$(TARGET_STRIP) $(TARGET_DIR)/usr/lib/asterisk/modules/*.so
ifneq ($(strip $(SF_PACKAGE_NVFAX)),y)
	rm -f $(TARGET_DIR)/usr/lib/asterisk/modules/app_nvfaxdetect.so
endif
ifneq ($(strip $(SF_PACKAGE_ASTERISK_G729)),y)
	rm -f $(TARGET_DIR)/usr/lib/asterisk/modules/codec_g729.so
endif
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
