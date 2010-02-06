
RESTORE_SOURCE=restore
RESTORE_DIR=$(DL_DIR)/$(RESTORE_SOURCE)
RESTORE_VERSION=trunk
RESTORE_WORKING=$(BUILD_DIR)/$(RESTORE_SOURCE)


$(RESTORE_DIR):
	mkdir $(RESTORE_DIR)
	cp $(SOURCES_DIR)/restore.c $(RESTORE_DIR)
	cp -rf $(SOURCES_DIR)/Makefile.restore $(RESTORE_DIR)/Makefile

$(RESTORE_WORKING)/.unpacked: $(RESTORE_DIR)
	rm -rf $(RESTORE_WORKING)
	cp -R $(RESTORE_DIR) $(RESTORE_WORKING)
	#-find $(RESTORE_WORKING) -type d -name .svn | xargs rm -rf
	touch $(RESTORE_WORKING)/.unpacked

$(RESTORE_WORKING)/.configured: $(RESTORE_WORKING)/.unpacked
	touch $(RESTORE_DIR)/.configured


restore: $(RESTORE_WORKING)/.configured
	cd $(RESTORE_WORKING); \
	$(MAKE) -C $(UCLINUX_DIR)/linux-2.6.x SUBDIRS=$(RESTORE_WORKING)

        # install
	mkdir -p $(TARGET_DIR)/lib/modules/$(shell ls $(TARGET_DIR)/lib/modules)/misc

	cp -f $(RESTORE_WORKING)/restore.ko \
        	$(TARGET_DIR)/lib/modules/$(shell ls $(TARGET_DIR)/lib/modules)/misc


restore-clean:
	rm -rf $(RESTORE_WORKING) 
################################################
#
# Toplevel Makefile options
#
#################################################
ifeq ($(strip $(SF_FX08)),y)
TARGETS+=restore
endif
