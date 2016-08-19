#include "plt.h"
#include "io.h"
#include "_soc.h"
#include "term.h"
#include "xmodem.h"
#include "flash.h"
#include "stm32f4xx.h"
#include "system_stm32f4xx.h"

#include <string.h>

unsigned clk_init(void)
{
	SystemCoreClockUpdate();
	return SystemCoreClock;
}

void pinmux_init()
{
	GPIO_InitTypeDef GPIO_InitStructure;

	// Enable the GPIOA peripheral clock.
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	// Make PA2, PA3 as alternative function of USART2.
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

	// Initialize PA2, PA3.
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

}

void boot(unsigned addr)
{
	char *arg, *p;
	if (!addr)
		addr = BASE_FLASH + 0x8000;
	p = (char *)(BASE_SRAM + SZ_SRAM - 256);
	*(unsigned *)p = 0xBEEFBEEF;
	p += 4;
	while ((arg = strtok(0, " "))) {
		strcpy(p, arg);
		p += strlen(arg) + 1;
	}
	*p = 0;
	_printf("go %x\r\n", addr);
	asm volatile ("bx  %0\n"::"r" (addr | 1));
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
