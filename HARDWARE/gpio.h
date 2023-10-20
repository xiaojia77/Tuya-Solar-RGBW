#ifndef __GPIO_H
#define __GPIO_H

#include "main.h"
#include "py32f002b_ll_gpio.h"
#include "task.h"

//CDS 
#define GPIO_CDS_PORT GPIOA
#define GPIO_CDS_PIN LL_GPIO_PIN_6
#define CDS_EXTI_LINE	LL_EXTI_LINE_6
#define CDS_EXTI_CONFIG_PORT	LL_EXTI_CONFIG_PORTA
#define CDS_EXTI_CONFIG_LINE LL_EXTI_CONFIG_LINE6
#define CDS_EXTI_IRQn	EXTI4_15_IRQn
// MOS开关
#define GPIO_MOS_REAL_PORT GPIOA
#define GPIO_MOS_REAL_PIN  LL_GPIO_PIN_1
// 电测检测开关
#define GPIO_BAT_EN_PORT GPIOB
#define GPIO_BAT_EN_PIN  LL_GPIO_PIN_4
// WB_EN
#define GPIO_WB_EN_PORT GPIOB
#define GPIO_WB_EN_PIN LL_GPIO_PIN_3
// WB_PORT
#define GPIO_WB_PORT GPIOB
#define GPIO_WB_PIN LL_GPIO_PIN_2
#define WB_PORT LL_GPIO_IsInputPinSet(GPIO_WB_PORT,GPIO_WB_PIN)		

#define MOS_ON 0
#define MOS_OFF 1

extern void GPIO_Init(void);
extern void GPIO_CDS_EXIT_INIT(void);
extern void GPIO_CDS_ADC_INIT(void);
	
#endif
