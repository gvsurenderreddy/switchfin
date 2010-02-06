#########################################################
# rsync for uClinux  
# Mark Hindess Feb 2008
#
#########################################################

RSYNC_SITE=http://downloads.openwrt.org/sources
RSYNC_VERSION=2.6.5
RSYNC_SOURCE=rsync-2.6.5.tar.gz
RSYNC_UNZIP=zcat
RSYNC_DIR=$(BUILD_DIR)/rsync-$(RSYNC_VERSION)
RSYNC_CONFIGURE_OPTS=--host=bfin-linux-uclibc --disable-locale --disable-ipv6

$(DL_DIR)/$(RSYNC_SOURCE):
	$(WGET) -P $(DL_DIR) $(RSYNC_SITE)/$(RSYNC_SOURCE)

rsync-source: $(DL_DIR)/$(RSYNC_SOURCE)

$(RSYNC_DIR)/.unpacked: $(DL_DIR)/$(RSYNC_SOURCE)
	$(RSYNC_UNZIP) $(DL_DIR)/$(RSYNC_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(RSYNC_DIR)/.unpacked

$(RSYNC_DIR)/.configured: $(RSYNC_DIR)/.unpacked
	chmod a+x $(RSYNC_DIR)/configure
	$(PATCH_KERNEL) $(RSYNC_DIR) package/rsync rsync.patch
	cd $(RSYNC_DIR); ./configure $(RSYNC_CONFIGURE_OPTS)
	touch $(RSYNC_DIR)/.configured
	mkdir -p $(TARGET_DIR)/bin


rsync: $(RSYNC_DIR)/.configured
	make -C $(RSYNC_DIR)/ STAGEDIR=$(STAGING_DIR)
	cp -f $(RSYNC_DIR)/rsync $(TARGET_DIR)/bin/rsync
	$(TARGET_STRIP) $(TARGET_DIR)/bin/rsync

rsync-dirclean:
	rm -rf $(RSYNC_DIR)

###############################################
#
# Toplevel Makefile options
#
#################################################
ifeq ($(strip $(SF_PACKAGE_RSYNC)),y)
TARGETS+=rsync
endif
