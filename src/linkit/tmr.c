#include "plt.h"
#include "io.h"
#include "_soc.h"

#define GPT_CTRL_EN	1

#define time_after(a,b)  ((long)(b) - (long)(a) < 0)
#define time_before(a,b) time_after(b,a)

static void delay_time(int n, unsigned count)
{
	unsigned end_count, current;
	unsigned b;
	if (n == 0)
		b = BASE_GPT + 0x40;
	else if (n == 1)
		b = BASE_GPT + 0x44;
	else
		b = BASE_GPT + 0x34;
	end_count = reg(b) + count + 1;
	current = reg(b);
	while (time_before(current, end_count)) {
		current = reg(b);
	}
}

static void delay_ms(int n, unsigned ms)
{
	unsigned count;

	count = 32 * ms;
	count += (7 * ms) / 10;
	count += (6 * ms) / 100;
	count += (8 * ms) / 1000;
	delay_time(n, count);
}

static int started;

void tmr_delay(int ms)
{
	int n = 2;
	unsigned b = (BASE_GPT + (n + 1) * 0x10);
	if (!started) {
		reg(b + 0) |= GPT_CTRL_EN | (1 << 1);
		started = 1;
	}
	delay_ms(n, ms);
}
