#########################################################
# dyndns client for Switchfin
# Dimitar Penev, Aug. 2010
#
# Copyright @ 2010 SwitchFin <dpn@switchfin.org>
#########################################################
IKSEMEL_SITE=http://iksemel.googlecode.com/files/
IKSEMEL_VERSION=1.4
IKSEMEL_SOURCE=iksemel-$(IKSEMEL_VERSION).tar.gz
IKSEMEL_CROSS_COMPILE_PATCH=iksemel.patch
IKSEMEL_DIR_BASENAME=iksemel
IKSEMEL_DIR=$(BUILD_DIR)/$(IKSEMEL_DIR_BASENAME)-$(IKSEMEL_VERSION)

#IKSEMEL_CFLAGS=-g -mfdpic -mfast-fp -ffast-math -D__FIXED_PT__ -D__BLACKFIN__ -I$(STAGING_INC) -fno-jump-tables
#IKSEMEL_CFLAGS+= $(if $(filter $(SF_SPANDSP_CALLERID),y), -DUSE_SPANDSP_CALLERID)
#IKSEMEL_LDFLAGS=-mfdpic -L$(STAGING_LIB) -lpthread -ldl -ltonezone -lsqlite3 -lspeexdsp -lspandsp -ltiff
IKSEMEL_CONFIGURE_OPTS= --host=bfin-linux-uclibc --disable-largefile --prefix=$(TARGET_DIR)/usr

$(DL_DIR)/$(IKSEMEL_SOURCE):
	$(WGET) -P $(DL_DIR) $(IKSEMEL_SITE)/$(IKSEMEL_SOURCE)

$(IKSEMEL_DIR)/.unpacked: $(DL_DIR)/$(IKSEMEL_SOURCE)
	tar -zxf $(DL_DIR)/$(IKSEMEL_SOURCE) -C $(BUILD_DIR)
	touch $(IKSEMEL_DIR)/.unpacked

$(IKSEMEL_DIR)/.configured: $(IKSEMEL_DIR)/.unpacked
	patch -p0 -d $(IKSEMEL_DIR) < package/iksemel/$(IKSEMEL_CROSS_COMPILE_PATCH)
	cd $(IKSEMEL_DIR); CFLAGS="$(IKSEMEL_CFLAGS)" LDFLAGS="$(IKSEMEL_LDFLAGS)" GNU_LD=0 ./configure $(IKSEMEL_CONFIGURE_OPTS)
	touch $(IKSEMEL_DIR)/.configured

iksemel: $(IKSEMEL_DIR)/.configured
	make -C $(IKSEMEL_DIR) CC=bfin-linux-uclibc-gcc STRIP=bfin-linux-uclibc-strip LD=bfin-linux-uclibc-gcc os=linux
	cd $(IKSEMEL_DIR); make install	


ifeq ($(strip $(SF_PACKAGE_IKSEMEL)),y)
iksemel_: iksemel
else
iksemel_:
	rm -f $(TARGET_DIR)/bin/iksemel
endif


iksemel-dirclean:
	rm -rf $(IKSEMEL_DIR)

################################################
#
# Toplevel Makefile options
#
#################################################
TARGETS+=iksemel_

