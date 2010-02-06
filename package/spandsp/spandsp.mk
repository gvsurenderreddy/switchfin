#########################################################
# spandsp for uClinux and Asterisk, 
# Jeff Knighton Feb 2008
#
# Inherited from BAPS
#########################################################

SPANDSP_SITE=http://www.soft-switch.org/downloads/spandsp/old
SPANDSP_VERSION=0.0.4
SPANDSP_SOURCE=spandsp-0.0.4pre16.tgz
TIFF_VERSION=3.8.2
SPANDSP_UNZIP=zcat
SPANDSP_DIR=$(BUILD_DIR)/spandsp-$(SPANDSP_VERSION)
SPANDSP_CONFIGURE_OPTS=--host=bfin-linux-uclibc --enable-fixed-point
TIFF_LDFLAGS=-L$(BUILD_DIR)/tiff-$(TIFF_VERSION)/libtiff/.libs
TIFF_CFLAGS=-I$(BUILD_DIR)/tiff-$(TIFF_VERSION)/libtiff


$(DL_DIR)/$(SPANDSP_SOURCE):
	$(WGET) -P $(DL_DIR) $(SPANDSP_SITE)/$(SPANDSP_SOURCE)

spandsp-source: $(DL_DIR)/$(SPANDSP_SOURCE)

$(SPANDSP_DIR)/.unpacked: $(DL_DIR)/$(SPANDSP_SOURCE)
	zcat $(DL_DIR)/$(SPANDSP_SOURCE) | tar -C $(BUILD_DIR) -xf -
	touch $(SPANDSP_DIR)/.unpacked

$(SPANDSP_DIR)/.configured: $(SPANDSP_DIR)/.unpacked
	cp -v -f $(BASE_DIR)/package/spandsp/config.sub-spandsp-$(SPANDSP_VERSION) $(SPANDSP_DIR)/config/config.sub
	cp -v -f $(BASE_DIR)/package/spandsp/configure-spandsp-$(SPANDSP_VERSION) $(SPANDSP_DIR)/configure
	chmod a+x $(SPANDSP_DIR)/configure
	cd $(SPANDSP_DIR); LDFLAGS=$(TIFF_LDFLAGS) CFLAGS=$(TIFF_CFLAGS) ./configure $(SPANDSP_CONFIGURE_OPTS)
	touch $(SPANDSP_DIR)/.configured

spandsp: libtiff $(SPANDSP_DIR)/.configured
	make  LDFLAGS=$(TIFF_LDFLAGS) CFLAGS=$(TIFF_CFLAGS) STAGEDIR=$(STAGING_DIR) -C $(SPANDSP_DIR)/
	#copy header files to staging directory
	mkdir -p $(STAGING_DIR)/usr/include/spandsp
	cp -f $(SPANDSP_DIR)/src/spandsp/* $(STAGING_DIR)/usr/include/spandsp
	cp -f $(SPANDSP_DIR)/src/.libs/libspandsp* $(STAGING_DIR)/usr/lib/
	#copy to the package location
	cp -f $(SPANDSP_DIR)/src/.libs/libspandsp.so.0 $(TARGET_DIR)/lib
	cd $(TARGET_DIR)/lib; ln -sf libspandsp.so.0 libspandsp.so
	$(TARGET_STRIP) $(TARGET_DIR)/lib/libspandsp.so.0

spandsp-dirclean:
	rm -rf $(SPANDSP_DIR)

