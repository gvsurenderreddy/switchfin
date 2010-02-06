deps_config := \
	../Config.in

.config include/config.h: $(deps_config)

$(deps_config):
