#ifndef __ir_H
#define __ir_H

#include "main.h"
#include "py32f002b_ll_exti.h"
#include "task.h"
#include "time.h"
#include "debug.h"

#define IR_EN_ENANBLE 1  //IR ʹ�ܿ���
//IR����
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
//�ж����ȼ�
#define Ir_Priority 1 
//������
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

// TO �滻�� PY32F002B T4�ļ�����
#define TIME_COUNT TIM14->CNT
#define IR_RX_PORT LL_GPIO_IsInputPinSet(GPIO_IR_PORT,GPIO_IR_PIN)						//ֻ��

typedef struct _Ir
{
	//���ݻ���
	uint32_t DataTemp;
	//�ظ����� ��λ���8-9ms
	uint16_t RepeatCount;
	//�������־λ
	uint8_t GuideFlag;
	//���ݽ�����ɱ�־λ
	uint8_t ReceiveDataFlag;
	//��ʱ��־λ ��û���ź������ˣ�
	uint8_t TimeOutFlag ;
	//��ʱʱ�� 
	uint8_t Timeout;
	//�ߵ�ƽʱ��  (�����ж��źŸߵ�)
	uint8_t HighLevelTime;
}Ir_s;
extern Ir_s Ir;

extern void Ir_ExtiCallback(void);		//�ⲿ�жϻص�
extern void Ir_Time14IRQCallback(void);	//��ʱ������ص�
extern void Ir_Init(void);				//�����ʼ��
extern uint8_t Ir_Scan(void);				//����ɨ��
extern void Ir_CommandReceiv(uint8_t key);
#endif
