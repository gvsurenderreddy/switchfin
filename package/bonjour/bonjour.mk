#########################################################
# avahi  for  Switchfin
# Dimitar Penev, March 2010
#
# Copyright @ 2010 SwitchFin <dpn@switchfin.org>
#########################################################

BONJOUR_SITE=http://www.opensource.apple.com/darwinsource/tarballs/other/mDNSResponder-107.6.tar.gz
BONJOUR_VERSION=107.6
BONJOUR_SOURCE=mDNSResponder-107.6.tar.gz
BONJOUR_CROSS_COMPILE_PATCH=bonjour.patch
BONJOUR_UNZIP=bzcat
BONJOUR_DIR_BASENAME=mDNSResponder-$(BONJOUR_VERSION)
BONJOUR_DIR=$(BUILD_DIR)/$(BONJOUR_DIR_BASENAME)
BONJOUR_CONFIGURE_OPTS=--host=bfin-linux-uclibc --with-distro=none


$(DL_DIR)/$(BONJOUR_SOURCE):
	$(WGET) -P $(DL_DIR) $(BONJOUR_SITE)/$(BONJOUR_SOURCE)

$(BONJOUR_DIR)/.unpacked: $(DL_DIR)/$(BONJOUR_SOURCE)
	mkdir -p $(BONJOUR_DIR)
	tar -xf $(DL_DIR)/$(BONJOUR_SOURCE) -C $(BUILD_DIR)
	touch $(BONJOUR_DIR)/.unpacked

$(BONJOUR_DIR)/.configured: $(BONJOUR_DIR)/.unpacked
	patch -p0 -d $(BONJOUR_DIR) < package/bonjour/$(BONJOUR_CROSS_COMPILE_PATCH)
	cp -p package/sources/bonjour/nss.h $(STAGING_DIR)/usr/include/
	touch $(BONJOUR_DIR)/.configured

bonjour: $(BONJOUR_DIR)/.configured
#	make -C $(BONJOUR_DIR)/mDNSPosix/ CC=bfin-linux-uclibc-gcc STRIP=bfin-linux-uclibc-strip LD=bfin-linux-uclibc-ld os=linux STAG_INC=$(STAGING_INC) \
        LINKOPTS="-shared -L$(STAGING_DIR)/usr/lib -lc -L/home/dpenev/switchfin/switchfin/trunk/build_pr1/uClinux-dist/linux-2.6.x/arch/blackfin/lib/"
	make -C $(BONJOUR_DIR)/mDNSPosix/ CC=bfin-linux-uclibc-gcc STRIP=bfin-linux-uclibc-strip LD=bfin-linux-uclibc-gcc os=linux STAG_INC=$(STAGING_INC)
	mkdir -p $(TARGET_DIR)/usr/sbin/
	mkdir -p $(TARGET_DIR)/usr/bin/
	mkdir -p $(TARGET_DIR)/usr/lib/
	mkdir -p $(TARGET_DIR)/lib/
	cp -f $(BONJOUR_DIR)/mDNSPosix/build/prod/mdnsd $(TARGET_DIR)/usr/sbin/
	cp -f $(BONJOUR_DIR)/mDNSPosix/build/prod/libdns_sd.so $(TARGET_DIR)/lib/
	cp -f $(BONJOUR_DIR)/mDNSPosix/build/prod/libnss_mdns-0.2.so $(TARGET_DIR)/lib/
	cp -f $(BONJOUR_DIR)/Clients/build/dns-sd $(TARGET_DIR)/usr/bin/
	cd $(TARGET_DIR)/lib/; ln -sf libnss_mdns-0.2.so libnss_mdns.so.2





bonjour-dirclean:
	rm -rf $(BONJOUR_DIR)

################################################
#
# Toplevel Makefile options
#
#################################################
ifeq ($(strip $(SF_PACKAGE_BONJOUR)),y)
TARGETS+=bonjour
endif

