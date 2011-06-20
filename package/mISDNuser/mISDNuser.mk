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
# Copyright @ 2010 SwitchFin <dpn@switchfin.org>
#########################################################################

mISDNuser_REV=541
mISDNuser_NAME=mISDNuser
mISDNuser_DIR=$(BUILD_DIR)/$(mISDNuser_NAME)-$(mISDNuser_REV)
mISDNuser_SITE=https://switchfin.svn.sourceforge.net/svnroot/switchfin/mISDNuser/trunk
mISDNuser_CONFIG=$(UCLINUX_KERNEL_SRC)/.config


mISDNuserCFLAGS += -g -Wall -I $(mISDNuser_DIR)/include -I $(mISDN_DIR)/include -ffast-math -D__FIXED_PT__  -I$(STAGING_INC)
mISDNuserCFLAGS += -D CLOSE_REPORT=1 -mfdpic -D__BLACKFIN__


$(DL_DIR)/$(mISDNuser_NAME)-$(mISDNuser_REV):
	$(SVN) -r $(mISDNuser_REV) $(mISDNuser_SITE) $(DL_DIR)/$(mISDNuser_NAME)-$(mISDNuser_REV)

$(mISDNuser_DIR)/.unpacked: $(DL_DIR)/$(mISDNuser_NAME)-$(mISDNuser_REV)
	rm -rf $(mISDNuser_DIR)
	cp -R $(DL_DIR)/$(mISDNuser_NAME)-$(mISDNuser_REV) $(mISDNuser_DIR)
	touch $(mISDNuser_DIR)/.unpacked


$(mISDNuser_DIR)/.configured: $(mISDNuser_DIR)/.unpacked
	touch $(mISDNuser_DIR)/.configured

mISDNuser: mISDN $(mISDNuser_DIR)/.configured
	$(MAKE) -C "$(mISDNuser_DIR)"  ARCH="bfin-uclinux" \
		CC="gcc" LIBDIR="usr/lib"\
		CROSS="$(TARGET_CROSS)" \
		CFLAGS="$(mISDNuserCFLAGS)" \
		LDFLAGS="-L$(STAGING_LIB) -mfdpic -lncurses -lpthread -ldl" \
		MISDNDIR="$(mISDN_DIR)" \
		INSTALL_PREFIX=$(STAGING_DIR)

	$(MAKE) -C "$(mISDNuser_DIR)" MISDNDIR=$(mISDN_DIR) INSTALL_PREFIX=$(STAGING_DIR) TARGET=install install



mISDNuser-configure: $(mISDNuser_DIR)/.configured

mISDNuser-clean:
	rm -f $(mISDNuser_DIR)/.configured
	-$(MAKE) -C $(mISDNuser_DIR) clean

mISDNuser-dirclean:
	rm -rf $(mISDNuser_DIR)
mISDNuser-image:
	$(MAKE) -C "$(mISDNuser_DIR)" MISDNDIR=$(mISDN_DIR) INSTALL_PREFIX=$(TARGET_DIR) TARGET=install install

################################################
#
# Toplevel Makefile options
#
#################################################
ifeq ($(strip $(SF_PACKAGE_MISDNUSER)),y)
TARGETS+=mISDNuser
endif
