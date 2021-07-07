#include "fw_info.h"

volatile const fw_info_t fw_info_var __attribute__((used)) __attribute__((section("fw_info"))) = 
{
  .magic = APP_FW_MAGIC  // init struct with magic num
};

