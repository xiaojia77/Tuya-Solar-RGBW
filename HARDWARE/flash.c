
#include "flash.h"
#include "led.h"

uint32_t FlashData[6] = {0x10000000, 0x20000000, 0x30000000, 0x40000000, 0x50000000, 0x60000000};
static void APP_FlashErase(void)
{
  uint32_t PAGEError = 0;
  FLASH_EraseInitTypeDef EraseInitStruct;

  EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGEERASE;         /* 擦写类型FLASH_TYPEERASE_PAGEERASE=Page擦, FLASH_TYPEERASE_SECTORERASE=Sector擦 */
  EraseInitStruct.PageAddress = FLASH_USER_START_ADDR;           /* 擦写起始地址 */
                                                                 // EraseInitStruct.NbPages  = sizeof(FlashData) / FLASH_PAGE_SIZE;                                   /* 需要擦写的页数量 */
  EraseInitStruct.NbPages = 1;                                   /* 需要擦写的页数量 */
  if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK) /* 执行page擦除,PAGEError返回擦写错误的page,返回0xFFFFFFFF,表示擦写成功 */
  {
    DEBUG_INFO("HAL_FLASHEx_Erase fail");
    APP_ErrorHandler();
  }
}
/**
 * @brief  写FLASH
 * @param  无
 * @retval 无
 */
static void APP_FlashProgram(void)
{
  uint32_t flash_program_start = FLASH_USER_START_ADDR;                     /* flash写起始地址 */
  uint32_t flash_program_end = (FLASH_USER_START_ADDR + sizeof(FlashData)); /* flash写结束地址 */
  uint32_t *src = (uint32_t *)FlashData;                                    /* 数组指针 */

  while (flash_program_start < flash_program_end)
  {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_PAGE, flash_program_start, src) == HAL_OK) /* 执行Program */
    {
      flash_program_start += FLASH_PAGE_SIZE; /* flash起始指针指向第一个page */
      src += FLASH_PAGE_SIZE / 4;             /* 更新数据指针 */
    }
  }
}
/**
 * @brief  查空FLASH
 * @param  无
 * @retval 无
 */
static void APP_FlashBlank(void)
{
  uint32_t addr = 0;

  while (addr < sizeof(FlashData))
  {
    if (0xFFFFFFFF != HW32_REG(FLASH_USER_START_ADDR + addr))
    {
      DEBUG_INFO("APP_FlashBlank fail");
      APP_ErrorHandler();
    }
    addr += 4;
  }
}
/**
 * @brief  校验FLASH
 * @param  无
 * @retval 无
 */
static void APP_FlashVerify(void)
{
  uint32_t addr = 0;

  while (addr < sizeof(FlashData))
  {
    if (FlashData[addr / 4] != HW32_REG(FLASH_USER_START_ADDR + addr))
    {
      DEBUG_INFO("APP_FlashVerify fail");
      APP_ErrorHandler();
    }
    addr += 4;
  }
}

void Flash_Init_Rdata()
{
    FlashData[SET_H] = HW32_REG(FLASH_USER_START_ADDR + 0);
    FlashData[SET_S] = HW32_REG(FLASH_USER_START_ADDR + 4);
    FlashData[SET_V] = HW32_REG(FLASH_USER_START_ADDR + 8);
    FlashData[SET_W_MODE] = HW32_REG(FLASH_USER_START_ADDR + 12);
    FlashData[SET_RGB_MODE] = HW32_REG(FLASH_USER_START_ADDR + 16);
    FlashData[SET_CurrentGear] = HW32_REG(FLASH_USER_START_ADDR + 20);
    DEBUG_INFO("----------------------------Rread FLASH Data----------------------------");
    DEBUG_INFO("FLASH Data[SET_H] = %d", FlashData[SET_H]);
    DEBUG_INFO("FLASH Data[SET_S] = %d", FlashData[SET_S]);
    DEBUG_INFO("FLASH Data[SET_DSET_VLDW] = %d", FlashData[SET_V]);
    DEBUG_INFO("FLASH Data[SET_W_MODE] = %d", FlashData[SET_W_MODE]);
    DEBUG_INFO("FLASH Data[SET_RGB_MODE] = %d", FlashData[SET_RGB_MODE]);
    DEBUG_INFO("FLASH Data[SET_CurrentGear] = %d", FlashData[SET_CurrentGear]);
    RGB.h = FlashData[SET_H];
    if (RGB.h > 360)
    {
      RGB.h = 0;
    }
    RGB.s = FlashData[SET_S];
    if (RGB.s > 1000)
    {
      RGB.s = 0;
    }
    RGB.vTemp = FlashData[SET_V];
    if (RGB.vTemp > 1000)
    {
      RGB.vTemp = 1000;
    }
    RGB.W_Mode = FlashData[SET_W_MODE];
    if (RGB.W_Mode > 1)
    {
      RGB.W_Mode = 1;
    }
    RGB.LastCommand = FlashData[SET_RGB_MODE];
    if (RGB.LastCommand > 21)
    {
      RGB.LastCommand = IR_WRITE_MODE;
    }
    RGB.CurrentGear = FlashData[SET_CurrentGear];
    if (RGB.CurrentGear > 20)
    {
      RGB.CurrentGear = 20;
    }
}

void Flash_Data_Write()
{
    if (RGB.Command != IR_RGB_MODE && RGB.Command != TY_MUSIC_MODE)
    {
        if (FlashData[SET_H] != RGB.h)
        {
          Flash_Write();
          DEBUG_INFO("%d  %d", RGB.h, FlashData[SET_H]);
        }
        if (FlashData[SET_S] != RGB.s)
        {
          Flash_Write();
        }
    }
    if (RGB.Command != TY_MUSIC_MODE)
    {
        if (FlashData[SET_V] != RGB.vTemp)
        {
          Flash_Write();
        }
    }
    if (FlashData[SET_W_MODE] != RGB.W_Mode)
    {
        Flash_Write();
    }
    if (FlashData[SET_RGB_MODE] != RGB.LastCommand)
    {
        Flash_Write();
    }
    if (FlashData[SET_CurrentGear] != RGB.CurrentGear)
    {
        Flash_Write();
    }
}

void Flash_Write()
{
    FlashData[SET_H] = RGB.h;
    FlashData[SET_S] = RGB.s;
    FlashData[SET_V] = RGB.vTemp;
    FlashData[SET_W_MODE] = RGB.W_Mode;
    FlashData[SET_RGB_MODE] = RGB.LastCommand;
    FlashData[SET_CurrentGear] = RGB.CurrentGear;

    /*解锁FLASH*/
    HAL_FLASH_Unlock();

    /*擦除FLASH*/
    APP_FlashErase();

    /*查空FLASH*/
    APP_FlashBlank();

    /*写FLASH*/
    APP_FlashProgram();

    /*锁定FLASH*/
    HAL_FLASH_Lock();

    /*校验FLASH*/
    APP_FlashVerify();

    DEBUG_INFO("Flash_Write succeed");
    DEBUG_INFO("FLASH Data[SET_H] = %d", HW32_REG(FLASH_USER_START_ADDR + 0));
    DEBUG_INFO("FLASH Data[SET_S] = %d", HW32_REG(FLASH_USER_START_ADDR + 4));
    DEBUG_INFO("FLASH Data[SET_DSET_VLDW] = %d", HW32_REG(FLASH_USER_START_ADDR + 8));
    DEBUG_INFO("FLASH Data[SET_W_MODE] = %d", HW32_REG(FLASH_USER_START_ADDR + 12));
    DEBUG_INFO("FLASH Data[SET_RGB_MODE] = %d", HW32_REG(FLASH_USER_START_ADDR + 16));

    DEBUG_INFO("FLASH Data[SET_H] = %d", FlashData[SET_H]);
    DEBUG_INFO("FLASH Data[SET_S] = %d", FlashData[SET_S]);
    DEBUG_INFO("FLASH Data[SET_DSET_VLDW] = %d", FlashData[SET_V]);
    DEBUG_INFO("FLASH Data[SET_W_MODE] = %d", FlashData[SET_W_MODE]);
    DEBUG_INFO("FLASH Data[SET_RGB_MODE] = %d", FlashData[SET_RGB_MODE]);
    DEBUG_INFO("FLASH Data[SET_CurrentGear] = %d", FlashData[SET_CurrentGear]);
}

/**
 * @brief  写option空间函数
 * @param  无
 * @retval 无
 */
void APP_FlashOBProgram(void)
{
  FLASH_OBProgramInitTypeDef OBInitCfg;

  HAL_FLASH_Unlock();    /* 解锁FLASH */
  HAL_FLASH_OB_Unlock(); /* 解锁OPTION */

  OBInitCfg.OptionType = OPTIONBYTE_USER;
  OBInitCfg.USERType = OB_USER_BOR_EN | OB_USER_BOR_LEV | OB_USER_IWDG_SW | OB_USER_IWDG_STOP | OB_USER_SWD_NRST_MODE;

  OBInitCfg.USERConfig = OB_BOR_ENABLE | OB_BOR_LEVEL_1p9_2p0 | OB_IWDG_SW | OB_IWDG_STOP_FREEZE | MODE_PC0 ;

  /* 启动option byte编程 */
  HAL_FLASH_OBProgram(&OBInitCfg);

  HAL_FLASH_Lock();    /* 锁定FLASH */
  HAL_FLASH_OB_Lock(); /* 锁定OPTION */

  /* 产生一个复位，option byte装载 */
  HAL_FLASH_OB_Launch();
}
