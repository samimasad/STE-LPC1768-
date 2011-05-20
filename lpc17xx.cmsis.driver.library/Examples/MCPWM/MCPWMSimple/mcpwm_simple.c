/**
 * @file	: mcpwm_simple.c
 * @purpose	: This simple example used to test MCPWM driver
 * @version	: 1.0
 * @date	: 27. May. 2009
 * @author	: HieuNguyen
 *----------------------------------------------------------------------------
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 **********************************************************************/
#include "lpc17xx_qei.h"
#include "lpc17xx_mcpwm.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_nvic.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_clkpwr.h"
#include "debug_frmwrk.h"



/************************** PRIVATE MACROS *************************/
/** MCPWM in 3-phase DC motor mode test */
#define DC_MODE_TEST			0

/** MCPWM in 3-phase AC motor mode test */
#define AC_MODE_TEST			0

/** MCPWM tested with Capture function */
#define CAPTURE_MODE_TEST 		1


/************************** PRIVATE TYPES *************************/

/************************** PRIVATE VARIABLES *************************/

/* Pin configuration data */
/** Motor Control Channel 0 Output A */
const PINSEL_CFG_Type mcpwm_mco0a_pin[1] = {{1, 19, 1, 0, 0}};
/** Motor Control Channel 0 Output B */
const PINSEL_CFG_Type mcpwm_mco0b_pin[1] = {{1, 22, 1, 0, 0}};

/** Motor Control Channel 1 Output A */
const PINSEL_CFG_Type mcpwm_mco1a_pin[1] = {{1, 25, 1, 0, 0}};
/** Motor Control Channel 1 Output B */
const PINSEL_CFG_Type mcpwm_mco1b_pin[1] = {{1, 26, 1, 0, 0}};

/** Motor Control Channel 2 Output A */
const PINSEL_CFG_Type mcpwm_mco2a_pin[1] = {{1, 28, 1, 0, 0}};
/** Motor Control Channel 2 Output B */
const PINSEL_CFG_Type mcpwm_mco2b_pin[1] = {{1, 29, 1, 0, 0}};

/** Motor Control Feed Back Channel 0 - MCI0 */
const PINSEL_CFG_Type mcpwm_mcfb0_pin[1] = {{1, 20, 1, 0, 0}};
/** Motor Control Feed Back Channel 1 - MCI1 */
const PINSEL_CFG_Type mcpwm_mcfb1_pin[1] = {{1, 23, 1, 0, 0}};
/** Motor Control Feed Back Channel 2 - MCI2 */
const PINSEL_CFG_Type mcpwm_mcfb2_pin[1] = {{1, 24, 1, 0, 0}};

/** Motor Control Low-active abort feed back */
const PINSEL_CFG_Type mcpwm_mcabort_pin[1] = {{ 1, 21, 1, 0, 0}};

#if CAPTURE_MODE_TEST
/** Capture configuration data */
MCPWM_CAPTURE_CFG_Type captureCfg;
/** capture flag */
__IO FlagStatus CapFlag;
/** Latest capture value */
__IO uint32_t CapVal;
#endif

/************************** PRIVATE FUNCTIONS *************************/

/**
 * @brief MCPWM interrupt handler sub-routine
 */
void MCPWM_IRQHandler(void)
{

#if CAPTURE_MODE_TEST
	// Check whether if capture event interrupt is set
	if (MCPWM_GetIntStatus(LPC_MCPWM, MCPWM_INTFLAG_CAP0)) {
		if (CapFlag == RESET) {
			// Store capture value
			CapVal = MCPWM_GetCapture(LPC_MCPWM, 0);
			// toggle capture flag
			CapFlag = SET;
			// Disable interrupt for capture event
			MCPWM_IntConfig(LPC_MCPWM, MCPWM_INTFLAG_CAP0, DISABLE);
		}
		// Clear pending interrupt
		MCPWM_IntClear(LPC_MCPWM, MCPWM_INTFLAG_CAP0);
	}
#endif
}


/*********************************************************************//**
 * @brief		Delay millisecond
 * @param[in]	Timer Timer peripheral, should be TIM0..3
 * @param[in]	time delay time value in ms
 * @return 		None
 **********************************************************************/
void Timer_Wait(LPC_TIM_TypeDef *Timer, uint32_t time)
{
	TIM_TIMERCFG_Type TIM_ConfigStruct;
	TIM_MATCHCFG_Type TIM_MatchConfigStruct;

	// Initialize timer 0, prescale count time of 1ms
	TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_USVAL;
	TIM_ConfigStruct.PrescaleValue	= 1000;
	// use channel 0, MR0
	TIM_MatchConfigStruct.MatchChannel = 0;
	// Enable interrupt when MR0 matches the value in TC register
	TIM_MatchConfigStruct.IntOnMatch   = TRUE;
	//Enable reset on MR0: TIMER will not reset if MR0 matches it
	TIM_MatchConfigStruct.ResetOnMatch = FALSE;
	//Stop on MR0 if MR0 matches it
	TIM_MatchConfigStruct.StopOnMatch  = TRUE;
	//do no thing for external output
	TIM_MatchConfigStruct.ExtMatchOutputType =TIM_EXTMATCH_NOTHING;
	// Set Match value, count value is time (timer * 1000uS =timer mS )
	TIM_MatchConfigStruct.MatchValue   = time;

	// Set configuration for Tim_config and Tim_MatchConfig
	TIM_Init(Timer, TIM_TIMER_MODE,&TIM_ConfigStruct);
	TIM_ConfigMatch(Timer,&TIM_MatchConfigStruct);
	// To start timer 0
	TIM_Cmd(Timer,ENABLE);
	while ( !(TIM_GetIntStatus(Timer,0)));
	TIM_ClearIntPending(Timer,0);
}



/*********************************************************************//**
 * @brief	Main MCPWM program body
 **********************************************************************/
int c_entry(void)
{
	// MCPWM Channel configuration data
	MCPWM_CHANNEL_CFG_Type channelsetup[3];

	// DeInit NVIC and SCBNVIC
	NVIC_DeInit();
	NVIC_SCBDeInit();

	/* Configure the NVIC Preemption Priority Bits:
	 * two (2) bits of preemption priority, six (6) bits of sub-priority.
	 * Since the Number of Bits used for Priority Levels is five (5), so the
	 * actual bit number of sub-priority is three (3)
	 */
	NVIC_SetPriorityGrouping(0x05);

	//  Set Vector table offset value
#if (__RAM_MODE__==1)
	NVIC_SetVTOR(0x10000000);
#else
	NVIC_SetVTOR(0x00000000);
#endif

	/*
	 * Initialize debug framework
	 */
	debug_frmwrk_init();
	_DBG_("Hello MCPWM ...");

	/* Initializes pin corresponding to MCPWM function */
	PINSEL_ConfigPin((PINSEL_CFG_Type *)&mcpwm_mco0a_pin[0]);
	PINSEL_ConfigPin((PINSEL_CFG_Type *)&mcpwm_mco0b_pin[0]);

	PINSEL_ConfigPin((PINSEL_CFG_Type *)&mcpwm_mco1a_pin[0]);
	PINSEL_ConfigPin((PINSEL_CFG_Type *)&mcpwm_mco1b_pin[0]);

	PINSEL_ConfigPin((PINSEL_CFG_Type *)&mcpwm_mco2a_pin[0]);
	PINSEL_ConfigPin((PINSEL_CFG_Type *)&mcpwm_mco2b_pin[0]);

	PINSEL_ConfigPin((PINSEL_CFG_Type *)&mcpwm_mcfb0_pin[0]);
	PINSEL_ConfigPin((PINSEL_CFG_Type *)&mcpwm_mcfb1_pin[0]);
	PINSEL_ConfigPin((PINSEL_CFG_Type *)&mcpwm_mcfb2_pin[0]);

	PINSEL_ConfigPin((PINSEL_CFG_Type *)&mcpwm_mcabort_pin[0]);

	/* Disable interrupt for MCPWM  */
	NVIC_DisableIRQ(MCPWM_IRQn);
	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(MCPWM_IRQn, ((0x01<<3)|0x01));


	/* Init MCPWM peripheral */
	MCPWM_Init(LPC_MCPWM);

	channelsetup[0].channelType = MCPWM_CHANNEL_EDGE_MODE;
	channelsetup[0].channelPolarity = MCPWM_CHANNEL_PASSIVE_LO;
	channelsetup[0].channelDeadtimeEnable = DISABLE;
	channelsetup[0].channelDeadtimeValue = 0;
	channelsetup[0].channelUpdateEnable = ENABLE;
	channelsetup[0].channelTimercounterValue = 0;
	channelsetup[0].channelPeriodValue = 300;
	channelsetup[0].channelPulsewidthValue = 0;

	channelsetup[1].channelType = MCPWM_CHANNEL_EDGE_MODE;
	channelsetup[1].channelPolarity = MCPWM_CHANNEL_PASSIVE_LO;
	channelsetup[1].channelDeadtimeEnable = DISABLE;
	channelsetup[1].channelDeadtimeValue = 0;
	channelsetup[1].channelUpdateEnable = ENABLE;
	channelsetup[1].channelTimercounterValue = 0;
	channelsetup[1].channelPeriodValue = 300;
	channelsetup[1].channelPulsewidthValue = 100;

	channelsetup[2].channelType = MCPWM_CHANNEL_EDGE_MODE;
	channelsetup[2].channelPolarity = MCPWM_CHANNEL_PASSIVE_LO;
	channelsetup[2].channelDeadtimeEnable = DISABLE;
	channelsetup[2].channelDeadtimeValue = 0;
	channelsetup[2].channelUpdateEnable = ENABLE;
	channelsetup[2].channelTimercounterValue = 0;
	channelsetup[2].channelPeriodValue = 300;
	channelsetup[2].channelPulsewidthValue = 200;

	MCPWM_ConfigChannel(LPC_MCPWM, 0, &channelsetup[0]);
	MCPWM_ConfigChannel(LPC_MCPWM, 1, &channelsetup[1]);
	MCPWM_ConfigChannel(LPC_MCPWM, 2, &channelsetup[2]);

#if DC_MODE_TEST
	/*
	 * - DC mode enabled.
	 * - Invert Output enabled
	 * - A0 and A1 output pin is internally routed to A0 signal
	 */
	MCPWM_DCMode(LPC_MCPWM, ENABLE, ENABLE, (MCPWM_PATENT_A0|MCPWM_PATENT_A1));
#endif

#if AC_MODE_TEST
	/*
	 * - AC mode is enabled.
	 */
	MCPWM_ACMode(LPC_MCPWM, ENABLE);
#endif

#if CAPTURE_MODE_TEST
	/*
	 * Capture mode in this case is used to detect the falling edge on MCO0B output pin.
	 * The MCFB0 input pin therefore must be connected to MCO0B.
	 * - Capture Channel 0.
	 * - Capture falling edge on MCFB0 input pin.
	 * - Interrupt enabled on capture event.
	 */
	captureCfg.captureChannel = 0;
	captureCfg.captureFalling = ENABLE;
	captureCfg.captureRising = DISABLE;
	captureCfg.hnfEnable = DISABLE;
	captureCfg.timerReset = DISABLE;
	MCPWM_ConfigCapture(LPC_MCPWM, 0, &captureCfg);

	// Reset flag for the first time
	CapFlag = RESET;

	// Enable interrupt for capture event on MCI0 (MCFB0)
	MCPWM_IntConfig(LPC_MCPWM, MCPWM_INTFLAG_CAP0, ENABLE);

	/* Enable interrupt for MCPWM  */
	NVIC_EnableIRQ(MCPWM_IRQn);
#endif

	MCPWM_Start(LPC_MCPWM, ENABLE, ENABLE, ENABLE);

	// Main loop
	while (1) {
		Timer_Wait(LPC_TIM0, 1000);

		channelsetup[0].channelPulsewidthValue = (channelsetup[0].channelPulsewidthValue >= 300) ?
												0 : channelsetup[0].channelPulsewidthValue + 20;
		channelsetup[1].channelPulsewidthValue = (channelsetup[1].channelPulsewidthValue >= 300) ?
												0 : channelsetup[1].channelPulsewidthValue + 20;
		channelsetup[2].channelPulsewidthValue = (channelsetup[2].channelPulsewidthValue >= 300) ?
												0 : channelsetup[2].channelPulsewidthValue + 20;
		_DBG_("Update!");
		MCPWM_WriteToShadow(LPC_MCPWM, 0, &channelsetup[0]);
		MCPWM_WriteToShadow(LPC_MCPWM, 1, &channelsetup[1]);
		MCPWM_WriteToShadow(LPC_MCPWM, 2, &channelsetup[2]);

		// Check capture flag is set or not
		if (CapFlag) {
			// Print out the value
			_DBG("Capture Value: ");
			_DBD32(CapVal); _DBG_("");

			// Setup a new capture event
			MCPWM_ConfigCapture(LPC_MCPWM, 0, &captureCfg);

			// Re-Enable interrupt for capture event on MCI0 (MCFB0)
			MCPWM_IntConfig(LPC_MCPWM, MCPWM_INTFLAG_CAP0, ENABLE);

			// Reset flag
			CapFlag = RESET;
		}
	}

    /* Loop forever */
    while(1);
    return 1;
}

/* With ARM and GHS toolsets, the entry point is main() - this will
   allow the linker to generate wrapper code to setup stacks, allocate
   heap area, and initialize and copy code and data segments. For GNU
   toolsets, the entry point is through __start() in the crt0_gnu.asm
   file, and that startup code will setup stacks and data */
int main(void)
{
    return c_entry();
}


#ifdef  DEBUG
/*******************************************************************************
* @brief		Reports the name of the source file and the source line number
* 				where the CHECK_PARAM error has occurred.
* @param[in]	file Pointer to the source file name
* @param[in]    line assert_param error line source number
* @return		None
*******************************************************************************/
void check_failed(uint8_t *file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while(1);
}
#endif
