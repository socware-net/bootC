#ifndef PLT0325
#define PLT0325

void pinmux_init(void);

unsigned freq_init(void);

void plt_init(void);

void tmr_delay(int ms);

void boot(unsigned addr);

void update(int n);

extern unsigned plt_ram, plt_ram_sz;

#endif
