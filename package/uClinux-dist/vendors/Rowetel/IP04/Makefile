#
#	Makefile -- Build instructions for ADI/Blackfin
#

.EXPORT_ALL_VARIABLES:
include $(LINUX_CONFIG)
include $(CONFIG_CONFIG)

VERSIONSTR=UI2.16

ROMFS_DIRS-y = bin etc etc/dhcpc home lib mnt mnt/mmc proc sys usr var root home tmp var/run var/lib/misc persistent
ROMFS_DIRS-y += var/run/asterisk var/log var/lock var/lib var/lib/asterisk/sounds var/tmp
ROMFS_DIRS-y +=  var/lib/asterisk/static-http var/spool var/spool/asterisk var/spool/asterisk/outgoing

ETC_FILES-y = \
	filesystems group mime.types modprobe.conf motd \
	rc rpc TZ tux-bf.gif fstab  mime.types
WEB_FILES-y =

ETC_FILES-$(CONFIG_NET) += hosts host.conf protocols services
ETC_FILES-$(CONFIG_USER_INETD_INETD) += inetd.conf
ETC_FILES-$(CONFIG_USER_BUSYBOX_INETD) += inetd.conf

ROMFS_DIRS-$(CONFIG_USER_BOA_SRC_BOA) += home/httpd home/httpd/cgi-bin etc/boa var/log/boa
ETC_FILES-$(CONFIG_USER_BOA_SRC_BOA) += boa.conf
WEB_FILES-$(CONFIG_USER_BOA_SRC_BOA) += boa.conf index.html tux-bf.gif tux-wink.gif tuxsit_small.gif favicon.ico

all::

romfs.post:: romfs.shared.libs

romfs::
	mkdir -p $(ROMFSDIR)
	cd $(ROMFSDIR) && mkdir -p $(ROMFS_DIRS-y)
	for i in scripts/*; do \
                [ -x $(ROMFSDIR)/$$i ] || $(ROMFSINST) $$i /bin/`basename $$i`; \
        done
	chmod 1777 $(ROMFSDIR)/tmp
	$(ROMFSINST) -s bin /sbin

	$(ROMFSINST) /etc/inittab
	$(ROMFSINST) ../common/index.html /var/lib/asterisk/static-http/index.html

	set -e ; \
	for f in $(ETC_FILES-y) ; do \
		$(ROMFSINST) ../common/$$f /etc/$$f ; \
	done ; \
        $(ROMFSINST) profile /etc/profile ; \
	$(ROMFSINST) issue /etc/issue ; \
	$(ROMFSINST) ../common/dahdi_update.sh /bin/dahdi_update.sh ; \
	for f in $(WEB_FILES-y) ; do \
		$(ROMFSINST) ../common/$$f /home/httpd/$$f ; \
	done

	echo "$(VERSIONSTR) -- `date`" > $(ROMFSDIR)/etc/version
ifeq ($(CONFIG_BLACKFIN_INSTALL_UCLINUX_CONFIG),y)
	gzip -9c $(ROOTDIR)/config/.config > $(ROMFSDIR)/root/uclinux-config.gz
	gzip -9c $(ROOTDIR)/.config > $(ROMFSDIR)/root/vendor-board-config.gz
endif

ifeq ($(CONFIG_USER_DEV_DYNAMIC),y)
DEVICE_TABLE = device_table-min.txt
else
ifeq ($(CONFIG_USER_BUSYBOX_MDEV),y)
$(warning )
$(warning You have busybox's mdev enabled.  The static device tree will be)
$(warning overridden during boot by a tmpfs in /dev.)
$(warning )
endif
DEVICE_TABLE = device_table.txt
endif

# these targets can be found in AnalogDevices/vendor.mak
image::
	mkdir -p $(IMAGEDIR)
	rm -rf $(IMAGEDIR)/*

	$(MAKE) image.rootfs.all
	$(MAKE) image.kernel.all
	$(MAKE) image.uimage.all

clean::

.PHONY: all clean image romfs

include ../../Rowetel/vendor.mak
