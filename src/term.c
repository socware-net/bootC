#include <stdarg.h>
#include "uart.h"

void uart_putc(unsigned int _c);

#define BL_SLIM_UDIV_R(N, D, R) (((R)=(N)%(D)), ((N)/(D)))
#define BL_SLIM_UDIV(N, D) ((N)/(D))
#define BL_SLIM_UMOD(N, D) ((N)%(D))
#define BL_MAXCHARS     512
#define BL_MAXFRACT     10000
#define BL_NUM_FRACT     4

static char buf[BL_MAXCHARS] = { 0 };

static void itoa(char **buf, unsigned i, unsigned base)
{
	char *s;
#define LEN   20
	unsigned rem;
	static char rev[LEN + 1];

	rev[LEN] = 0;
	if (i == 0) {
		(*buf)[0] = '0';
		++(*buf);
		return;
	}
	s = &rev[LEN];
	while (i) {
		i = BL_SLIM_UDIV_R(i, base, rem);
		if (rem < 10) {
			*--s = rem + '0';
		} else if (base == 16) {
			*--s = "abcdef"[rem - 10];
		}
	}
	while (*s) {
		(*buf)[0] = *s++;
		++(*buf);
	}
}

static void _print(char *fmt, va_list ap)
{
	int ival;
	char *p, *sval;
	char *bp, cval;

	bp = buf;
	*bp = 0;

	for (p = fmt; *p; p++) {
		if (*p != '%') {
			*bp++ = *p;
			continue;
		}
		switch (*++p) {
		case 'd':
			ival = va_arg(ap, int);
			if (ival < 0) {
				*bp++ = '-';
				ival = -ival;
			}
			itoa(&bp, ival, 10);
			break;

		case 'x':
			ival = va_arg(ap, int);
			if (ival < 0) {
				*bp++ = '-';
				ival = -ival;
			}
			*bp++ = '0';
			*bp++ = 'x';
			itoa(&bp, ival, 16);
			break;
		case 'c':
			cval = va_arg(ap, int);
			*bp++ = cval;
			break;
		case 's':
			for (sval = va_arg(ap, char *); *sval; sval++) {
				*bp++ = *sval;
			}
			break;
		default:
			break;
		}
	}

	*bp = 0;
	for (bp = buf; *bp; bp++) {
		uart_putc(*bp);
	}
}

void _printf(char *str, ...)
{
	if (str == 0) {
		return;
	}

	va_list ap;
	va_start(ap, str);
	_print(str, ap);
	va_end(ap);
}

static char line[128];

static int pos;

static int c = 0;

char *_gets()
{
	pos = 0;
	do {
		c = uart_getc();
		if (c < 0)
			continue;
		if (c == '\b' && pos > 0) {
			pos--;
			uart_putc(c);
			uart_putc(' ');
		} else if (c == (int)'\r') {
			uart_putc('\n');
		} else if (pos < sizeof(line)) {
			line[pos++] = c;
		}
		uart_putc(c);
	} while (c != (int)'\r');
	line[pos] = 0;
	return line;
}
