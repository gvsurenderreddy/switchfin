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
# Primary Authors: mark@astfin.org, pawel@astfin.org
# Copyright @ 2010 SwitchFin <dpn@switchfin.org>
#######################################################################
#########################################################
# Persistent file system for Switchfin
#########################################################

PERSISTENT_DIST=defaults.tgz
PERSISTENT_ZIP=zcat
PERSISTENT_LOC=$(TARGET_DIR)/$(PERSISTENT_DIST)

persistent: 
ifeq ($(strip $(SF_PR1_APPLIANCE)),y)
ifeq ($(strip $(SF_PACKAGE_BONJOUR)),y)
	cd package/persistent/pr1/persistent/etc/rc.d; \
	ln -sf ../init.d/mdnsd S45mdnsd
endif
ifeq ($(strip $(SF_PACKAGE_LEC)),y)
	cd package/persistent/pr1/persistent/etc/rc.d; \
	ln -sf ../init.d/lec S39lec
endif
ifeq ($(strip $(SF_PACKAGE_PROCWATCH)),y)
	cd package/persistent/pr1/persistent/etc/rc.d; \
	ln -sf ../init.d/asteriskwatch S90asteriskwatch
endif
#ifeq ($(strip $(SF_PACKAGE_VTUN)),y)
#	cp package/persistent/vtun/vtund package/persistent/pr1/persistent/etc/init.d; \
#	cp package/persistent/vtun/vtund.conf package/persistent/pr1/persistent/etc
#endif
	cd package/persistent/pr1; \
	chmod +x persistent/etc/init.d/*; \
	tar zcvf $(PERSISTENT_LOC) --owner 0 --group 0 --exclude '.svn' persistent/
	cd package/persistent/pr1/persistent/etc/rc.d; \
	rm -f S39lec S45mdnsd S90asteriskwatch
endif
ifeq ($(strip $(SF_BR4_APPLIANCE)),y)
ifeq ($(strip $(SF_PACKAGE_BONJOUR)),y)
	cd package/persistent/br4/persistent/etc/rc.d; \
	ln -sf ../init.d/mdnsd S45mdnsd
endif
ifeq ($(strip $(SF_PACKAGE_LEC)),y)
	cd package/persistent/br4/persistent/etc/rc.d; \
	ln -sf ../init.d/lec S39lec
endif
ifeq ($(strip $(SF_PACKAGE_PROCWATCH)),y)
	cd package/persistent/br4/persistent/etc/rc.d; \
	ln -sf ../init.d/asteriskwatch S90asteriskwatch
endif
ifeq ($(strip $(SF_PACKAGE_VTUN)),y)
	cp package/persistent/vtun/vtund package/persistent/br4/persistent/etc/init.d/; \
	cp package/persistent/vtun/vtund.conf package/persistent/br4/persistent/etc/
endif
	cd package/persistent/br4; \
	chmod +x persistent/etc/init.d/*; \
	tar zcvf $(PERSISTENT_LOC) --owner 0 --group 0 --exclude '.svn' persistent/
	cd package/persistent/br4/persistent/etc/rc.d; \
	rm -f S39lec S45mdnsd S90asteriskwatch
endif
ifeq ($(strip $(SF_IP04)),y)
ifeq ($(strip $(SF_PACKAGE_BONJOUR)),y)
	cd package/persistent/ip04/persistent/etc/rc.d; \
	ln -sf ../init.d/mdnsd S45mdnsd
endif
ifeq ($(strip $(SF_PACKAGE_PROCWATCH)),y)
	cd package/persistent/ip04/persistent/etc/rc.d; \
	ln -sf ../init.d/asteriskwatch S90asteriskwatch
endif
ifeq ($(strip $(SF_PACKAGE_HOME_AUTO)),y)
	cd package/persistent/ip04/persistent/etc/rc.d; \
	ln -sf ../init.d/wltc S85wltc
endif
ifeq ($(strip $(SF_PACKAGE_DAHDI_GSM1)),y)
	patch -p0 package/persistent/ip04/persistent/etc/init.d/dahdi < package/persistent/dahdi.patch
endif
ifeq ($(strip $(SF_PACKAGE_VTUN)),y)
	cp package/persistent/vtun/vtund package/persistent/ip04/persistent/etc/init.d/; \
	cp package/persistent/vtun/vtund.conf package/persistent/ip04/persistent/etc/
endif
	cd package/persistent/ip04; \
	chmod +x persistent/etc/init.d/*; \
	tar zcvf $(PERSISTENT_LOC) --owner 0 --group 0 --exclude '.svn' persistent/
	cd package/persistent/ip04/persistent/etc/rc.d; \
	rm -f S45mdnsd S90asteriskwatch
ifeq ($(strip $(SF_PACKAGE_DAHDI_GSM1)),y)
	patch -p0 -R package/persistent/ip04/persistent/etc/init.d/dahdi < package/persistent/dahdi.patch
endif
endif
ifeq ($(strip $(SF_IP01)),y)
ifeq ($(strip $(SF_PACKAGE_BONJOUR)),y)
	cd package/persistent/ip01/persistent/etc/rc.d; \
	ln -sf ../init.d/mdnsd S45mdnsd
endif
ifeq ($(strip $(SF_PACKAGE_PROCWATCH)),y)
	cd package/persistent/ip01/persistent/etc/rc.d; \
	ln -sf ../init.d/asteriskwatch S90asteriskwatch
endif
ifeq ($(strip $(SF_PACKAGE_VTUN)),y)
	cp package/persistent/vtun/vtund package/persistent/ip01/persistent/etc/init.d/; \
	cp package/persistent/vtun/vtund.conf package/persistent/ip01/persistent/etc/
endif
	cd package/persistent/ip01; \
	chmod +x persistent/etc/init.d/*; \
	tar zcvf $(PERSISTENT_LOC) --owner 0 --group 0 --exclude '.svn' persistent/
	cd package/persistent/ip01/persistent/etc/rc.d; \
	rm -f S45mdnsd S90asteriskwatch
endif

ifeq ($(strip $(SF_FX08)),y)
ifeq ($(strip $(SF_PACKAGE_BONJOUR)),y)
	cd package/persistent/fx08/persistent/etc/rc.d; \
	ln -sf ../init.d/mdnsd S45mdnsd
endif
ifeq ($(strip $(SF_PACKAGE_PROCWATCH)),y)
	cd package/persistent/fx08/persistent/etc/rc.d; \
	ln -sf ../init.d/asteriskwatch S90asteriskwatch
endif
ifeq ($(strip $(SF_PACKAGE_VTUN)),y)
	cp package/persistent/vtun/vtund package/persistent/fx08/persistent/etc/init.d/; \
	cp package/persistent/vtun/vtund.conf package/persistent/fx08/persistent/etc/
endif
	cd package/persistent/fx08; \
	chmod +x persistent/etc/init.d/*; \
	tar zcvf $(PERSISTENT_LOC) --owner 0 --group 0 --exclude '.svn' persistent/
	cd package/persistent/fx08/persistent/etc/rc.d; \
	rm -f S45mdnsd S90asteriskwatch
endif

ifeq ($(strip $(SF_PACKAGE_PPPOE)),y)
	-rm -rf $(TARGET_DIR)/etc/ppp/ 
endif

persistent-dirclean:
	rm -f $(PERSISTENT_LOC)

persistent-clean:
	rm -f $(PERSISTENT_LOC)


