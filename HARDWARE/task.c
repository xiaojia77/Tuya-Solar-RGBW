
#include "task.h"
#include "led.h"
#include "bluetooth.h"
#include "string.h "

#define  TAKS_IT_ON			SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk  	// 开启任务中断
#define  TAKS_IT_OFF		SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |SysTick_CTRL_ENABLE_Msk   

xSys Sys;
xTY_t TY;
void hex2str(uint16_t data, uint8_t * s, int len)
{
    int i;
    s[len] = 0;
    for (i = len - 1; i >= 0; i--, data >>= 4)
    {
        if ((data & 0xf) <= 9)
            s[i] = (data & 0xf) + '0';
        else
            s[i] = (data & 0xf) + 'A' - 0x0a;
    }
}
void Sys_Init()
{
	memset(&Sys,0,sizeof(xSys));
	LL_GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = LL_GPIO_PIN_3;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
void TY_Init()
{
	LL_GPIO_InitTypeDef GPIO_InitStruct;

	GPIO_InitStruct.Pin = GPIO_TY_EN_PIN;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIO_TY_EN_PORT, &GPIO_InitStruct);
	TY_ENANBLE;
	TY_RST_1;
	
	memset(&TY,0,sizeof(xTY_t));
	
	TY.SceneData [0] = '0';   //场景
	TY.SceneData [1] = '0';
	
	TY.ColourData [0] = '0';  //H数据
	TY.ColourData [1] = '0';
	TY.ColourData [2] = '0';
	TY.ColourData [3] = '0';
	
	TY.ColourData [4] = '0';  //S数据
	TY.ColourData [5] = '0';
	TY.ColourData [6] = '0';
	TY.ColourData [7] = '0';
	
	TY.ColourData [8] = '0';  //V数据
	TY.ColourData [9] = '1';
	TY.ColourData [10] = 'f';
	TY.ColourData [11] = '4';
	
	
	TY.bright = 900;
	TY.WorkMode = 0;
	TY.TempValue = 0;
	TY.GroupSync = 1;
}
void Sys_Timer1ms_Handle(void) //1MS
{
	static uint16_t LedOffFilter = 0;
	//正常进入睡眠
	if( (/*  BAT_CDS_RX == 0 ) && ( ( !RGB.OnFlag && ( Bat.Status == BAT_DISCHARGE ) ) ||*/  Sys.LowVoltageFlag ) )
	{
		++Sys.EntreSleepTimeCount;
		if(Sys.EntreSleepTimeCount > 5000)Sys.EnterSleepFlag = 1;
		//if(Sys.LowVoltageFlag && Sys.EntreSleepTimeCount > 2000)Sys.EnterSleepFlag = 1;
	}
	else Sys.EntreSleepTimeCount = 0;
	
	if( LED_IndicatorOnFlag && !RGB.OnFlag && ( Bat.Status == BAT_DISCHARGE ) && BAT_CDS_RX == 0)
	{
			if(++LedOffFilter > 4000 )
			{
					LedOffFilter = 0;
					LED_IndicatorOnFlag  = 0;
			}
	}
	else LedOffFilter = 0;
}
void TY_Updata_Bright()
{
	uint8_t sdata[10];
	TY.bright = (RGB.v - 100) * 1000 / 900; 
	
	hex2str(TY.bright,sdata,4);
	DEBUG_INFO("%s ",sdata);
	
	TY.ColourData [8] = sdata[0];
	TY.ColourData [9] = sdata[1];
	TY.ColourData [10] = sdata[2];
	TY.ColourData [11] = sdata[3];
	
	mcu_dp_string_update(DPID_COLOUR_DATA,TY.ColourData,12); 
	mcu_dp_value_update(DPID_BRIGHT_VALUE,TY.bright);
}
void Ir_Power_OFF()
{
	IR_DISANBLE;
	Ir_ReScan();
	LL_EXTI_DisableIT(IR_EXTI_LINE);
}
void Ir_Power_ON()
{
	IR_ENANBLE;
	LL_EXTI_EnableIT(IR_EXTI_LINE);
}
void BLE_Power_OFF()
{
	/*GPIOB配置*/
	LL_GPIO_InitTypeDef GPIO_InitStruct;
	/*选择4号引脚*/
	GPIO_InitStruct.Pin = LL_GPIO_PIN_4;
	/*选择复用模式*/
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	/*选择输出速度*/
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	/*选择输出模式*/
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	/*选择上拉*/
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	/*复用为USART1功能*/
	GPIO_InitStruct.Alternate = LL_GPIO_AF1_USART1;
	/*GPIOB初始化*/
	LL_GPIO_Init(GPIOB,&GPIO_InitStruct);

	/*选择5号引脚*/
	GPIO_InitStruct.Pin = LL_GPIO_PIN_5;
	/*复用为USART1功能*/
	GPIO_InitStruct.Alternate = LL_GPIO_MODE_OUTPUT;
	/*GPIOB初始化*/
	LL_GPIO_Init(GPIOB,&GPIO_InitStruct);

	LL_GPIO_ResetOutputPin(GPIOB,LL_GPIO_PIN_4);
	LL_GPIO_ResetOutputPin(GPIOB,LL_GPIO_PIN_5);
	
	TY_DISANBLE;
	
	TY_RST_0;

	TY.BleOnflag = 0;
		
}
void BLE_Power_ON()
{
	
	
	/*GPIOB配置*/
	LL_GPIO_InitTypeDef GPIO_InitStruct;
	/*选择4号引脚*/
	GPIO_InitStruct.Pin = LL_GPIO_PIN_4;
	/*选择复用模式*/
	GPIO_InitStruct.Mode = LL_GPIO_MODE_ALTERNATE;
	/*选择输出速度*/
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	/*选择输出模式*/
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	/*选择上拉*/
	GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
	/*复用为USART1功能*/
	GPIO_InitStruct.Alternate = LL_GPIO_AF1_USART1;
	/*GPIOB初始化*/
	LL_GPIO_Init(GPIOB,&GPIO_InitStruct);

	/*选择5号引脚*/
	GPIO_InitStruct.Pin = LL_GPIO_PIN_5;
	/*复用为USART1功能*/
	GPIO_InitStruct.Alternate = LL_GPIO_AF1_USART1;
	/*GPIOB初始化*/
	LL_GPIO_Init(GPIOB,&GPIO_InitStruct);
	
	TY_ENANBLE;
	
	TY_RST_1;

	TY.BleOnflag = 1;
	
}
void Sys_EntreSleep()
{
	Sys.EnterSleepFlag = 1;
}
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
void Adc_RefVoltage_ON()
{
	__IO uint32_t wait_loop_index = 0;
	LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1),LL_ADC_PATH_INTERNAL_VREFINT);
	/* Vrefint 等待稳定时间 */
	wait_loop_index = ((LL_ADC_DELAY_VREFINT_STAB_US * (SystemCoreClock / (100000 * 2))) / 10);
	while(wait_loop_index != 0)
	{
		wait_loop_index--;
	}
	LL_ADC_Enable(ADC1);
	LL_mDelay(1);
}
void Adc_RefVoltage_OFF() //关闭内部通道防止耗电
{
	LL_mDelay(1);										//等待ADC等中断处理完成 延迟一小会
	while(LL_ADC_REG_IsConversionOngoing(ADC1)); 		//确保没有在转换的
	LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1),LL_ADC_PATH_INTERNAL_NONE);
	LL_ADC_Disable(ADC1);
	while(LL_ADC_IsDisableOngoing(ADC1));				//确保ADC关闭完成进入低功耗
}
// 充电唤醒(外部) 红外唤醒(外部) 蓝牙唤醒(外部)  定时唤醒(内部)
void Sys_EnterSleep_Handle(void)
{
	volatile uint32_t reg;
	DEBUG_INFO("SysEnterSleep");
	//关闭任务中断
	TAKS_IT_OFF; 
	//写flash 保存数据
	Flash_Data_Write(); 
	//使能蓝牙低功耗
	//enable_low_power();
	//延迟一会	
	//LL_mDelay(100);	
	LL_mDelay(1);	
	LL_IWDG_ReloadCounter(IWDG); // IWDG 清零
	//关闭定时器T1
 	LL_TIM_DisableCounter(TIM1);
	LL_TIM_DisableIT_UPDATE(TIM1);
	APP_DeConfigPWMChannel(); // 关闭PWM输出 睡眠唤醒时要关闭PWM模块 并将IO拉低
	//关闭定时器T14
	LL_TIM_DisableCounter(TIM14); 
	LL_TIM_DisableIT_UPDATE(TIM14);
	Ir_ReScan();
	//清除标志位
	LL_EXTI_ClearFlag(BAT_CDS_EXTI_LINE);	//清除CDS外部中断标志
	LL_EXTI_ClearFlag(IR_EXTI_LINE);			//清除红外外部中断标志
	LL_TIM_ClearFlag_UPDATE(TIM14);				//清除定时器标志
	LL_TIM_ClearFlag_UPDATE(TIM1);
	//关闭RGB灯
	LED_RGB_Off_Handle();
	LL_TIM_OC_SetCompareCH1(TIM1 , RGB_PWM  ); //RED
	LL_TIM_OC_SetCompareCH2(TIM1 , RGB_PWM  ); //BLUE
	LL_TIM_OC_SetCompareCH3(TIM1 , RGB_PWM  ); //BLUE 
	LL_TIM_OC_SetCompareCH4(TIM1 , RGB_PWM  ); //GREEN
	RGB.Dispaly_v = 0;
	//关闭LED指示灯
	Led_Off(); 
	//参考电压关闭
	Adc_RefVoltage_OFF(); 
	
	Sys.EnterSleepFlag = 0;		//变量清空
	Sys.EntreSleepTimeCount = 0; //睡眠时间
	//唤醒事件清空
	Sys.EnterSleepFlag = 0;
	Sys.EntreSleepTimeCount = 0;
	Sys.IrWakeUPFlag = 0;
	Sys.ChargWakeUPFlag = 0;
	Sys.LPTIMWakeUPFlag = 0;
	Sys.UartMWakeUPFlag = 0;


	if(Sys.LowVoltageFlag)
	{
		Sys.LowVoltageFlag = 0;
		//关闭红外  
		Ir_Power_OFF();	
		//关闭蓝牙
		BLE_Power_OFF();
	}
	else 
	{	
		LL_EXTI_EnableIT(LL_EXTI_LINE_4); //开启串口中断唤醒
	}
	
Resleep:
	reg = LL_GPIO_ReadInputPort(GPIOA);
	reg = LL_GPIO_ReadInputPort(GPIOB);
	reg = LL_GPIO_ReadInputPort(GPIOC);
	APP_EnterStop();	//进入睡眠
	__NOP();__NOP();__NOP();	//等待系统稳定
	if(Sys.LPTIMWakeUPFlag)
	{
		DEBUG_INFO("LPTIMWakeUP");
		Sys.SleepTimeCount++; //1分钟加一
		if(Sys.SleepTimeCount > 1440 * 4)  //大于24小时 关闭红外蓝牙  60 * 24 = 1440 
		{
			Sys.SleepTimeCount = 0;
			//关闭红外  
			Ir_Power_OFF();	
			//关闭蓝牙
			BLE_Power_OFF();	
			DEBUG_INFO("24 shutdown");
		}
	}
	LL_IWDG_ReloadCounter(IWDG); // IWDG 清零
	LL_mDelay(1);

	if( !Sys.ChargWakeUPFlag && !Sys.IrWakeUPFlag && !Sys.UartMWakeUPFlag)goto Resleep;

	//if(Sys.ChargWakeUPFlag)Sys.SleepTimeCount = 0; // 清空计时

	DEBUG_INFO("IrWakeUPFlag %d",Sys.IrWakeUPFlag);
	DEBUG_INFO("ChargWakeUPFlag %d",Sys.ChargWakeUPFlag);
	DEBUG_INFO("UartMWakeUPFlag %d",Sys.UartMWakeUPFlag);

	LL_TIM_EnableCounter(TIM1); 	//关闭定时器T1
	APP_ConfigPWMChannel(); 		
	
	LL_TIM_EnableCounter(TIM14); //关闭定时器T4
	LL_TIM_EnableIT_UPDATE(TIM14);
	
	Adc_RefVoltage_ON(); //参考电压开启
	LL_mDelay(1);
	
	// //开启红外
	// Ir_Power_ON();
	
	// //开启蓝牙
	// BLE_Power_ON();

	LL_EXTI_DisableIT(LL_EXTI_LINE_4);	//关闭蓝牙唤醒线
	
	//disable_low_power(); //退出蓝牙低功耗
	
	all_data_update(); //更新蓝牙数据

	TAKS_IT_ON; //开启任务中断
}
