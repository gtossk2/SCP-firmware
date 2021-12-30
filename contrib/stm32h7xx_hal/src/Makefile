BS_LIB_NAME := stm32_hal

BS_LIB_SOURCES += stm32_init.c \
                  stm32h7xx_it.c \
                  system_stm32h7xx.c \

BS_LIB_INCLUDES += $(STM32_HAL_DIR)/include
BS_LIB_INCLUDES += $(STM32_HAL_DIR)/git/Inc
BS_LIB_INCLUDES += $(ARCH_DIR)/include
BS_LIB_INCLUDES += $(FWK_DIR)/include

include $(BS_DIR)/lib.mk
