#########################################################
# spandsp for uClinux and Asterisk, 
# Jeff Knighton Feb 2008
#
# Inherited from BAPS
# Dimitar Penev updated to spandsp 0.0.6 dpn@switchvoice.com
#
# Copyright @ 2010 SwitchFin <dpn@switchfin.org>
#########################################################

SPANDSP_SITE=http://www.soft-switch.org/downloads/spandsp/
SPANDSP_VERSION=0.0.6
SPANDSP_SOURCE=spandsp-0.0.6pre17.tgz
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
	cd $(SPANDSP_DIR); LDFLAGS=$(TIFF_LDFLAGS) CFLAGS=$(TIFF_CFLAGS) ./configure $(SPANDSP_CONFIGURE_OPTS)
	cd $(SPANDSP_DIR); patch -p0 < $(BASE_DIR)/package/spandsp/spandsp.patch
	touch $(SPANDSP_DIR)/.configured

spandsp: libtiff $(SPANDSP_DIR)/.configured
	make  LDFLAGS=$(TIFF_LDFLAGS) CFLAGS=$(TIFF_CFLAGS) STAGEDIR=$(STAGING_DIR) -C $(SPANDSP_DIR)/
	#copy header files to staging directory
	mkdir -p $(STAGING_DIR)/usr/include/spandsp
	cp -rf $(SPANDSP_DIR)/src/spandsp/* $(STAGING_DIR)/usr/include/spandsp
	cp -f $(SPANDSP_DIR)/src/spandsp.h  $(STAGING_DIR)/usr/include/ #DPN for fax support

	cp -f $(SPANDSP_DIR)/src/.libs/libspandsp* $(STAGING_DIR)/usr/lib/
	
	#copy to the target location
	cp -f $(SPANDSP_DIR)/src/.libs/libspandsp.so $(TARGET_DIR)/lib
	$(TARGET_STRIP) $(TARGET_DIR)/lib/libspandsp.so

spandsp-dirclean:
	rm -rf $(SPANDSP_DIR)

