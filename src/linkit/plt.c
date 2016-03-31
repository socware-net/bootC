#include "plt.h"
#include "io.h"
#include "_soc.h"
#include "term.h"
#include "xmodem.h"
#include "flash.h"

#define FLASH_LOADER_SIZE           0x8000	/*  32KB */
#define FLASH_COMM_CONF_SIZE        0x1000	/*   4KB */
#define FLASH_STA_CONF_SIZE         0x1000	/*   4KB */
#define FLASH_AP_CONF_SIZE          0x1000	/*   4KB */
#define FLASH_N9_RAM_CODE_SIZE      0x71000	/* 452KB */
#define FLASH_CM4_XIP_CODE_SIZE     0xBF000	/* 764KB */
#define FLASH_TMP_SIZE              0xBF000	/* 764KB */
#define FLASH_USR_CONF_SIZE         0x5000	/*  20KB */
#define FLASH_EEPROM_SIZE         	0x1000	/*  4KB */

#define CM4_FLASH_LOADER_ADDR       0x0
#define CM4_FLASH_COMM_CONF_ADDR    (CM4_FLASH_LOADER_ADDR     + FLASH_LOADER_SIZE)
#define CM4_FLASH_STA_CONF_ADDR     (CM4_FLASH_COMM_CONF_ADDR  + FLASH_COMM_CONF_SIZE)
#define CM4_FLASH_AP_CONF_ADDR      (CM4_FLASH_STA_CONF_ADDR   + FLASH_STA_CONF_SIZE)
#define CM4_FLASH_N9_RAMCODE_ADDR   (CM4_FLASH_AP_CONF_ADDR    + FLASH_AP_CONF_SIZE)
#define CM4_FLASH_CM4_ADDR          (CM4_FLASH_N9_RAMCODE_ADDR + FLASH_N9_RAM_CODE_SIZE)
#define CM4_FLASH_TMP_ADDR          (CM4_FLASH_CM4_ADDR        + FLASH_CM4_XIP_CODE_SIZE)
#define CM4_FLASH_USR_CONF_ADDR     (CM4_FLASH_TMP_ADDR   	   + FLASH_TMP_SIZE)
#define CM4_FLASH_EEPROM_ADDR	(CM4_FLASH_USR_CONF_ADDR   + FLASH_USR_CONF_SIZE)

static unsigned freq, trap;

unsigned freq_init(void)
{

#define TOP_AON_CM4_STRAP_STA		(BASE_TOPCFG + 0x1C0)
#define TOP_AON_CM4_PWRCTLCR		(BASE_TOPCFG + 0x1B8)

	unsigned pw_ctl = 0;
	trap = reg(TOP_AON_CM4_STRAP_STA);

	pw_ctl = reg(TOP_AON_CM4_PWRCTLCR);
	pw_ctl = BI_C_FLD(pw_ctl, 10, 0);

	switch (BI_G_FLD(trap, 15, 13)) {
	case 0:
		freq = 20000000;	// 20Mhz
		pw_ctl |= 1 << 4;
		break;
	case 1:
		freq = 40000000;	// 40Mhz
		pw_ctl |= 1 << 9;
		break;
	case 2:
		freq = 26000000;	// 26Mhz
		pw_ctl |= 1 << 6;
		break;
	case 3:
		freq = 52000000;	// 52Mhz
		pw_ctl |= 1 << 10;
		break;
	case 4:
	case 5:
	case 6:
	case 7:
		freq = 40000000;	// fall through
		pw_ctl |= 1 << 9;
		break;
	}

	reg(TOP_AON_CM4_PWRCTLCR) = pw_ctl;
	return freq;
}

void pinmux(unsigned p, unsigned f)
{
	unsigned r;
	unsigned addr = BASE_PINMUX + 0x20 + ((p >> 3) * 4);
	unsigned lsb = (p & 0x7) * 4;
	r = reg(addr);
	r &= ~(0x7 << lsb);
	r |= (f & 0x7) << lsb;
	reg(addr) = r;
}

void pinmux_init()
{
	pinmux(0, 7);
	pinmux(1, 7);
	pinmux(2, 7);
	pinmux(3, 7);
}

void boot(unsigned addr)
{
	if (!addr)
		addr = CM4_FLASH_CM4_ADDR + flash_off;
	_printf("go %x\n", addr);
	asm volatile ("bx  %0\n"::"r" (addr | 1));
}

typedef struct {
	char *desc;
	unsigned off, sz;
} part_t;

part_t parts[] = {
	{"boot", 0x00000000, FLASH_LOADER_SIZE},
	{"wifi", CM4_FLASH_N9_RAMCODE_ADDR, FLASH_N9_RAM_CODE_SIZE},
	{"app ", CM4_FLASH_CM4_ADDR, FLASH_CM4_XIP_CODE_SIZE},
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
	plt_ram = 0x20000000;
	plt_ram_sz = 256 << 10;
}
