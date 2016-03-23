#ifndef TMR
#define TMR

void _tmr_delay(int n, int ms);

#define tmr_delay(_ms)	_tmr_delay(2, _ms)

#endif
