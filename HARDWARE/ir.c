#include "ir.h"
#include "led.h"
#include "stdlib.h"
#include "mcu_api.h"
#include "protocol.h"
#include "bluetooth.h"
#include "system.h"
#include "task.h"

Ir_s Ir;
void APP_ConfigTIM14(void)
{
	/*����TIM1*/
	LL_TIM_InitTypeDef TIM14CountInit = {0};

	/*ʹ��TIM1ʱ��*/
	LL_APB1_GRP2_EnableClock(RCC_APBENR2_TIM14EN);
	// 24MHZ
	TIM14CountInit.ClockDivision       = LL_TIM_CLOCKDIVISION_DIV1;/* ����Ƶ             */
	TIM14CountInit.CounterMode         = LL_TIM_COUNTERMODE_UP;    /* ����ģʽ�����ϼ��� */
	TIM14CountInit.Prescaler           = (24*64)-1;                   /* ʱ��Ԥ��Ƶ��8000   */
	TIM14CountInit.Autoreload          = 256-1;                   /* �Զ���װ��ֵ��1000 */
	TIM14CountInit.RepetitionCounter   = 0;                        /* �ظ�����ֵ��0      */
	
	/*��ʼ��TIM1*/
	LL_TIM_Init(TIM14,&TIM14CountInit);

	/*������±�־λ*/
	LL_TIM_ClearFlag_UPDATE(TIM14);

	/*ʹ��UPDATE�ж�*/
	LL_TIM_EnableIT_UPDATE(TIM14);

	/*ʹ��TIM1������*/
	LL_TIM_EnableCounter(TIM14);

	/*����UPDATE�ж�����*/
	NVIC_EnableIRQ(TIM14_IRQn);
	NVIC_SetPriority(TIM14_IRQn,Ir_Priority);
}
void Ir_Init(void)
{
	APP_ConfigTIM14();
	LL_GPIO_InitTypeDef GPIO_InitStruct;

	#if IR_EN_ENANBLE == 1
	/* ����GPIO_IR_EN���ģʽ */
	GPIO_InitStruct.Pin = GPIO_IR_EN_PIN;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIO_IR_EN_PORT, &GPIO_InitStruct);
	LL_GPIO_SetOutputPin(GPIO_IR_EN_PORT, GPIO_IR_EN_PIN);
	#endif 

	// ����PA0����ģʽ 
	GPIO_InitStruct.Pin = GPIO_IR_PIN;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIO_IR_PORT, &GPIO_InitStruct);

	// ѡ��EXTI0���ⲿ�ж����� 
	LL_EXTI_SetEXTISource(IR_EXTI_CONFIG_PORT, IR_EXTI_CONFIG_LINE);

	// ����EXTI0Ϊ�жϡ����½��ش��� 
	LL_EXTI_InitTypeDef EXTI_InitStruct;
	EXTI_InitStruct.Line = IR_EXTI_LINE;
	EXTI_InitStruct.LineCommand = ENABLE;
	EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
	EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING_FALLING;
	LL_EXTI_Init(&EXTI_InitStruct);

	/* ʹ���ж� */

	NVIC_SetPriority(IR_EXTI_IRQn, Ir_Priority);
	NVIC_EnableIRQ(IR_EXTI_IRQn);
}
void Ir_ExtiCallback(void)
{
	if(IR_RX_PORT)
		TIME_COUNT = 0;
	else
	{
		Ir.HighLevelTime = TIME_COUNT;
		if(Ir.GuideFlag)
		{
			if (Ir.HighLevelTime <= IR_DATA0_TH_H && Ir.HighLevelTime >= IR_DATA0_TH_L) 	//0
				Ir.DataTemp = Ir.DataTemp << 1;
			else if (Ir.HighLevelTime <= IR_DATA1_TH_H && Ir.HighLevelTime >= IR_DATA1_TH_L) // 1
				Ir.DataTemp = Ir.DataTemp << 1 | 0x01;
			else if (Ir.HighLevelTime <= IR_REDATA_TH_H && Ir.HighLevelTime >= IR_REDATA_TH_L)	//�ظ�
			{
				Ir.RepeatCount ++;
				Ir.Timeout = 0;
			}
		}
		else if(Ir.HighLevelTime >= IR_GUID_TH_L && Ir.HighLevelTime <= IR_GUID_TH_H)	//����
		{
			Ir.GuideFlag = 1;
			Ir.RepeatCount = 0;
			Ir.Timeout = 0;
			Ir.TimeOutFlag = 0;
		}
	}
}
void Ir_Time14IRQCallback(void)
{
	if(Ir.GuideFlag)
	{
		if(!Ir.Timeout)	
			Ir.ReceiveDataFlag = 1;
		if(Ir.Timeout<10) //163MS
			Ir.Timeout++;
		else
		{
			Ir.GuideFlag = 0;
			Ir.TimeOutFlag = 1;
		}
	}
}
void Ir_ReScan()
{
	Ir.GuideFlag = 0;
	Ir.ReceiveDataFlag = 0;
	Ir.DataTemp  = 0;
}
uint8_t Ir_Scan(void)
{
	uint8_t *IrData = (uint8_t *)&Ir.DataTemp;
	uint8_t key = 0;
	if(Ir.ReceiveDataFlag  /* &&  IrData[3] == Ir_GUIDE */)
	{
		if( ( IrData[3] == (uint8_t)(~IrData[2])) && ( IrData[1] == (uint8_t)(~IrData[0]) ))
		{
			key = IrData[1];
		}
		if(!key || !Ir.GuideFlag)
		{
			Ir.ReceiveDataFlag = 0;
		}
	}
	return key;
}
void Ir_CommandReceiv(uint8_t key)
{
	if(key)
	{
		switch(key)
		{
			case IR_COMMAND_POWEROFF:
			if(Ir.RepeatCount<=3 && Ir.TimeOutFlag)
			{
				//	BLE_Power_OFF();
					LED_RGB_Off_Handle();
				//	Sys_EntreSleep();
					Ir_ReScan();
					DEBUG_INFO("Power OFF");
				}
				if(Ir.RepeatCount>3)
				{
					BLE_Power_OFF();
					Ir_Power_OFF();
					LED_RGB_Off_Handle();
					Sys_EntreSleep();
					Ir_ReScan();
					DEBUG_INFO("BLE OFF and IR OFF");
				}	
				RGB.Command = IR_POWEROFF;
				break;
			case IR_COMMAND_LEDON:
				if(RGB.OnFlag == 0)
				{
					if(++RGB.ResetCnt>= 4)
					{
						RGB.ResetCnt = 0;
						bt_uart_write_frame(4, 0);
					}
					DEBUG_INFO("RGB.ResetCnt %d",RGB.ResetCnt);
				}
				LED_RGB_On_Handle();
				RGB.Command = IR_LEDON;
				DEBUG_INFO("LEDON");
				break;
			case IR_COMMAND_LEDOFF:	
				if(RGB.OnFlag == 1)	RGB.ResetCnt++; //������
				LED_RGB_Off_Handle();
				RGB.Command = IR_LEDOFF;
				DEBUG_INFO("LEDOFF");
				break;
			case IR_COMMAND_LIAGHUP: 
				if(RGB.OnFlag)
				{
					if(Ir.RepeatCount<=3 && Ir.TimeOutFlag)
					{
						if( ( RGB.v + 150) < 1000 )
						{
							RGB.v+=150;
						}
						else 
						{
							RGB.Dispaly_v = 0;
							RGB.v = 1000;
						}
						TY_Updata_Bright();
						Ir_ReScan();
					}
					if(Ir.RepeatCount>3)
					{
						if(RGB.v<1000)RGB.v+=5;	
						else 
						{
							Ir_ReScan();
							RGB.v = 1000;
							RGB.Dispaly_v = 0; //ˢ��
						}
						if(Ir.TimeOutFlag) 
						{
							TY_Updata_Bright();
							Ir_ReScan();
						}
					}	
					//DEBUG_INFO("LIAGHUP");	
					
					RGB.vTemp = RGB.v;
				}
				break;
			case IR_COMMAND_LIAGHDONW:
				if(RGB.OnFlag)
				{
					if(Ir.RepeatCount<=3 && Ir.TimeOutFlag)
					{
						if( (RGB.v - 150 )>100)RGB.v-=150;
						else 
						{
							RGB.v = 100;
							RGB.Dispaly_v = 0;
						}
						TY_Updata_Bright();
						Ir_ReScan();
					}
					if(Ir.RepeatCount>3)
					{
						if(RGB.v>100)RGB.v-=5;
						else
						{
							RGB.v = 100;
							Ir_ReScan();
							RGB.Dispaly_v = 0; //ˢ��
						}
						if(Ir.TimeOutFlag) 
						{
							TY_Updata_Bright();
							Ir_ReScan();
						}
					}
					//DEBUG_INFO("LIAGHDONW");	
					
					RGB.vTemp = RGB.v;
				}
				break;
			case IR_COMMAND_3H:
				RGB.Dispaly_v = 0;
				LED_RGB_On_Handle();
			//	RGB.SetOffTime = 3 * 60 * 60000 /5; 
				RGB.SetOffTime = 1000 *5 /5; 
				RGB.Command =IR_LEDON;
				DEBUG_INFO("3H");
				break;
			case IR_COMMAND_5H:
				RGB.Dispaly_v = 0;
				LED_RGB_On_Handle();
			//	RGB.SetOffTime = 5 * 60 * 60000 /5;
				RGB.SetOffTime = 1000 *10 /5; 
				RGB.Command =IR_LEDON;
				DEBUG_INFO("5H");
				break;
			case IR_COMMAND_WRITE_MODE:
				LED_RGB_SetHSV(0,0,RGB.vTemp);
				LED_RGB_SetDisplayHSV(RGB.h,RGB.s,0);
				LED_RGB_On_Handle();
				RGB.Command =IR_WRITE_MODE;
				DEBUG_INFO("WRITE_MODE");
				break;
			case IR_COMMAND_RGB_MODE:
				RGB.OnFlag = 1;
				LED_RGB_On_Handle();
				RGB.Command =IR_RGB_MODE;
				DEBUG_INFO("RGB_MODE");
				break;
			case IR_COMMAND_RED_MODE:
				LED_RGB_SetHSV(0,1000,RGB.vTemp);
				LED_RGB_SetDisplayHSV(RGB.h,RGB.s,0);
				LED_RGB_On_Handle();
				RGB.Command =IR_RED_MODE;
				DEBUG_INFO("RED_MODE");
				break;
			case IR_COMMAND_GREEN_MODE:	
				LED_RGB_SetHSV(120,1000,RGB.vTemp);
				LED_RGB_SetDisplayHSV(RGB.h,RGB.s,0);
				LED_RGB_On_Handle();
				RGB.Command =IR_GREEN_MODE;
				DEBUG_INFO("GREEN_MODE");
				break;
			case IR_COMMAND_BLUE_MODE:
				LED_RGB_SetHSV(240,1000,RGB.vTemp);
				LED_RGB_SetDisplayHSV(RGB.h,RGB.s,0);
				LED_RGB_On_Handle();
				RGB.Command =IR_RED_MODE;
				DEBUG_INFO("IR_BLUE_MODE");
				break;
		}
	//	DEBUG_INFO("IR key %X",key);
	//	DEBUG_INFO("Ir.DataTemp %x",Ir.DataTemp);
		if( (key != IR_COMMAND_LIAGHUP)   && ( key != IR_COMMAND_LIAGHDONW  )  && \
			(key != IR_COMMAND_POWEROFF) 	)Ir_ReScan();
			
	}
	
}