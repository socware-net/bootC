#include "plt.h"
#include "io.h"
#include "_soc.h"
#include "stm32f4xx.h"
#include "system_stm32f4xx.h"
#include "stm32f4xx_tim.h"
#include "misc.h"

#define TIM		TIM3
#define TIMCLK		RCC_APB1Periph_TIM3
static int is_init;

static void tmr_init(void)
{
	TIM_TimeBaseInitTypeDef tbase;
	unsigned short scaler;
	// TIM clock
	RCC_APB1PeriphClockCmd(TIMCLK, ENABLE);
	/* -----------------------------------------------------------------------
	   TIM3 Configuration: Output Compare Timing Mode:

	   In this example TIM3 input clock (TIM3CLK) is set to 2 * APB1 clock (PCLK1),
	   since APB1 prescaler is different from 1.
	   TIM3CLK = 2 * PCLK1
	   PCLK1 = HCLK / 4
	   => TIM3CLK = HCLK / 2 = SystemCoreClock /2

	   To get TIM3 counter clock at 6 MHz, the prescaler is computed as follows:
	   Prescaler = (TIM3CLK / TIM3 counter clock) - 1
	   Prescaler = ((SystemCoreClock /2) /6 MHz) - 1
	   ----------------------------------------------------------------------- */
	// Compute the prescaler value
	scaler = (unsigned short)((SystemCoreClock / 2) / 1000) - 1;

	// Time base configuration
	tbase.TIM_Period = 65535;
	tbase.TIM_Prescaler = 0;
	tbase.TIM_ClockDivision = 0;
	tbase.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM, &tbase);

	// Prescaler configuration
	TIM_PrescalerConfig(TIM, scaler, TIM_PSCReloadMode_Immediate);
	// TIM enable counter
	TIM_Cmd(TIM, ENABLE);
}

void tmr_delay(int ms)
{
	unsigned pc, cc;

	if (!is_init) {
		tmr_init();
		is_init = 1;
	}
	pc = TIM_GetCounter(TIM);
	while (ms > 0) {
		cc = TIM_GetCounter(TIM);
		if (cc != pc) {
			ms -= cc - pc;
			pc = cc;
		}
	}
}
