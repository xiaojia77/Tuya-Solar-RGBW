
#include "gpio.h"

void GPIO_Init()	//初始化
{

	/* 初始化GPIO */
	LL_GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.Pin = GPIO_MOS_REAL_PIN;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT; 
	GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
	GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIO_MOS_REAL_PORT, &GPIO_InitStruct);
	LL_GPIO_ResetOutputPin(GPIO_MOS_REAL_PORT, GPIO_MOS_REAL_PIN);
	
	
	GPIO_InitStruct.Pin = GPIO_BAT_EN_PIN;
	LL_GPIO_Init(GPIO_BAT_EN_PORT, &GPIO_InitStruct);
	LL_GPIO_ResetOutputPin(GPIO_BAT_EN_PORT, GPIO_BAT_EN_PIN);
	
	
	GPIO_InitStruct.Pin = GPIO_WB_EN_PIN;
	LL_GPIO_Init(GPIO_WB_EN_PORT, &GPIO_InitStruct);
	LL_GPIO_ResetOutputPin(GPIO_WB_EN_PORT, GPIO_WB_EN_PIN);
	
	GPIO_InitStruct.Pin = GPIO_WB_PIN;
	GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
	LL_GPIO_Init(GPIO_WB_PORT, &GPIO_InitStruct);
	
}
