#ifndef __MAPPING_CFG_H
#define __MAPPING_CFG_H

#define FLASH_MEM_START          0x08000000
#define FLASH_MEM_END            0x0800FFFF //for stm32f103c8t6
#define FLASH_PAGE_SIZE          0x400

#define FLASH_TOTAL_PAGES        ((FLASH_MEM_END - FLASH_MEM_START + 1) / FLASH_PAGE_SIZE)

#define MAP_BOOTLOADER_PAGES     16
#define MAP_WORKING_PAGES        (FLASH_TOTAL_PAGES - MAP_BOOTLOADER_PAGES)

#define MAP_OTA_REGION_PAGES     (MAP_WORKING_PAGES / 2) // equal size for ota and main
#define MAP_MAIN_REGION_PAGES    (MAP_WORKING_PAGES / 2)

//***********************************************************************************************************
#define MAP_BOOTLDR_ADDR_START   (FLASH_MEM_START)
#define MAP_BOOTLDR_ADDR_END     (MAP_BOOTLDR_ADDR_START + (MAP_BOOTLOADER_PAGES * FLASH_PAGE_SIZE) - 1)
#define MAP_BOOTLDR_SIZE         ((MAP_BOOTLDR_ADDR_END - MAP_BOOTLDR_ADDR_START) + 1)

#define MAP_MAIN_ADDR_START      (MAP_BOOTLDR_ADDR_END + 1)
#define MAP_MAIN_ADDR_END        (MAP_MAIN_ADDR_START + (MAP_MAIN_REGION_PAGES * FLASH_PAGE_SIZE) - 1)
#define MAP_MAIN_SIZE            ((MAP_MAIN_ADDR_END - MAP_MAIN_ADDR_START) + 1)

#define MAP_OTA_ADDR_START       (MAP_MAIN_ADDR_END + 1)
#define MAP_OTA_ADDR_END         (MAP_OTA_ADDR_START + (MAP_OTA_REGION_PAGES * FLASH_PAGE_SIZE) - 1)
#define MAP_OTA_SIZE             ((MAP_OTA_ADDR_END - MAP_OTA_ADDR_START) + 1)

//---------------------------------------------------------------------------------------------------------
//      16      |                         24                     |                 24                     |
//---------------------------------------------------------------------------------------------------------
//|  BOOTLOADER |                 APP_MAIN_REGION                |           APP_OTA_REGION               |
//---------------------------------------------------------------------------------------------------------


#endif //__MAPPING_CFG_H

