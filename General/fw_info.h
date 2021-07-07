#ifndef __FW_INFO_H
#define __FW_INFO_H

#include <stdint.h>

#define APP_FW_MAGIC (0xDEADBEAF)

typedef struct {
  uint32_t magic;      // patern num to finde end of image
  struct {
    uint8_t major;
    uint8_t minor;
    uint8_t patch;
    uint8_t reserved1;
  } version;
  uint32_t time;      // build time utc
  uint32_t sha;       // commit sha
  uint8_t reserved2[12];
  uint32_t crc32;     // firmware image crc32
} fw_info_t;

#endif //__FW_INFO_H
