#ifndef __BOOT_H
#define __BOOT_H

#include "main.h"
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "usbd_dfu_if.h"


#define FLASH_ERASE_TIME      (uint16_t)50
#define FLASH_PROGRAM_TIME    (uint16_t)50


#define FLASH_MEM_START         (0x08000000UL)
#define FLASH_MEM_END           (0x0800FFFFUL) //for stm32f103c8t6

#define FLASH_TOTAL_PAGES       ((FLASH_MEM_END - FLASH_MEM_START + 1) / FLASH_PAGE_SIZE)

#define APP_BOOTLOADER_PAGES     (16U)
#define APP_BOOT_CONF_PAGES      (1U)
#define APP_SWAP_REGION_PAGES    (1U)
#define APP_WORKING_PAGES        (FLASH_TOTAL_PAGES - APP_BOOTLOADER_PAGES - APP_BOOT_CONF_PAGES - APP_SWAP_REGION_PAGES)

#define APP_OTA_REGION_PAGES     (APP_WORKING_PAGES / 2) // equal size for ota and main
#define APP_MAIN_REGION_PAGES    (APP_WORKING_PAGES / 2)

//***********************************************************************************************************
#define APP_BOOTLDR_ADDR_START   (FLASH_MEM_START)
#define APP_BOOTLDR_ADDR_END     (APP_BOOTLDR_ADDR_START + (APP_BOOTLOADER_PAGES * FLASH_PAGE_SIZE) - 1)

#define APP_MAIN_ADDR_START      (APP_BOOTLDR_ADDR_END + 1)
#define APP_MAIN_ADDR_END        (APP_MAIN_ADDR_START + (APP_MAIN_REGION_PAGES * FLASH_PAGE_SIZE) - 1)

#define APP_OTA_ADDR_START       (APP_MAIN_ADDR_END + 1)
#define APP_OTA_ADDR_END         (APP_OTA_ADDR_START + (APP_OTA_REGION_PAGES * FLASH_PAGE_SIZE) - 1)

#define APP_SWAP_ADDR_START      (APP_OTA_ADDR_END + 1)
#define APP_SWAP_ADDR_END        (APP_SWAP_ADDR_START + (APP_SWAP_REGION_PAGES * FLASH_PAGE_SIZE) - 1)

#define APP_BOOT_CFG_ADDR_START  (APP_SWAP_ADDR_END + 1)
#define APP_BOOT_CFG_ADDR_END    (APP_BOOT_CFG_ADDR_START + (APP_BOOT_CONF_PAGES * FLASH_PAGE_SIZE) - 1)

//---------------------------------------------------------------------------------------------------------
//      16      |            23            |           23        |        1        |          1           |
//---------------------------------------------------------------------------------------------------------
//|  BOOTLOADER |      APP_MAIN_REGION     |     APP_OTA_REGION  | APP_SWAP_REGION | APP_BOOT_CONF_REGION |
//---------------------------------------------------------------------------------------------------------

#define APP_FW_MAGIC (0xDEADBEAF)

typedef enum 
{
  BOOT_STATUS_OK = 0,
  BOOT_STATUS_ERROR,
  BOOT_STATUS_BUSY,
  BOOT_STATUS_CRC = 4,
  BOOT_STATUS_NO_INSTALL
} boot_status_t;

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



boot_status_t Boot_IsAppInstalled(void);
boot_status_t Boot_ValidateCrc(uint32_t addr, fw_info_t *fw_info);
boot_status_t Boot_AppValidation(void);
boot_status_t Boot_AppInstall(void);
boot_status_t Boot_EraseSection(uint32_t addr, uint32_t pages);
boot_status_t Boot_CopySection(uint32_t dest_addr, uint32_t source_addr, uint32_t pages);

void Boot_DfuFwUpdateCheck(void);
void Boot_DfuFwUpdateFeedback(void);
uint32_t* Boot_FindeFwInfo(uint32_t addr);

#endif //__APP_H

