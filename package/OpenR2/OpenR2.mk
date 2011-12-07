####################################################
# OpenR2 package for Edgepbx PR1 Appliance 	   #
#						   #	
#						   #		
####################################################

OPENR2_SITE=http://openr2.googlecode.com/files
OPENR2_REVISION=1.3.2
OPENR2_SUBDIR=openr2-$(OPENR2_REVISION)
OPENR2=openr2-$(OPENR2_REVISION).tar.gz
OPENR2_WORKING=$(BUILD_DIR)/$(OPENR2_SUBDIR)
OPENR2_CONFIGURE_OPTS=--host=bfin-linux-uclibc --enable-fixed-point --disable-static --includedir=$(STAGING_INC) --libdir=$(TARGET_DIR)/lib

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
	cp -rf  $(DAHDI_DIR)/linux/include/dahdi $(BASE_DIR)/toolchain/opt/uClinux/bfin-linux-uclibc/bfin-linux-uclibc/include/dahdi
	cd $(OPENR2_WORKING); \
	./configure $(OPENR2_CONFIGURE_OPTS)
	touch $(OPENR2_WORKING) /.configured


openr2: $(OPENR2_WORKING)/.configured
	cd $(OPENR2_WORKING); \
	make -C $(OPENR2_WORKING)/ all install-strip


openr2-clean:
	rm -rf $(OPENR2_WORKING)

ifeq ($(strip $(SF_PACKAGE_OPENR2)),y)
openr2_: openr2
else
openr2_:
#	rm -f $(TARGET_DIR)/foo
endif

################################################
#
# Toplevel Makefile options
#
#################################################
TARGETS+=openr2_
