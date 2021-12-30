#ifndef __STM32_INIT_H_
#define __STM32_INIT_H_

#include "stm32h7xx_ll_bus.h"
#include "stm32h7xx_ll_gpio.h"
#include "stm32h7xx_ll_pwr.h"
#include "stm32h7xx_ll_rcc.h"
#include "stm32h7xx_ll_tim.h"
#include "stm32h7xx_ll_usart.h"
#include "stm32h7xx_ll_utils.h"

/* Definition LED1-GREEN*/
#define LED1_PIN                           LL_GPIO_PIN_0
#define LED1_GPIO_PORT                     GPIOB
#define LED1_GPIO_CLK_ENABLE()             LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOB)

/* Definition LED2-ORANGE*/
#define LED2_PIN                           LL_GPIO_PIN_1
#define LED2_GPIO_PORT                     GPIOE
#define LED2_GPIO_CLK_ENABLE()             LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOE)

/* Definition LED3-RED*/
#define LED3_PIN                           LL_GPIO_PIN_14
#define LED3_GPIO_PORT                     GPIOB
#define LED3_GPIO_CLK_ENABLE()             LL_AHB4_GRP1_EnableClock(LL_AHB4_GRP1_PERIPH_GPIOB)

void SystemClock_Config(void);

#endif
