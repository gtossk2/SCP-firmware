/*
 * Arm SCP/MCP Software
 * Copyright (c) 2021, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <tc1_mock_psu.h>

#include <mod_mock_psu.h>

#include <fwk_element.h>
#include <fwk_module.h>

static const struct fwk_element element_table[MOCK_PSU_ELEMENT_IDX_COUNT + 1] = {
    [MOCK_PSU_ELEMENT_IDX_KLEIN] = {
        .name = "DVFS_GROUP_KLEIN",
        .data =
            &(const struct mod_mock_psu_element_cfg){
                .async_alarm_id = FWK_ID_NONE_INIT,
                .async_alarm_api_id = FWK_ID_NONE_INIT,

                .async_response_id = FWK_ID_NONE_INIT,
                .async_response_api_id = FWK_ID_NONE_INIT,

                .default_enabled = true,
                .default_voltage = 550,
            },
    },
    [MOCK_PSU_ELEMENT_IDX_MAKALU] = {
        .name = "DVFS_GROUP_MAKALU",
        .data =
            &(const struct mod_mock_psu_element_cfg){
                .async_alarm_id = FWK_ID_NONE_INIT,
                .async_alarm_api_id = FWK_ID_NONE_INIT,

                .async_response_id = FWK_ID_NONE_INIT,
                .async_response_api_id = FWK_ID_NONE_INIT,

                .default_enabled = true,
                .default_voltage = 550,
            },
    },
    [MOCK_PSU_ELEMENT_IDX_MAKALU_ELP] = {
        .name = "DVFS_GROUP_MAKALU_ELP",
        .data =
            &(const struct mod_mock_psu_element_cfg){
                .async_alarm_id = FWK_ID_NONE_INIT,
                .async_alarm_api_id = FWK_ID_NONE_INIT,

                .async_response_id = FWK_ID_NONE_INIT,
                .async_response_api_id = FWK_ID_NONE_INIT,

                .default_enabled = true,
                .default_voltage = 550,
            },
    },
    { 0 }
};

static const struct fwk_element *get_element_table(fwk_id_t module_id)
{
    return element_table;
}

const struct fwk_module_config config_mock_psu = {
    .elements = FWK_MODULE_DYNAMIC_ELEMENTS(get_element_table),
};
