
#include "time.h"
uint32_t time1_count;
uint32_t time4_count;
static void APP_ConfigLptimClock(void)
{
  /* 使能LSI */
  LL_RCC_LSI_Enable();
  while(LL_RCC_LSI_IsReady() != 1)
  {
  }
  
  /* 选择LTPIM的时钟源为LSI */
  LL_RCC_SetLPTIMClockSource(LL_RCC_LPTIM1_CLKSOURCE_LSI);
  
  /* 使能LPTIM */
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_LPTIM1);
}

/**
  * @brief  配置LPTIM
  * @param  无
  * @retval 无
  */
static void APP_ConfigLptim(void)
{
  /* 使能NVIC中断 */
  NVIC_SetPriority(LPTIM1_IRQn, 0);
  NVIC_EnableIRQ(LPTIM1_IRQn);
  
  /* 使能LPTIM匹配中断 */
  LL_LPTIM_EnableIT_ARRM(LPTIM);
  
  /* 使能LPTIM */
  //LL_LPTIM_Enable(LPTIM);
  
  /* 设置重载值 */
  //LL_LPTIM_SetAutoReload(LPTIM,1000); //7680 2分钟唤醒一次
  
  /* 启动连续次模式 */
  LL_LPTIM_StartCounter(LPTIM, LL_LPTIM_OPERATING_MODE_CONTINUOUS);
	
}
void Lptim_Init(void)
{
	LL_LPTIM_InitTypeDef LPTIM_InitStruct = {0};
	// 配置LPTIM对应的EXTI Line29为事件唤醒模式 //
	LL_EXTI_EnableIT(LL_EXTI_LINE_29);   // 屏蔽EXTI Line29中断唤醒 //
	//LL_EXTI_EnableEvent(LL_EXTI_LINE_29); // 使能EXTI Line29事件唤醒 //

	// 配置LPTIM的时钟源为LSI //
	APP_ConfigLptimClock();

	// 初始化LPTIM //
	LPTIM_InitStruct.Prescaler = LL_LPTIM_PRESCALER_DIV128;        // 128分频 //
	LPTIM_InitStruct.UpdateMode = LL_LPTIM_UPDATE_MODE_IMMEDIATE;  // 立即更新模式 //
	if (LL_LPTIM_Init(LPTIM, &LPTIM_InitStruct) != SUCCESS)
	{
		APP_ErrorHandler();
	}
	APP_ConfigLptim();
}

void APP_Time4IRQCallback()
{

}
void APP_Time1IRQCallback()
{

}
