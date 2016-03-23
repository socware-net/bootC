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
#ifndef PMSA160120
#define PMSA160120

#include "io.h"

enum {
	MPU_TYPE = 0xE000ED90,	// R    MPU Type Register
	MPU_CTRL = 0xE000ED94,	// R/W  Control Register
	MPU_RNR = 0xE000ED98,	// R/W  MPU Region Number Register
	MPU_RBAR = 0xE000ED9C,	// R/W  MPU Region Base Address Register
	MPU_RASR = 0xE000EDA0,	// R/W  MPU Region Attribute and Size Register
};

static inline int mpu_reg_no()
{
	return BI_G_FLD(reg(MPU_TYPE), 15, 8);
}

static inline void mpu_set(int n)
{
	reg(MPU_RNR) = n;
}

typedef union {
	unsigned reg;
	struct __attribute__ ((packed)) {
		unsigned addr:27;
		unsigned valid:1;
		unsigned regn:4;
	} fld;
} mpu_base_t;

#define mpu_base	((mpu_base_t *) 0xE000ED9C)

typedef union {
	unsigned reg;
	struct __attribute__ ((packed)) {
		unsigned res0:3;
		unsigned xn:1;
		unsigned res1:1;
		unsigned ap:3;
		unsigned res2:2;
		unsigned tex:3;
		unsigned s:1;
		unsigned c:1;
		unsigned b:1;
		unsigned srd:8;
		unsigned res3:2;
		unsigned size:5;
		unsigned enable:1;
	};
} mpu_attr_t;

#define mpu_attr	((mpu_attr_t *) 0xE000EDA0)

#endif
