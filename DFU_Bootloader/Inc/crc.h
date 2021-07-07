#ifndef _CRC_H
#define _CRC_H

#include <stdint.h>


uint32_t crc32(uint8_t *data, uint16_t length);
uint32_t crc32_mpeg_2(uint8_t *data, uint16_t length);

#endif // CRC_H
