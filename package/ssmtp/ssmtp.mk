#######################################################################
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# The Free Software Foundation; version 3 of the License.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# Copyright @ 2008 Astfin <mark@astfin.org>
# Primary Authors: mark@astfin.org
# Copyright @ 2010 SwitchFin <dpn@switchfin.org>
#######################################################################

##########################################
# SSMTP package for Switchfin
##########################################

SSMTP_VERSION=2.62
SSMTP_NAME=ssmtp_$(SSMTP_VERSION)
SSMTP_DIR=$(BUILD_DIR)/ssmtp
SSMTP_SOURCE=$(SSMTP_NAME).orig.tar.gz
SSMTP_SITE=http://ftp.de.debian.org/debian/pool/main/s/ssmtp
SSMTP_UNZIP=zcat

STAGING_INC=$(STAGING_DIR)/usr/include
STAGING_LIB=$(STAGING_DIR)/usr/lib

SSMTP_CFLAGS+=-O2 -Wall -D__uClinux__ -DEMBED -fno-builtin -mfdpic
SSMTP_CFLAGS+=-I$(UCLINUX_DIST) -isystem $(STAGING_INC) -I$(STAGING_INC)
SSMTP_CFLAGS+=-DLOGFILE -DSTDC_HEADERS=1 -DHAVE_LIMITS_H=1 -DHAVE_STRINGS_H=1 
SSMTP_CFLAGS+=-DHAVE_UNISTD_H=1 -DHAVE_LIBNSL=1 -DRETSIGTYPE=void -DHAVE_VPRINTF=1 
SSMTP_CFLAGS+=-DHAVE_GETHOSTNAME=1 -DHAVE_SOCKET=1 -DHAVE_STRDUP=1 -DHAVE_STRSTR=1 -DHAVE_MD5=1 -DHAVE_SSL=1
SSMTP_CFLAGS+=-DREWRITE_DOMAIN=1  $(EXTRADEFS) -g
SSMTP_LDFLAGS=-mfdpic -L$(STAGING_LIB) -lpthread -ldl
SSMTP_DEP=""
SSMTPCONFDIR=/etc/ssmtp
MAILFILE_SOURCES=$(SOURCES_DIR)/mailfile

# Configuration files
CONFIGURATION_FILE=$(SSMTPCONFDIR)/ssmtp.conf
REVALIASES_FILE=$(SSMTPCONFDIR)/revaliases

INSTALLED_CONFIGURATION_FILE=$(CONFIGURATION_FILE)
INSTALLED_REVALIASES_FILE=$(REVALIASES_FILE)

EXTRADEFS=\
-DSSMTPCONFDIR=\"$(SSMTPCONFDIR)\" \
-DCONFIGURATION_FILE=\"$(CONFIGURATION_FILE)\" \
-DREVALIASES_FILE=\"$(REVALIASES_FILE)\" \

$(DL_DIR)/$(SSMTP_SOURCE):
	$(WGET) -P $(DL_DIR) $(SSMTP_SITE)/$(SSMTP_SOURCE)

$(SSMTP_DIR)/.unpacked: $(DL_DIR)/$(SSMTP_SOURCE)
	$(SSMTP_UNZIP) $(DL_DIR)/$(SSMTP_SOURCE) | tar -C $(BUILD_DIR) $(TAR_OPTIONS) -
	touch $(SSMTP_DIR)/.unpacked


$(SSMTP_DIR)/.configured: $(SSMTP_DIR)/.unpacked
#	$(PATCH_KERNEL) $(SSMTP_DIR) package/ssmtp ssmtp.patch
	touch $(SSMTP_DIR)/.configured

xgethostname.o: $(SSMTP_DIR)/xgethostname.c $(SSMTP_DIR)/xgethostname.h
	bfin-linux-uclibc-gcc $(SSMTP_CFLAGS) -c -o $(SSMTP_DIR)/xgethostname.o $<

ssmtp.o: $(SSMTP_DIR)/ssmtp.c $(SSMTP_DIR)/ssmtp.h
	bfin-linux-uclibc-gcc $(SSMTP_CFLAGS) -c -o $(SSMTP_DIR)/ssmtp.o $<

arpadate.o: $(SSMTP_DIR)/arpadate.c
	bfin-linux-uclibc-gcc $(SSMTP_CFLAGS) -c -o $(SSMTP_DIR)/arpadate.o $<

base64.o: $(SSMTP_DIR)/base64.c
	bfin-linux-uclibc-gcc $(SSMTP_CFLAGS) -c -o $(SSMTP_DIR)/base64.o $<

ssmtp: $(SSMTP_DIR)/.configured xgethostname.o ssmtp.o arpadate.o base64.o
	bfin-linux-uclibc-gcc -o $(SSMTP_DIR)/ssmtp $(SSMTP_DIR)/xgethostname.o $(SSMTP_DIR)/ssmtp.o $(SSMTP_DIR)/arpadate.o $(SSMTP_DIR)/base64.o -L$(STAGING_LIB) -lssl -lcrypto

	mkdir -p $(TARGET_DIR)
	mkdir -p $(TARGET_DIR)/bin
	cp -v $(SSMTP_DIR)/ssmtp $(TARGET_DIR)/bin/
	
	#add mailfile support (requires uuencode from the busybox)
	cp -v $(MAILFILE_SOURCES)/mailfile.sh $(TARGET_DIR)/bin/
	chmod 777 $(TARGET_DIR)/bin/mailfile.sh

ssmtp-clean:
	rm -f $(SSMTP_DIR)/.configured
	 -$(MAKE) -C $(SSMTP_DIR) clean

ssmtp-dirclean:
	rm -rf $(SSMTP_DIR)

ifeq ($(strip $(SF_PACKAGE_SSMTP)),y)
ssmtp_: ssmtp
else
ssmtp_:
	rm -f $(TARGET_DIR)/bin/ssmtp
	rm -f $(TARGET_DIR)/bin/mailfile.sh
endif

################################################
#
# Toplevel Makefile options
#
#################################################
TARGETS+=ssmtp_
