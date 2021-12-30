// SPDX-License-Identifier: BSD-3-Clause

/* written 2021 by john chung */

#ifndef FMW_MEMORY_H
#define FMW_MEMORY_H

#include "scp_mmap.h"

#define FMW_MEM_MODE ARCH_MEM_MODE_DUAL_REGION_RELOCATION

/*
 * RAM data memory
 */
#define FMW_MEM1_SIZE SCP_DTC_RAM_SIZE
#define FMW_MEM1_BASE SCP_DTC_RAM_BASE

// FLASH
#define FMW_MEM0_SIZE SCP_BOOT_ROM_SIZE
#define FMW_MEM0_BASE SCP_BOOT_ROM_BASE

#endif /* FMW_MEMORY_H */
