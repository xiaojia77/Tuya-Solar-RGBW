

#include "usart.h"
#include "mcu_api.h"

uint8_t aTxBuffer[] = "UART Test \r\n";
uint8_t aRxBuffer[30];

uint8_t *TxBuff = NULL;
__IO uint16_t TxSize = 0;
__IO uint16_t TxCount = 0;

uint8_t *RxBuff = NULL;
__IO uint16_t RxSize = 0;
__IO uint16_t RxCount = 0;

__IO ITStatus UartReady = RESET;

/**
  * @brief  USART配置函数
  * @param  USARTx：USART模块，可以是USART1、USART2
  * @retval 无
  */
void APP_ConfigUsart(USART_TypeDef *USARTx,uint32_t BaudRate)
{
	/*使能时钟、初始化引脚、使能NVIC中断*/
	if (USARTx == USART1) 
	{
		/*使能GPIOB时钟*/
		LL_IOP_GRP1_EnableClock(LL_IOP_GRP1_PERIPH_GPIOB);
		/*使能USART1时钟*/
		LL_APB1_GRP2_EnableClock(LL_APB1_GRP2_PERIPH_USART1);

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
		
		// 选择EXTI0做外部中断输入 
		LL_EXTI_SetEXTISource(RX_EXTI_CONFIG_PORT, RX_EXTI_CONFIG_LINE);

		// 配置EXTI0为中断、上下降沿触发 
		LL_EXTI_InitTypeDef EXTI_InitStruct;
		EXTI_InitStruct.Line = RX_EXTI_LINE;
		EXTI_InitStruct.LineCommand = ENABLE;
		EXTI_InitStruct.Mode = LL_EXTI_MODE_IT;
		EXTI_InitStruct.Trigger = LL_EXTI_TRIGGER_RISING_FALLING;
		LL_EXTI_Init(&EXTI_InitStruct);

		/* 使能中断 */

		NVIC_SetPriority(RX_EXTI_IRQn, 2);
		NVIC_EnableIRQ(RX_EXTI_IRQn);
		LL_EXTI_DisableIT(LL_EXTI_LINE_4);

		/*设置USART1中断优先级*/
		NVIC_SetPriority(USART1_IRQn,0);
		/*使能USART1中断*/
		NVIC_EnableIRQ(USART1_IRQn);
	}
  
	/*配置USART功能*/
	LL_USART_InitTypeDef USART_InitStruct;
	/*设置波特率*/
	USART_InitStruct.BaudRate = BaudRate;
	/*设置数据长度*/
	USART_InitStruct.DataWidth = LL_USART_DATAWIDTH_8B;
	/*停止位*/
	USART_InitStruct.StopBits = LL_USART_STOPBITS_1;
	/*设置校验位*/
	USART_InitStruct.Parity = LL_USART_PARITY_NONE;
	USART_InitStruct.TransferDirection = LL_USART_DIRECTION_TX_RX;
	USART_InitStruct.HardwareFlowControl = LL_USART_HWCONTROL_NONE;
	USART_InitStruct.OverSampling = LL_USART_OVERSAMPLING_16;
	/*USART初始化*/
	LL_USART_Init(USARTx, &USART_InitStruct);

	/*配置为全双工异步模式*/
	LL_USART_ConfigAsyncMode(USARTx);

	/*使能UART模块*/
	LL_USART_Enable(USARTx);
	
	/*使能接收数据寄存器非空中断*/    //使能接收
	LL_USART_EnableIT_RXNE(USARTx);
}
/**
  * @brief  USART发送函数
  * @param  USARTx：USART模块，可以是USART1、USART2
  * @param  pData：发送缓冲区
  * @param  Size：发送缓冲区大小
  * @retval 无
  */
void APP_UsartTransmit_IT(USART_TypeDef *USARTx, uint8_t *pData, uint16_t Size)
{
	TxBuff = pData;
	TxSize = Size;
	TxCount = Size;

	/*使能发送数据寄存器空中断*/
	LL_USART_EnableIT_TXE(USARTx);
}

/**
  * @brief  USART接收函数
  * @param  USARTx：USART模块，可以是USART1、USART2
  * @param  pData：接收缓冲区
  * @param  Size：接收缓冲区大小
  * @retval 无
  */
void APP_UsartReceive_IT(USART_TypeDef *USARTx, uint8_t *pData, uint16_t Size)
{
	RxBuff = pData;
	RxSize = Size;
	RxCount = Size;

	/*使能接收奇偶校验错误中断*/
	LL_USART_EnableIT_PE(USARTx);

	/*使能接收错误中断*/
	LL_USART_EnableIT_ERROR(USARTx);

	/*使能接收数据寄存器非空中断*/
	LL_USART_EnableIT_RXNE(USARTx);
}

/**
  * @brief  USART中断处理函数
  * @param  USARTx：USART模块，可以是USART1、USART2
  * @retval 无
  */
void APP_UsartIRQCallback(USART_TypeDef *USARTx)
{
	
  /* 接收数据寄存器不为空 */

	//接收
	if ((LL_USART_IsActiveFlag_RXNE(USARTx) != RESET) && (LL_USART_IsEnabledIT_RXNE(USARTx) != RESET))
	{
		uart_receive_input(LL_USART_ReceiveData8(USARTx)); 
	}
	LL_USART_ClearFlag_ORE(USARTx); //防止死机

	/* 发送数据寄存器空 */ 
//	if ((LL_USART_IsActiveFlag_TXE(USARTx) != RESET) && (LL_USART_IsEnabledIT_TXE(USARTx) != RESET))
//	{
//	LL_USART_TransmitData8(USARTx, *TxBuff);
//	TxBuff++;
//	if (--TxCount == 0U)
//	{ 
//		LL_USART_DisableIT_TXE(USARTx);
//		
//		LL_USART_EnableIT_TC(USARTx);
//	}
//	return;
//	}

//	/* 发送完成 */
//	if ((LL_USART_IsActiveFlag_TC(USARTx) != RESET) && (LL_USART_IsEnabledIT_TC(USARTx) != RESET))
//	{
//	LL_USART_DisableIT_TC(USARTx);
//	UartReady = SET;

//	return;
//	}
}
#if (defined (__CC_ARM)) || (defined (__ARMCC_VERSION) && (__ARMCC_VERSION >= 6010050))
/**
  * @brief  writes a character to the usart
  * @param  ch
  *         *f
  * @retval the character
  */
int fputc(int ch, FILE *f)
{
	/* Send a byte to USART */
	LL_USART_TransmitData8(DEBUG_USART, ch);
	while (!LL_USART_IsActiveFlag_TC(DEBUG_USART));
	LL_USART_ClearFlag_TC(DEBUG_USART);

	return (ch);
}

/**
  * @brief  get a character from the usart
  * @param  *f
  * @retval a character
  */
int fgetc(FILE *f)
{
	int ch;
	while (!LL_USART_IsActiveFlag_RXNE(DEBUG_USART));
	ch = LL_USART_ReceiveData8(DEBUG_USART);
	return (ch);
}

#elif defined(__ICCARM__)
/**
  * @brief  writes a character to the usart
  * @param  ch
  *         *f
  * @retval the character
  */
int putchar(int ch)
{
  /* Send a byte to USART */
  LL_USART_TransmitData8(DEBUG_USART, ch);
  while (!LL_USART_IsActiveFlag_TC(DEBUG_USART));
  LL_USART_ClearFlag_TC(DEBUG_USART);

  return (ch);
}
#endif
