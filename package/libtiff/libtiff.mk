#########################################################
# libtiff for uClinux and Asterisk, 
# Jeff Knighton Feb 2008
#
# Inherited from BAPS
# Updated for spandsp 0.0.6 dpn@switchfin.org
# 
# Copyright @ 2010 SwitchFin <dpn@switchfin.org>
#########################################################

LIBTIFF_SITE=http://dl.maptools.org/dl/libtiff
LIBTIFF_VERSION=3.8.2
LIBTIFF_SOURCE=tiff-3.8.2.tar.gz
LIBTIFF_UNZIP=zcat
LIBTIFF_DIR=$(BUILD_DIR)/tiff-$(LIBTIFF_VERSION)
LIBTIFF_CONFIGURE_OPTS=--host=bfin-linux-uclibc 

$(DL_DIR)/$(LIBTIFF_SOURCE):
	$(WGET) -P $(DL_DIR) $(LIBTIFF_SITE)/$(LIBTIFF_SOURCE)

libtiff-source: $(DL_DIR)/$(LIBTIFF_SOURCE)

$(LIBTIFF_DIR)/.unpacked: $(DL_DIR)/$(LIBTIFF_SOURCE)
	zcat $(DL_DIR)/$(LIBTIFF_SOURCE) | tar -C $(BUILD_DIR) -xf -
	touch $(LIBTIFF_DIR)/.unpacked

$(LIBTIFF_DIR)/.configured: $(LIBTIFF_DIR)/.unpacked
	cd $(LIBTIFF_DIR); ./configure $(LIBTIFF_CONFIGURE_OPTS)
	touch $(LIBTIFF_DIR)/.configured

libtiff: $(LIBTIFF_DIR)/.configured
	make -C $(LIBTIFF_DIR)/ STAGEDIR=$(STAGING_DIR)
	cp -f $(LIBTIFF_DIR)/libtiff/.libs/libtiff* $(STAGING_DIR)/usr/lib/
	cp -f $(LIBTIFF_DIR)/libtiff/tiff.h $(STAGING_DIR)/usr/include/     
	cp -f $(LIBTIFF_DIR)/libtiff/tiffconf.h $(STAGING_DIR)/usr/include/
	cp -f $(LIBTIFF_DIR)/libtiff/tiffio.h $(STAGING_DIR)/usr/include/
	cp -f $(LIBTIFF_DIR)/libtiff/tiffvers.h $(STAGING_DIR)/usr/include/
ifeq ($(strip $(SF_PACKAGE_TIFF2PDF)),y)
	cp -f $(LIBTIFF_DIR)/tools/.libs/tiff2pdf $(TARGET_DIR)/bin/
endif
ifeq ($(strip $(SF_PACKAGE_FAX2TIFF)),y)
	cp -f $(LIBTIFF_DIR)/tools/.libs/fax2tiff $(TARGET_DIR)/bin/
endif


	cp -f $(LIBTIFF_DIR)/libtiff/.libs/libtiff.so.3 $(TARGET_DIR)/lib
	$(TARGET_STRIP) $(TARGET_DIR)/lib/libtiff.so.3
	cd $(TARGET_DIR)/lib/; ln -sf libtiff.so.3 libtiff.so

libtiff-dirclean:
	rm -rf $(LIBTIFF_DIR)

