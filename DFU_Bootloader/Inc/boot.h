#ifndef __BOOT_H
#define __BOOT_H

#include "main.h"
#include "fw_info.h"
#include "mapping_cfg.h"
#include "stm32f1xx.h"
#include "stm32f1xx_hal.h"
#include "usbd_dfu_if.h"

#define FLASH_ERASE_TIME      (uint16_t)50
#define FLASH_PROGRAM_TIME    (uint16_t)50


typedef enum 
{
  BOOT_STATUS_OK = 0,
  BOOT_STATUS_ERROR,
  BOOT_STATUS_BUSY,
  BOOT_STATUS_CRC = 4,
  BOOT_STATUS_NO_INSTALL
} boot_status_t;


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

