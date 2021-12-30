// SPDX-License-Identifier: BSD-3-Clause

/* written 2021 by John Chung */

#include "stm32_init.h"

#include <mod_stm32_timer.h>
#include <mod_timer.h>

#include <fwk_assert.h>
#include <fwk_event.h>
#include <fwk_id.h>
#include <fwk_interrupt.h>
#include <fwk_log.h>
#include <fwk_macros.h>
#include <fwk_mm.h>
#include <fwk_module.h>
#include <fwk_notification.h>
#include <fwk_status.h>
#include <fwk_time.h>

#include <stddef.h>
#include <stdint.h>

extern uint64_t gTicks;

/* Device content */
struct dev_ctx {
    const struct mod_stm32_timer_dev_config *config;
};

static struct mod_stm32_timer_ctx {
    bool initialized; /* Whether the device context has been initialized */

    struct dev_ctx *table; /* Device context table */
} mod_stm32_timer_ctx;

/*
 * Functions fulfilling the Timer module's driver interface
 */

static TIM_TypeDef *get_timer_base(uint8_t timer)
{
    TIM_TypeDef *TIMx = NULL;

    switch (timer) {
    case 1:
        TIMx = TIM1;
        break;
    default:
        FWK_LOG_CRIT("[TIMER] Not supported timer number");
        fwk_assert(0);
        break;
    }

    return TIMx;
}

static IRQn_Type get_timer_irq(uint8_t timer)
{
    IRQn_Type IRQn;

    switch (timer) {
    case 1:
        IRQn = TIM1_UP_IRQn;
        break;
    default:
        FWK_LOG_CRIT("[TIMER] Not supported timer number irq");
        fwk_assert(0);
        break;
    }

    return IRQn;
}

static int enable(fwk_id_t dev_id)
{
    struct dev_ctx *ctx;
    TIM_TypeDef *TIMx = NULL;

    ctx = mod_stm32_timer_ctx.table + fwk_id_get_element_idx(dev_id);

    TIMx = get_timer_base(ctx->config->timer_number);

    LL_TIM_EnableCounter(TIMx);
    return FWK_SUCCESS;
}

static int disable(fwk_id_t dev_id)
{
    struct dev_ctx *ctx;
    TIM_TypeDef *TIMx;

    ctx = mod_stm32_timer_ctx.table + fwk_id_get_element_idx(dev_id);

    TIMx = get_timer_base(ctx->config->timer_number);

    LL_TIM_DisableCounter(TIMx);

    if (LL_TIM_IsActiveFlag_UPDATE(TIMx)) {
        LL_TIM_ClearFlag_UPDATE(TIMx);
    }
    return FWK_SUCCESS;
}

static int get_counter(fwk_id_t dev_id, uint64_t *value)
{
    // return by microsecond
    *value = gTicks * 1000;

    return FWK_SUCCESS;
}

static int set_timer(fwk_id_t dev_id, uint64_t timestamp)
{
    uint32_t reload = 0;
    struct dev_ctx *ctx;
    TIM_TypeDef *TIMx;

    ctx = mod_stm32_timer_ctx.table + fwk_id_get_element_idx(dev_id);

    timestamp = (uint64_t)(timestamp / 1000) - gTicks; // ms
    reload = (timestamp * 10) - 1; // To reload ticks

    TIMx = get_timer_base(ctx->config->timer_number);

    LL_TIM_SetAutoReload(TIMx, reload);

    return FWK_SUCCESS;
}

static int get_timer(fwk_id_t dev_id, uint64_t *timestamp)
{
    // return by microsecond
    *timestamp = gTicks * 1000;

    return FWK_SUCCESS;
}

static int get_frequency(fwk_id_t dev_id, uint32_t *frequency)
{
    *frequency = FWK_MHZ;

    return FWK_SUCCESS;
}

static const struct mod_timer_driver_api module_api = {
    .enable = enable,
    .disable = disable,
    .set_timer = set_timer,
    .get_timer = get_timer,
    .get_counter = get_counter,
    .get_frequency = get_frequency,
};

/*
 * Functions fulfilling the framework's module interface
 */

static int stm32_timer_init(
    fwk_id_t module_id,
    unsigned int element_count,
    const void *data)
{
    mod_stm32_timer_ctx.table =
        fwk_mm_alloc(element_count, sizeof(struct dev_ctx));

    return FWK_SUCCESS;
}

static int stm32_timer_device_init(
    fwk_id_t element_id,
    unsigned int unused,
    const void *data)
{
    struct dev_ctx *ctx;

    ctx = mod_stm32_timer_ctx.table + fwk_id_get_element_idx(element_id);

    ctx->config = data;

    if (ctx->config->timer_number == 0) {
        return FWK_E_DEVICE;
    }

    return FWK_SUCCESS;
}

static int gtimer_process_bind_request(
    fwk_id_t requester_id,
    fwk_id_t id,
    fwk_id_t api_type,
    const void **api)
{
    /* No binding to the module */
    if (fwk_module_is_valid_module_id(id)) {
        return FWK_E_ACCESS;
    }

    *api = &module_api;

    return FWK_SUCCESS;
}

static void stm32_timer_control_init(struct dev_ctx *ctx)
{
    /* Prescaler declaration */
    uint32_t uwPrescalerValue = 0;
    TIM_TypeDef *TIMx = NULL;
    IRQn_Type IRQn = 0;

    /* Peripheral clock enable */
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_TIM1);

    /* Compute the prescaler value to have TIMx counter clock equal to 10000 Hz
     */
    uwPrescalerValue = (uint32_t)((40000 / 2) - 1);

    TIMx = get_timer_base(ctx->config->timer_number);

    LL_TIM_SetPrescaler(TIMx, uwPrescalerValue);
    LL_TIM_SetCounterMode(TIMx, LL_TIM_COUNTERMODE_UP);
    LL_TIM_SetAutoReload(TIMx, 1000 - 1); // Init at 100ms
    LL_TIM_SetClockDivision(TIMx, LL_TIM_CLOCKDIVISION_DIV1);
    LL_TIM_SetRepetitionCounter(TIMx, 0);

    // Enable TIM1 Update Interrupt
    LL_TIM_EnableIT_UPDATE(TIMx);

    IRQn = get_timer_irq(ctx->config->timer_number);
    // Enable NVIC Interrupt IRQ
    NVIC_SetPriority(IRQn, 0x0);
    NVIC_EnableIRQ(IRQn);
}

static int stm32_timer_start(fwk_id_t id)
{
    struct dev_ctx *ctx;

    if (!fwk_id_is_type(id, FWK_ID_TYPE_ELEMENT)) {
        return FWK_SUCCESS;
    }

    ctx = mod_stm32_timer_ctx.table + fwk_id_get_element_idx(id);

    // init at 100 ms
    stm32_timer_control_init(ctx);

    return FWK_SUCCESS;
}

/*
 * Module descriptor
 */

const struct fwk_module module_stm32_timer = {
    .api_count = 1,
    .type = FWK_MODULE_TYPE_DRIVER,
    .init = stm32_timer_init,
    .element_init = stm32_timer_device_init,
    .start = stm32_timer_start,
    .process_bind_request = gtimer_process_bind_request,
};

static fwk_timestamp_t mod_stm32_timer_timestamp(const void *ctx)
{
    // return by nanosecond
    return (fwk_timestamp_t)gTicks * 1000000;
}

struct fwk_time_driver mod_stm32_timer_driver(
    const void **ctx,
    const struct mod_stm32_timer_dev_config *cfg)
{
    *ctx = cfg;

    return (struct fwk_time_driver){
        .timestamp = mod_stm32_timer_timestamp,
    };
}
