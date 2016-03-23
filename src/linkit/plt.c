#include "plt.h"
#include "io.h"
#include "_soc.h"
#include "pinmux.h"

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

void pinmux_init()
{
	pinmux(0, 7);
	pinmux(1, 7);
	pinmux(2, 7);
	pinmux(3, 7);
}
