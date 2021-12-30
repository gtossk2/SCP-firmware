// SPDX-License-Identifier: BSD-3-Clause

/* written 2021 by John Chung */

#include "mod_uart3.h"
#include "stm32_init.h"

#include <fwk_assert.h>
#include <fwk_id.h>
#include <fwk_macros.h>
#include <fwk_mm.h>
#include <fwk_module.h>
#include <fwk_status.h>

static struct mod_stm32_uart3_ctx {
    bool initialized; /* Whether the context has been initialized */
    bool open; /* Whether the device has an open file stream */
} mod_stm32_uart3_ctx = {
    .initialized = false,
    .open = false,
};
/*
 * Framework handler functions
 */
static int mod_stm32_uart3_init(
    fwk_id_t module_id,
    unsigned int element_count,
    const void *data)
{
    return FWK_SUCCESS;
}

static int mod_stm32_uart3_element_init(
    fwk_id_t element_id,
    unsigned int unused,
    const void *data)
{
    return FWK_SUCCESS;
}

static void delay()
{
    for (int i = 0; i < 30000; i++)
        ;
}

static int stm32_uart3_init(uint32_t baudrate)
{
    /* Configure USART3 for debug output. Configure pins and uart peripheral */

    /* Enable PORTD clock for GPIO port D */
    LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOD);

    /* Configure PD8 as Tx Pin : Alternate function 7, High Speed, Push pull,
     * Pull up */
    LL_GPIO_SetPinMode(GPIOD, LL_GPIO_PIN_8, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_8_15(GPIOD, LL_GPIO_PIN_8, LL_GPIO_AF_7);
    LL_GPIO_SetPinSpeed(GPIOD, LL_GPIO_PIN_8, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinOutputType(GPIOD, LL_GPIO_PIN_8, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinPull(GPIOD, LL_GPIO_PIN_8, LL_GPIO_PULL_UP);

    /* Configure PD9 as Rx Pin : Alternate function 7, High Speed, Push pull,
     * Pull up */
    LL_GPIO_SetPinMode(GPIOD, LL_GPIO_PIN_9, LL_GPIO_MODE_ALTERNATE);
    LL_GPIO_SetAFPin_8_15(GPIOD, LL_GPIO_PIN_9, LL_GPIO_AF_7);
    LL_GPIO_SetPinSpeed(GPIOD, LL_GPIO_PIN_9, LL_GPIO_SPEED_FREQ_HIGH);
    LL_GPIO_SetPinOutputType(GPIOD, LL_GPIO_PIN_9, LL_GPIO_OUTPUT_PUSHPULL);
    LL_GPIO_SetPinPull(GPIOD, LL_GPIO_PIN_9, LL_GPIO_PULL_UP);

    /* Enable the peripheral clock of UART */
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_USART3);

    /* TX/RX direction */
    LL_USART_SetTransferDirection(USART3, LL_USART_DIRECTION_TX_RX);

    /* 8 data bit, 1 start bit, 1 stop bit, no parity */
    LL_USART_ConfigCharacter(
        USART3,
        LL_USART_DATAWIDTH_8B,
        LL_USART_PARITY_NONE,
        LL_USART_STOPBITS_1);

    LL_USART_SetBaudRate(
        USART3,
        SystemCoreClock / 4,
        LL_USART_PRESCALER_DIV1,
        LL_USART_OVERSAMPLING_16,
        baudrate);

    // LL_USART_Enable( USART3 );
    return FWK_SUCCESS;
}

static int stm32_uart_open(const struct fwk_io_stream *stream)
{
    if (!mod_stm32_uart3_ctx.initialized) {
        // UART3 initialization
        SystemInit();
        SystemClock_Config();
        delay();

        stm32_uart3_init(115200);

        mod_stm32_uart3_ctx.initialized = true;
    }

    if (mod_stm32_uart3_ctx.open) {
        // Refuse to open twice
        return FWK_E_BUSY;
    }

    // Enable UART3
    LL_USART_Enable(USART3);
    mod_stm32_uart3_ctx.open = true;

    return FWK_SUCCESS;
}

static int stm32_uart_close(const struct fwk_io_stream *stream)
{
    fwk_assert(mod_stm32_uart3_ctx.open);

    // Disable UART3
    LL_USART_Disable(USART3);
    mod_stm32_uart3_ctx.open = false;

    return FWK_SUCCESS;
}

int stm32_uart_getch(const struct fwk_io_stream *stream, char *ch)
{
    fwk_assert(mod_stm32_uart3_ctx.open);

    while (!LL_USART_IsActiveFlag_RXNE_RXFNE(USART3)) {
        return FWK_PENDING;
    }

    *ch = LL_USART_ReceiveData9(USART3) & 0x7F;

    return FWK_SUCCESS;
}

int stm32_uart_putch(const struct fwk_io_stream *stream, char ch)
{
    fwk_assert(mod_stm32_uart3_ctx.open);

    if (ch == '\n') {
        stm32_uart_putch(stream, '\r');
    }

    while (!LL_USART_IsActiveFlag_TXE(USART3)) {
    }

    LL_USART_TransmitData9(USART3, ch);

    return FWK_SUCCESS;
}

const struct fwk_module module_uart3 = {
    .type = FWK_MODULE_TYPE_DRIVER,
    .init = mod_stm32_uart3_init,
    .element_init = mod_stm32_uart3_element_init,
    .adapter =
        (struct fwk_io_adapter){
            .open = stm32_uart_open,
            .getch = stm32_uart_getch,
            .putch = stm32_uart_putch,
            .close = stm32_uart_close,
        },
};

const struct fwk_module_config config_uart3 = { 0 };
