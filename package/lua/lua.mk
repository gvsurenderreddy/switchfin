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
# Primary Author: mark@astfin.org
# Copyright @ 2010 SwitchFin <dpn@switchfin.org>
#########################################################################


lua: $(UCLINUX_DIR)/.built
	find $(UCLINUX_DIR)/user/lua/ -type f -name lua.h | xargs -i cp {} $(STAGING_INC)
	find $(UCLINUX_DIR)/user/lua/ -type f -name lauxlib.h | xargs -i cp {} $(STAGING_INC)
	find $(UCLINUX_DIR)/user/lua/ -type f -name lualib.h | xargs -i cp {} $(STAGING_INC)
	find $(UCLINUX_DIR)/user/lua/ -type f -name luaconf.h | xargs -i cp {} $(STAGING_INC)

ifeq ($(strip $(SF_PACKAGE_LUA)),y)
lua_: lua
else
lua_:
	rm -f $(TARGET_DIR)/usr/bin/lua
	rm -f $(TARGET_DIR)/usr/lib/asterisk/modules/pbx_lua.so
endif

################################################
#
# Toplevel Makefile options
#
#################################################
TARGETS+=lua_

