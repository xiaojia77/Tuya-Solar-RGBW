#ifndef __LED_H
#define __LED_H

#include "main.h"
#include "task.h"
#include "py32f002b_ll_gpio.h"
#include "mcu_api.h"
#include "protocol.h"

#define GPIO_L3_PORT GPIOA
#define GPIO_L3_PIN LL_GPIO_PIN_7

#define GPIO_L2_PORT GPIOC
#define GPIO_L2_PIN LL_GPIO_PIN_0

#define GPIO_L1_PORT GPIOC
#define GPIO_L1_PIN LL_GPIO_PIN_1

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

//#define RGB_PWM 2400
//#define RGB_Light_MAX 2200//最大亮度 PWM
//#define RGB_Light_MIN 240//最小亮度 PWM

#define RGB_PWM 6000
#define RGB_Light_MAX 6000 * 0.95//最大亮度 PWM
#define RGB_Light_MIN 6000 * 0.1//最小亮度 PWM

#define TIME_10MIN     10 * 60000 /5
#define TIME_1H   1  * 60 * 60000 /5
#define TIME_3H   5  * 60 * 60000 /5 
#define TIME_5H   5  * 60 * 60000 /5 
#define TIME_11H  11 * 60 * 60000 /5
#define	TIME_18H  18 * 60 * 60000 /5

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
	
	//TY_CONNECT_MODE, //呼吸灯模式
	
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
	volatile uint8_t OnFlag;

	volatile uint8_t W_Mode; //白光模式  1 RGBW  RGB模拟
	
	volatile uint8_t ResetCnt; //复位计数
	volatile uint16_t ResetTime; 
	
	//HSV色彩空间
	volatile uint16_t h; //色相 0 -360
	volatile uint16_t s; //饱和 0 - 1000
	volatile uint16_t v; //明度 0 - 1000
	volatile uint16_t Setv; //明度 0 - 1000  用来保存亮度的数据
	//W 白光
	volatile uint16_t w;
	volatile uint16_t wTemp; //明度 0 - 1000  用来保存亮度的数据	
	volatile uint16_t wflash;
	
	//RGB色彩空间
	uint8_t Rvalue; 
	uint8_t Gvalue;
	uint8_t Bvalue;
	uint16_t Wvalue;  //白光PWM
	
	//实际的PWM值
	volatile uint16_t Rpwm;
	volatile uint16_t Gpwm;
	volatile uint16_t Bpwm;
	volatile uint16_t Wpwm;  //白光PWM
	//显示缓存
	volatile uint16_t Dispaly_h;
	volatile uint16_t Dispaly_s;
	volatile uint16_t Dispaly_v;
	volatile uint16_t Dispaly_w;
	//音乐律动步进
/*	int8_t Music_inc_h;
	int8_t Music_inc_s;
	int8_t Music_inc_v;*/

	volatile uint8_t Command; 	 //指令
	volatile uint8_t LastCommand; //上一指令
	
	volatile uint32_t Time;		 //打开的时间
	volatile uint32_t SetOffTime; //设置关闭的时间 3H 5H
 	
	//降序用
	volatile uint32_t StepTime; //步进时间 	
	volatile uint32_t StepTimecnt; //步进时间计数
	volatile uint16_t Powersaving_set_v; //要去的亮度
	
	volatile uint8_t CurrentGear;
	
	//降序时间
	volatile uint16_t ColourSwTimer;  // LED闪烁计数器
	
}xRGB;

extern xRGB RGB;

extern uint8_t LED_DisplayTemp; 		// 0 全灭  1-L1 2-L2 3-L3 4-L4 5-L5
extern uint8_t LED_IndicatorOnFlag; 	//指示灯开启 标志位

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
extern void LED_RGB_W_display(uint16_t w);
extern void RGB_App_Handle(void);


#endif
