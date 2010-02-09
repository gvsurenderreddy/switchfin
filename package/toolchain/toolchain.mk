#########################################################################
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# The Free Software Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# Copyright @ 2008 Astfin <mark@astfin.org>
# Primary Authors: mark@astfin.org, pawel@astfin.org
# Copyright @ 2010 SwitchFin <dpn@switchvoice.com>
#########################################################################

##########################################
# toolchain package for SwitchFin.org
##########################################
ifeq ($(strip $(HOST_ARCH)),64bit)
TOOLCHAIN_BIN_A=blackfin-toolchain-uclibc-default-09r1.1-2.x86_64.tar.bz2
TOOLCHAIN_SITE_A=http://blackfin.uclinux.org/gf/download/frsrelease/470/7338
TOOLCHAIN_BIN_B=blackfin-toolchain-09r1.1-2.x86_64.tar.bz2
TOOLCHAIN_SITE_B=http://blackfin.uclinux.org/gf/download/frsrelease/470/7330
else
TOOLCHAIN_BIN_A=blackfin-toolchain-uclibc-default-09r1.1-2.i386.tar.bz2
TOOLCHAIN_SITE_A=http://blackfin.uclinux.org/gf/download/frsrelease/470/7320
TOOLCHAIN_BIN_B=blackfin-toolchain-09r1.1-2.i386.tar.bz2
TOOLCHAIN_SITE_B=http://blackfin.uclinux.org/gf/download/frsrelease/470/7316
endif
TOOLCHAIN_UNZIP=tar
TOOLCHAIN_BUILD=$(BASE_DIR)/toolchain

$(DL_DIR)/$(TOOLCHAIN_BIN_A):
	cd $(DL_DIR); ${WGET} -p $(TOOLCHAIN_SITE_A)/$(TOOLCHAIN_BIN_A)
	if [ ! -f $@ ]; then \
		exit 1; \
	fi

$(DL_DIR)/$(TOOLCHAIN_BIN_B):
	cd $(DL_DIR); ${WGET} -p $(TOOLCHAIN_SITE_B)/$(TOOLCHAIN_BIN_B)
	if [ ! -f $@ ]; then \
		exit 1; \
	fi


$(TOOLCHAIN_BUILD)/.unpacked_a: $(DL_DIR)/$(TOOLCHAIN_BIN_A)
	mkdir -p $(TOOLCHAIN_BUILD)
	$(TOOLCHAIN_UNZIP) -xjf $(DL_DIR)/$(TOOLCHAIN_BIN_A) -C $(TOOLCHAIN_BUILD)
	if [ -d $(TOOLS_BIN3) ]; then \
		touch $(TOOLCHAIN_BUILD)/.unpacked_a; \
	fi

$(TOOLCHAIN_BUILD)/.unpacked_b: $(DL_DIR)/$(TOOLCHAIN_BIN_B)
	mkdir -p $(TOOLCHAIN_BUILD)
	$(TOOLCHAIN_UNZIP) -xjf $(DL_DIR)/$(TOOLCHAIN_BIN_B) -C $(TOOLCHAIN_BUILD)
	if [ -d $(TOOLS_BIN2) ]; then \
		touch $(TOOLCHAIN_BUILD)/.unpacked_b; \
	fi


toolchain: $(TOOLCHAIN_BUILD)/.unpacked_a $(TOOLCHAIN_BUILD)/.unpacked_b

toolchain-clean:

toolchain-dirclean:
	rm -rf $(TOOLCHAIN_BUILD)
