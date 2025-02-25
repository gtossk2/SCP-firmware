#
# Arm SCP/MCP Software
# Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

#
# Create the firmware target.
#

add_executable(rdv1mc-bl2)

target_include_directories(
    rdv1mc-bl2 PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../include"
                           "${CMAKE_CURRENT_SOURCE_DIR}")

# cmake-lint: disable=E1122

target_sources(
    rdv1mc-bl2
    PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/config_system_power.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_armv7m_mpu.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_power_domain.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_ppu_v1.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_mhu2.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_smt.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_scmi.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_sds.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_timer.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_gtimer.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_cmn650.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_scmi_system_power.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_scmi_power_domain.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_system_pll.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_pik_clock.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_css_clock.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_clock.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/config_apcontext.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/../src/config_system_info.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/../src/config_pl011.c"
            "${CMAKE_CURRENT_SOURCE_DIR}/../src/config_sid.c")

#
# Some of our firmware includes require CMSIS.
#

target_link_libraries(rdv1mc-bl2 PUBLIC cmsis::core-m)

#
# We explicitly add the CMSIS include directories to our interfaceinclude
# directories. Each module target adds these include directories totheir own,
# allowing them to include any firmware includes we expose.
#

target_include_directories(
    rdv1mc-bl2
    PUBLIC $<TARGET_PROPERTY:cmsis::core-m,INTERFACE_INCLUDE_DIRECTORIES>)
