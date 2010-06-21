#########################################################################
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# The Free Software Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# Copyright @ 2010 SwitchFin
#########################################################################

##########################################
# Procwatch package for SwitchFin
##########################################


PROCWATCH_DIR=$(BUILD_DIR)/procwatch
FILE=procwatch.c
PROCWATCH_SVN=http://svn.digium.com/svn/aadk/uClinux/trunk/uClinux-dist/user/procwatch
PROCWATCH_FILE=$(PROCWATCH_DIR)/$(FILE)
CFLAGS = -I$(UCLINUX_DIR)/linux-2.6.x/arch/blackfin/include/ -I$(UCLINUX_DIR)/linux-2.6.x/include/ -I.
BUILD_CFLAGS += $(CFLAGS)

$(PROCWATCH_DIR)/.unpacked:
	mkdir -p dl
	svn co $(PROCWATCH_SVN) $(PROCWATCH_DIR)
	touch $(PROCWATCH_DIR)/.unpacked

procwatch: $(PROCWATCH_DIR)/.unpacked
	$(CC) $(PROCWATCH_FILE) $(BUILD_CFLAGS) $(LDFLAGS) -o $(PROCWATCH_DIR)/$@ $(LDLIBS$(LDLIBS_$@))
	cp -f $(PROCWATCH_DIR)/procwatch $(TARGET_DIR)/bin 
	touch $(PROCWATCH_DIR)/.built
	
all: procwatch

procwatch-dirclean:
	rm -rf $(PROCWATCH_DIR)

################################################
# 
# Toplevel Makefile options
#
#################################################
ifeq ($(strip $(SF_PACKAGE_PROCWATCH)),y)
TARGETS+=procwatch
endif
