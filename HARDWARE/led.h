#ifndef __LED_H
#define __LED_H

#include "main.h"
#include "task.h"
#include "py32f002b_ll_gpio.h"
#include "mcu_api.h"
#include "protocol.h"

#define GPIO_L1_PORT GPIOB
#define GPIO_L1_PIN LL_GPIO_PIN_7

#define GPIO_L2_PORT GPIOC
#define GPIO_L2_PIN LL_GPIO_PIN_1

#define GPIO_L3_PORT GPIOA
#define GPIO_L3_PIN LL_GPIO_PIN_7

#define L1_SET		LL_GPIO_SetOutputPin(GPIO_L1_PORT,GPIO_L1_PIN);
#define L1_RESET	LL_GPIO_ResetOutputPin(GPIO_L1_PORT,GPIO_L1_PIN);

#define L2_SET		LL_GPIO_SetOutputPin(GPIO_L2_PORT,GPIO_L2_PIN);
#define L2_RESET	LL_GPIO_ResetOutputPin(GPIO_L2_PORT,GPIO_L2_PIN);

#define L3_SET		LL_GPIO_SetOutputPin(GPIO_L3_PORT,GPIO_L3_PIN);
#define L3_RESET	LL_GPIO_ResetOutputPin(GPIO_L3_PORT,GPIO_L3_PIN);

#define L1_OUTPUT   LL_GPIO_SetPinMode(GPIO_L1_PORT, GPIO_L1_PIN, LL_GPIO_MODE_OUTPUT);
#define L2_OUTPUT   LL_GPIO_SetPinMode(GPIO_L2_PORT, GPIO_L2_PIN, LL_GPIO_MODE_OUTPUT);
#define L3_OUTPUT   LL_GPIO_SetPinMode(GPIO_L3_PORT, GPIO_L3_PIN, LL_GPIO_MODE_OUTPUT);


#define L1_INPUT   LL_GPIO_SetPinMode(GPIO_L1_PORT, GPIO_L1_PIN, LL_GPIO_MODE_INPUT);
#define L2_INPUT   LL_GPIO_SetPinMode(GPIO_L2_PORT, GPIO_L2_PIN, LL_GPIO_MODE_INPUT);
#define L3_INPUT   LL_GPIO_SetPinMode(GPIO_L3_PORT, GPIO_L3_PIN, LL_GPIO_MODE_INPUT);

#define RGB_PWM 2400
#define RGB_Light_MAX 2200//������� PWM
#define RGB_Light_MIN 240//��С���� PWM

#define TIME_10MIN 10 * 60000 /5
#define TIME_1H   60 * 60000  /5
#define TIME_5H  5*60 * 60000 /5 
#define TIME_11H 11*60 * 60000 /5
#define	TIME_18H 18*60 * 60000 /5

enum RGB_Mode
{
	IR_POWEROFF = 0,
	IR_LEDON,
	IR_LEDOFF,
	IR_LIAGHUP,
	IR_LIAGHDONW,
	IR_3H,
	IR_5H,
	IR_WRITE_MODE,
	IR_RGB_MODE,
	IR_RED_MODE,
	IR_GREEN_MODE,
	IR_BLUE_MODE,
	
	//TY_CONNECT_MODE, //������ģʽ
	
	TY_RGB_MODE,
	TY_colour_MODE,
	TY_bright_MODE,
	TY_temp_MODE,
	TY_scene_MODE,
	TY_MUSIC_MODE,
	TY_ON_MODE,
	TY_OFF_MODE,
	
	TY_Reset_MOde,
};

typedef struct _xRGB
{
	uint8_t OnFlag;
	
	uint8_t ResetCnt; //��λ����
	uint16_t ResetTime; 
	
	//HSVɫ�ʿռ�
	uint16_t h; //ɫ�� 0 -360
	uint16_t s; //���� 0 - 1000
	uint16_t v; //���� 0 - 1000
	uint16_t vTemp; //���� 0 - 1000  �����������ȵ�����
		
	//RGBɫ�ʿռ�
	uint8_t Rvalue; 
	uint8_t Gvalue;
	uint8_t Bvalue;
	
	//ʵ�ʵ�PWMֵ
	uint16_t Rpwm;
	uint16_t Gpwm;
	uint16_t Bpwm;
	
	//��ʾ����
	uint16_t Dispaly_h;
	uint16_t Dispaly_s;
	uint16_t Dispaly_v;
		
	//�����ɶ�����
/*	int8_t Music_inc_h;
	int8_t Music_inc_s;
	int8_t Music_inc_v;*/
	
	uint8_t Command; 	 //ָ��
	uint8_t LastCommand; //��һָ��
	
	uint32_t Time;		 //�򿪵�ʱ��
	uint32_t SetOffTime; //���ùرյ�ʱ�� 3H 5H
 	
	//������
	uint32_t StepTime; //����ʱ�� 	
	uint32_t StepTimecnt; //����ʱ�����
	uint16_t Powersaving_set_v; //Ҫȥ������
	
}xRGB;

extern xRGB RGB;

extern uint8_t LED_DisplayTemp; // 0 ȫ��  1-L1 2-L2 3-L3 4-L4 5-L5

extern void LED_Init(void); 
extern void LED_Scan_Handle(void);
extern void Led_Off(void);
extern void LED_DisplayBatGear_Handle(void);

extern void LED_RGB_init(void);
extern void LED_RGB_To_OnStatus(void);
extern void LED_RGB_On_Handle(void);
extern void LED_RGB_Off_Handle(void);
extern void LED_RGB_SetHSV(uint16_t h, uint16_t s, uint16_t v);
extern void LED_RGB_SetDisplayHSV(uint16_t h, uint16_t s, uint16_t v);
extern void led_strip_hsv2rgb(uint16_t h, uint16_t s, uint16_t v, uint8_t *r, uint8_t *g, uint8_t *b);
extern void LED_RGB_SetDisplayHSV(uint16_t h, uint16_t s, uint16_t v);
extern void LED_RGB_display(uint8_t r,uint8_t g,uint8_t b);
extern void LED_RGB_HSVdisplay(uint16_t h, uint16_t s, uint16_t v);
extern void RGB_App_Handle(void);


#endif
