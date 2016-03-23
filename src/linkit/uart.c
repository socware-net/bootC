#include "io.h"
#include "_soc.h"
#include "uart.h"

#define UART_WLS_8		0x0003
#define UART_NONE_PARITY	0x0000
#define UART_1_STOP		0x0000

#define UART_RBR                (0x0)
#define UART_THR                (0x0)
#define UART_IER                (0x4)
#define UART_IIR                (0x8)
#define UART_FCR                (0x8)
#define UART_LCR                (0xc)
#define UART_MCR                (0x10)
#define UART_LSR                (0x14)
#define UART_MSR                (0x18)
#define UART_SCR                (0x1c)
#define UART_DLL                (0x0)
#define UART_DLH                (0x4)
#define UART_EFR                (0x8)
#define UART_AUTOBAUD_EN        (0x20)
#define UART_RATE_STEP          (0x24)
#define UART_STEP_COUNT         (0x28)
#define UART_SAMPLE_COUNT       (0x2c)
#define UART_AUTOBAUD_REG       (0x30)
#define UART_RATE_FIX_REG       (0x34)
#define UART_ESCAPE_DATA        (0x40)
#define UART_ESCAPE_EN          (0x44)
#define UART_FRACDIV_L          (0x54)
#define UART_FRACDIV_M          (0x58)

typedef unsigned short u16;

static unsigned freq;

void uart_init(unsigned _freq)
{
	unsigned baudrate = BAUD;
	u16 databit = UART_WLS_8;
	u16 parity = UART_NONE_PARITY;
	u16 stopbit = UART_1_STOP;
	unsigned UART_BASE = BASE_UART0;
	u16 fraction_L_mapping[] =
	    { 0x00, 0x10, 0x44, 0x92, 0x29, 0xaa, 0xb6, 0xdb, 0xad, 0xff,
		0xff
	};
	u16 fraction_M_mapping[] =
	    { 0x00, 0x00, 0x00, 0x00, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01,
		0x03
	};
	u16 control_word;
	unsigned data, uart_lcr, high_speed_div, sample_count, sample_point,
	    fraction;

	freq = _freq;
	// 100, 300, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200, 230400, 460800, 921600
	reg(UART_BASE + UART_RATE_STEP) = 0x3;
	// based on sampe_count * baud_pulse, baud_rate = system clock frequency / sampe_count
	uart_lcr = reg(UART_BASE + UART_LCR);	// DLAB start

#define UART_LCR_DLAB                   0x0080
	reg(UART_BASE + UART_LCR) = (uart_lcr | UART_LCR_DLAB);
	data = freq / baudrate;
	high_speed_div = (data >> 8) + 1;	// divided by 256
	{
		sample_count = data / high_speed_div - 1;
		if (sample_count == 3) {
			sample_point = 0;
		} else {
			sample_point = sample_count / 2 - 1;
		}
		//if (!(data >= 4)) {
		//}
	}
	fraction = (freq * 10 / baudrate / data - (sample_count + 1) * 10) % 10;
	reg(UART_BASE + UART_DLL) = (high_speed_div & 0x00ff);
	reg(UART_BASE + UART_DLH) = ((high_speed_div >> 8) & 0x00ff);
	reg(UART_BASE + UART_STEP_COUNT) = sample_count;
	reg(UART_BASE + UART_SAMPLE_COUNT) = sample_point;
	reg(UART_BASE + UART_FRACDIV_M) = fraction_M_mapping[fraction];
	reg(UART_BASE + UART_FRACDIV_L) = fraction_L_mapping[fraction];
	reg(UART_BASE + UART_LCR) = (uart_lcr);	/* DLAB end */
	reg(UART_BASE + UART_FCR) = 0xF7;
#define UART_DATA_MASK                  0x0003
#define UART_PARITY_MASK                0x0038
#define UART_STOP_MASK                  0x0004

	control_word = readw(UART_BASE + UART_LCR);	/* DLAB start */
	control_word &= ~UART_DATA_MASK;
	control_word |= databit;
	control_word &= ~UART_STOP_MASK;
	control_word |= stopbit;
	control_word &= ~UART_PARITY_MASK;
	control_word |= parity;
	writew(control_word, (void *)(UART_BASE + UART_LCR));
}

int uart_getc(void)
{
	char c;
	if (reg(BASE_UART0 + UART_LSR) & 0x1) {
		c = reg(BASE_UART0 + UART_RBR);
		return c;
	} else {
		return -1;
	}
}

void uart_putc(unsigned int _c)
{
	char c = _c;
	while (!(reg(BASE_UART0 + UART_LSR) & 0x20)) ;
	reg(BASE_UART0 + UART_RBR) = c;
	return;
}
