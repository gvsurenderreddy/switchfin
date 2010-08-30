#########################################################
# dyndns client for Switchfin
# Dimitar Penev, Aug. 2010
#
# Copyright @ 2010 SwitchFin <dpn@switchfin.org>
#########################################################

INADYN_SITE=http://www.inatech.eu/inadyn
INADYN_VERSION=1.96.2
INADYN_SOURCE=inadyn.v$(INADYN_VERSION).zip
INADYN_CROSS_COMPILE_PATCH=inadyn.patch
INADYN_DIR_BASENAME=inadyn
INADYN_DIR=$(BUILD_DIR)/$(INADYN_DIR_BASENAME)


$(DL_DIR)/$(INADYN_SOURCE):
	$(WGET) -P $(DL_DIR) $(INADYN_SITE)/$(INADYN_SOURCE)

$(INADYN_DIR)/.unpacked: $(DL_DIR)/$(INADYN_SOURCE)
	unzip $(DL_DIR)/$(INADYN_SOURCE) -d $(BUILD_DIR)
	touch $(INADYN_DIR)/.unpacked

$(INADYN_DIR)/.configured: $(INADYN_DIR)/.unpacked
	patch -p1 -d $(INADYN_DIR) < package/inadyn/$(INADYN_CROSS_COMPILE_PATCH)
	touch $(INADYN_DIR)/.configured

inadyn: $(INADYN_DIR)/.configured
	make -C $(INADYN_DIR) CC=bfin-linux-uclibc-gcc STRIP=bfin-linux-uclibc-strip LD=bfin-linux-uclibc-gcc os=linux
	mkdir -p $(TARGET_DIR)/bin/
	cp -f $(INADYN_DIR)/bin/linux/inadyn $(TARGET_DIR)/bin/
	$(TARGET_STRIP) $(TARGET_DIR)/bin/inadyn	


ifeq ($(strip $(SF_PACKAGE_INADYN)),y)
inadyn_: inadyn
else
inadyn_:
	rm -f $(TARGET_DIR)/bin/inadyn
endif


inadyn-dirclean:
	rm -rf $(INADYN_DIR)

################################################
#
# Toplevel Makefile options
#
#################################################
TARGETS+=inadyn_

