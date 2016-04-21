/*-****************************************************************************/
/*-                                                                           */
/*-            Copyright (c) 2013 by SOCware Inc.                             */
/*-                                                                           */
/*-  This software is copyrighted by and is the sole property of SOCware, Inc.*/
/*-  All rights, title, ownership, or other interests in the software remain  */
/*-  the property of SOCware, Inc. The source code is for FREE short-term     */
/*-  evaluation, educational or non-commercial research only. Any commercial  */
/*-  application may only be used in accordance with the corresponding license*/
/*-  agreement. Any unauthorized use, duplication, transmission, distribution,*/
/*-  or disclosure of this software is expressly forbidden.                   */
/*-                                                                           */
/*-  Knowledge of the source code may NOT be used to develop a similar product*/
/*-                                                                           */
/*-  This Copyright notice may not be removed or modified without prior       */
/*-  written consent of SOCware, Inc.                                         */
/*-                                                                           */
/*-  SOCWare, Inc. reserves the right to modify this software                 */
/*-  without notice.                                                          */
/*-                                                                           */
/*-             socware.help@gmail.com                                        */
/*-             http://socware.net                                            */
/*-                                                                           */
/*-****************************************************************************/

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

#include "io.h"
#include "uart.h"
#include "plt.h"
#include "term.h"
#include "plt.h"
#include "xmodem.h"
#include "flash.h"

static char *prompt = "bootC>";

static int tokeni(unsigned *i)
{
	char *t = strtok(0, " ");
	if (!t)
		return 0;
	*i = strtol(t, 0, 0);
	return 1;
}

typedef struct {
	char c;
	void (*f) (void);
	char *desc;
} cmd_t;

static void _dump(int mode)
{
	unsigned i, a, sz, word;
	if (!tokeni(&a) || !tokeni(&sz))
		return;
	a = BI_ALN(a, 4);
	sz = BI_RUP(sz, 4);
	for (i = 0; i < sz; i += sizeof(unsigned)) {
		if (!(i & BI_TMSK(4)))
			_printf("\r\n%x: ", mode ? i : (a + i));
		word = readl((void *)(a + i));
		if (mode) {
			word = ((word >> 24) & 0xff) |
			    ((word << 8) & 0xff0000) |
			    ((word >> 8) & 0xff00) |
			    ((word << 24) & 0xff000000);
		}
		_printf("%x ", word);
	}
	_printf("\r\n");
	_printf("\r\n");
}

static void c_dump(void)
{
	_dump(0);
}

static void c_xdump(void)
{
	_dump(1);
}

static void c_set(void)
{
	unsigned i, a, sz, v;
	if (!tokeni(&a) || !tokeni(&sz) || !tokeni(&v))
		return;
	a = BI_ALN(a, 2);
	sz = BI_RUP(sz, 2);
	for (i = 0; i < sz; i += sizeof(unsigned))
		*(unsigned *)(a + i) = v;
	_printf("%x-%x, %x\r\n", a, a + sz, v);
}

static unsigned load_addr;

static void c_go(void)
{
	unsigned addr;
	if (tokeni(&addr))
		boot(addr);
	else
		boot(load_addr);
}

static void load_x(void *priv, unsigned char *b, int l)
{
	unsigned *pos = (unsigned *)priv;
	if (l < 0)
		return;
	memcpy((void *)(*pos), b, l);
	*pos += l;
}

extern char _stack;

static void c_load(void)
{
	unsigned pos;
	if (!tokeni(&load_addr)) {
		load_addr = BI_RUP((unsigned)&_stack, 10);
		_printf("load %x\r\n", load_addr);
	}
	pos = load_addr;
	xmodem_block_rx(load_x, &pos);
	_printf("\r\n\r\n%x-%x, %d\r\n", load_addr, pos, (pos - load_addr));
}

static void c_erase(void)
{
	int r;
	unsigned off, sz;
	if (!tokeni(&off) || !tokeni(&sz))
		return;
	if ((r = _flash_erase(off, sz)))
		_printf("%d\n", r);
}

static void c_fread(void)
{
	int r;
	unsigned off, sz, addr;
	if (!tokeni(&off) || !tokeni(&sz) || !tokeni(&addr))
		return;
	if ((r = _flash_read(off, sz, (void *)addr)))
		_printf("%d\n", r);
}

static void c_fwrite(void)
{
	int r;
	unsigned off, sz, addr;
	if (!tokeni(&off) || !tokeni(&sz) || !tokeni(&addr))
		return;
	if ((r = _flash_write(off, sz, (void *)addr)))
		_printf("%d\n", r);
}

static void c_update(void)
{
	unsigned partition;
	if (!tokeni(&partition))
		partition = -1;
	update(partition);
}

extern cmd_t cmds[];

static void c_help(void)
{
	int i;
	for (i = 0; cmds[i].c != 0; i++)
		_printf("    %c %s\r\n", cmds[i].c, cmds[i].desc);
	_printf("\r\n");
}

cmd_t cmds[] = {
	{'h', c_help, "(help)"}
	,
	{'g', c_go, "(go) addr"}
	,
	{'d', c_dump, "(dump) addr size"}
	,
	{'x', c_xdump, "(xxd dump) addr size"}
	,
	{'S', c_set, "(set) addr size pattern"}
	,
	{'l', c_load, "(load) addr"}
	,
	{'E', c_erase, "(erase) flash-offset size"}
	,
	{'f', c_fread, "(flash read) flash-offset size addr"}
	,
	{'F', c_fwrite, "(flash write) flash-offset size addr"}
	,
	{'U', c_update, "(update) partition-no(U help)"}
	,
	{0, 0}
	,
};

static void cmd(char *cmd)
{
	int i;
	for (i = 0; cmds[i].c != 0; i++) {
		if (cmd[0] == cmds[i].c) {
			strtok(cmd, " ");
			cmds[i].f();
			break;
		}
	}
}

int main(void)
{
	int c, wait;
	unsigned freq;
	freq = freq_init();
	pinmux_init();
	plt_init();
	uart_init(freq);
	_flash_init();
	_printf("\r\nram   %x, %x\r\n", plt_ram, plt_ram_sz);
	_printf("flash %x, %x, page=%dK\r\n", flash_off, flash_sz,
		flash_page >> 10);
	_printf("\r\n%s\r\n", prompt);
	wait = WAIT;
	while (wait > 0 && (c = uart_getc()) == -1) {
		_printf("\r%d ", wait);
		tmr_delay(1000);
		wait--;
	}
	_printf("\r\n");
	if (c == -1)
		cmd("g");
	else
		cmd("h");
	while (1) {
		_printf("%s", prompt);
		cmd(_gets());
	}
	return 0;
}

void _reloc(unsigned la)
{
	extern char _data_end, __isr_vector;
	unsigned sz, t0;
	int i;
	char *dst, *src;

	sz = (unsigned)&_data_end - (unsigned)&__isr_vector;
	dst = (void *)VA;
	src = (void *)la;
	for (i = 0; i < sz; i++)
		*dst++ = *src++;
	asm volatile (""
		      "ldr	%0,=_jp	\n"
		      "bx	%0	\n"
		      ".global _jp	\n"
		      ".thumb		\n"
		      ".thumb_func	\n" "_jp:\n":"=r" (t0)
		      :);
}

void _init()
{
	extern char _bss_sta, _bss_end;
	unsigned sz = (unsigned)&_bss_end - (unsigned)&_bss_sta;
	memset(&_bss_sta, 0, sz);
}

void printf(char *str, ...)
{
	if (str == 0) {
		return;
	}
	va_list ap;
	va_start(ap, str);
	_print(str, ap);
	va_end(ap);
	_printf("\r");
}
