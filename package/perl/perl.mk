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
# Primary Authors: mark@astfin.org, pawel@astfin.org, li770426@gmail.com
# Copyright @ 2010 SwitchFin <dpn@switchfin.org>
#########################################################################

PERL_VERSION=5.8.6
PERL_DIRNAME=perl-$(PERL_VERSION)
PERL_DIR=$(BUILD_DIR)/$(PERL_DIRNAME)
PERL_SITE=ftp://ftp.cpan.org/pub/CPAN/src/5.0
PERL_SOURCE=perl-$(PERL_VERSION).tar.gz
PERL_CONFIGURE_OPTS=


$(DL_DIR)/$(PERL_SOURCE):
	mkdir -p dl
	wget -P $(DL_DIR) $(PERL_SITE)/$(PERL_SOURCE)


$(PERL_DIR)/.unpacked: $(DL_DIR)/$(PERL_SOURCE)
	zcat $(DL_DIR)/$(PERL_SOURCE) | tar -C $(BUILD_DIR) -xf -
	patch -d $(PERL_DIR) -p1 < package/perl/perl.patch
	touch $(PERL_DIR)/.unpacked


$(PERL_DIR)/.configured: $(PERL_DIR)/.unpacked
	cd $(PERL_DIR); \
	rm -f config.sh Policy.sh; \
	sh Configure -de
	touch $(PERL_DIR)/.configured

perl: $(PERL_DIR)/.configured
	$(MAKE) -C $(PERL_DIR) -f Makefile.micro \
		CC="bfin-linux-uclibc-gcc" OPTIMIZE="$(TARGET_CFLAGS)"
	mkdir -p $(TARGET_DIR)/bin
	cp $(PERL_DIR)/microperl $(TARGET_DIR)/bin
	bfin-linux-uclibc-strip $(TARGET_DIR)/bin/microperl
	mkdir -p $(TARGET_DIR)/usr/local/lib/perl5/5.9
	cp -Rf $(PERL_DIR)/lib/* $(TARGET_DIR)/usr/local/lib/perl5/5.9


perl-dirclean:
	rm -rf $(PERL_DIR)

ifeq ($(strip $(SF_PACKAGE_PERL)),y)
perl_: perl
else
perl_:
	rm -f $(TARGET_DIR)/bin/microperl
	rm -rf $(TARGET_DIR)/usr/local/lib/perl5/
endif

################################################
#
# Toplevel Makefile options
#
#################################################
TARGETS+=perl_
