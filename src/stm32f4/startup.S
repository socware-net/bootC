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

  	.section  ".vectors","ax"
	.global  __isr_vector
__isr_vector:
	.word  _stack
	.word  _start
	.word  0
	.word  0
	.word  0
	.word  0
	.word  0
	.word  0
	.word  0
	.word  0
	.word  0
	.word  0
	.word  0
	.word  0
	.word  0
	.word  0
	.section  .text._start
	.weak  _start
	.type  _start, %function
_start:  
	cpsid	i
	ldr	sp, = _stack
1:
	ldr	r0,= _init_sec
	blx	r0
	ldr	r0,= main
	bx	r0
2:
	b	2b
