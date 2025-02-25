#
# Arm SCP/MCP Software
# Copyright (c) 2015-2021, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

ifndef BS_FIRMWARE_MK
BS_FIRMWARE_MK := 1

include $(BS_DIR)/defs.mk
include $(BS_DIR)/toolchain.mk

$(info == Building firmware $(FIRMWARE))

#
# Validate that all mandatory definitions are defined
#
ifeq ($(BS_FIRMWARE_CPU),)
    $(error "You must define BS_FIRMWARE_CPU. Aborting...")
endif


ifeq ($(BS_FIRMWARE_HAS_NOTIFICATION),)
    $(error "You must define BS_FIRMWARE_HAS_NOTIFICATION. \
             Valid options are: 'yes' and 'no'. \
             Aborting...")
endif

ifneq ($(filter-out yes no,$(BS_FIRMWARE_HAS_MULTITHREADING)),)
    $(error "Invalid parameter for BS_FIRMWARE_HAS_MULTITHREADING. \
             Valid options are: 'yes' and 'no'. \
             Aborting...")
endif

ifneq ($(filter-out yes no,$(BS_FIRMWARE_HAS_NOTIFICATION)),)
    $(error "Invalid parameter for BS_FIRMWARE_HAS_NOTIFICATION. \
             Valid options are: 'yes' and 'no'. \
             Aborting...")
endif

export BS_FIRMWARE_CPU
export BS_FIRMWARE_HAS_MULTITHREADING
export BS_FIRMWARE_HAS_NOTIFICATION
export BS_FIRMWARE_USE_NEWLIB_NANO_SPECS

#
# Generate FIRMWARE_MODULES_LIST and list of excluded APIs for each module in
# the following format:
#
# <MODULE_NAME>_DISABLED_APIS = <API_1> <API_2> ...
#
# NOTE: <MODULE_NAME> and <API_n> are all in uppercase
#
SPLIT_INPUT = $(subst $(comma), ,$(1))
GET_MODULE_NAME = $(word 1,$(SPLIT_INPUT))

define process_module_entry
    export $(call to_upper,$(GET_MODULE_NAME))_DISABLED_APIS = \
    $(call to_upper,$(filter-out $(GET_MODULE_NAME),$(SPLIT_INPUT)))

    FIRMWARE_MODULES_LIST += $(GET_MODULE_NAME)
    DEFINES += BUILD_HAS_MOD_$(call to_upper,$(GET_MODULE_NAME))=1
endef

$(foreach entry, \
          $(BS_FIRMWARE_MODULES), \
          $(eval $(call process_module_entry, $(entry))))

include $(BS_DIR)/cpu.mk

BUILD_PRODUCT_DIR := $(BUILD_DIR)/product/$(PRODUCT)
BUILD_FIRMWARE_DIR := $(BUILD_PRODUCT_DIR)/$(FIRMWARE)
BIN_DIR := $(BUILD_FIRMWARE_DIR)/$(MODE)/bin
OBJ_DIR := $(BUILD_FIRMWARE_DIR)/$(MODE)/obj
PRODUCT_MODULES_DIR := $(PRODUCT_DIR)/module
FIRMWARE_DIR := $(PRODUCT_DIR)/$(FIRMWARE)

TARGET := $(BIN_DIR)/$(FIRMWARE)
TARGET_BIN := $(TARGET).bin
TARGET_ELF := $(TARGET).elf
TARGET_SREC := $(TARGET).srec

ifeq ($(BS_LINKER),ARM)
    TARGET_GOAL := $(TARGET_BIN)
else ifeq ($(BS_TOOLCHAIN),LLVM)
    TARGET_GOAL := $(TARGET_BIN)
else
    TARGET_GOAL := $(TARGET_SREC)
endif

vpath %.c $(FIRMWARE_DIR)
vpath %.S $(FIRMWARE_DIR)
vpath %.c $(PRODUCT_DIR)/src
vpath %.S $(PRODUCT_DIR)/src

goal: $(TARGET_GOAL)

ifneq ($(BS_ARCH_CPU),host)
    ifeq ($(BS_LINKER),ARM)
        SCATTER_SRC = $(ARCH_DIR)/$(BS_ARCH_VENDOR)/$(BS_ARCH_ARCH)/src/arch.scatter.S
    else
        SCATTER_SRC = $(ARCH_DIR)/$(BS_ARCH_VENDOR)/$(BS_ARCH_ARCH)/src/arch.ld.S
    endif

    SCATTER_PP = $(OBJ_DIR)/ld_preproc.s
endif

#
# Sources
#
SOURCES = $(BS_FIRMWARE_SOURCES)

#
# Modules
#
ALL_STANDARD_MODULES := $(patsubst $(MODULES_DIR)/%,%, \
    $(wildcard $(MODULES_DIR)/*))
ALL_PRODUCT_MODULES := $(patsubst $(PRODUCT_MODULES_DIR)/%,%, \
    $(wildcard $(PRODUCT_MODULES_DIR)/*))

# Check for conflicts between module names
CONFLICTING_MODULES := $(filter $(ALL_PRODUCT_MODULES), $(ALL_STANDARD_MODULES))
ifneq ($(CONFLICTING_MODULES),)
    $(error "The following module(s) in '$(PRODUCT)' conflict with modules of \
             the same name in $(MODULES_DIR): $(CONFLICTING_MODULES). \
             Aborting...")
endif

# Check for missing or invalid modules
MISSING_MODULES := $(filter-out $(ALL_STANDARD_MODULES) $(ALL_PRODUCT_MODULES),\
                   $(FIRMWARE_MODULES_LIST) $(BS_FIRMWARE_MODULE_HEADERS_ONLY))
ifneq ($(MISSING_MODULES),)
    $(error "Missing or invalid module(s): $(MISSING_MODULES). Aborting...")
endif

# Collect both the product and non product-specific module directory paths
MODULE_PATHS := $(wildcard $(MODULES_DIR)/* $(PRODUCT_MODULES_DIR)/*)

# Filter out the module src/lib directory paths
SOURCE_MODULE_PATHS := $(wildcard $(addsuffix /src,$(MODULE_PATHS)))
LIBRARY_MODULE_PATHS := $(wildcard $(addsuffix /lib,$(MODULE_PATHS)))

# Pull the module names from the module source directory paths
SOURCE_MODULES := \
    $(patsubst $(MODULES_DIR)/%/src,%,$(SOURCE_MODULE_PATHS))
SOURCE_MODULES := \
    $(patsubst $(PRODUCT_MODULES_DIR)/%/src,%,$(SOURCE_MODULES))

# Select the source modules for the current firmware
SOURCE_MODULES := $(filter $(FIRMWARE_MODULES_LIST),$(SOURCE_MODULES))

# Pull the module names from the module library directory paths
LIBRARY_MODULES := \
    $(patsubst $(MODULES_DIR)/%/lib,%,$(LIBRARY_MODULE_PATHS))
LIBRARY_MODULES := \
    $(patsubst $(PRODUCT_MODULES_DIR)/%/lib,%,$(LIBRARY_MODULES))

# Select the library modules for the current firmware
LIBRARY_MODULES := $(filter $(FIRMWARE_MODULES_LIST),$(LIBRARY_MODULES))

# Divide libraries into two groups
LIBRARY_MODULES_STANDARD := $(filter $(LIBRARY_MODULES),$(ALL_STANDARD_MODULES))
LIBRARY_MODULES_PRODUCT := $(filter $(LIBRARY_MODULES),$(ALL_PRODUCT_MODULES))

# Modules selected to be built into the firmware
BUILD_STANDARD_MODULES := $(filter $(ALL_STANDARD_MODULES), \
                                   $(SOURCE_MODULES))
BUILD_PRODUCT_MODULES := $(filter $(ALL_PRODUCT_MODULES), \
                                  $(SOURCE_MODULES))

# Module selected to have their headers made available for inclusion by other
# modules and their configuration files. These modules are not built into the
# firmware.
HEADER_STANDARD_MODULES := $(filter $(BS_FIRMWARE_MODULE_HEADERS_ONLY), \
                                    $(ALL_STANDARD_MODULES))
HEADER_PRODUCT_MODULES := $(filter $(BS_FIRMWARE_MODULE_HEADERS_ONLY), \
                                   $(ALL_PRODUCT_MODULES))

ifneq ($(findstring $(BS_FIRMWARE_CPU),$(ARMV8A_CPUS)),)
    INCLUDES += $(OS_DIR)/Include
    INCLUDES += $(FREERTOS_DIR)/../../Source/include
    INCLUDES += $(FREERTOS_DIR)/../../Source/portable/GCC/ARM_CA53_64_Rcar
else
    INCLUDES += $(OS_DIR)/RTX/Source
    INCLUDES += $(OS_DIR)/RTX/Include
    INCLUDES += $(OS_DIR)/../Core/Include
endif

ifeq ($(BS_FIRMWARE_HAS_MULTITHREADING),yes)
    BUILD_SUFFIX := $(MULTHREADING_SUFFIX)
    BUILD_HAS_MULTITHREADING := yes
else
    BUILD_HAS_MULTITHREADING := no
endif

export BUILD_HAS_MULTITHREADING

ifeq ($(BS_FIRMWARE_HAS_NOTIFICATION),yes)
    BUILD_SUFFIX := $(BUILD_SUFFIX)$(NOTIFICATION_SUFFIX)
    BUILD_HAS_NOTIFICATION := yes
else
    BUILD_HAS_NOTIFICATION := no
endif
export BUILD_HAS_NOTIFICATION

ifeq ($(BS_FIRMWARE_HAS_FAST_CHANNELS),yes)
    BUILD_HAS_FAST_CHANNELS := yes
else
    BUILD_HAS_FAST_CHANNELS := no
endif

ifeq ($(BS_FIRMWARE_HAS_PERF_PLUGIN_HANDLER),yes)
    BUILD_HAS_SCMI_PERF_PLUGIN_HANDLER := yes
else
    BUILD_HAS_SCMI_PERF_PLUGIN_HANDLER := no
endif

ifeq ($(BS_FIRMWARE_HAS_SCMI_NOTIFICATIONS),yes)
    BUILD_HAS_SCMI_NOTIFICATIONS := yes
else
    BUILD_HAS_SCMI_NOTIFICATIONS := no
endif

ifeq ($(BS_FIRMWARE_HAS_SCMI_SENSOR_EVENTS),yes)
    BUILD_HAS_SCMI_SENSOR_EVENTS := yes
else
    BUILD_HAS_SCMI_SENSOR_EVENTS := no
endif

ifeq ($(BS_FIRMWARE_HAS_CLOCK_TREE_MGMT),yes)
    BUILD_HAS_CLOCK_TREE_MGMT := yes
else
    BUILD_HAS_CLOCK_TREE_MGMT := no
endif

# Add directories to the list of targets to build
LIB_TARGETS_y += $(patsubst %,$(MODULES_DIR)/%/src, \
                            $(BUILD_STANDARD_MODULES))
LIB_TARGETS_y += $(patsubst %,$(PRODUCT_MODULES_DIR)/%/src, \
                            $(BUILD_PRODUCT_MODULES))

# Add lib path to the list of modules to link
MODULE_LIBS_y += $(patsubst %, \
    $(BUILD_FIRMWARE_DIR)/module/%$(BUILD_SUFFIX)/$(MODE)/lib/lib.a, \
    $(BUILD_STANDARD_MODULES) $(BUILD_PRODUCT_MODULES))

# Add path for libraries
MODULE_LIBS_y += $(foreach module,$(LIBRARY_MODULES_STANDARD), \
    $(MODULES_DIR)/$(module)/lib/mod_$(module).a)
MODULE_LIBS_y += $(foreach module,$(LIBRARY_MODULES_PRODUCT), \
    $(PRODUCT_MODULES_DIR)/$(module)/lib/*.a)

# Create a list of include directories from the selected modules and libraries
MODULE_INCLUDES += $(patsubst %,$(MODULES_DIR)/%/include, \
                              $(BUILD_STANDARD_MODULES) \
                              $(LIBRARY_MODULES_STANDARD))
MODULE_INCLUDES += $(patsubst %,$(PRODUCT_MODULES_DIR)/%/include, \
                              $(BUILD_PRODUCT_MODULES) \
                              $(LIBRARY_MODULES_PRODUCT))
MODULE_INCLUDES += $(patsubst %,$(MODULES_DIR)/%/include, \
                              $(HEADER_STANDARD_MODULES))
MODULE_INCLUDES += $(patsubst %,$(PRODUCT_MODULES_DIR)/%/include, \
                              $(HEADER_PRODUCT_MODULES))

# Default product include directories
PRODUCT_INCLUDES += $(PRODUCT_DIR)
PRODUCT_INCLUDES += $(PRODUCT_DIR)/include

# Add the firmware directory to the main INCLUDES list
INCLUDES += $(FIRMWARE_DIR)

# Add module, product and library includes to the main INCLUDES list
export INCLUDES += $(MODULE_INCLUDES) $(PRODUCT_INCLUDES)

#
# Standard libraries
#
LIB_TARGETS_y += $(ARCH_DIR)/src
LIB_TARGETS_y += $(FWK_DIR)/src

LIBS_y += $(call lib_path,arch$(BUILD_SUFFIX))
LIBS_y += $(call lib_path,framework$(BUILD_SUFFIX))

# Add the CLI Debugger library
INCLUDES += $(DBG_DIR)/include
ifeq ($(BUILD_HAS_DEBUGGER),yes)
    LIB_TARGETS_y += $(DBG_DIR)/src
    LIBS_y += $(call lib_path,debugger$(BUILD_SUFFIX))
endif

# Add the STM32 HAL Library
INCLUDES += $(STM32_HAL_DIR)/git/Inc
INCLUDES += $(STM32_HAL_DIR)/include
ifeq ($(BUILD_HAS_STM32_HAL),yes)
		LIB_TARGETS_y += $(STM32_HAL_DIR)/src
		LIBS_y += $(call lib_path,contrib/stm32h7xx_hal$(BUILD_SUFFIX))
endif

#
# Additional library dependencies
#

include $(ARCH_DIR)/$(BS_ARCH_VENDOR)/vendor.mk

LIBS_y += $(BS_LIB_DEPS)

SOURCES += $(BUILD_FIRMWARE_DIR)/fwk_module_list.c
$(BUILD_FIRMWARE_DIR)/fwk_module_list.c: gen_module
EXTRA_DEP := gen_module
export EXTRA_DEP
export BUILD_FIRMWARE_DIR
export FIRMWARE_MODULES_LIST
export DEFINES

include $(BS_DIR)/rules.mk

LDFLAGS_GCC += -Wl,-Map=$(TARGET).map

ifneq ($(BS_ARCH_CPU),host)
    LDFLAGS_GCC += -Wl,-n
endif

LDFLAGS_GCC +=  \
    -Wl,--start-group \
        $(BUILTIN_LIBS) \
        $(MODULE_LIBS_y) \
        $(LIBS_y) \
        $(OBJECTS) \
        $(LIBS_GROUP_END) \
    -Wl,--end-group

LDFLAGS_ARM += \
    $(BUILTIN_LIBS) \
    $(MODULE_LIBS) \
    $(MODULE_LIBS_y) \
    $(LIBS_y) \
    $(OBJECTS)

.SECONDEXPANSION:

$(TARGET_ELF): $(LIB_TARGETS_y) $(SCATTER_PP) $(OBJECTS) | $$(@D)/
	$(call show-action,LD,$@)
	$(LD) $(LDFLAGS) -o $@
	$(SIZE) $@

$(SCATTER_PP): $(SCATTER_SRC) | $$(@D)/
	$(call show-action,GEN,$@)
	$(CC) $(CFLAGS) $(CPPFLAGS) $< -o $@

$(TARGET_BIN): $(TARGET_ELF) | $$(@D)/
	$(call show-action,BIN,$@)
	$(OBJCOPY) $< $(OCFLAGS) $@
	cp $@ $(BIN_DIR)/firmware.bin

$(TARGET_SREC): $(TARGET_BIN)
	$(call show-action,SREC,$@)
	$(OBJCOPY) -O srec $(TARGET_ELF) $(basename $@).srec
endif
