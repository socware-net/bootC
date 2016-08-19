#include "plt.h"
#include "io.h"
#include "_soc.h"
#include "term.h"
#include "xmodem.h"
#include "flash.h"
#include "stm32f4xx.h"
#include "system_stm32f4xx.h"

#include <string.h>

unsigned freq_init(void)
{
	SystemCoreClockUpdate();
	return SystemCoreClock;
}

void pinmux(unsigned p, unsigned f)
{
}

void pinmux_init()
{
}

void boot(unsigned addr)
{
}

typedef struct {
	char *desc;
	unsigned off, sz;
} part_t;

part_t parts[] = {
	{0, 0, 0},
};

struct {
	unsigned pos, end;
	int ov;
} usta;

static void update_flash(void *priv, unsigned char *b, int l)
{

	if (l < 0)
		return;
	if (usta.pos + l > usta.end) {
		usta.ov = 1;
		return;
	}
	if (!(usta.pos & BI_TMSK(flash_align)))
		_flash_erase(usta.pos, flash_page);
	_flash_write(usta.pos, l, b);
	usta.pos += l;
}

void update(int n)
{
	if (n < 0 || n >= sizeof(parts) / sizeof(part_t)) {
		int i;
		for (i = 0; parts[i].desc; i++)
			_printf("%d %s %x %dK\r\n", i,
				parts[i].desc, parts[i].off, parts[i].sz >> 10);
		return;
	}
	usta.pos = parts[n].off;
	usta.end = parts[n].off + parts[n].sz;
	usta.ov = 0;
	xmodem_block_rx(update_flash, 0);
	if (usta.ov)
		_printf("-1");
}

unsigned plt_ram, plt_ram_sz;

void plt_init()
{
	plt_ram = BASE_SRAM;
	plt_ram_sz = (16 + 112) << 10;
}
