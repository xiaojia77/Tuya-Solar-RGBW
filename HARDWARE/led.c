
#include "led.h"
#include "adc.h"
#include "system.h"
#include "string.h"

uint8_t LED_DisplayTemp = 0;   
uint8_t LED_IndicatorOnFlag = 0;

xRGB RGB;

void LED_RGB_SetHSV(uint16_t h, uint16_t s, uint16_t v)
{
	if(v<100)v=100;
	if(v>1000)v=1000;
	RGB.h = h;
	RGB.s = s;
	RGB.v = v;
	RGB.Setv = v;
}
void LED_RGB_SetDisplayHSV(uint16_t h, uint16_t s, uint16_t v)
{
	RGB.Dispaly_h = h;
	RGB.Dispaly_s = s;
	RGB.Dispaly_v = v;
}
void led_strip_hsv2rgb(uint16_t h, uint16_t s, uint16_t v, uint8_t *r, uint8_t *g, uint8_t *b)
{
    h %= 360; // h -> [0,360]
    uint16_t rgb_max = v * 255 / 1000;
    uint16_t rgb_min = rgb_max * (1000 - s) / 1000;
    uint16_t i = h / 60;
    uint16_t diff = h % 60;
    // RGB adjustment amount by hue
    uint16_t rgb_adj = (rgb_max - rgb_min) * diff / 60;

    switch (i) {
    case 0:
        *r = rgb_max;
        *g = rgb_min + rgb_adj;
        *b = rgb_min;
        break;
    case 1:
        *r = rgb_max - rgb_adj;
        *g = rgb_max;
        *b = rgb_min;
        break;
    case 2:
        *r = rgb_min;
        *g = rgb_max;
        *b = rgb_min + rgb_adj;
        break;
    case 3:
        *r = rgb_min;
        *g = rgb_max - rgb_adj;
        *b = rgb_max;
        break;
    case 4:
        *r = rgb_min + rgb_adj;
        *g = rgb_min;
        *b = rgb_max;
        break;
    default:
        *r = rgb_max;
        *g = rgb_min;
        *b = rgb_max - rgb_adj;
        break;
    }
}
void LED_RGB_HSVdisplay(uint16_t h, uint16_t s, uint16_t v)
{
	led_strip_hsv2rgb(h,s,v,&RGB.Rvalue,&RGB.Gvalue,&RGB.Bvalue); //RGB HSV 显示
	LED_RGB_display(RGB.Rvalue,RGB.Gvalue,RGB.Bvalue);
}
void LED_RGB_display(uint8_t r,uint8_t g,uint8_t b) //RGB 888 显示
{
	RGB.Rvalue = r;
	RGB.Gvalue = g;
	RGB.Bvalue = b;
	RGB.Rpwm = (r * RGB_Light_MAX / 255)  * RGB.CurrentGear / 20  ; //r/255*RGB_Light_MAX
	RGB.Gpwm = (g * RGB_Light_MAX / 255)  * RGB.CurrentGear / 20 ; //r/255*RGB_Light_MAX
	RGB.Bpwm = (b * RGB_Light_MAX / 255)  * RGB.CurrentGear / 20 ; //r/255*RGB_Light_MAX
	LL_TIM_OC_SetCompareCH4(TIM1 , RGB_PWM - RGB.Rpwm ); //RED
	LL_TIM_OC_SetCompareCH2(TIM1 , RGB_PWM - RGB.Gpwm ); //GREEN
	LL_TIM_OC_SetCompareCH1(TIM1 , RGB_PWM - RGB.Bpwm ); //BLUE
}
void LED_RGB_W_display(uint16_t w) //白光亮度设置
{
	RGB.Wvalue = w;
	RGB.Wpwm =  (w * RGB_Light_MAX / 1000 )  * RGB.CurrentGear / 20; //r/255*RGB_Light_MAX
	LL_TIM_OC_SetCompareCH3(TIM1 , RGB_PWM - RGB.Wpwm ); //W
}

void LED_Init()	
{

	LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOA);
	LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
	LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOC);
	
	LL_GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT; 
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_MEDIUM;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	

	GPIO_InitStruct.Pin = GPIO_L1_PIN;				//初始化L1
	LL_GPIO_Init(GPIO_L1_PORT, &GPIO_InitStruct);
	L1_RESET;
	
	GPIO_InitStruct.Pin = GPIO_L2_PIN;				//初始化L2
	LL_GPIO_Init(GPIO_L2_PORT, &GPIO_InitStruct);
	L2_RESET;
			
	
	GPIO_InitStruct.Pin = GPIO_L3_PIN;				//初始化L3
	LL_GPIO_Init(GPIO_L3_PORT, &GPIO_InitStruct);
	L3_RESET

}
void LED_Scan_Handle()
{
	static uint8_t ScanCount = 0;

	L1_RESET;L2_RESET;L3_RESET;
	if(LED_IndicatorOnFlag )
	{
		switch(ScanCount)
		{	
			case 0:  // L1
				if(LED_DisplayTemp<1)break;
				L1_OUTPUT;L2_OUTPUT;L3_INPUT; // L1
				L2_SET;L1_RESET;  
				break;
			case 1:  // L2
				if(LED_DisplayTemp<2)break;
				L1_OUTPUT;L2_OUTPUT;L3_INPUT; // L2
				L1_SET;L2_RESET;  
				break;
			case 2:  // L3
				if(LED_DisplayTemp<3)break;
				L2_OUTPUT;L3_OUTPUT;L1_INPUT; // L3
				L3_SET;L2_RESET;  
				break;
			case 3:  // L4
				if(LED_DisplayTemp<4)break;
				L2_OUTPUT;L3_OUTPUT;L1_INPUT; // L4 
				L2_SET;L3_RESET;  
				break;
			case 4:  // L5
				if(LED_DisplayTemp<5)break;
				L1_OUTPUT;L3_OUTPUT;L2_INPUT; // L5
				L3_SET;L1_RESET;  	
				break;
			default: 
				ScanCount = 0;
				break;
		}
		if(++ScanCount > 4)ScanCount = 0;
	}
}
void Led_On(void)
{
	LED_IndicatorOnFlag = 1;
}
void Led_Off(void)
{
	L1_OUTPUT;L2_OUTPUT;L3_OUTPUT; 
	L1_RESET;L2_RESET;L3_RESET;  
	LED_IndicatorOnFlag = 0;
}
void APP_ConfigTIM1(void) //1500
{
	/*配置TIM1*/
	LL_TIM_InitTypeDef TIM1CountInit = {0};

	/*使能TIM1时钟*/
	LL_APB1_GRP2_EnableClock(RCC_APBENR2_TIM1EN);

	TIM1CountInit.ClockDivision       = LL_TIM_CLOCKDIVISION_DIV1;	/* 不分频             */
	TIM1CountInit.CounterMode         = LL_TIM_COUNTERMODE_UP;   	/* 计数模式：向上计数 */
	TIM1CountInit.Prescaler           = 0;                   		/* 时钟预分频：8000   */
	TIM1CountInit.Autoreload          = RGB_PWM;                   	/* 自动重装载值：1000 */
	TIM1CountInit.RepetitionCounter   = 0;                      	/* 重复计数值：0      */
	
	DEBUG_INFO("----------------------------TIM1 CONFIG----------------------------");
	DEBUG_INFO("TIM11CountInit.Prescaler:%d",TIM1CountInit.Prescaler );
	DEBUG_INFO("TIM11CountInit.Autoreload:%d",TIM1CountInit.Autoreload );
	DEBUG_INFO("NVIC_SetPriority:%d ", 1 );
	
	/*初始化TIM1*/
	LL_TIM_Init(TIM1,&TIM1CountInit);

	/*清除更新标志位*/
	LL_TIM_ClearFlag_UPDATE(TIM1);

	/*使能UPDATE中断*/
//	LL_TIM_EnableIT_UPDATE(TIM1);

	/*使能TIM1计数器*/
	LL_TIM_EnableCounter(TIM1);
	
	/*主输出使能*/   //给PWM用
	LL_TIM_EnableAllOutputs(TIM1);

	/*开启UPDATE中断请求*/
	NVIC_EnableIRQ(TIM1_BRK_UP_TRG_COM_IRQn);
	NVIC_SetPriority(TIM1_BRK_UP_TRG_COM_IRQn,0);
}
void APP_ConfigPWMChannel(void)
{
	LL_GPIO_InitTypeDef TIM1CH1MapInit= {0};
	LL_TIM_OC_InitTypeDef TIM_OC_Initstruct ={0};

	TIM1CH1MapInit.Pin        = LL_GPIO_PIN_0;
	TIM1CH1MapInit.Mode       = LL_GPIO_MODE_ALTERNATE;
	TIM1CH1MapInit.Alternate  = LL_GPIO_AF_2; 
	LL_GPIO_Init(GPIOA,&TIM1CH1MapInit);

	TIM1CH1MapInit.Pin        = LL_GPIO_PIN_0;
	TIM1CH1MapInit.Mode       = LL_GPIO_MODE_ALTERNATE;
	TIM1CH1MapInit.Alternate  = LL_GPIO_AF_2; 
	LL_GPIO_Init(GPIOB,&TIM1CH1MapInit);

	TIM1CH1MapInit.Pin        = LL_GPIO_PIN_1;
	TIM1CH1MapInit.Mode       = LL_GPIO_MODE_ALTERNATE;
	TIM1CH1MapInit.Alternate  = LL_GPIO_AF_3; 
	LL_GPIO_Init(GPIOB,&TIM1CH1MapInit);

	TIM1CH1MapInit.Pin        = LL_GPIO_PIN_2;
	TIM1CH1MapInit.Mode       = LL_GPIO_MODE_ALTERNATE;
	TIM1CH1MapInit.Alternate  = LL_GPIO_AF_3; 
	LL_GPIO_Init(GPIOB,&TIM1CH1MapInit);
	/*配置PWM通道*/
	TIM_OC_Initstruct.OCMode        = LL_TIM_OCMODE_PWM1;       /* 通道模式：PWM2       */
	TIM_OC_Initstruct.OCState       = LL_TIM_OCSTATE_ENABLE;    /* 通道状态：开启       */
	TIM_OC_Initstruct.OCPolarity    = LL_TIM_OCPOLARITY_LOW;   /* 通道有效极性：高电平 */
	TIM_OC_Initstruct.OCIdleState   = LL_TIM_OCIDLESTATE_LOW;   /* 通道空闲极性：低电平 */
	/*通道1 white */
	TIM_OC_Initstruct.CompareValue  = RGB_PWM;
	/*配置通道1*/
	LL_TIM_OC_Init(TIM1,LL_TIM_CHANNEL_CH1,&TIM_OC_Initstruct);
	/*通道2 BLUE*/
	TIM_OC_Initstruct.CompareValue  = RGB_PWM;
	/*配置通道2*/
	LL_TIM_OC_Init(TIM1,LL_TIM_CHANNEL_CH2,&TIM_OC_Initstruct);
	/*通道3 RED */  
	TIM_OC_Initstruct.CompareValue  = RGB_PWM;
	/*配置通道3*/
	LL_TIM_OC_Init(TIM1,LL_TIM_CHANNEL_CH3,&TIM_OC_Initstruct);
	/*通道3 GREEN */  
	TIM_OC_Initstruct.CompareValue  = RGB_PWM;
	/*配置通道4*/
	LL_TIM_OC_Init(TIM1,LL_TIM_CHANNEL_CH4,&TIM_OC_Initstruct);
}

void APP_DeConfigPWMChannel(void)
{
	LL_GPIO_InitTypeDef TIM1CH1MapInit= {0};
	LL_TIM_OC_InitTypeDef TIM_OC_Initstruct ={0};

	TIM1CH1MapInit.Mode = LL_GPIO_MODE_OUTPUT; 
	TIM1CH1MapInit.Speed = LL_GPIO_SPEED_FREQ_MEDIUM;
	TIM1CH1MapInit.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	TIM1CH1MapInit.Pull = LL_GPIO_PULL_NO;

	TIM1CH1MapInit.Pin        = LL_GPIO_PIN_0;
	LL_GPIO_ResetOutputPin(GPIOA,LL_GPIO_PIN_0);
	LL_GPIO_Init(GPIOA,&TIM1CH1MapInit);

	TIM1CH1MapInit.Pin        = LL_GPIO_PIN_0;
	LL_GPIO_ResetOutputPin(GPIOB,LL_GPIO_PIN_0);
	LL_GPIO_Init(GPIOB,&TIM1CH1MapInit);

	TIM1CH1MapInit.Pin        = LL_GPIO_PIN_1;
	LL_GPIO_ResetOutputPin(GPIOB,LL_GPIO_PIN_1);
	LL_GPIO_Init(GPIOB,&TIM1CH1MapInit);
	
	TIM1CH1MapInit.Pin        = LL_GPIO_PIN_2;
	LL_GPIO_ResetOutputPin(GPIOB,LL_GPIO_PIN_2);
	LL_GPIO_Init(GPIOB,&TIM1CH1MapInit);

	/*配置PWM通道*/
	TIM_OC_Initstruct.OCMode        = LL_TIM_OCMODE_PWM1;       /* 通道模式：PWM2       */
	TIM_OC_Initstruct.OCState       = LL_TIM_OCSTATE_DISABLE;    /* 通道状态：开启       */
	TIM_OC_Initstruct.OCPolarity    = LL_TIM_OCPOLARITY_LOW;   /* 通道有效极性：高电平 */
	TIM_OC_Initstruct.OCIdleState   = LL_TIM_OCIDLESTATE_LOW;   /* 通道空闲极性：低电平 */
	/*通道1 white */
	TIM_OC_Initstruct.CompareValue  = RGB_PWM;
	/*配置通道1*/
	LL_TIM_OC_Init(TIM1,LL_TIM_CHANNEL_CH1,&TIM_OC_Initstruct);
	/*通道2 BLUE*/
	TIM_OC_Initstruct.CompareValue  = RGB_PWM;
	/*配置通道2*/
	LL_TIM_OC_Init(TIM1,LL_TIM_CHANNEL_CH2,&TIM_OC_Initstruct);
	/*通道3 RED */  
	TIM_OC_Initstruct.CompareValue  = RGB_PWM;
	/*配置通道3*/
	LL_TIM_OC_Init(TIM1,LL_TIM_CHANNEL_CH3,&TIM_OC_Initstruct);
	/*通道3 GREEN */  
	TIM_OC_Initstruct.CompareValue  = RGB_PWM;
	/*配置通道4*/
	LL_TIM_OC_Init(TIM1,LL_TIM_CHANNEL_CH4,&TIM_OC_Initstruct);
}
void LED_RGB_init(void)
{
	APP_ConfigTIM1();
	APP_ConfigPWMChannel();
	//开机默认100%的白光
	memset(&RGB,0,sizeof(xRGB));
	RGB.h = 0;
	RGB.s = 0;
	RGB.v = 1000;
	RGB.Setv = 1000;
}	
void LED_RGB_Off_Handle(void)
{
	if(RGB.OnFlag == 1)
	{
		if(Bat.Status == BAT_DISCHARGE)
		{
			Led_Off();
		}
		
		RGB.OnFlag = 0;
		RGB.Time = 0;
		
		RGB.SetOffTime = 0;
		RGB.StepTime = 0;
		RGB.StepTimecnt = 0;
		RGB.ResetTime = 0;
		
		mcu_dp_bool_update(DPID_SWITCH_LED,RGB.OnFlag);
		RGB.Command = IR_LEDOFF;
	}
}
void LED_RGB_To_OnStatus()
{
	RGB.ColourSwTimer = 0;
	if(RGB.OnFlag == 0)
	{
		Led_On();
		RGB.OnFlag = 1;
		LED_IndicatorOnFlag = 1;
		//配网间隔
		RGB.ResetTime = 0;
		RGB.Time = 0;
		//降序和步进
		RGB.SetOffTime = 0;
		RGB.StepTime = 0;
		RGB.StepTimecnt = 0;
		//睡眠时间清零
		Sys.SleepTimeCount = 0;
		mcu_dp_bool_update(DPID_SWITCH_LED,RGB.OnFlag);
	}
}
void LED_RGB_On_Handle(void)
{
	
	LED_RGB_SetHSV(RGB.h,RGB.s,RGB.Setv);
	LED_RGB_SetDisplayHSV(RGB.h,RGB.s,0);
	RGB.ColourSwTimer = 0;
	//if(RGB.OnFlag == 0)
	{
		Led_On();
		RGB.OnFlag = 1;
		LED_IndicatorOnFlag = 1;//开启指示灯
		//配网间隔
		RGB.ResetTime = 0;
		RGB.Time = 0;
		
		//降序和步进
		RGB.SetOffTime = 0;
		RGB.StepTime = 0;
		RGB.StepTimecnt = 0;
		
		//睡眠时间清零
		Sys.SleepTimeCount = 0;
		
		mcu_dp_bool_update(DPID_SWITCH_LED,RGB.OnFlag);
		//RGB.Command = IR_LEDON;
	}
}
void LED_DisplayBatGear_Handle(void)  // 100ms
{
	static uint8_t low_flash,charge_flash,flash_cnt;
	low_flash = !low_flash; 		  //低压闪烁
	if(++flash_cnt >= 5)
	{
		flash_cnt = 0;
		charge_flash = !charge_flash;
	}
	if(Bat.Status == BAT_DISCHARGE)   //放电
	{
		if(Bat.Gera)LED_DisplayTemp = Bat.Gera;
		else 
		{
			if(Bat.ReCheckFlag)  LED_DisplayTemp= 1;
			else LED_DisplayTemp= low_flash;
		}
	}
	else if(Bat.Status == BAT_CHARGE) //充电
	{
		if(charge_flash )
		{
			if(Bat.Gera)LED_DisplayTemp =  Bat.Gera - 1;
			else  LED_DisplayTemp = 1;
		}
		else LED_DisplayTemp =  Bat.Gera ;
	}
	else							  //满电
	{
		LED_DisplayTemp =  Bat.Gera ;
	}
}
void IR_RGB_MODE_handle()
{
	static uint16_t status=0,time_cnt;
	if(++RGB.ColourSwTimer >= 1000)
	{
		RGB.ColourSwTimer = 0;
		if(++status>2)status = 0;
		switch(status)
		{
			case 0:
				LED_RGB_SetHSV(0,1000,RGB.Setv);
				LED_RGB_SetDisplayHSV(0,1000,RGB.Setv);
			//	LED_RGB_HSVdisplay(0,1000,RGB.Setv);
				break;
			case 1:
				LED_RGB_SetHSV(120,1000,RGB.Setv);
				LED_RGB_SetDisplayHSV(120,1000,RGB.Setv);
				//LED_RGB_HSVdisplay(120,1000,RGB.Setv);
				break;
			case 2:
				LED_RGB_SetHSV(240,1000,RGB.Setv);
				LED_RGB_SetDisplayHSV(240,1000,RGB.Setv);
				//LED_RGB_HSVdisplay(240,1000,RGB.Setv);
				break;
		}
	}
}
void RGB_App_Handle(void) // 5MS时间
{
	static uint8_t Timer5ms;
	/*if (++RGB.ResetTime > 800) //4S 后清零  配用计数
	{
		RGB.ResetTime = 0;
		RGB.ResetCnt  = 0;
	}*/
	if (RGB.OnFlag) 
	{	
		RGB.Time++; //UINT 5MS
		RGB.StepTimecnt++; //UINT 5MS
		if(Bat.SaveEnergMode && Bat.Status == BAT_DISCHARGE) //节能模式 根据时间和电池调节亮度
		{
			if (RGB.Time < TIME_10MIN) //nothing
			{
				RGB.StepTime = 1 * 1000; // 时间步进间距68S
				RGB.Powersaving_set_v = 1000;	//测试看有没有降序用 
			}
			else if (RGB.Time >= TIME_10MIN && RGB.Time < TIME_1H)
			{
				if (Bat.Percent > 70)
					RGB.StepTime = ( Bat.Percent - 70 ) * 1000; // 时间步进间距30S
				else
					RGB.StepTime = 5 * 1000; //
				RGB.Powersaving_set_v = 700;	   // 800
			}
			else if (RGB.Time >= TIME_1H && RGB.Time < TIME_5H) // 1-2小时 70%-50%
			{
				RGB.StepTime = Bat.Percent  * 1000; // 时间步进间距90S
				RGB.Powersaving_set_v = 500;			   // 600
			}
			else if (RGB.Time >= TIME_5H && RGB.Time < TIME_11H)
			{
				RGB.StepTime = (Bat.Percent + 20)  * 1000; // 时间步进间距120S
				RGB.Powersaving_set_v = 200;					// 400
			}
			else if (RGB.Time >= TIME_11H)
			{
				RGB.StepTime = 250 * 1000;; // 时间步进间距250S
				RGB.Powersaving_set_v = 100;	 //
			}
			if (RGB.Time < TIME_18H)
			{
				if (RGB.StepTimecnt > RGB.StepTime)
				{
					RGB.StepTimecnt = 0;
					if(RGB.v > RGB.Powersaving_set_v)
					{
						RGB.v -= 5; //降序  0.5%的亮度
						
						TY_Updata_Bright();
					}
					//LED_RGB_HSVdisplay(RGB.h,RGB.s,RGB.v);
					DEBUG_INFO("SaveEnergMode  v = %d",RGB.v);
				}
			}
		}
		if(RGB.Time>RGB.SetOffTime &&  RGB.SetOffTime ) //定时关闭
		{ 
			LED_RGB_Off_Handle();
			RGB.Command = IR_LEDOFF;
		}
	}
	static uint8_t TY_Reset_Mode_Status = 0;
	if(bt_work_state == 0) //未配网模式
	{                                                                                                                                                                                                                                                                                                                                                                                                                 
		if(RGB.OnFlag) 
		{
			switch(TY_Reset_Mode_Status)
			{
				case 0:
					if(RGB.Dispaly_v)RGB.Dispaly_v--;
					else TY_Reset_Mode_Status = 1;
					break;
				case 1:	
					if(RGB.Dispaly_v<600)RGB.Dispaly_v++;
					else TY_Reset_Mode_Status = 0;
					break;
				default:
					TY_Reset_Mode_Status = 0;
					break;
			}
			if(RGB.W_Mode) //白光模式
			{
				LED_RGB_HSVdisplay(RGB.h,RGB.s,RGB.Dispaly_v/3);
				LED_RGB_W_display(RGB.Dispaly_v);
			}
			else
			{
				LED_RGB_HSVdisplay(RGB.h,RGB.s,RGB.Dispaly_v);
				LED_RGB_W_display(0);
			}
		}
		else 
		{
			LED_RGB_display(0,0,0);
			RGB.Dispaly_v = 0;
			LED_RGB_W_display(0);
		}
	}
	else
	{
		switch(RGB.Command) //RGB 模式
		{
			//亮度渐变上去
			case TY_ON_MODE:
			case IR_RED_MODE:
			case IR_GREEN_MODE:
			case IR_BLUE_MODE:
			case IR_LEDON:
			case TY_scene_MODE:
				if(RGB.Dispaly_v<RGB.v)RGB.Dispaly_v+=1;
				if(RGB.Dispaly_v>RGB.v)RGB.Dispaly_v-=1;
				if(RGB.Dispaly_w<RGB.w)RGB.Dispaly_w+=1;
				if(RGB.Dispaly_w<RGB.w)RGB.Dispaly_w+=1;
				LED_RGB_HSVdisplay(RGB.h,RGB.s,RGB.Dispaly_v);
				LED_RGB_W_display(0);
				break;

			//亮度渐灭下去
			case TY_OFF_MODE:
			case IR_LEDOFF:
			case IR_POWEROFF:
				if(RGB.Dispaly_v)
				{
					if(RGB.Dispaly_v)RGB.Dispaly_v--;
					else RGB.Dispaly_v=0;
//					if(RGB.Dispaly_w>5)RGB.Dispaly_w-=5;
//					else RGB.Dispaly_w=0;
					if(RGB.LastCommand == IR_WRITE_MODE)
					{
						if(RGB.W_Mode)
						{
							LED_RGB_HSVdisplay(RGB.h,RGB.s,RGB.Dispaly_v/3);
							LED_RGB_W_display(RGB.Dispaly_v);
						}
						else
						{
							LED_RGB_HSVdisplay(RGB.h,RGB.s,RGB.Dispaly_v);
							LED_RGB_W_display(0);
						}
					}
					else 
					{
						LED_RGB_HSVdisplay(RGB.h,RGB.s,RGB.Dispaly_v);
						LED_RGB_W_display(0);
					}
				}		
				break;

			//RGB 三色互换	
			case IR_RGB_MODE: 
				IR_RGB_MODE_handle();
				LED_RGB_W_display(0); //W灯关闭
				if(RGB.Dispaly_v<RGB.v)RGB.Dispaly_v+=1;
				if(RGB.Dispaly_v>RGB.v)RGB.Dispaly_v-=1;
				LED_RGB_HSVdisplay(RGB.h,RGB.s,RGB.Dispaly_v);
				break;	

			//RGB 调色
			case TY_colour_MODE:
				if(RGB.Dispaly_v<RGB.v)RGB.Dispaly_v+=1;
				if(RGB.Dispaly_v>RGB.v)RGB.Dispaly_v-=1;
				LED_RGB_HSVdisplay(RGB.h,RGB.s,RGB.Dispaly_v);
				LED_RGB_W_display(0);
				break;	
			
			//白光
			case IR_WRITE_MODE:
			case TY_temp_MODE:
			case TY_bright_MODE:	
				if(RGB.Dispaly_v<RGB.v)RGB.Dispaly_v+=1;
				if(RGB.Dispaly_v>RGB.v)RGB.Dispaly_v-=1;
				if(!RGB.wflash && RGB.Time > 75*5)
				{
					RGB.wflash = 1;
					RGB.Dispaly_v = 0;
					
				}
				if(RGB.W_Mode)
				{
					LED_RGB_HSVdisplay(RGB.h,RGB.s,RGB.Dispaly_v/3);
					LED_RGB_W_display(RGB.Dispaly_v);
				}
				else
				{
					LED_RGB_HSVdisplay(RGB.h,RGB.s,RGB.Dispaly_v);
					LED_RGB_W_display(0);
				}
				break;

			//音乐律动 渐变
			case TY_MUSIC_MODE:
				if(++Timer5ms >= 7)
				{
					Timer5ms = 0;
					if(RGB.Dispaly_h<RGB.h)RGB.Dispaly_h++;
					if(RGB.Dispaly_h>RGB.h)RGB.Dispaly_h--;
					if(RGB.Dispaly_s<RGB.s)RGB.Dispaly_s++;
					if(RGB.Dispaly_s>RGB.s)RGB.Dispaly_s--;
					if(RGB.Dispaly_v<RGB.v)RGB.Dispaly_v++;
					if(RGB.Dispaly_v>RGB.v)RGB.Dispaly_v--;
					LED_RGB_HSVdisplay(RGB.Dispaly_h,RGB.Dispaly_s,RGB.Dispaly_v);
					LED_RGB_W_display(0); //W灯关闭
				}
				break;
		}
	}
}
