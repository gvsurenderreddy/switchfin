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
# Update 27 July 2009, Dimitar Penev dpn@ucpbx.com
#        uClinux 2009R1-RC2 (kernel 2.6.28.10)
#        blackfin toolchain 2009R1-RC7
# Copyright @ 2010 SwitchFin <dpn@switchfin.org>
#########################################################################


#--------------------------------------------------------------
#
# Just run 'make menuconfig', configure stuff, then run 'make'.
# You shouldn't need to mess with anything beyond this point...
#
#--------------------------------------------------------------
TOPDIR=./
CONFIG_CONFIG_IN = Config.in
CONFIG_DEFCONFIG = .defconfig
CONFIG = config

# Pull in the user's configuration file
ifeq ($(filter $(noconfig_targets),$(MAKECMDGOALS)),)
-include $(TOPDIR).config
endif

TAR_OPTIONS=$(subst ",, $(SF_TAR_OPTIONS)) -xf
#"

ifeq ($(strip $(SF_HAVE_DOT_CONFIG)),y)

TARGETS:=""
include package/Makefile.in
#############################################################
#
# You should probably leave this stuff alone unless you know
# what you are doing.
#
#############################################################

all: world

#In this section, we neeed .config
include .config.cmd

# We explicitly specify the build order of the first targets,
# otherwise with 'include package/*/*.mk' they all end up in
# alphabetical order.  The alternative is to use explicit dependencies
# to define the build order, for example have the asterisk target
# depend on uClinux.  The advantage of an explicit build order defined
# here is faster asterisk build time and less build system output to
# wade through when u change one line in asterisk.

TARGETS+=toolchain uClinux persistent

# We also need the various per-package makefiles which we add to TARGETS
include package/*/*.mk

TARGETS_CLEAN:=$(patsubst %,%-clean, $(TARGETS))
TARGETS_SOURCE:=$(patsubst %,%-source, $(TARGETS))
TARGETS_DIRCLEAN:=$(patsubst %,%-dirclean,$(TARGETS))
TARGETS_IMAGE:=$(patsubst %,%-image,$(TARGETS))

#############################################################################
# Custom Path
#############################################################################
TOOLS_BIN1=$(BASE_DIR)/toolchain/opt/uClinux/bfin-uclinux/bin
TOOLS_BIN2=$(BASE_DIR)/toolchain/opt/uClinux/bfin-linux-uclibc/bin
TOOLS_BIN3=$(BASE_DIR)/toolchain/opt/uClinux/bfin-linux-uclibc/bfin-linux-uclibc/lib/
PATH:= /usr/local/sbin:/usr/local/bin:/sbin:/bin:/usr/sbin:/usr/bin:/root/bin
PATH:= $(PATH):$(TOOLS_BIN1):$(TOOLS_BIN2)

world: $(DL_DIR) $(BUILD_DIR) $(STAGING_DIR) $(TARGET_DIR) $(TARGETS)
ifeq ($(strip $(SF_INSTALL_ELF_TRIM_LIBS)),y)
	ROMFSDIR=$(TARGET_DIR) $(UCLINUX_DIR)/vendors/AnalogDevices/trim-libs.sh
endif

.PHONY: all world clean dirclean distclean source $(TARGETS) \
        $(TARGETS_CLEAN) $(TARGETS_DIRCLEAN) $(TARGETS_SOURCE) $(TARGETS_IMAGE) \
        $(DL_DIR) $(BUILD_DIR) $(TOOL_BUILD_DIR) $(STAGING_DIR)


#############################################################
#
# staging and target directories do NOT list these as
# dependancies anywhere else
#
#############################################################
$(DL_DIR):
	@mkdir -p $(DL_DIR)

$(BUILD_DIR):
	@mkdir -p $(BUILD_DIR)

$(TOOL_BUILD_DIR):
	@mkdir -p $(TOOL_BUILD_DIR)

$(STAGING_DIR):
#	@mkdir -p $(STAGING_DIR)/lib
#	@mkdir -p $(STAGING_DIR)/include
#	@mkdir -p $(STAGING_DIR)/usr/include
#	@mkdir -p $(STAGING_DIR)/usr/lib
#	@mkdir -p $(STAGING_DIR)/$(REAL_GNU_TARGET_NAME)

$(TARGET_DIR):
	mkdir -p $(TARGET_DIR)

# DR: - this skeleton stuff might be useful later, at the moment
#       the root fs is created elsewhere
#
#	if [ -f "$(TARGET_SKELETON)" ] ; then \
#                zcat $(TARGET_SKELETON) | tar -C $(BUILD_DIR) -xf -; \
#       fi;
#	if [ -d "$(TARGET_SKEL_DIR)" ] ; then \
#                cp -a $(TARGET_SKEL_DIR)/* $(TARGET_DIR)/; \
#        fi;
#	if [ -d "$(TARGET_SKEL2_DIR)" ] ; then \
#                cp -af $(TARGET_SKEL2_DIR)/* $(TARGET_DIR)/; \
#        fi;
#
#	touch $(STAGING_DIR)/fakeroot.env
#	-find $(TARGET_DIR) -type d -name CVS | xargs rm -rf
#	-find $(TARGET_DIR) -type d -name .svn | xargs rm -rf
#
#	@echo $(PATH)

.PHONY: image
image: $(TARGETS_IMAGE)

	@echo "" 
	@echo "" 
	@echo "" 
	@echo "==========================================================" 
	@echo "Please note that to create md5 uImage you will need"
	@echo "coreutils ver. 6.9 or later"
	@echo "==========================================================" 
	@echo "" 
	@echo "" 
	@echo "" 
	@sleep 5;
	-$(MAKE) -C $(UCLINUX_DIR) image ROMFSDIR=$(TARGET_DIR) IMAGEDIR=$(IMAGE_DIR)
	gcc $(SOURCES_DIR)/zeropad.c -o $(IMAGE_DIR)/zeropad -Wall
	$(IMAGE_DIR)/zeropad $(IMAGE_DIR)/uImage $(IMAGE_DIR)/fix_image.astfin 0x20000
	mv -f $(IMAGE_DIR)/fix_image.astfin $(IMAGE_DIR)/uImage
	config/package-image.sh $(IMAGE_DIR)/uImage $(IMAGE_DIR)/uImage-md5
	@if [ -f $(UBOOT_DIR)/u-boot.ldr ] ; then \
		cp -v $(UBOOT_DIR)/u-boot.ldr $(IMAGE_DIR); \
	fi

###########################################################
#
# Cleanup
#
##########################################################
clean:  
	rm -rf $(UCLINUX_DIR) $(STAGING_DIR) $(TARGET_DIR) $(IMAGE_DIR)

clean-config:
	rm -rf $(UCLINUX_DIR)/.configured

dirclean: $(TARGETS_DIRCLEAN)
	rm -rf $(BUILD_DIR)
	rm -rf .config

else # ifeq ($(strip $(SF_HAVE_DOT_CONFIG)),y)


#########################################################
#
# Configuration
#
#########################################################

all: menuconfig

endif # ifeq ($(strip $(SF_HAVE_DOT_CONFIG)),y)

$(CONFIG)/conf:
	$(MAKE) -C $(CONFIG) conf
	-@if [ ! -f .config ] ; then \
		cp $(CONFIG_DEFCONFIG) .config; \
	fi
$(CONFIG)/mconf:
	$(MAKE) -C $(CONFIG) ncurses conf mconf
	 -@if [ ! -f .config ] ; then \
		cp $(CONFIG_DEFCONFIG) .config; \
	fi

menuconfig: $(CONFIG)/mconf
	@$(CONFIG)/mconf $(CONFIG_CONFIG_IN)
