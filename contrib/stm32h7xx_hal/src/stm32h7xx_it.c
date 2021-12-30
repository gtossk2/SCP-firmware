#include "stm32_init.h"
#include "stm32h7xx_it.h"

#include <stdint.h>

/******************************************************************************/
/*            Cortex-M7 Processor Exceptions Handlers                         */
/******************************************************************************/

uint64_t gTicks = 0;

void NMI_Handler(void)
{
}

/******************************************************************************/
void HardFault_Handler(void)
{
    /* Go to infinite loop when Hard Fault exception occurs */
    while (1) {
    }
}

/******************************************************************************/
void MemManage_Handler(void)
{
    /* Go to infinite loop when Memory Manage exception occurs */
    while (1) {
    }
}

/******************************************************************************/
void BusFault_Handler(void)
{
    /* Go to infinite loop when Bus Fault exception occurs */
    while (1) {
    }
}

/******************************************************************************/
void UsageFault_Handler(void)
{
    /* Go to infinite loop when Usage Fault exception occurs */
    while (1) {
    }
}

/******************************************************************************/
void SVC_Handler(void)
{
}

/******************************************************************************/
void DebugMon_Handler(void)
{
}

/******************************************************************************/
void PendSV_Handler(void)
{
}

/******************************************************************************/
void SysTick_Handler(void)
{
    gTicks++;
    if ((gTicks % (500)) == 0) {
        LL_GPIO_TogglePin(LED1_GPIO_PORT, LED1_PIN);
    }
}
