#ifndef __ir_H
#define __ir_H

#include "main.h"
#include "py32f002b_ll_exti.h"
#include "task.h"
#include "time.h"
#include "debug.h"

#define IR_EN_ENANBLE 1  //IR 使能控制
//IR命令
#define  IR_COMMAND_POWEROFF 0XA2
#define  IR_COMMAND_LEDON 0X62
#define  IR_COMMAND_LEDOFF 0x22
#define  IR_COMMAND_LIAGHUP 0xC2
#define  IR_COMMAND_LIAGHDONW 0xB0
#define  IR_COMMAND_3H 0xe0
#define  IR_COMMAND_5H 0x90
#define  IR_COMMAND_WRITE_MODE 0x10 
#define  IR_COMMAND_RGB_MODE  0x98
#define  IR_COMMAND_RED_MODE 0xA8
#define  IR_COMMAND_GREEN_MODE 0x68
#define  IR_COMMAND_BLUE_MODE 0X18
//中断优先级
#define Ir_Priority 1 
//引导码
#define Ir_GUIDE 0x01


#define  IR_DATA0_TH_H        19 
#define  IR_DATA0_TH_L        2
#define  IR_DATA1_TH_H        28 
#define  IR_DATA1_TH_L        21
#define  IR_GUID_TH_H         73 
#define  IR_GUID_TH_L         65
#define  IR_REDATA_TH_H       48
#define  IR_REDATA_TH_L       26

#define GPIO_IR_PORT GPIOA
#define GPIO_IR_PIN LL_GPIO_PIN_5
#define IR_EXTI_LINE	LL_EXTI_LINE_5
#define IR_EXTI_CONFIG_PORT	LL_EXTI_CONFIG_PORTA
#define IR_EXTI_CONFIG_LINE LL_EXTI_CONFIG_LINE5
#define IR_EXTI_IRQn	EXTI4_15_IRQn

#if IR_EN_ENANBLE == 1
#define GPIO_IR_EN_PORT GPIOA
#define GPIO_IR_EN_PIN LL_GPIO_PIN_6
#define IR_ENANBLE   LL_GPIO_SetOutputPin(GPIO_IR_EN_PORT,GPIO_IR_EN_PIN);
#define IR_DISANBLE	 LL_GPIO_ResetOutputPin(GPIO_IR_EN_PORT,GPIO_IR_EN_PIN);
#else 
#define IR_ENANBLE
#define IR_DISANBLE
#endif 

// TO 替换成 PY32F002B T4的计数器
#define TIME_COUNT TIM14->CNT
#define IR_RX_PORT LL_GPIO_IsInputPinSet(GPIO_IR_PORT,GPIO_IR_PIN)						//只读

typedef struct _Ir
{
	//数据缓存
	uint32_t DataTemp;
	//重复计数 单位大概8-9ms
	uint16_t RepeatCount;
	//引导码标志位
	uint8_t GuideFlag;
	//数据接收完成标志位
	uint8_t ReceiveDataFlag;
	//超时标志位 （没有信号输入了）
	uint8_t TimeOutFlag ;
	//超时时间 
	uint8_t Timeout;
	//高电平时间  (用来判断信号高低)
	uint8_t HighLevelTime;
}Ir_s;
extern Ir_s Ir;

extern void Ir_ExtiCallback(void);		//外部中断回调
extern void Ir_Time14IRQCallback(void);	//定时器溢出回调
extern void Ir_Init(void);				//红外初始化
extern uint8_t Ir_Scan(void);				//红外扫描
extern void Ir_CommandReceiv(uint8_t key);
#endif
