// SPDX-License-Identifier: BSD-3-Clause

/* written 2021 by John Chung */

#ifndef MOD_STM32_TIMER_H
#define MOD_STM32_TIMER_H

#include <fwk_id.h>

#include <stdint.h>

/*!
 * \addtogroup GroupModules Modules
 * \{
 */

/*!
 * \brief STM32 timer device descriptor
 */
struct mod_stm32_timer_dev_config {
    /*! Timer number */
    const uint8_t timer_number;
};

/*!
 * \brief Get the framework time driver for a generic timer device.
 *
 * \details This function is intended to be used by a firmware to register a
 *      generic timer as the driver for the framework time component.
 *
 * \param[out] ctx Pointer to storage for the context passed to the driver.
 * \param[in] cfg Generic timer configuration.
 *
 * \return Framework time driver for the given device.
 */
struct fwk_time_driver mod_stm32_timer_driver(
    const void **ctx,
    const struct mod_stm32_timer_dev_config *cfg);

/*!
 * \}
 */

/*!
 * \}
 */

#endif /* MOD_GTIMER_H */
