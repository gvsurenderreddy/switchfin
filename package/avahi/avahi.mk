#########################################################
# avahi  for  Switchfin
# Dimitar Penev, March 2010
#
# Copyright @ 2010 SwitchFin <dpn@switchfin.org>
#########################################################

AVAHI_SITE=http://avahi.org/download/
AVAHI_VERSION=0.6.19
AVAHI_SOURCE=avahi-0.6.19.tar.gz
AVAHI_CROSS_COMPILE_PATCH=
AVAHI_UNZIP=bzcat
AVAHI_DIR_BASENAME=avahi-$(AVAHI_VERSION)
AVAHI_DIR=$(BUILD_DIR)/$(AVAHI_DIR_BASENAME)
AVAHI_CONFIGURE_OPTS=--host=bfin-linux-uclibc --with-distro=none


$(DL_DIR)/$(AVAHI_SOURCE):
	$(WGET) -P $(DL_DIR) $(AVAHI_SITE)/$(AVAHI_SOURCE)

$(AVAHI_DIR)/.unpacked: $(DL_DIR)/$(AVAHI_SOURCE)
	mkdir -p $(AVAHI_DIR)
	tar -xf $(DL_DIR)/$(AVAHI_SOURCE) -C $(BUILD_DIR)
	touch $(AVAHI_DIR)/.unpacked

$(AVAHI_DIR)/.configured: $(AVAHI_DIR)/.unpacked
	chmod a+x $(AVAHI_DIR)/configure
#	patch -p1 -d $(AVAHI_DIR) < package/vim/$(AVAHI_CROSS_COMPILE_PATCH)
	cd $(AVAHI_DIR); LDFLAGS="-L$(STAGING_DIR)/usr/lib" ./configure $(AVAHI_CONFIGURE_OPTS)
	touch $(AVAHI_DIR)/.configured

avahi: $(AVAHI_DIR)/.configured
	make -C $(AVAHI_DIR)/ STAGEDIR=$(STAGING_DIR)
	mkdir -p $(TARGET_DIR)/bin	
	mkdir -p $(TARGET_DIR)/usr/share/vim
	cp -f $(AVAHI_DIR)/src/vim $(TARGET_DIR)/bin/vim
	echo set nocompatible > $(TARGET_DIR)/usr/share/vim/vimrc
	touch $(PKG_BUILD_DIR)/.built

avahi-dirclean:
	rm -rf $(AVAHI_DIR)

################################################
#
# Toplevel Makefile options
#
#################################################
ifeq ($(strip $(SF_PACKAGE_AVAHI)),y)
TARGETS+=avahi
endif

