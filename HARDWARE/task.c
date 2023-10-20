
#include "task.h"
#include "led.h"
#include "bluetooth.h"
#include "string.h "

#define  TAKS_IT_ON			SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk |SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk  	// ���������ж�
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
	LL_GPIO_SetOutputPin(GPIO_TY_EN_PORT, GPIO_TY_EN_PIN);
	
	memset(&TY,0,sizeof(xTY_t));
	
	TY.SceneData [0] = '0';   //����
	TY.SceneData [1] = '0';
	
	TY.ColourData [0] = '0';  //H����
	TY.ColourData [1] = '0';
	TY.ColourData [2] = '0';
	TY.ColourData [3] = '0';
	
	TY.ColourData [4] = '0';  //S����
	TY.ColourData [5] = '0';
	TY.ColourData [6] = '0';
	TY.ColourData [7] = '0';
	
	TY.ColourData [8] = '0';  //V����
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
	//��������˯��
	if( ( !RGB.OnFlag && ( Bat.Status == BAT_DISCHARGE )  ) ||  Sys.LowVoltageFlag )
	{
		++Sys.EntreSleepTimeCount;
		if(Sys.EntreSleepTimeCount > 15000)Sys.EnterSleepFlag = 1;
		if(Sys.LowVoltageFlag && Sys.EntreSleepTimeCount > 2000)Sys.EnterSleepFlag = 1;
	}
	else Sys.EntreSleepTimeCount = 0;
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
}
void Ir_Power_ON()
{
	IR_ENANBLE;
}
void BLE_Power_OFF()
{
	TY_DISANBLE;
}
void BLE_Power_ON()
{
	TY_ENANBLE;
}
void Sys_EntreSleep()
{
	Sys.EnterSleepFlag = 1;
}
static void APP_EnterStop(void)
{
  /* ʹ��PWRʱ�� */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);
  /* �͹�������ģʽ */
  LL_PWR_SetLprMode(LL_PWR_LPR_MODE_LPR);
  /* SRAM��ѹ������LDO���һ�� */
  LL_PWR_SetStopModeSramVoltCtrl(LL_PWR_SRAM_RETENTION_VOLT_CTRL_LDO);
  /* ����DeepSleepģʽ */
  LL_LPM_EnableDeepSleep();
/*	
	�ȴ��ж�ָ��    WFEģʽ �¼����Ѳ����ж�
  __SEV();
  __WFE();
  __WFE();
*/
  /* �ȴ��ж�ָ�� */
  __WFI();

  LL_LPM_EnableSleep();
}
void Adc_RefVoltage_ON()
{
	__IO uint32_t wait_loop_index = 0;
	LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1),LL_ADC_PATH_INTERNAL_VREFINT);
	/* Vrefint �ȴ��ȶ�ʱ�� */
	wait_loop_index = ((LL_ADC_DELAY_VREFINT_STAB_US * (SystemCoreClock / (100000 * 2))) / 10);
	while(wait_loop_index != 0)
	{
		wait_loop_index--;
	}
}
void Adc_RefVoltage_OFF() //�ر��ڲ�ͨ����ֹ�ĵ�
{
	LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1),LL_ADC_PATH_INTERNAL_NONE);
}
// ��绽��(�ⲿ) ���⻽��(�ⲿ) ��������(�ⲿ)  ��ʱ����(�ڲ�)
void Sys_EnterSleep_Handle(void)
{
	DEBUG_INFO("SysEnterSleep");
	
	TAKS_IT_OFF; //�ر������ж�
	
	LED_RGB_Off_Handle();
	LL_TIM_OC_SetCompareCH1(TIM1 , RGB_PWM  ); //RED
	LL_TIM_OC_SetCompareCH4(TIM1 , RGB_PWM  ); //GREEN
	LL_TIM_OC_SetCompareCH3(TIM1 , RGB_PWM); //BLUE
	RGB.Dispaly_v = 0;
	
	//ʹ�������͹���
	enable_low_power();
	
	//�ӳ�һ��	
	LL_mDelay(50);

	//������Ϣ  data_handle();
	
	LL_IWDG_ReloadCounter(IWDG); // IWDG ����
	
	
	Sys.EnterSleepFlag = 0;
	Sys.IrWakeUPFlag = 0;
	Sys.ChargWakeUPFlag = 0;
	Sys.LPTIMWakeUPFlag = 0;
	Sys.UartMWakeUPFlag = 0;
	Sys.EntreSleepTimeCount = 0;
	DEBUG_INFO("IrWakeUPFlag %d",Sys.IrWakeUPFlag);
	DEBUG_INFO("ChargWakeUPFlag %d",Sys.ChargWakeUPFlag);
	DEBUG_INFO("UartMWakeUPFlag %d",Sys.UartMWakeUPFlag);
	
	if(Sys.LowVoltageFlag)
	{
		//�رպ���  
		Ir_Power_OFF();	
		//�ر�����
		BLE_Power_OFF();
		//�ر�LPTIME����
		LL_LPTIM_Disable(LPTIM);
	}
	else 
	{
		LL_LPTIM_Enable(LPTIM);
		
		LL_EXTI_EnableIT(LL_EXTI_LINE_4);
	}
	
	LL_LPTIM_SetAutoReload(LPTIM,15360 );  //1����
	LL_LPTIM_StartCounter(LPTIM, LL_LPTIM_OPERATING_MODE_CONTINUOUS);
	LL_mDelay(1);
	
	LED_RGB_Off_Handle(); //�رյ�
	
	Led_Off(); //�ر�LEDָʾ��
	
	Adc_RefVoltage_OFF(); //�ο���ѹ�ر�
	
Resleep:
	APP_EnterStop();	//����˯��
	if(Sys.LPTIMWakeUPFlag)
	{
		DEBUG_INFO("LPTIMWakeUP");
		Sys.SleepTimeCount++; //1���Ӽ�һ
		if(Sys.SleepTimeCount > 5)  //����24Сʱ �رպ�������  60 * 24 = 1440 
		{
			Sys.SleepTimeCount = 0;
			//�رպ���  
			Ir_Power_OFF();	
			//�ر�����
			BLE_Power_OFF();	
			//�ر�LPTIME
			LL_LPTIM_Disable(LPTIM);
			//ֻ��CDS���Ի���
			DEBUG_INFO("entry shut down mode");
		}
	}
	
	if( !Sys.ChargWakeUPFlag && !Sys.IrWakeUPFlag && !Sys.UartMWakeUPFlag)goto Resleep;
	DEBUG_INFO("IrWakeUPFlag %d",Sys.IrWakeUPFlag);
	DEBUG_INFO("ChargWakeUPFlag %d",Sys.ChargWakeUPFlag);
	DEBUG_INFO("UartMWakeUPFlag %d",Sys.UartMWakeUPFlag);
	
	Adc_RefVoltage_ON(); //�ο���ѹ����
	
	//��������
	Ir_Power_ON();
	
	//��������
	BLE_Power_ON();
	LL_mDelay(1);
	//�ӳ�һ��
	
	//������Ϣ
	
	//ʹ��LPTIM������  
	LL_LPTIM_EnableResetAfterRead(LPTIM);
	LL_LPTIM_GetCounter(LPTIM); //���������
	LL_LPTIM_GetCounter(LPTIM); //���������������
    LL_LPTIM_DisableResetAfterRead(LPTIM);
	LL_mDelay(1);
	LL_LPTIM_Disable(LPTIM);
	LL_EXTI_DisableIT(LL_EXTI_LINE_4);
	
	disable_low_power();
	
	all_data_update(); //������������
	
	TAKS_IT_ON; //���������ж�
}
