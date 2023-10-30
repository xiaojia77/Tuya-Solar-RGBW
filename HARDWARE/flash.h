#ifndef __FLASH_H
#define __FLASH_H


#include "main.h"
#include "task.h"
#include "py32f002b_hal_flash.h"

#define SET_H 0
#define SET_S 1
#define SET_V 2
#define SET_W_MODE 3
#define SET_RGB_MODE 4

#define MODE_PC0 OB_SWD_PB6_GPIO_PC0



/* Private define ------------------------------------------------------------*/
#define FLASH_USER_START_ADDR     0x08005000
/* Private variables ---------------------------------------------------------*/
typedef struct
{
  uint32_t arrA[4];
} NewDataType;                                                      /* 结构体定义 */
#define VarA (*(volatile NewDataType *)FLASH_USER_START_ADDR)       /* Flash存储地址定义 */

extern uint32_t FlashData[5];

extern void Flash_Write(void);

extern void APP_FlashOBProgram(void);

extern void Flash_Init_Rdata(void);

extern void Flash_Data_Write(void);

#endif
