#include "boot.h"
#include "crc.h"

#define APP_UPDATE_TIMEOUT  (2000U)

static volatile uint32_t process_end = 0;
static volatile uint32_t prev_value = 0;
static volatile uint32_t prev_time = 0;

static boot_status_t Boot_RoleBackCheck(fw_info_t *main, fw_info_t *ota);


boot_status_t Boot_EraseSection(uint32_t addr, uint32_t pages)
{
  uint32_t PageError = 0;
  FLASH_EraseInitTypeDef eraseinit;
  eraseinit.TypeErase   = FLASH_TYPEERASE_PAGES;
  eraseinit.NbPages     = 1;
  
  while(pages--)
  {
    eraseinit.PageAddress = (addr + (pages * FLASH_PAGE_SIZE));
    HAL_FLASHEx_Erase(&eraseinit, &PageError);
  }
  return BOOT_STATUS_OK;
}

boot_status_t Boot_CopySection(uint32_t dest_addr, uint32_t source_addr, uint32_t pages)
{
  boot_status_t status = BOOT_STATUS_ERROR;
  
  HAL_FLASH_Unlock();
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGERR);
  
  while(pages--)
  {
    Boot_EraseSection(dest_addr + (pages * FLASH_PAGE_SIZE), 1);
    for(uint16_t count = 0; count < FLASH_PAGE_SIZE / sizeof(uint32_t); count++)
    {
      status = (boot_status_t)HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, dest_addr + (pages * FLASH_PAGE_SIZE) + (count * sizeof(uint32_t)), *(uint32_t *)(source_addr + (pages * FLASH_PAGE_SIZE) + (count * sizeof(uint32_t))));
      if (BOOT_STATUS_OK != status) break;
    }
  }
  HAL_FLASH_Lock();
  return status;
}

boot_status_t Boot_AppInstall(void)
{
  boot_status_t status = BOOT_STATUS_ERROR;
  uint32_t pages = APP_OTA_REGION_PAGES;
  uint32_t buff[FLASH_PAGE_SIZE / sizeof(uint32_t)] = {0};
  
  HAL_FLASH_Unlock();
  __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_WRPERR | FLASH_FLAG_PGERR);

  while(pages--)
  {
    uint32_t addrMain = APP_MAIN_ADDR_START + (pages * FLASH_PAGE_SIZE);
    uint32_t addrOta  = APP_OTA_ADDR_START  + (pages * FLASH_PAGE_SIZE);

    memcpy(buff, (const void *)((__IO uint32_t *)(APP_MAIN_ADDR_START + (pages * FLASH_PAGE_SIZE))), sizeof(buff));
    
    status = Boot_CopySection(addrMain, addrOta, 1);
    if (BOOT_STATUS_OK != status) break;
    status = Boot_CopySection(addrOta, (uint32_t)buff, 1);
    if (BOOT_STATUS_OK != status) break;
  }
  HAL_FLASH_Lock();
  return status;  
}



boot_status_t Boot_IsAppInstalled(void)
{
  boot_status_t status = BOOT_STATUS_NO_INSTALL;
  // check is the application installed
  if(((*(__IO uint32_t *) APP_MAIN_ADDR_START) & 0x2FFE0000) == 0x20000000)
  {
    status = BOOT_STATUS_OK;
  }
  return status;
}

boot_status_t Boot_ValidateCrc(uint32_t Addr, fw_info_t *fw_info)
{
  static uint32_t crc = 0;
  static uint32_t fw_size = 0;

  fw_size = ((uint32_t)fw_info - Addr) + (sizeof(fw_info_t) - sizeof(uint32_t));
  crc = crc32((uint8_t*)Addr, fw_size);
  if (crc == fw_info->crc32)
  {
    return BOOT_STATUS_OK;
  }
  return BOOT_STATUS_CRC;
}

boot_status_t Boot_AppValidation(void)
{
  boot_status_t status = BOOT_STATUS_CRC;
  fw_info_t *fw_info_main = NULL;
  
  status = Boot_IsAppInstalled();
  if (BOOT_STATUS_OK == status)
  {
    fw_info_main = (fw_info_t *)Boot_FindeFwInfo(APP_MAIN_ADDR_START);
    status = Boot_ValidateCrc(APP_MAIN_ADDR_START, fw_info_main);
  }
  return status;
}


void Boot_DfuFwUpdateCheck(void)
{
  fw_info_t *fw_info_ota  = NULL;
  fw_info_t *fw_info_main = NULL;
  
  do 
  {
    if (0 == process_end) break;
    
    if (prev_value != process_end)
    {
      prev_value = process_end;
      prev_time = HAL_GetTick();
      break;
    }
    
    if ((HAL_GetTick()- prev_time) > APP_UPDATE_TIMEOUT) // wait untill the process_end stop to update
    {
      fw_info_ota = (fw_info_t *)Boot_FindeFwInfo(APP_OTA_ADDR_START);     //check is fw valid in ota partition 
      if (NULL == fw_info_ota)
      {
        break;
      }
      
      if (BOOT_STATUS_OK != Boot_ValidateCrc(APP_OTA_ADDR_START, fw_info_ota))
      {
        Boot_EraseSection(APP_OTA_ADDR_START, APP_OTA_REGION_PAGES);
        process_end = 0;
        break;
      }
      
      fw_info_main = (fw_info_t *)Boot_FindeFwInfo(APP_MAIN_ADDR_START);
      if (NULL != fw_info_main)
      {
        if (BOOT_STATUS_OK != Boot_RoleBackCheck(fw_info_main, fw_info_ota))
        {
          break;
        }
      }
      
      Boot_AppInstall();
      NVIC_SystemReset();
    }
  } while(0);
}

void Boot_DfuFwUpdateFeedback(void)
{
  process_end++;
}

uint32_t* Boot_FindeFwInfo(uint32_t addr)
{
  uint32_t* ret = NULL;
  uint32_t temp = (APP_MAIN_ADDR_START == addr) ? APP_MAIN_ADDR_END : APP_OTA_ADDR_END;
  
  for(uint32_t i = addr; i < temp; i+=4)
  {
    if (APP_FW_MAGIC == *(__IO uint32_t*)i)
    {
      ret = (uint32_t *)i;
      break;
    }
  }
  return ret;
}

static boot_status_t Boot_RoleBackCheck(fw_info_t *main, fw_info_t *ota)
{
  boot_status_t status = BOOT_STATUS_ERROR;  
  uint32_t ver_main = 0;
  uint32_t ver_ota  = 0;
  ver_main = ((main->version.major << 16) | (main->version.minor << 8) | (main->version.patch & 0xff));
  ver_ota  = ((ota->version.major << 16)  | (ota->version.minor << 8)  | (ota->version.patch & 0xff));
  if (ver_ota >= ver_main)  // the same fw version is ok
  {
      status = BOOT_STATUS_OK;
  }
  return status;
}
