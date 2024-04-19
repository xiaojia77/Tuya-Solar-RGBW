
#include "iwdg.h"

void APP_IwdgConfig(void)
{
	//uint32_t time = 6000; //MS
	/* 使能LSI */
	LL_RCC_LSI_Enable();
	while (LL_RCC_LSI_IsReady() == 0U) {;}

	/* 使能IWDG */
	LL_IWDG_Enable(IWDG);

	/* 开启写权限 */
	LL_IWDG_EnableWriteAccess(IWDG);

	/* 设置IWDG分频 */
	LL_IWDG_SetPrescaler(IWDG, LL_IWDG_PRESCALER_32); /* T=1MS */

	/* 设置喂狗事件*/
	LL_IWDG_SetReloadCounter(IWDG, 300 ); 
		
	/* IWDG初始化*/
	while (LL_IWDG_IsReady(IWDG) == 0U) {;}
			
	/* 喂狗 */
	LL_IWDG_ReloadCounter(IWDG);
}

