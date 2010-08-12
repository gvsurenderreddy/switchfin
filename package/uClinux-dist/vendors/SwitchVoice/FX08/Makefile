#
#	Makefile -- Build instructions for ADI/Blackfin
#

.EXPORT_ALL_VARIABLES:
include $(LINUX_CONFIG)
include $(CONFIG_CONFIG)

ROMFSIMG = $(IMAGEDIR)/rootfs
IMAGE    = $(IMAGEDIR)/linux

MKFS_JFFS2  = $(ROOTDIR)/user/mtd-utils/build/mkfs.jffs2
MKFS_YAFFS  = $(ROOTDIR)/user/blkfin-apps/yaffs-utils/utils/mkyaffsimage
MKFS_YAFFS2 = $(ROOTDIR)/user/blkfin-apps/yaffs-utils/utils/mkyaffs2image

LINUXBOOTDIR = $(ROOTDIR)/$(LINUXDIR)/arch/$(ARCH)/boot

# Blocks must be a multiple of 1024
BLOCKS   = 15360
#BLOCKS   = 20480
INODES   = 1024

ROMFS_DIRS-y = bin etc etc/dhcpc home lib mnt proc sys usr var root home tmp var/run var/lib/misc
ETC_FILES-y = \
	filesystems group host.conf inetd.conf issue mime.types \
	modprobe.conf motd passwd protocols services  tux-bf.gif

ROMFS_DIRS = bin dev etc etc/dhcpc home lib persistent
ROMFS_DIRS += mnt mnt/flash /mnt/spiflash /mnt/mmc /mnt/mmc1
ROMFS_DIRS += mnt/uba /mnt/uba1 mnt/sda /mnt/sda1 /mnt/configs
ROMFS_DIRS += proc sys usr var root home tmp home/httpd home/httpd/cgi-bin 
ROMFS_DIRS += var/run var/run/asterisk var/log var/lock var/lib var/tmp
ROMFS_DIRS += var/lib/misc var/lib/asterisk/static-http var/spool var/spool/asterisk var/spool/asterisk/outgoing

all::

romfs.post:: romfs.shared.libs


romfs::
	cd $(ROMFSDIR) && mkdir -p $(ROMFS_DIRS-y)
	$(ROMFSINST) -s bin /sbin
	
	mkdir -p $(ROMFSDIR)
	for i in $(ROMFS_DIRS); do \
		mkdir -p $(ROMFSDIR)/$$i; \
	done
	for i in scripts/*; do \
                [ -x $(ROMFSDIR)/$$i ] || $(ROMFSINST) $$i /bin/`basename $$i`; \
        done
	chmod 1777 $(ROMFSDIR)/tmp
	$(ROMFSINST) -s bin /sbin
	$(ROMFSINST) /etc/fstab
	$(ROMFSINST) /etc/rc
	$(ROMFSINST) /etc/rpc
	$(ROMFSINST) /etc/inittab
	$(ROMFSINST) /etc/services
	$(ROMFSINST) /etc/protocols
	$(ROMFSINST) /etc/group
	$(ROMFSINST) /etc/motd
	$(ROMFSINST) /etc/securesingle
	$(ROMFSINST) /etc/issue
	$(ROMFSINST) /etc/host.conf
	$(ROMFSINST) /etc/modprobe.conf
	$(ROMFSINST) /etc/firmwareversion
	$(ROMFSINST) /bin/dir
	$(ROMFSINST) /var/lib/asterisk/static-http/index.html
	#gzip -dc defaults.tgz | ( cd $(ROMFSDIR) ; tar xvf - )
	#gzip -dc mini-sounds.tgz | ( cd $(ROMFSDIR)/var/lib/asterisk/sounds ; tar xvf - )

	echo "`svnversion $(ROMFSDIR)/../../ | cut -d: -f2 | tr -d [:alpha:]` -- `date`" > $(ROMFSDIR)/etc/version	
ifeq ($(CONFIG_BLACKFIN_INSTALL_UCLINUX_CONFIG),y)
	gzip -9c $(ROOTDIR)/config/.config > $(ROMFSDIR)/root/uclinux-config.gz
endif

BSS      = $$($(CROSS_COMPILE)objdump -h $(IMAGEDIR)/vmlinux | awk '/.bss/ {print "0x"$$4}')
BSS_SIZE = $$($(CROSS_COMPILE)objdump -h $(IMAGEDIR)/vmlinux | awk '/.bss/ {print "0x"$$3}')
ADDR     = $$(echo $$(($(BSS) + $(BSS_SIZE))))
ENTRY    = $$($(CROSS_COMPILE)nm $(IMAGE).$$FS | awk '$$NF == "__start" {print $$1}')

DEVICE_TABLE = device_table.txt

image::
	mkdir -p $(IMAGEDIR)
	rm -rf $(IMAGEDIR)/*
	rm -rf $(ROMFSDIR)/dev
#
# make romfs.[fstype] images, and add them to linux(elf file) if possible
#
	cp $(ROOTDIR)/$(LINUXDIR)/linux $(IMAGEDIR)/vmlinux
	$(STRIP) -g $(IMAGEDIR)/vmlinux
	set -e ; \
	if [ -x $(MKFS_JFFS2) ] && [ -n "$(CONFIG_JFFS_FS)" -o -n "$(CONFIG_JFFS2_FS)" ]; then \
		$(MKFS_JFFS2) -l -d $(ROMFSDIR) -D $(DEVICE_TABLE) -o $(ROMFSIMG).jffs2; \
	fi
ifeq ($(CONFIG_YAFFS_FS),y)
	set -e ; \
	if [ -x "$(MKFS_YAFFS)" -a -n "$(CONFIG_YAFFS_YAFFS1)" ]; then \
		$(MKFS_YAFFS) $(ROMFSDIR) $(ROMFSIMG).yaffs > /dev/null ; \
	fi ; \
	if [ -x "$(MKFS_YAFFS2)" -a -n "$(CONFIG_YAFFS_YAFFS2)" ]; then \
		$(MKFS_YAFFS2) $(ROMFSDIR) $(ROMFSIMG).yaffs2 > /dev/null ; \
	fi
endif
ifeq ($(CONFIG_CRAMFS),y)
	$(CROSS_COMPILE)mkcramfs -z -D $(DEVICE_TABLE) $(ROMFSDIR) $(ROMFSIMG).cramfs
	$(OBJCOPY) --add-section .romfs=$(ROMFSIMG).cramfs \
		--adjust-section-vma .romfs=$(ADDR) --no-adjust-warnings \
		--set-section-flags .romfs=alloc,load,data $(IMAGEDIR)/vmlinux $(IMAGE).cramfs
endif
ifeq ($(CONFIG_EXT2_FS),y)
	$(CROSS_COMPILE)genext2fs -m 0 -i $(INODES) -b $(BLOCKS) -d $(ROMFSDIR) -D $(DEVICE_TABLE) $(ROMFSIMG).ext2
	$(OBJCOPY) --add-section .romfs=$(ROMFSIMG).ext2 \
		--adjust-section-vma .romfs=$(ADDR) --no-adjust-warnings \
		--set-section-flags .romfs=alloc,load,data $(IMAGEDIR)/vmlinux $(IMAGE).ext2
endif
ifeq ($(CONFIG_ROMFS_FS),y)
	set -e ; \
	if which genromfs >/dev/null 2>&1; then \
		$(ROOTDIR)/tools/mkdevdir.sh $(ROMFSDIR) $(DEVICE_TABLE); \
		genromfs -f $(ROMFSIMG).romfs -d $(ROMFSDIR); \
		$(OBJCOPY) --add-section .romfs=$(ROMFSIMG).romfs \
			--adjust-section-vma .romfs=$(ADDR) --no-adjust-warnings \
			--set-section-flags .romfs=alloc,load,data $(IMAGEDIR)/vmlinux $(IMAGE).romfs ; \
		rm -rf $(ROMFSDIR)/dev/*; \
	fi
endif

ifeq ($(CONFIG_MTD_UCLINUX),y)

#
# make a compressed image, link the first one to uImage
#
	set -e ; \
	for FS in cramfs ext2 romfs ; \
	do \
		if [ -f "$(IMAGE).$$FS" ] ; then \
			$(OBJCOPY) -O binary -S $(IMAGE).$$FS $(IMAGE).bin; \
			gzip -f9 $(IMAGE).bin; \
			$(CROSS_COMPILE)mkimage -A blackfin -O linux -T kernel \
				-C gzip -a $(CONFIG_BOOT_LOAD) -e $(ENTRY) -n "uClinux Kernel and $$FS" \
				-d $(IMAGE).bin.gz $(IMAGEDIR)/uImage.$$FS; \
			rm $(IMAGE).bin.gz; \
			ln -sf linux.$$FS $(IMAGEDIR)/linux ; \
			ln -sf uImage.$$FS $(IMAGEDIR)/uImage ; \
		fi; \
	done
	rm -f $(IMAGEDIR)/vmlinux
	rm -f $(IMAGEDIR)/vmImage

else

	cp $(LINUXBOOTDIR)/vmImage $(IMAGEDIR)/vmImage

endif

clean::

.PHONY: all clean image romfs

include ../vendor.mak
