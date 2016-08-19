/*-****************************************************************************/
/*-                                                                           */
/*-            Copyright (c) of hyperCOS.                                     */
/*-                                                                           */
/*-  This software is copyrighted by and is the sole property of socware.net. */
/*-  All rights, title, ownership, or other interests in the software remain  */
/*-  the property of socware.net. The source code is FREE for short-term      */
/*-  evaluation, educational or non-commercial research only. Any commercial  */
/*-  application may only be used in accordance with the corresponding license*/
/*-  agreement. Any unauthorized use, duplication, transmission, distribution,*/
/*-  or disclosure of this software is expressly forbidden.                   */
/*-                                                                           */
/*-  Knowledge of the source code may NOT be used to develop a similar product*/
/*-                                                                           */
/*-  This Copyright notice may not be removed or modified without prior       */
/*-  written consent of socware.net.                                          */
/*-                                                                           */
/*-  socware.net reserves the right to modify this software                   */
/*-  without notice.                                                          */
/*-                                                                           */
/*-  To contact socware.net:                                                  */
/*-                                                                           */
/*-             socware.help@gmail.com                                        */
/*-                                                                           */
/*-****************************************************************************/
OUTPUT_FORMAT("elf32-littlearm", "elf32-bigarm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_start)
STARTUP(startup.o)
GROUP( libbootC.a libc.a libstm32f4xx.a)
MEMORY
{
    LOADER          (arw) : ORIGIN = 0x20000000,    LENGTH = 0x00009000        /* 36k */
    SYSRAM          (arw) : ORIGIN = 0x20009000,    LENGTH = 0x00037000        /* 256k */
}

SECTIONS
{
	. = 0x20000000;
	.vectors	: { KEEP(*(.vectors))}
	.vectors.mon	: { KEEP(*(.vectors.mon))}
	.text           : { *(.text .text.* .rodata*)}>LOADER
	_data_load= . ;
	_data_sta = .;
	.data           : { *(.data .data.* )}>LOADER
	_data_end = . ;
	. = ALIGN(4);
	_bss_sta = .;
	.bss            : { *(.bss .bss.* ) }>SYSRAM
	_bss_end = .;
	_end = . ;

	PROVIDE(_stack = _end + 8096 );

	.comment         0 : { *(.comment) }
	.debug           0 : { *(.debug) }
	.line            0 : { *(.line) }
	.debug_srcinfo   0 : { *(.debug_srcinfo) }
	.debug_sfnames   0 : { *(.debug_sfnames) }
	.debug_aranges   0 : { *(.debug_aranges) }
	.debug_pubnames  0 : { *(.debug_pubnames) }
	.debug_info      0 : { *(.debug_info .gnu.linkonce.wi.*) }
	.debug_abbrev    0 : { *(.debug_abbrev) }
	.debug_line      0 : { *(.debug_line) }
	.debug_frame     0 : { *(.debug_frame) }
	.debug_str       0 : { *(.debug_str) }
	.debug_loc       0 : { *(.debug_loc) }
	.debug_macinfo   0 : { *(.debug_macinfo) }
	.debug_weaknames 0 : { *(.debug_weaknames) }
	.debug_funcnames 0 : { *(.debug_funcnames) }
	.debug_typenames 0 : { *(.debug_typenames) }
	.debug_varnames  0 : { *(.debug_varnames) }
	.note.gnu.arm.ident 0 : { KEEP (*(.note.gnu.arm.ident)) }
	.ARM.attributes  0 : { KEEP (*(.ARM.attributes)) }
	/DISCARD/ : { *(.note.GNU-stack) *(.note.gnu*)}
}
