// SPDX-License-Identifier: BSD-3-Clause

/* written 2021 by John Chung */

#include <mod_timer.h>

#include <fwk_element.h>
#include <fwk_id.h>
#include <fwk_module.h>
#include <fwk_module_idx.h>

#include <fmw_cmsis.h>

/*
 * Timer HAL config
 */
static const struct fwk_element timer_dev_table[] = {
    [0] = {
        .name = "TIMER 1",
        .data = &((struct mod_timer_dev_config) {
            .id = FWK_ID_ELEMENT_INIT(FWK_MODULE_IDX_STM32_TIMER, 0),
            .timer_irq = TIM1_UP_IRQn,
        }),
        .sub_element_count = 1,
    },
    [1] = { 0 },
};

static const struct fwk_element *timer_get_dev_table(fwk_id_t module_id)
{
    return timer_dev_table;
}

const struct fwk_module_config config_timer = {
    .elements = FWK_MODULE_DYNAMIC_ELEMENTS(timer_get_dev_table),
};
