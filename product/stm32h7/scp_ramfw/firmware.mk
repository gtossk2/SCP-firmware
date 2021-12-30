BS_FIRMWARE_CPU := cortex-m7
BS_FIRMWARE_HAS_NOTIFICATION := yes
BS_FIRMWARE_HAS_FAST_CHANNELS := no
BS_FIRMWARE_USE_NEWLIB_NANO_SPECS := no

BS_FIRMWARE_MODULE_HEADERS_ONLY := \
		power_domain \
		timer

BS_FIRMWARE_MODULES := \
		uart3 \
	 	led \
		stm32_timer \
		timer

BS_FIRMWARE_SOURCES := \
		config_stm32_timer.c \
    config_timer.c

ifeq ($(BUILD_HAS_DEBUGGER),yes)
    BS_FIRMWARE_MODULES += debugger_cli
    BS_FIRMWARE_SOURCES += config_debugger_cli.c
endif

include $(BS_DIR)/firmware.mk
