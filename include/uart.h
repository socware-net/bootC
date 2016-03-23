#ifndef __UART_H__
#define __UART_H__

void uart_init(unsigned _freq);

void uart_putc(unsigned int c);

int uart_getc(void);

#endif
