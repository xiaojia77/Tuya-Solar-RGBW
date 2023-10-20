
#ifndef __USART_H
#define __USART_H

#include <stdio.h>
#include "main.h"
#include "py32f002b_ll_usart.h"

#define DEBUG_USART USART1

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

