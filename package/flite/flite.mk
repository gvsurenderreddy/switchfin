FLITE_VERSION=1.4
FLITE_SITE=http://www.speech.cs.cmu.edu/flite/packed/flite-$(FLITE_VERSION)
FLITE_SOURCE=flite-$(FLITE_VERSION)-release.tar.bz2
FLITE_UNZIP=bzcat
FLITE_DIR=$(BUILD_DIR)/flite-$(FLITE_VERSION)-release
FLITE_CONFIGURE_OPTS=--host=$(FLAT_COMPILE) --with-audio=none

FLITE_VOXES=cmu_time_awb cmu_us_awb cmu_us_kal cmu_us_kal16 cmu_us_rms cmu_us_slt
FLITE_SELECTED_VOXES=$(foreach VOX,$(FLITE_VOXES),$(if $(filter $(SF_FLITE_VOX_$(VOX)),y),$(VOX)))

$(DL_DIR)/$(FLITE_SOURCE):
	$(WGET) -P $(DL_DIR) $(FLITE_SITE)/$(FLITE_SOURCE)

flite-source: $(DL_DIR)/$(FLITE_SOURCE)

$(FLITE_DIR)/.unpacked: $(DL_DIR)/$(FLITE_SOURCE)
	$(FLITE_UNZIP) $(DL_DIR)/$(FLITE_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(FLITE_DIR)/.unpacked

$(FLITE_DIR)/.configured: $(FLITE_DIR)/.unpacked
	$(PATCH_KERNEL) $(FLITE_DIR) package/flite flite-gnuconfig.patch
	$(PATCH_KERNEL) $(FLITE_DIR) package/flite flite-no-wchar.patch
	cd $(FLITE_DIR); ac_cv_header_sys_soundcard_h=no ./configure $(FLITE_CONFIGURE_OPTS)
	touch $(FLITE_DIR)/.configured

flite: $(FLITE_DIR)/.configured
	$(MAKE) -C $(FLITE_DIR)/include/
	$(MAKE) -C $(FLITE_DIR)/src/
	$(MAKE) -C $(FLITE_DIR)/lang/ VOXES="$(FLITE_SELECTED_VOXES)"
	$(MAKE) -C $(FLITE_DIR)/main/ ../bin/flite VOXES="$(FLITE_SELECTED_VOXES)"
	$(INSTALL) -m0755 $(FLITE_DIR)/bin/flite $(TARGET_DIR)/usr/bin/flite

flite-clean:
	$(MAKE) -C $(FLITE_DIR)/ clean

flite-dirclean:
	rm -rf $(FLITE_DIR)/

ifeq ($(strip $(SF_PACKAGE_FLITE)),y)
flite_: flite
else
flite_:
	rm -f $(TARGET_DIR)/usr/bin/flite
endif

###############################################
#
# Toplevel Makefile options
#
#################################################
TARGETS+=flite_
