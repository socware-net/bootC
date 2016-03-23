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

#include "io.h"
#include "uart.h"
#include "tmr.h"
#include "term.h"
#include "plt.h"

static void c_help(char *cmd)
{
	char *mfmt = "%s\r\n";
	_printf(mfmt, "h(help)");
	_printf(mfmt, "e(erase) flash-offset size");
	_printf(mfmt, "f(flash) flash-offset size");
	_printf(mfmt, "l(load) addr");
	_printf(mfmt, "g(go) addr");
	_printf(mfmt, "u(update) partition-number");
	_printf(mfmt, "w(halt)");
}

static void c_go(char *cmd)
{
	_printf("go\r\n");
//      asm volatile ("bx  %0\n"::"r"
//                    ((0x10000000 + CM4_FLASH_CM4_ADDR) | 0x1));
}

struct {
	char c;
	void (*f) (char *);
} cmds[] = {
	{
	'h', c_help}, {
	'g', c_go}, {
0, 0},};

void cmd(char *cmd)
{
	int i;
	for (i = 0; cmds[i].c != 0; i++) {
		if (cmd[0] == cmds[i].c) {
			cmds[i].f(cmd);
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
	uart_init(freq);
	_printf("\r\nbootC\r\n");
	wait = WAIT * 10;
	while (wait > 0 && (c = uart_getc()) == -1) {
		tmr_delay(1000);
		wait--;
		_printf("\r%d ", wait);
	}
	_printf("\r\n");
	if (c == -1) {
		cmd("go");
	} else {
		cmd("h");
		while (1) {
			_printf("bootC>");
			cmd(_gets());
		}
	}
	return 0;
}

void _reloc()
{
	extern char _data_end, __isr_vector;
	unsigned sz, t0;
	int i;
	char *dst, *src;

	sz = (unsigned)&_data_end - (unsigned)&__isr_vector;
	dst = (void *)VA;
	//FIXME
	src = (void *)LA;
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
