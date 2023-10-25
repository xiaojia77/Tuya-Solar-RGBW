/**
  ******************************************************************************
  * @file    py32f002b_it.c
  * @author  MCU Application Team
  * @brief   Interrupt Service Routines.
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
#include "py32f002b_it.h"
#include "usart.h"
#include "time.h"
#include "adc.h"
#include "ir.h"
#include "adc.h"
#include "led.h"
#include "task.h"
#include "debug.h"
/* Private includes ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private user code ---------------------------------------------------------*/
/* External variables --------------------------------------------------------*/

/******************************************************************************/
/*           Cortex-M0+ Processor Interruption and Exception Handlers         */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  while (1)
  {
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)	//滴答定时器中断
{
	HAL_IncTick();
	LED_Scan_Handle();
	Sys_Timer1ms_Handle();
}
/******************************************************************************/
/* PY32F002B Peripheral Interrupt Handlers                                     */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file.                                          */
/******************************************************************************/
/******************************************************************************/
/* PY32F002B Peripheral Interrupt Handlers                                     */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file.                                          */
/******************************************************************************/
void USART1_IRQHandler(void)		//串口中断
{
	APP_UsartIRQCallback(USART1);
}
void TIM1_BRK_UP_TRG_COM_IRQHandler(void)	//TIM1 中断
{
	uint8_t i;
	LL_TIM_ClearFlag_UPDATE(TIM1);
	LL_TIM_DisableIT_UPDATE(TIM1);
	NVIC_DisableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
	for(i=0;i<30;i++)__NOP(); //避开采集到尖峰电压
	LL_GPIO_TogglePin(GPIOA,LL_GPIO_PIN_3);
	LL_ADC_REG_StartConversion(ADC1); //马上开始采集
}
void TIM14_IRQHandler(void)		//T1M14 中断
{
	LL_TIM_ClearFlag_UPDATE(TIM14);
	Ir_Time14IRQCallback();
}
void ADC_COMP_IRQHandler(void)	//ADC中断
{	
	APP_AdcGrpRegularUnitaryConvCompleteCallback();
	LL_ADC_ClearFlag_EOC(ADC1);
}
void LPTIM1_IRQHandler(void)
{
	LL_LPTIM_ClearFLAG_ARRM(LPTIM);
	Sys.LPTIMWakeUPFlag = 1;
}
void EXTI0_1_IRQHandler(void)
{

}
void EXTI4_15_IRQHandler(void)
{
	if(LL_EXTI_IsActiveFlag(RX_EXTI_LINE))
	{
		 LL_EXTI_ClearFlag(RX_EXTI_LINE);
		 Sys.UartMWakeUPFlag = 1;
	}
	if(LL_EXTI_IsActiveFlag(IR_EXTI_LINE))
	{
		LL_EXTI_ClearFlag(IR_EXTI_LINE);
		Sys.IrWakeUPFlag = 1;
		Ir_ExtiCallback();
	}
	
}
void EXTI2_3_IRQHandler(void)
{
	if(LL_EXTI_IsActiveFlag(BAT_CDS_EXTI_LINE))
	{
			
		LL_EXTI_ClearFlag(BAT_CDS_EXTI_LINE);
		Sys.ChargWakeUPFlag = 1;
	}
}
/************************ (C) COPYRIGHT Puya *****END OF FILE******************/
