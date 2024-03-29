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
SPANDSP_SOURCE=spandsp-0.0.6pre20.tgz
TIFF_VERSION=3.8.2
SPANDSP_UNZIP=zcat
SPANDSP_DIR=$(BUILD_DIR)/spandsp-$(SPANDSP_VERSION)
SPANDSP_CONFIGURE_OPTS=--host=bfin-linux-uclibc --enable-fixed-point --disable-static --includedir=$(STAGING_INC) --libdir=$(TARGET_DIR)/lib
TIFF_LDFLAGS=-L$(BUILD_DIR)/tiff-$(TIFF_VERSION)/libtiff/.libs
TIFF_CPPFLAGS=-I$(BUILD_DIR)/tiff-$(TIFF_VERSION)/libtiff


$(DL_DIR)/$(SPANDSP_SOURCE):
	$(WGET) -P $(DL_DIR) $(SPANDSP_SITE)/$(SPANDSP_SOURCE)

spandsp-source: $(DL_DIR)/$(SPANDSP_SOURCE)

$(SPANDSP_DIR)/.unpacked: $(DL_DIR)/$(SPANDSP_SOURCE)
	zcat $(DL_DIR)/$(SPANDSP_SOURCE) | tar -C $(BUILD_DIR) -xf -
	touch $(SPANDSP_DIR)/.unpacked

$(SPANDSP_DIR)/.configured: $(SPANDSP_DIR)/.unpacked
	cd $(SPANDSP_DIR); LDFLAGS="$(TIFF_LDFLAGS)" CPPFLAGS="$(TIFF_CPPFLAGS)" \
	ac_cv_func_malloc_0_nonnull=yes ac_cv_func_realloc_0_nonnull=yes \
	./configure $(SPANDSP_CONFIGURE_OPTS)

	touch $(SPANDSP_DIR)/.configured

spandsp: libtiff $(SPANDSP_DIR)/.configured
	make -C $(SPANDSP_DIR)/ all install-strip

	# libtool archives are only useful for static libraries.
	rm -f $(TARGET_DIR)/lib/libspandsp.la

	ln -snf $(TARGET_DIR)/lib/libspandsp.* $(STAGING_LIB)/

spandsp-dirclean:
	rm -rf $(SPANDSP_DIR)
