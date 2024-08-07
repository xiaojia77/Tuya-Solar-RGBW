/**
  ******************************************************************************
  * @file    main.c
  * @author  MCU Application Team
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) Puya Semiconductor Co.
  * All rights reserved.</center></h2>
  *
  * <h2><center>&copy; Copyright (c) 2016 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "time.h"
#include "usart.h"
#include "iwdg.h"
#include "gpio.h"
#include "led.h"
#include "debug.h"
#include "Ir.h"
#include "mcu_api.h"
#include "bluetooth.h"
#include "system.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
/* Private define ------------------------------------------------------------*/
#define  TAKS_IT_ON			SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk  	// 开启任务中断
#define  TAKS_IT_OFF		SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |SysTick_CTRL_ENABLE_Msk   	  							// 开启任务中断
	
/* Private variables ---------------------------------------------------------*/
/* Private user code ---------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
static void APP_SystemClockConfig(void);
static void APP_EnterStop(void)
{
  /* 使能PWR时钟 */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  /* 低功耗运行模式 */
  LL_PWR_SetLprMode(LL_PWR_LPR_MODE_LPR);
  /* SRAM电压跟数字LDO输出一致 */
  LL_PWR_SetStopModeSramVoltCtrl(LL_PWR_SRAM_RETENTION_VOLT_CTRL_LDO);
  /* 进入DeepSleep模式 */
  LL_LPM_EnableDeepSleep();
/*	
	等待中断指令    WFE模式 事件唤醒不进中断
  __SEV();
  __WFE();
  __WFE();
*/
  /* 等待中断指令 */
  __WFI();

  LL_LPM_EnableSleep();
}
int main(void)
{
	#ifdef DEBUG
//	uint32_t i =0;
	#endif
	uint32_t debugtick=0;
	uint32_t LEDtick=0;      //100ms
	uint32_t BatChecktick=0; //20ms
	uint32_t RGBApptick=0;   //5ms
	
	/* 开SYSCFG时钟 */
	LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_SYSCFG);
	/* 配置系统时钟 */
	APP_SystemClockConfig();
	/* 使能GPIO全部时钟	*/
	LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_ALL);
	

	LL_mDelay(500); //给jink复位用 
		
	LED_Init();         // LED初始化
	
	L1_OUTPUT;L2_OUTPUT;L3_INPUT; // L1
	L2_SET;L1_RESET;  
	LL_mDelay(150);
	L1_OUTPUT;L2_OUTPUT;L3_INPUT; // L2
	L1_SET;L2_RESET;  
	LL_mDelay(150);
	L2_OUTPUT;L3_OUTPUT;L1_INPUT; // L3
	L3_SET;L2_RESET;
	LL_mDelay(150);
	L2_OUTPUT;L3_OUTPUT;L1_INPUT; // L4 
	L2_SET;L3_RESET;  
	LL_mDelay(150);
	L1_OUTPUT;L3_OUTPUT;L2_INPUT; // L5
	L3_SET;L1_RESET; 
	LL_mDelay(150);
	L1_OUTPUT;L3_OUTPUT;L3_OUTPUT; // L5
	L1_RESET;L2_RESET;L3_RESET; 

	
	
	APP_ConfigUsart(USART1,115200);
	#if DEBUG
	LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_ALL);
	DEBUG_PRINTF("\r\n");
	DEBUG_INFO("-----------------------------------INIT START---------------------------------");
	DEBUG_INFO("----------------------------APP_SystemClock CONFIG----------------------------");
	DEBUG_INFO("HSI = 24MH");
	DEBUG_INFO("----------------------------RST_PORT CHECK----------------------------");

	#endif	

	if(READ_BIT(FLASH->OPTR, OB_USER_IWDG_STOP) != OB_IWDG_STOP_FREEZE )	//检测端口信息 如果复位脚没用则复用
	{	
		DEBUG_INFO("IWDG_STOP When Sleep");
		APP_FlashOBProgram(); //把复位脚复用成普通引脚
	}
	if(READ_BIT(FLASH->OPTR, OB_USER_SWD_NRST_MODE) != OB_SWD_PB6_GPIO_PC0 )	//检测端口信息  
	{	
		DEBUG_INFO("RST_PORT is reused as GPIO");
		APP_FlashOBProgram(); //把复位脚复用成普通引脚
	}
	else DEBUG_INFO("RST_PORT is used as GPIO");
	
	
	//GPIO_Init();	    // 引脚初始化
	
	Sys_Init();			//系统初始化
	
	APP_AdcConfig();    // ADC初始化
	
	Bat_GpioInit();     //Bat检测初始化
	
  Ir_Init();  	    // 红外初始化
	
	TY_Init();			// TY初始化
	
	LED_RGB_init();     // RGB初始化
	
	bt_protocol_init(); // 涂鸦蓝牙初始化
	
	Lptim_Init(); 		// 低功耗定时器唤醒
	
	LED_IndicatorOnFlag = 1; //指示灯开启
	
	Bat.ChargeUpFlag = 0;  //只降不升关闭
	Sys.LowVoltageFlag = 0;
	Bat.SolarMode = 1;	   //太阳能功能开启
	Bat.SaveEnergMode = 1; //节能开启
	
	RGB.W_Mode = 0; //默认RGBW模式	
	RGB.wflash = 1;	//闪烁关闭
	
	RGB.StepTime = 1000; 
	RGB.Powersaving_set_v = 1000;	

	RGB.CurrentGear = 20; //默认20档
	
	RGB.LastCommand = IR_WRITE_MODE; //默认白光

	Flash_Init_Rdata(); //获取之前的数据

	Ir_Power_ON();  //开启红外
	
	BLE_Power_ON(); //开启蓝牙
	
	LL_mDelay(50);
	
	APP_IwdgConfig();	// 看门狗
	
	enable_low_power();  //使能低功耗
	
	//  disable_low_power(); //不使能低功耗 TY发送心跳包
	
	Bat.ReCheckFlag = 1;
	Bat.ChargeUpFlag = 0;
	
	TAKS_IT_ON; 	       // 开启任务中断  


	while (1)
	{
		LL_IWDG_ReloadCounter(IWDG); 			// IWDG 清零
		
		bt_uart_service(); 			 //串口消息队列服务函数
		
		//#if DEBUG == 1
		if( HAL_GetTick() > debugtick)
		{		
		//	if( bt_work_state != 2)bt_uart_write_frame( 2, 0);
			debugtick = HAL_GetTick() + 1000;	
//			DEBUG_PRINTF("SysTick:%02d-%02d-%02d \r\n",debugtick/3600000%24,debugtick/60000%60,debugtick/1000%60);
//			DEBUG_INFO("Bat.Percent = %d%%  Bat.Voltage %dMV",Bat.Percent,Adc.BatVoltage);
//			DEBUG_INFO("bt_work_state = %d ",bt_work_state);
//			DEBUG_INFO("Sys.LowVoltageFlag = %d",Sys.LowVoltageFlag);
//			DEBUG_PRINTF("---------------------------------------Data Print---------------------------------------\r\n");
		}
		//#endif
		
		if( HAL_GetTick() - LEDtick  < UINT32_MAX / 2) //100ms
		{
			LEDtick = HAL_GetTick() + 100;
			LED_DisplayBatGear_Handle();		//LED指示灯显示
			BatVolatageToPercent_handle();
		}
		if( HAL_GetTick() - BatChecktick  < UINT32_MAX / 2 )
		{
			BatChecktick = HAL_GetTick() + 1;
			Bat_StatusCheck_Handle();			//电池状态检测
			Adc_BatVolatageCollection_handle(); //电量采集
		}
		if( HAL_GetTick() - RGBApptick  < UINT32_MAX / 2)
		{
			RGBApptick = HAL_GetTick() + 1;
			RGB_App_Handle();					//RGB灯 控制函数
			Ir_CommandReceiv(Ir_Scan());		//红外命令解析 函数
		}
		if(Sys.EnterSleepFlag)
		{
			Sys_EnterSleep_Handle();			//睡眠处理函数
		}
	}
}
static void APP_SystemClockConfig(void)
{
  /* 使能HSI */
	LL_RCC_HSI_Enable();
	while(LL_RCC_HSI_IsReady() != 1)
	{
	}

	/* 设置 AHB 分频*/
	LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);

	/* 配置HSISYS作为系统时钟源 */
	LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_HSISYS);
	while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_HSISYS)
	{
	}
	//LL_RCC_SetHSIDiv(LL_RCC_SYSCLK_DIV_1); // 24MHZ / 1 = 24MHZ
	LL_RCC_SetHSIDiv(LL_RCC_HSI_DIV_1); // 24MHZ / 4 = 6MHZ
	/* 设置 APB1 分频*/
	LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_1);
	//LL_Init1msTick(24000000);
	LL_Init1msTick(24000000);

	/* 更新系统时钟全局变量SystemCoreClock(也可以通过调用SystemCoreClockUpdate函数更新) */
	// LL_SetSystemCoreClock(24000000);
	LL_SetSystemCoreClock(24000000);
	
	SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
					 SysTick_CTRL_ENABLE_Msk   	;
						   
}
void APP_ErrorHandler(void)
{
  /* 无限循环 */
  while (1)
  {
	  
  }
}
#ifdef  USE_FULL_ASSERT
/**
  * @brief  输出产生断言错误的源文件名及行号
  * @param  file：源文件名指针
  * @param  line：发生断言错误的行号
  * @retval 无
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* 用户可以根据需要添加自己的打印信息,
     例如: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* 无限循环 */
  while (1)
  {
  }
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT Puya *****END OF FILE******************/
