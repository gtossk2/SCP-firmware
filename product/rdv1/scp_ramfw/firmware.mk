#
# Arm SCP/MCP Software
# Copyright (c) 2020-2021, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

BS_FIRMWARE_CPU := cortex-m7
BS_FIRMWARE_HAS_NOTIFICATION := yes
BS_FIRMWARE_USE_NEWLIB_NANO_SPECS := no
BS_FIRMWARE_HAS_FAST_CHANNELS := yes

BS_FIRMWARE_MODULES := \
    armv7m_mpu \
    sid \
    system_info \
    pcid \
    pl011 \
    pik_clock \
    css_clock \
    clock \
    gtimer \
    timer \
    cmn650 \
    apcontext \
    ppu_v1 \
    system_power \
    mhu2 \
    smt \
    scmi \
    sds \
    system_pll \
    power_domain \
    scmi_power_domain \
    scmi_system_power \
    platform_system \
    psu \
    mock_psu \
    dvfs \
    scmi_perf

BS_FIRMWARE_SOURCES := \
    config_system_power.c \
    config_sid.c \
    config_system_info.c \
    config_armv7m_mpu.c \
    config_pl011.c \
    config_power_domain.c \
    config_ppu_v1.c \
    config_mhu2.c \
    config_smt.c \
    config_scmi.c \
    config_sds.c \
    config_timer.c \
    config_gtimer.c \
    config_cmn650.c \
    config_scmi_system_power.c \
    config_system_pll.c \
    config_pik_clock.c \
    config_css_clock.c \
    config_clock.c \
    config_apcontext.c \
    config_scmi_power_domain.c \
    config_psu.c \
    config_mock_psu.c \
    config_dvfs.c \
    config_scmi_perf.c

include $(BS_DIR)/firmware.mk
