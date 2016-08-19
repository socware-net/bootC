#include "io.h"
#include "_soc.h"
#include "uart.h"
#include "stm32f4xx_usart.h"

#include <string.h>

#define UART	USART2

void uart_init(unsigned _freq)
{
	USART_InitTypeDef USART_InitStructure;

	// Enable the USART2 peripheral clock.
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	// 115200 8N1, non-flow
	USART_InitStructure.USART_BaudRate = BAUD;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl =
	    USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART, &USART_InitStructure);

	USART_Cmd(UART, ENABLE);
}

int uart_getc(void)
{
	if (USART_GetFlagStatus(UART, USART_FLAG_RXNE) == RESET)
		return -1;
	return USART_ReceiveData(UART);
}

void uart_putc(unsigned int c)
{
	while (USART_GetFlagStatus(UART, USART_FLAG_TC) == RESET) ;
	USART_SendData(UART, (uint16_t) c);
	return;
}
