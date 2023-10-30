#ifndef __TASK_H
#define __TASK_H

#include "main.h"
#include "adc.h"
#include "flash.h"
#include "ir.h"
#include "gpio.h"
#define unchar volatile unsigned char
#define unint volatile unsigned int
#define unlong volatile unsigned long
	
#define GPIO_TY_EN_PORT GPIOB	 // 涂鸦供电
#define GPIO_TY_EN_PIN LL_GPIO_PIN_7

#define TY_ENANBLE    LL_GPIO_SetOutputPin(GPIO_TY_EN_PORT,GPIO_TY_EN_PIN);
#define TY_DISANBLE 	LL_GPIO_ResetOutputPin(GPIO_TY_EN_PORT,GPIO_TY_EN_PIN);

#define GPIO_TY_RST_PORT GPIOA  // 涂鸦复位
#define GPIO_TY_RST_PIN LL_GPIO_PIN_1

#define TY_RST_1  LL_GPIO_SetOutputPin(GPIO_TY_RST_PORT,GPIO_TY_RST_PIN);
#define TY_RST_0  LL_GPIO_ResetOutputPin(GPIO_TY_RST_PORT,GPIO_TY_RST_PIN);


typedef struct _xSys
{
	uint8_t EnterSleepFlag;
	uint8_t LowVoltageFlag;
	
	uint16_t EntreSleepTimeCount;  
	uint16_t SleepTimeCount;  
	
	uint8_t IrWakeUPFlag;		
	uint8_t ChargWakeUPFlag;
	uint8_t LPTIMWakeUPFlag;
	uint8_t UartMWakeUPFlag;
}xSys;

typedef struct _TY
{
	uint16_t bright;
	uint8_t WorkMode;
	uint8_t Scene;
	uint16_t TempValue;
	uint8_t GroupSync;
	uint8_t ColourData[12];  //12
	uint8_t SceneData[2];   // 2
}xTY_t;

extern xSys Sys;

extern xTY_t TY; 


extern void Sys_EnterSleep_Handle(void);
extern void Sys_EntreSleep(void);
extern void Sys_Timer1ms_Handle(void);
extern void Sys_Init(void);

extern void TY_Init(void);
extern void TY_Updata_Bright(void);

extern void Ir_Power_OFF(void);
extern void Ir_Power_ON(void);

extern void BLE_Power_OFF(void);
extern void BLE_Power_ON(void);


#endif
