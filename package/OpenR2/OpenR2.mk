####################################################
# OpenR2 package for Edgepbx PR1 Appliance 	   #
#						   #	
#						   #		
####################################################

OPENR2_SITE=http://openr2.googlecode.com/files
OPENR2_REVISION=1.3.1
OPENR2_SUBDIR=openr2-$(OPENR2_REVISION)
OPENR2=openr2-$(OPENR2_REVISION).tar.gz
OPENR2_WORKING=$(BUILD_DIR)/$(OPENR2_SUBDIR)


$(DL_DIR)/$(OPENR2):
	mkdir -p dl
	cd dl; \
	wget $(OPENR2_SITE)/$(OPENR2); \
	cd ..

$(OPENR2_WORKING)/.unpacked: $(DL_DIR)/$(OPENR2)
	mkdir -p $(OPENR2_WORKING)
	tar -xzf $(DL_DIR)/$(OPENR2) -C $(BUILD_DIR)
	touch $(OPENR2_WORKING)/.unpacked

$(OPENR2_WORKING)/.configured: $(OPENR2_WORKING)/.unpacked




openr2: $(OPENR2_WORKING)/.configured





openr2-clean:
	rm -rf $(OPENR2_WORKING)

ifeq ($(strip $(SF_PACKAGE_OPENR2)),y)
openr2_: openr2
else
openr2_:
	rm -f $(TARGET_DIR)/usr/sbin/{edge,supernode} 
endif

################################################
#
# Toplevel Makefile options
#
#################################################
TARGETS+=openr2_
