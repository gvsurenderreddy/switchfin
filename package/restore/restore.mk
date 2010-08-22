
RESTORE_SOURCE=restore
RESTORE_VERSION=trunk
RESTORE_WORKING=$(BUILD_DIR)/$(RESTORE_SOURCE)


$(RESTORE_WORKING)/.unpacked:
	mkdir $(RESTORE_WORKING)
	cp $(SOURCES_DIR)/restore/restore.c $(RESTORE_WORKING)
	cp -rf $(SOURCES_DIR)/restore/Makefile.restore $(RESTORE_WORKING)/Makefile
	touch $(RESTORE_WORKING)/.unpacked

$(RESTORE_WORKING)/.configured: $(RESTORE_WORKING)/.unpacked
	touch $(RESTORE_WORKING)/.configured


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
ifeq ($(strip $(SF_IP04)),y)
TARGETS+=restore
endif
ifeq ($(strip $(SF_FX08)),y) 
TARGETS+=restore
endif
