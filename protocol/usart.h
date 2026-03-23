#ifndef _usart_h_
#define _usart_h_

#include "stm32f10x.h"
#include "stdio.h"

void Usart1Init(u32 bound);
void Usart1DMASendData(uint8_t *sendBuff, u16 len);
void UartInit(void);

#endif

