
#include "flash.h"

uint32_t FlashData[4]={0x10000000,0x20000000,0x30000000,0x40000000};
static void APP_FlashErase(void)
{
  uint32_t PAGEError = 0;
  FLASH_EraseInitTypeDef EraseInitStruct;

  EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGEERASE;                                     /* 擦写类型FLASH_TYPEERASE_PAGEERASE=Page擦, FLASH_TYPEERASE_SECTORERASE=Sector擦 */
  EraseInitStruct.PageAddress = FLASH_USER_START_ADDR;                                         /* 擦写起始地址 */
 // EraseInitStruct.NbPages  = sizeof(FlashData) / FLASH_PAGE_SIZE;                                   /* 需要擦写的页数量 */
  EraseInitStruct.NbPages  = 1;                                   /* 需要擦写的页数量 */
  if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)                               /* 执行page擦除,PAGEError返回擦写错误的page,返回0xFFFFFFFF,表示擦写成功 */
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
  uint32_t flash_program_start = FLASH_USER_START_ADDR ;                                /* flash写起始地址 */
  uint32_t flash_program_end = (FLASH_USER_START_ADDR + sizeof(FlashData));                  /* flash写结束地址 */
  uint32_t *src = (uint32_t *)FlashData;                                                     /* 数组指针 */

  while (flash_program_start < flash_program_end)
  {
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_PAGE, flash_program_start, src) == HAL_OK)  /* 执行Program */
    {
      flash_program_start += FLASH_PAGE_SIZE;                                           /* flash起始指针指向第一个page */
      src += FLASH_PAGE_SIZE / 4;                                                       /* 更新数据指针 */
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

void Flash_Write()
{
	 LL_ADC_Disable(ADC1);
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
	
	LL_ADC_Enable(ADC1);

	LL_ADC_REG_StartConversion(ADC1);
	
	DEBUG_INFO("Flash_Write succeed");
	DEBUG_INFO("FLASH Data[SET_DELAY] = %d",  HW32_REG(FLASH_USER_START_ADDR + 0));
	DEBUG_INFO("FLASH Data[SET_PWM] = %d",  HW32_REG(FLASH_USER_START_ADDR + 4));
	DEBUG_INFO("FLASH Data[SET_DLDW] = %d",  HW32_REG(FLASH_USER_START_ADDR + 8));
	DEBUG_INFO("FLASH Data[SET_WBMODE] = %d",  HW32_REG(FLASH_USER_START_ADDR + 12));
}
/**
  * @brief  写option空间函数
  * @param  无
  * @retval 无
  */
void APP_FlashOBProgram(void)
{
  FLASH_OBProgramInitTypeDef OBInitCfg;

  HAL_FLASH_Unlock();        /* 解锁FLASH */
  HAL_FLASH_OB_Unlock();     /* 解锁OPTION */
  
  OBInitCfg.OptionType = OPTIONBYTE_USER;
  OBInitCfg.USERType = OB_USER_BOR_EN | OB_USER_BOR_LEV | OB_USER_IWDG_SW | OB_USER_IWDG_STOP | OB_USER_SWD_NRST_MODE;

  OBInitCfg.USERConfig = OB_BOR_DISABLE | OB_BOR_LEVEL_3p1_3p2 | OB_IWDG_SW | OB_IWDG_STOP_ACTIVE | MODE_PC0 ;

  /* 启动option byte编程 */
  HAL_FLASH_OBProgram(&OBInitCfg);

  HAL_FLASH_Lock();      /* 锁定FLASH */
  HAL_FLASH_OB_Lock();   /* 锁定OPTION */

  /* 产生一个复位，option byte装载 */
  HAL_FLASH_OB_Launch();
}
