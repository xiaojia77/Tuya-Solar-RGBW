#ifndef __ADC_H
#define __ADC_H

#include "main.h"
#include "py32f002b_ll_adc.h"
#include "usart.h"
#include "task.h"

#define BAT_CDS_PORT GPIOB
#define BAT_CDS_PIN LL_GPIO_PIN_3
#define BAT_CDS_RX LL_GPIO_IsInputPinSet(BAT_CDS_PORT,BAT_CDS_PIN)	//只读


#define BAT_CDS_EXTI_LINE	LL_EXTI_LINE_3
#define BAT_CDS_EXTI_CONFIG_PORT	LL_EXTI_CONFIG_PORTB
#define BAT_CDS_EXTI_CONFIG_LINE LL_EXTI_CONFIG_LINE3
#define BAT_CDS_EXTI_IRQn	EXTI2_3_IRQn

#define BAT_Protect_Voltage 2650 // 低压保护电压

typedef struct _xADC
{
	//累加值
	volatile uint16_t Temp; 
	volatile uint16_t TempMin;
	volatile uint16_t TempMax;
	//累加变量
	volatile uint16_t TempCnt;
	//电池电压
	volatile uint16_t BatVoltage;
	
	volatile uint16_t NewBatVoltage;
}xADC;

#define BAT_DISCHARGE 0 
#define BAT_CHARGE 1
#define BAT_FULL 2
typedef struct _xBat
{
	//挡位
	volatile uint8_t Gera;  // 0 - 5
	//百分比
	volatile int16_t Percent;  // 0 - 100  int只是方便计算
	//状态
	volatile uint8_t Status;  // 0 放电 1 充电 2 充满 3 低压保护
	//只降不升标志位
	volatile uint8_t ChargeUpFlag; //只升不降 只降不升开始
	//太阳能模式
	volatile uint8_t SolarMode; //太阳能模式 光控开灯 
	//节能模式
	volatile uint8_t SaveEnergMode; //降序 自动调节亮度
	
	volatile uint8_t ReCheckFlag;
	
}xBat;

extern xADC Adc;
extern xBat Bat;

extern __IO uint16_t uhADCxConvertedData_Voltage_mVolt;	//读取到电压值
extern void Bat_GpioInit(void); // bat_cds初始化
extern void APP_AdcConfig(void);  	//ADC 配置
extern void APP_AdcCalibrate(void); //ADC 校准
extern void APP_AdcEnable(void);		//ADC 开启
extern void APP_AdcGrpRegularUnitaryConvCompleteCallback(void);	//ADC中断回调函数
extern void Adc_BatVolatageCollection_handle(void);
extern void Adc_ReScan(void);
extern void BatVolatageToPercent_handle(void);
extern void Bat_StatusCheck_Handle(void);
#endif
