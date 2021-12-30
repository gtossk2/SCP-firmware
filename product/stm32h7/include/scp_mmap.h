// SPDX-License-Identifier: BSD-3-Clause

/* written 2021 by John Chung */

#ifndef SCP_MMAP_H
#define SCP_MMAP_H

#define SCP_BOOT_ROM_BASE            0x08000000
#define SCP_BOOT_ROM_SIZE            (2 * 1024 * 1024)
#define SCP_ITC_RAM_BASE             0x00000000
#define SCP_ITC_RAM_SIZE             (64 * 1024)
#define SCP_DTC_RAM_BASE             0x24000000
#define SCP_DTC_RAM_SIZE             (512 * 1024)
#define SCP_PERIPHERAL_BASE          0x40000000

#endif /* SCP_MMAP_H */
