#ifndef _UBOOT_CRC_H
#define _UBOOT_CRC_H

#include <stdint.h>

/* lib/crc32.c */
uint32_t crc32(uint32_t, const uint8_t*, uint32_t);
uint32_t crc32_no_comp(uint32_t, const uint8_t*, uint32_t);

#endif /* _UBOOT_CRC_H */

