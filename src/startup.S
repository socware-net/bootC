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

	.syntax unified
	.arch armv7-m

	.align 7
	.section ".vectors","ax"
	.global __isr_vector
__isr_vector:
	.word	0x20007758
	.word	_start+((FA-VA)|0x1)
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0
	.word	0

	.align 2
	.global  _start
	.text
_start:
	cpsid	i
	ldr	sp, = _stack

	mov	r0, pc
	ldr	r1, =_start
	subs	r1, r0, r1
	cmp	r0, #32
	bls.n	1f
	// r0 = LA (1k aligned)
	bic.w	r0, 0x3fC
	bic.w	r0, 0x3
	// r2 = VA-LA
	ldr	r2, = VA
	sub	r2, r2, r0
	ldr	r1, =_reloc
	sub	r1, r1, r2
	ldr	lr,=1f
	orr	lr, lr, #1
	bx	r1
1:
	ldr r0,= _init
	blx r0
	ldr r0,= main
	bx  r0
2:
	b		2b

