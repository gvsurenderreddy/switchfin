################################################
# OSLEC for Zaptel
################################################

OSLEC_DIR=$(DL_DIR)/oslec
OSLEC_SOURCE=oslec
OSLEC_SITE=http://svn.astfin.org/software/$(OSLEC_SOURCE)
OSLEC_VERSION=trunk
OSLEC_WORKING=$(BUILD_DIR)/$(OSLEC_SOURCE)


$(DL_DIR)/$(OSLEC_SOURCE):
	$(SVN) $(OSLEC_SITE)/$(OSLEC_VERSION) $(DL_DIR)/$(OSLEC_SOURCE)
	-find $(DL_DIR)/$(OSLEC_SOURCE) -type d -name .svn | xargs rm -rf

$(OSLEC_WORKING)/.unpacked: $(DL_DIR)/$(OSLEC_SOURCE)
	cp -Rf $(DL_DIR)/$(OSLEC_SOURCE) $(OSLEC_WORKING)
	touch $(OSLEC_WORKING)/.unpacked

$(OSLEC_WORKING)/.configured: $(OSLEC_WORKING)/.unpacked
	$(PATCH_KERNEL) $(OSLEC_WORKING) package/oslec oslec.patch
	touch $(OSLEC_WORKING)/.configured

oslec: uClinux-configure $(OSLEC_WORKING)/.configured
	make -C $(UCLINUX_DIR)/linux-2.6.x SUBDIRS=$(OSLEC_WORKING)/kernel modules
	mkdir -p $(TARGET_DIR)/lib/modules/$(shell ls $(TARGET_DIR)/lib/modules)/misc
	cp -f $(OSLEC_WORKING)/kernel/oslec.ko \
	$(TARGET_DIR)/lib/modules/$(shell ls $(TARGET_DIR)/lib/modules)/misc

oslec-clean:
	rm -rf $(OSLEC_WORKING)
oslec-dirclean:
	rm -rf $(OSLEC_WORKING)

################################################
#
# Toplevel Makefile options
#
#################################################
ifeq ($(strip $(SF_PACKAGE_ZAPTEL_OSLEC)),y)
#TARGETS+=oslec
endif
