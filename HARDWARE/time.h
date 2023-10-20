#ifndef __TIME_H
#define __TIME_H

#include "main.h"
#include "task.h"
#include "py32f002b_ll_tim.h"

extern uint32_t time1_count;
extern uint32_t time4_count;

extern void Lptim_Init(void);
extern void APP_ConfigTIM1(void); // 1ms
extern void APP_ConfigTIM4(void);
extern void APP_Time1IRQCallback(void);
extern void APP_Time4IRQCallback(void);	//TIM
extern void APP_ConfigPWMChannel(void); //pwm初始化
#endif
