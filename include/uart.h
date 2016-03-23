#ifndef UART0325
#define UART0325

void uart_init(unsigned _freq);

void uart_putc(unsigned int c);

int uart_getc(void);

#endif
