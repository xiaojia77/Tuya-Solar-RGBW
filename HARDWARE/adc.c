
#include "adc.h"
#include "mcu_api.h"
#include "led.h"
#include "protocol.h"

xADC Adc;
xBat Bat;

static uint8_t percent_temp[10]={0,0,0,0,0,0,0,0,0,0};
static uint8_t last_percent = 0,current_percent = 0 , gear_changetemp = 0;
/* Private define ------------------------------------------------------------*/
#define ADC_CALIBRATION_TIMEOUT_MS       ((uint32_t) 1)
#define VDDA_APPLI                       ((uint32_t)3300)
#define VAR_CONVERTED_DATA_INIT_VALUE    (__LL_ADC_DIGITAL_SCALE(LL_ADC_RESOLUTION_12B) + 1)

/* Private variables ---------------------------------------------------------*/
//__IO uint16_t uhADCxConvertedData = VAR_CONVERTED_DATA_INIT_VALUE;
//__IO uint16_t uhADCxConvertedData_Voltage_mVolt = 0;

void Adc_ReScan(void)
{
	Adc.TempCnt = 0;
	Adc.Temp = 0;
	Adc.TempMax = 0X0000;
	Adc.TempMin = 0XFFFF;
	Adc.TempCnt = 0;
}
void Bat_GpioInit(void)
{
	LL_GPIO_InitTypeDef GPIO_InitStruct={0};
	// 配置PA0输入模式 
	GPIO_InitStruct.Pin = BAT_CDS_PIN;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(BAT_CDS_PORT, &GPIO_InitStruct);
	
	// 选择EXTI0做外部中断输入 
	LL_EXTI_SetEXTISource(BAT_CDS_EXTI_CONFIG_PORT, BAT_CDS_EXTI_CONFIG_LINE);

	// 配置EXTI0为中断、上下降沿触发 
	LL_EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.Line = BAT_CDS_EXTI_LINE;
	EXTI_InitStruct.LineCommand = ENABLE;
	EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
	EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING_FALLING;
	LL_EXTI_Init(&EXTI_InitStruct);

	/* 使能中断 */

	NVIC_SetPriority(BAT_CDS_EXTI_IRQn, 2);
	NVIC_EnableIRQ(BAT_CDS_EXTI_IRQn);
	
}
void APP_AdcCalibrate(void)
{
  __IO uint32_t wait_loop_index = 0;
#if (USE_TIMEOUT == 1)
  uint32_t Timeout = 0;
#endif

  if (LL_ADC_IsEnabled(ADC1) == 0)
  {
    
    /* 使能校准 */
    LL_ADC_StartCalibration(ADC1);

#if (USE_TIMEOUT == 1)
    Timeout = ADC_CALIBRATION_TIMEOUT_MS;
#endif

    while (LL_ADC_IsCalibrationOnGoing(ADC1) != 0)
    {
#if (USE_TIMEOUT == 1)
      /* 检测校准是否超时 */
      if (LL_SYSTICK_IsActiveCounterFlag())
      {
        if(Timeout-- == 0)
        {

        }
      }
#endif
    }

    /* ADC校准结束和使能ADC之间的延时最低4个ADC Clock */
    LL_mDelay(2);
  }
}
/**
  * @brief  ADC使能函数.
  * @param  无
  * @retval 无
  */
void APP_AdcEnable(void)
{
  /* 使能ADC */
  LL_ADC_Enable(ADC1);

  /* 使能ADC 稳定时间，最低8个ADC Clock */
  LL_mDelay(1);
}
void APP_AdcConfig(void)
{
	uint8_t i;
	__IO uint32_t wait_loop_index = 0;
	LL_ADC_InitTypeDef ADC_Init;
	LL_ADC_REG_InitTypeDef LL_ADC_REG_InitType;
	ADC_Common_TypeDef ADC_Common_Type;

	/* ADC模块时钟使能 */
	LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_ADC1);

	/* ADC部分功能初始化 */
	ADC_Init.Clock= LL_ADC_CLOCK_ASYNC_HSI_DIV1;
	ADC_Init.DataAlignment=LL_ADC_DATA_ALIGN_RIGHT;
	ADC_Init.LowPowerMode=LL_ADC_LP_MODE_NONE;
	ADC_Init.Resolution=LL_ADC_RESOLUTION_12B;
	LL_ADC_Init(ADC1,&ADC_Init);
	/* 设置通道转换时间 */
	//LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_13CYCLES_5);
	//LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_13CYCLES_5);
	LL_ADC_SetSamplingTimeCommonChannels(ADC1, LL_ADC_SAMPLINGTIME_13CYCLES_5);

	/* ADC结构功能初始化 */
	LL_ADC_REG_InitType.ContinuousMode=LL_ADC_REG_CONV_CONTINUOUS;
	LL_ADC_REG_InitType.Overrun=LL_ADC_REG_OVR_DATA_OVERWRITTEN;
	LL_ADC_REG_InitType.SequencerDiscont=LL_ADC_REG_SEQ_DISCONT_DISABLE;
	LL_ADC_REG_InitType.TriggerSource=LL_ADC_REG_TRIG_SOFTWARE;
	LL_ADC_REG_Init(ADC1,&LL_ADC_REG_InitType);
	
	LL_ADC_REG_SetSequencerDiscont(ADC1, LL_ADC_REG_SEQ_DISCONT_1RANK);
	
	LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_4, LL_GPIO_MODE_ANALOG);
	LL_ADC_REG_SetSequencerChannels(ADC1,LL_ADC_CHANNEL_2);

	/* 设置通道Vrefint为转换通道 */
//	LL_ADC_REG_SetSequencerChannels(ADC1, LL_ADC_CHANNEL_VREFINT);

	/* 使能内部通道 */
	LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1),LL_ADC_PATH_INTERNAL_VREFINT);

	/* 选择1.5V为参考电压 */
	LL_ADC_SetVrefBufferVoltage(ADC1,LL_ADC_VREFBUF_1P5V);

	/* 使能参考电压 */
	LL_ADC_EnableVrefBufferVoltage(ADC1);

	/* Vrefint 等待稳定时间 */
	wait_loop_index = ((LL_ADC_DELAY_VREFINT_STAB_US * (SystemCoreClock / (100000 * 2))) / 10);
	while(wait_loop_index != 0)
	{
		wait_loop_index--;
	}

	
	DEBUG_INFO("----------------------------ADC CONFIG----------------------------");
	DEBUG_INFO("CONFIG:Single_Mode 、Trigg Software 、Data_Overwritten ");
	DEBUG_INFO(" LL_ADC_CHANNEL_3 (PA6) |  LL_ADC_CHANNEL_7  (PB0)");
	
	/* ADC校准 */
	APP_AdcCalibrate();
	/* 使能ADC */
	APP_AdcEnable();

	NVIC_SetPriority(ADC_COMP_IRQn, 1);

	NVIC_EnableIRQ(ADC_COMP_IRQn);	
	/* 使能EOC中断 */
	LL_ADC_EnableIT_EOC(ADC1);

	Adc.TempCnt = 0; 
	
	Bat.Status  = 0;
	Bat.Gera = 1;
	Bat.Percent = 10; //从第五格开始 
	for(i=0;i<sizeof(percent_temp);i++) percent_temp[i] = 10;
	
}
void APP_AdcGrpRegularUnitaryConvCompleteCallback()
{
	/* 获取ADC转换数据 */
	uint16_t temp = LL_ADC_REG_ReadConversionData12(ADC1);
    Adc.Temp += temp ;
	if(Adc.TempMax < temp)Adc.TempMax = temp;
	if(Adc.TempMin > temp)Adc.TempMin = temp;
	Adc.NewBatVoltage = temp * 1500 * 3 / 4096;
	Adc.TempCnt ++;
//	DEBUG_INFO("%d ", Adc.Temp);
}

void Adc_BatVolatageCollection_handle(void) //10MS 采样一次电压
{
	if(Adc.TempCnt >= 10) 
	{
		Adc.BatVoltage = ( Adc.Temp - Adc.TempMax - Adc.TempMin ) / 8;
		Adc.BatVoltage =  Adc.BatVoltage * 1500 * 3 / 4096 ;
	//	DEBUG_PRINTF("bat = %d \r\n",Adc.BatVoltage);
	//	PRINT(B,"%d",Adc.BatVoltage);
		BatVolatageToPercent_handle();
		Adc.Temp = 0;
		Adc.TempMax = 0X0000;
		Adc.TempMin = 0XFFFF;
		Adc.TempCnt = 0;
	}
	LL_TIM_ClearFlag_UPDATE(TIM1);
	LL_TIM_EnableIT_UPDATE(TIM1);
	NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
} 
void BatVolatageToPercent_handle(void) //100ms处理一次  电池可以稍微慢一点不过不能太慢 
{
	uint8_t i;
	int16_t percent;
	
	uint8_t const BatGear_thread[] = {10,20,40,64,81,101};  // percent-voltage   9 -2.65V   30-3.04V   65 - 3.15V  81 - 3.22  93 - 3.27
	uint8_t const BatGear_threadCharge[] = {0,24,41,66,82,101};  //percent-voltage  24-3.2V    35-3.35V   65 - 3.45V  76 - 3.5V 
	uint32_t add_temp = 0;
	
	if( Adc.BatVoltage )
	{
		if(Bat.Status == BAT_DISCHARGE) //放电百分比
		{
			if( Adc.BatVoltage < 3000) // 2.5-3.0
			{
				if(Adc.BatVoltage > 2500)
				{
					percent =  ( Adc.BatVoltage - 2500 ) * 30 / 500 ;
				}	
				else percent = 0;
			}	
			else  // 3-3.27
				percent =  30 + ( Adc.BatVoltage - 3000 )  *70 / 300;
		}
		else						//充电百分比
		{
			if( Adc.BatVoltage < 3300) 							// 2.80-3.3
			{
				if(Adc.BatVoltage> 2800)
				{
					percent =  ( Adc.BatVoltage - 2800 ) * 60 / 500;
				}
				else percent = 0;
			}		
			else 												// 3.33-3.58
				percent = 60 + ( Adc.BatVoltage - 3300 )  *40 / 300;			
		}
	}
	else  percent  = 0;

	//限幅
	if(percent > 100) percent = 100;
	else if(percent < 0 ) percent  =0;
	//平滑滤波
	for(i=0;i<sizeof(percent_temp) - 1;i++) percent_temp[i] = percent_temp[i+1];
	percent_temp[sizeof(percent_temp)  -1] = percent;
	for(i=0;i<sizeof(percent_temp);i++)add_temp += percent_temp[i];
	current_percent = add_temp /  sizeof(percent_temp) ;
	//DEBUG_PRINTF("current_percent = %d last_percent %d \r\n",current_percent,last_percent);
	if(Bat.Status == BAT_DISCHARGE) //放电状态下的 默认只降不升除非波动超过 10%
	{
		//上次的电压大于当前的电压 只降低不升
		if (last_percent >= current_percent  \
			|| ( (current_percent  - last_percent >= 6) && (current_percent > last_percent) && (!Bat.ChargeUpFlag) && (!RGB.OnFlag) )  )
		{
			Bat.ChargeUpFlag = 0;
			last_percent = current_percent;
			if (current_percent == 0)
						current_percent = 1;
			Bat.Percent = current_percent;
		}
	}
	else //只升不降
	{
		if (last_percent <= current_percent)
		{
			last_percent = current_percent;
			Bat.Percent = current_percent;
		}
	}
	//挡位判断
	if(Bat.Status == BAT_DISCHARGE)
	{ 
		for(i = 0; i < 6; i++)
		{ 
			if(Bat.Percent < BatGear_thread[i])// 再用叠加了回差的阈值判断档位
			{    
				Bat.Gera = i; //只降不升
				break;
			}
		}	
		if(Bat.Gera >= 6)Bat.Gera =  0;
	}
	else
	{
		for(i = 0; i < 6; i++) 
		{
			if(Bat.Percent < BatGear_threadCharge[i])// 再用叠加了回差的阈值判断档位
			{    
				Bat.Gera = i; //只降不升
				break;
			}
		}	
		if(Bat.Gera >= 6)Bat.Gera =  0;
	}
	
	//上报数据
	if(gear_changetemp != Bat.Gera )
	{
		gear_changetemp = Bat.Gera ;
		mcu_dp_enum_update( DPID_BATTERY_STATE,(Bat.Gera)/2);
	}
 	//DEBUG_PRINTF("Bat.Gera = %d \r\n",Bat.Gera);
	//DEBUG_PRINTF("Bat.Percent = %d \r\n",Bat.Percent);
	
}
void Bat_StatusCheck_Handle(void) // 10MS 定时器
{
	uint8_t i ;
	static uint16_t filter_cnt= 0,filter_cnt1 = 0; //滤波计数
	static uint16_t low_voltage_cnt = 0;
	switch(Bat.Status)
	{
		case BAT_DISCHARGE: //放电处理
			if(BAT_CDS_RX)
			{
				if(++filter_cnt >= 100) // 100MS进一次
				{
					filter_cnt = 0;
					Bat.Status = BAT_CHARGE;
					//开启红外
					Ir_Power_ON();
					//开启蓝牙
					BLE_Power_ON();
					LED_IndicatorOnFlag = 1;
					//Adc_ReScan();
					//for(i=0;i<sizeof(percent_temp);i++) percent_temp[i] = 0;
					current_percent = last_percent;
					Bat.Percent = current_percent;
					for(i=0;i<sizeof(percent_temp);i++) percent_temp[i] = Bat.Percent;
					Bat.ChargeUpFlag = 1; //只降不升开始
					if(Bat.SolarMode) //太阳能模式
						LED_RGB_Off_Handle(); //关灯
				}
			}
			else filter_cnt = 0;
			break;
		case BAT_CHARGE:
		case BAT_FULL:	
			if(!BAT_CDS_RX)
			{
				if(++filter_cnt >= 100) // 100MS进一次
				{
					filter_cnt = 0;
					if(Bat.SolarMode) //太阳能模式
					{
						LED_RGB_On_Handle();
						RGB.Command = RGB.LastCommand;
					}
					Bat.Status = BAT_DISCHARGE;
					//Adc_ReScan();
					//for(i=0;i<sizeof(percent_temp);i++) percent_temp[i] = Bat.Percent;
					current_percent = last_percent;
					Bat.Percent = current_percent;
					for(i=0;i<sizeof(percent_temp);i++) percent_temp[i] = Bat.Percent;
				}
			}
			else filter_cnt = 0;
			if(Bat.Percent > 95) //默认满电
			{
				if(++filter_cnt1 >= 100) // 10MS进一次
				{
					filter_cnt1 = 0;
					Bat.Status = BAT_FULL;
				}
			}
			else filter_cnt1 = 0;
			break;
	}
	
	if(Adc.NewBatVoltage < BAT_Protect_Voltage)  //小于2.56
	{
		if(++low_voltage_cnt > 100)
		{
			low_voltage_cnt = 0;
			Sys.LowVoltageFlag = 1;
			LED_RGB_Off_Handle();
		//if( ( Bat.Status == BAT_DISCHARGE ) && ( BAT_CDS_RX == 0) )//Sys.LowVoltageFlag = 1;		
		//	else 
			// {
			// 	Sys.LowVoltageFlag = 0;
			// 	if(RGB.OnFlag)  //充电保护
			// 	{
					
			// 		//BLE_Power_OFF();
			// 		//Led_Lmap_Light_OFF();
			// 		// Led.LampLightDisplayValue = 300;
			// 	}
			// }
		}
	}
	else low_voltage_cnt = 0;
	

}
