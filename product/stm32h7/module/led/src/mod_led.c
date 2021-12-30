// SPDX-License-Identifier: BSD-3-Clause

/* written 2021 by John Chung */
#include "mod_led.h"
#include "stm32_init.h"

#include <fwk_assert.h>
#include <fwk_id.h>
#include <fwk_log.h>
#include <fwk_macros.h>
#include <fwk_mm.h>
#include <fwk_module.h>
#include <fwk_status.h>

/*
 * Framework handler functions
 */
static int led_init(
    fwk_id_t module_id,
    unsigned int element_count,
    const void *data)
{
    return FWK_SUCCESS;
}

static int led_element_init(
    fwk_id_t element_id,
    unsigned int unused,
    const void *data)
{
    return FWK_SUCCESS;
}

static int stm32_led_init(void)
{
    /* Enable the LED1 Clock */
    LED1_GPIO_CLK_ENABLE();

    /* Configure IO in output push-pull mode to drive external LED1 */
    LL_GPIO_SetPinMode(LED1_GPIO_PORT, LED1_PIN, LL_GPIO_MODE_OUTPUT);

    /* Enable the LED2 Clock */
    LED2_GPIO_CLK_ENABLE();

    /* Configure IO in output push-pull mode to drive external LED1 */
    LL_GPIO_SetPinMode(LED2_GPIO_PORT, LED2_PIN, LL_GPIO_MODE_OUTPUT);

    /* Enable the LED3 Clock */
    LED3_GPIO_CLK_ENABLE();

    /* Configure IO in output push-pull mode to drive external LED3 */
    LL_GPIO_SetPinMode(LED3_GPIO_PORT, LED3_PIN, LL_GPIO_MODE_OUTPUT);

    return FWK_SUCCESS;
}

static int led_start(fwk_id_t id)
{
    /* Initialize LED1/LED2/LED3*/
    stm32_led_init();

    // Turn on all LED and off
    for (int i = 0; i < 2; i++) {
        LL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);
        LL_GPIO_TogglePin(LED2_GPIO_PORT, LED2_PIN);
        LL_GPIO_TogglePin(LED3_GPIO_PORT, LED3_PIN);
        for (int j = 0; j < 300000; j++)
            ;
    }

    FWK_LOG_INFO("[LED] System Ready");
    return FWK_SUCCESS;
}

const struct fwk_module module_led = {
    .type = FWK_MODULE_TYPE_DRIVER,
    .init = led_init,
    .element_init = led_element_init,
    .start = led_start,
};

const struct fwk_module_config config_led = { 0 };
