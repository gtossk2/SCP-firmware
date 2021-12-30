// SPDX-License-Identifier: BSD-3-Clause

/* written 2021 by John Chung */

#include <mod_stm32_timer.h>

#include <fwk_element.h>
#include <fwk_id.h>
#include <fwk_module.h>
#include <fwk_module_idx.h>
#include <fwk_time.h>

/*
 * STM32 timer driver config
 */
static const struct fwk_element stm32_timer_dev_table[] = {
    [0] = { .name = "TIMER 1",
            .data = &((struct mod_stm32_timer_dev_config){
                .timer_number = 1,
            }) },
    [1] = { 0 },
};

const struct fwk_module_config config_stm32_timer = {
    .elements = FWK_MODULE_STATIC_ELEMENTS_PTR(stm32_timer_dev_table),
};

struct fwk_time_driver fmw_time_driver(const void **ctx)
{
    return mod_stm32_timer_driver(
        ctx, config_stm32_timer.elements.table[0].data);
}
