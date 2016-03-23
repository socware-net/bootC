#include "pinmux.h"
#include "io.h"
#include "_soc.h"

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
