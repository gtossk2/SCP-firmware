#
# Arm SCP/MCP Software
# Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

#
# Create the firmware target.
#

add_executable(tc0-bl2)

target_include_directories(
    tc0-bl2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../include"
                   "${CMAKE_CURRENT_SOURCE_DIR}")

# cmake-lint: disable=E1122

target_sources(
    tc0-bl2
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/config_system_power.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_armv7m_mpu.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_pl011.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_power_domain.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_ppu_v1.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_mhu2.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_smt.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_scmi.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_sds.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_scmi_system_power.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_scmi_clock.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_scmi_perf.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_gtimer.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_timer.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_dvfs.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_psu.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_mock_voltage_domain.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_scmi_voltage_domain.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_voltage_domain.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_mock_psu.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_system_pll.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_pik_clock.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_css_clock.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_clock.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_scmi_power_domain.c")

if(SCP_ENABLE_RESOURCE_PERMISSIONS)
    target_sources(tc0-bl2 PRIVATE "config_resource_perms.c")
endif()

#
# Some of our firmware includes require CMSIS.
#

target_link_libraries(tc0-bl2 PUBLIC cmsis::core-m)

#
# We explicitly add the CMSIS include directories to our interfaceinclude
# directories. Each module target adds these include directories totheir own,
# allowing them to include any firmware includes we expose.
#

target_include_directories(tc0-bl2
    PUBLIC $<TARGET_PROPERTY:cmsis::core-m,INTERFACE_INCLUDE_DIRECTORIES>)

if(SCP_ENABLE_RESOURCE_PERMISSIONS)
    list(APPEND SCP_MODULES "resource-perms")
endif()

set(SCP_MODULES ${SCP_MODULES} PARENT_SCOPE)
set(SCP_MODULE_PATHS ${SCP_MODULE_PATHS} PARENT_SCOPE)
