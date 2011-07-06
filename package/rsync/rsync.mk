#########################################################
# rsync for uClinux  
# Mark Hindess Feb 2008
#
#########################################################

RSYNC_SITE=http://rsync.samba.org/ftp/rsync
RSYNC_VERSION=2.6.8
RSYNC_SOURCE=rsync-2.6.8.tar.gz
RSYNC_UNZIP=zcat
RSYNC_DIR=$(BUILD_DIR)/rsync-$(RSYNC_VERSION)
RSYNC_CONFIGURE_OPTS=--host=$(FLAT_COMPILE) --disable-locale --disable-ipv6

$(DL_DIR)/$(RSYNC_SOURCE):
	$(WGET) -P $(DL_DIR) $(RSYNC_SITE)/$(RSYNC_SOURCE)

rsync-source: $(DL_DIR)/$(RSYNC_SOURCE)

$(RSYNC_DIR)/.unpacked: $(DL_DIR)/$(RSYNC_SOURCE)
	$(RSYNC_UNZIP) $(DL_DIR)/$(RSYNC_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(RSYNC_DIR)/.unpacked

$(RSYNC_DIR)/.configured: $(RSYNC_DIR)/.unpacked
	$(PATCH_KERNEL) $(RSYNC_DIR) $(RSYNC_DIR)/patches threaded-receiver.diff
	$(PATCH_KERNEL) $(RSYNC_DIR) package/rsync rsync-thread_retval.diff
	$(PATCH_KERNEL) $(RSYNC_DIR) package/rsync rsync-thread_stack_256k.diff
	$(PATCH_KERNEL) $(RSYNC_DIR) package/rsync rsync-unsupported.diff
	$(PATCH_KERNEL) $(RSYNC_DIR) package/rsync rsync-gnuconfig.diff
	cd $(RSYNC_DIR); LDFLAGS="-Wl,-elf2flt=-s262144" ./configure $(RSYNC_CONFIGURE_OPTS)
	touch $(RSYNC_DIR)/.configured
	mkdir -p $(TARGET_DIR)/bin

rsync: $(RSYNC_DIR)/.configured
	make -C $(RSYNC_DIR)/ rsync
	install -m0755 $(RSYNC_DIR)/rsync $(TARGET_DIR)/bin/rsync

rsync-dirclean:
	rm -rf $(RSYNC_DIR)

ifeq ($(strip $(SF_PACKAGE_RSYNC)),y)
rsync_: rsync
else
rsync_:
	rm -f $(TARGET_DIR)/bin/rsync
endif

###############################################
#
# Toplevel Makefile options
#
#################################################
TARGETS+=rsync_
