#########################################################
# vim  for uClinux
# Mark Hindess Feb 2008
#
# 1.Mar. 2010 adopted from BAPS 
# Copyright @ 2010 SwitchFin <dpn@switchfin.org>
#########################################################

VIM_SITE=http://downloads.openwrt.org/sources
VIM_VERSION=7.1
VIM_SOURCE=vim-7.1.tar.bz2
VIM_CROSS_COMPILE_PATCH=vim-7.1-cross_compile-1.patch
VIM_UNZIP=bzcat
VIM_DIR_BASENAME=vim71
VIM_DIR=$(BUILD_DIR)/$(VIM_DIR_BASENAME)
VIM_CONFIGURE_OPTS=--host=bfin-linux-uclibc --prefix=/usr \
  --with-tlib=ncurses --with-features=small --without-x --disable-netbeans


$(DL_DIR)/$(VIM_SOURCE):
	$(WGET) -P $(DL_DIR) $(VIM_SITE)/$(VIM_SOURCE)

$(VIM_DIR)/.unpacked: $(DL_DIR)/$(VIM_SOURCE)
	mkdir -p $(VIM_DIR)
	tar -xjvf $(DL_DIR)/$(VIM_SOURCE) -C $(BUILD_DIR)
	touch $(VIM_DIR)/.unpacked

$(VIM_DIR)/.configured: $(VIM_DIR)/.unpacked
	chmod a+x $(VIM_DIR)/configure
	patch -p1 -d $(VIM_DIR) < package/vim/$(VIM_CROSS_COMPILE_PATCH)
	patch -p1 -d $(VIM_DIR) < package/vim/vim.patch
	cd $(VIM_DIR); LDFLAGS="-L$(STAGING_DIR)/usr/lib" ./configure $(VIM_CONFIGURE_OPTS)
	touch $(VIM_DIR)/.configured

vim: $(VIM_DIR)/.configured
	make -C $(VIM_DIR)/ STAGEDIR=$(STAGING_DIR)
	mkdir -p $(TARGET_DIR)/bin	
	mkdir -p $(TARGET_DIR)/usr/share/vim
	cp -f $(VIM_DIR)/src/vim $(TARGET_DIR)/bin/vim
	echo set nocompatible > $(TARGET_DIR)/usr/share/vim/vimrc

vim-dirclean:
	rm -rf $(VIM_DIR)

ifeq ($(strip $(SF_PACKAGE_VIM)),y)
vim_: vim
else
vim_:
	rm -f $(TARGET_DIR)/bin/vim
	rm -f $(TARGET_DIR)/usr/share/vim/vimrc	
endif
################################################
#
# Toplevel Makefile options
#
#################################################
TARGETS+=vim_
