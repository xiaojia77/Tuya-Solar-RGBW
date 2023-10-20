
#ifndef __USART_H
#define __USART_H

#include <stdio.h>
#include "main.h"
#include "py32f002b_ll_usart.h"

#define DEBUG_USART USART1


#define GPIO_RX_PORT GPIOA
#define GPIO_RX_PIN LL_GPIO_PIN_4
#define RX_EXTI_LINE	LL_EXTI_LINE_4
#define RX_EXTI_CONFIG_PORT	LL_EXTI_CONFIG_PORTA
#define RX_EXTI_CONFIG_LINE LL_EXTI_CONFIG_LINE4
#define RX_EXTI_IRQn	EXTI4_15_IRQn


extern uint8_t aTxBuffer[30];
extern uint8_t *TxBuff;
extern __IO uint16_t TxSize;
extern __IO uint16_t TxCount;
extern __IO ITStatus UartReady;

extern void APP_ConfigUsart(USART_TypeDef *USARTx,uint32_t BaudRate);
extern void APP_UsartTransmit_IT(USART_TypeDef *USARTx, uint8_t *pData, uint16_t Size);
extern void APP_UsartReceive_IT(USART_TypeDef *USARTx, uint8_t *pData, uint16_t Size);
extern void APP_UsartIRQCallback(USART_TypeDef *USARTx);

#endif

