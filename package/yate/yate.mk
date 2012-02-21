YATE_VERSION=3.3.2-1
YATE_SITE=http://yate.null.ro/tarballs/yate3
YATE_SOURCE=yate-$(YATE_VERSION).tar.gz
YATE_UNZIP=zcat
YATE_DIR=$(BUILD_DIR)/yate
YATE_CONFIGURE_OPTS=--host=$(CROSS_COMPILE) --prefix=/usr --sysconfdir=/etc --with-openssl=$(STAGING_INC)/.. --with-libspeex=$(STAGING_INC) --with-zlib=$(STAGING_INC)/.. --enable-dahdi --disable-zaptel --disable-wpcard --disable-tdmcard --disable-wanpipe --enable-ilbc --disable-rtti --without-libpq --without-mysql --without-libgsm  --without-amrnb --with-spandsp --without-pwlib --without-openh323 --without-libqt4

$(DL_DIR)/$(YATE_SOURCE):
	$(WGET) -P $(DL_DIR) $(YATE_SITE)/$(YATE_SOURCE)

yate-source: $(DL_DIR)/$(YATE_SOURCE)

$(YATE_DIR)/.unpacked: $(DL_DIR)/$(YATE_SOURCE)
	$(YATE_UNZIP) $(DL_DIR)/$(YATE_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(YATE_DIR)/.unpacked

$(YATE_DIR)/.configured: $(YATE_DIR)/.unpacked
	$(PATCH_KERNEL) $(YATE_DIR) package/yate yate-vfork.patch
	$(PATCH_KERNEL) $(YATE_DIR) package/yate yate-speex.patch

	# Don't regenerate.
	touch $(YATE_DIR)/configure

	# Broken on uClibc.
	sed -i "/enumroute/d" $(YATE_DIR)/modules/Makefile.in

	cd $(YATE_DIR); LDFLAGS="-L$(STAGING_LIB)" ./configure $(YATE_CONFIGURE_OPTS)
	touch $(YATE_DIR)/.configured

yate: $(YATE_DIR)/.configured
	$(MAKE) -C $(YATE_DIR)/
	$(MAKE) -C $(YATE_DIR)/modules/ install DESTDIR=$(TARGET_DIR)
	$(MAKE) -C $(YATE_DIR)/share/data/ install DESTDIR=$(TARGET_DIR)
	$(MAKE) -C $(YATE_DIR)/share/scripts/ install DESTDIR=$(TARGET_DIR)
	$(MAKE) -C $(YATE_DIR)/share/sounds/ install DESTDIR=$(TARGET_DIR)
	cp $(YATE_DIR)/libyate*.so* $(TARGET_DIR)/usr/lib/
	$(TARGET_STRIP) --strip-unneeded -o $(TARGET_DIR)/usr/bin/yate $(YATE_DIR)/yate
	$(TARGET_STRIP) --strip-unneeded $(TARGET_DIR)/usr/lib/libyate*.so* $(TARGET_DIR)/usr/lib/yate/*.yate $(TARGET_DIR)/usr/lib/yate/*/*.yate

yate-clean:
	$(MAKE) -C $(YATE_DIR)/ clean

yate-dirclean:
	rm -rf $(YATE_DIR)/

ifeq ($(strip $(SF_PACKAGE_YATE)),y)
yate_: yate
else
yate_:
	rm -rf $(TARGET_DIR)/{etc,usr/bin,usr/share}/yate $(TARGET_DIR)/usr/lib/libyate*.so* $(TARGET_DIR)/usr/lib/yate
endif

###############################################
#
# Toplevel Makefile options
#
#################################################
TARGETS+=yate_
