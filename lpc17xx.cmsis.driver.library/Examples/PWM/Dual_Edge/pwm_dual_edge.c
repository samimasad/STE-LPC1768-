/**
 * @file	: pwm_dual_edge.c
 * @purpose	: This program illustrates the PWM signal on 3 Channels in both edge mode
 * and single mode.
 * Peripheral clock for PWM: PWM_PCLK = CCLK / 4 = 72MHz/4 = 18MHz and there is no
 * prescale for PWM. The PWM timer/counter clock is at 18MHz. The base rate is set to 100
 * The base PWM frequency is at 18MHz/100 = 180 KHz.
 * Each PWM channel will be configured as following:
 * - Channel 2: Double Edge
 * - Channel 4: Double Edge
 * - Channel 5: Single Edge
 * The Match register values are as follows:
 * - MR0 = 100 (PWM rate)
 * - MR1 = 41, MR2 = 78 (PWM2 output)
 * - MR3 = 53, MR4 = 27 (PWM4 output)
 * - MR5 = 65 (PWM5 output)
 * PWM Duty on each PWM channel:
 * - Channel 2: Set by match 1, Reset by match 2.
 * - Channel 4: Set by match 3, Reset by match 4.
 * - Channel 5: Set by match 0, Reset by match 5.
 * Using Oscilloscope to observe the PWM signals
 *
 * @version	: 1.0
 * @date	: 22. Apr. 2009
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
#include "lpc17xx_pwm.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_nvic.h"
#include "lpc17xx_pinsel.h"
#include "debug_frmwrk.h"



/************************** PRIVATE MACROS *************************/

/************************** PRIVATE TYPES *************************/

/************************** PRIVATE VARIABLES *************************/
uint8_t menu1[] =
"********************************************************************************\n\r"
"Hello NXP Semiconductors \n\r"
"PWM demo \n\r"
"\t - MCU: LPC1766 \n\r"
"\t - Core: Cortex M3 \n\r"
"\t - Communicate via: UART0 - 9.6 kbps \n\r"
" This program illustrates the PWM signal on 3 Channels in both edge mode \n\r"
" and single mode. \n\r"
" Peripheral clock for PWM: PWM_PCLK = CCLK / 4 = 72MHz/4 = 18MHz and there is no \n\r"
" prescale for PWM. The PWM timer/counter clock is at 18MHz. The base rate is set to 100 \n\r"
" The base PWM frequency is at 18MHz/100 = 180 KHz. \n\r"
" Each PWM channel will be configured as following: \n\r"
" - Channel 2: Double Edge \n\r"
" - Channel 4: Double Edge \n\r"
" - Channel 5: Single Edge \n\r"
" The Match register values are as follows: \n\r"
" - MR0 = 100 (PWM rate) \n\r"
" - MR1 = 41, MR2 = 78 (PWM2 output) \n\r"
" - MR3 = 53, MR4 = 27 (PWM4 output) \n\r"
" - MR5 = 65 (PWM5 output) \n\r"
" PWM Duty on each PWM channel: \n\r"
" - Channel 2: Set by match 1, Reset by match 2. \n\r"
" - Channel 4: Set by match 3, Reset by match 4. \n\r"
" - Channel 5: Set by match 0, Reset by match 5. \n\r"
"  Using Oscilloscope to observe the PWM signals \n\r"
"********************************************************************************\n\r";
uint8_t menu2[] = "Demo terminated! \n\r";


// UART Configuration structure variable
UART_CFG_Type UARTConfigStruct;




/************************** PRIVATE FUNCTIONS *************************/
void print_menu(void);

/*********************************************************************//**
 * @brief		Print Welcome menu
 * @param[in]	none
 * @return 		None
 **********************************************************************/
void print_menu(void)
{
	_DBG(menu1);
}


/*********************************************************************//**
 * @brief	Main PWM program body
 **********************************************************************/
int c_entry(void)
{
	uint8_t temp;
	PWM_TIMERCFG_Type PWMCfgDat;
	PWM_MATCHCFG_Type PWMMatchCfgDat;
	PINSEL_CFG_Type PinCfg;

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

	/* Initialize debug */
	debug_frmwrk_init();

	// print welcome screen
	print_menu();

	/* PWM block section -------------------------------------------- */
	/* Initialize PWM peripheral, timer mode
	 * PWM prescale value = 1 (absolute value - tick value) */
	PWMCfgDat.PrescaleOption = PWM_TIMER_PRESCALE_TICKVAL;
	PWMCfgDat.PrescaleValue = 1;
	PWM_Init(LPC_PWM1, PWM_MODE_TIMER, (void *) &PWMCfgDat);

	/*
	 * Initialize PWM pin connect
	 */
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 2;
	for (temp = 0; temp <= 6; temp++){
		PinCfg.Pinnum = temp;
		PINSEL_ConfigPin(&PinCfg);
	}


	/* Set match value for PWM match channel 0 = 100, update immediately */
	PWM_MatchUpdate(LPC_PWM1, 0, 100, PWM_MATCH_UPDATE_NOW);
	/* PWM Timer/Counter will be reset when channel 0 matching
	 * no interrupt when match
	 * no stop when match */
	PWMMatchCfgDat.IntOnMatch = DISABLE;
	PWMMatchCfgDat.MatchChannel = 0;
	PWMMatchCfgDat.ResetOnMatch = ENABLE;
	PWMMatchCfgDat.StopOnMatch = DISABLE;
	PWM_ConfigMatch(LPC_PWM1, &PWMMatchCfgDat);

	/* Configure each PWM channel: --------------------------------------------- */
	/* - Channel 2: Double Edge
	 * - Channel 4: Double Edge
	 * - Channel 5: Single Edge
	 * The Match register values are as follows:
	 * - MR0 = 100 (PWM rate)
	 * - MR1 = 41, MR2 = 78 (PWM2 output)
	 * - MR3 = 53, MR4 = 27 (PWM4 output)
	 * - MR5 = 65 (PWM5 output)
	 * PWM Duty on each PWM channel:
	 * - Channel 2: Set by match 1, Reset by match 2.
	 * - Channel 4: Set by match 3, Reset by match 4.
	 * - Channel 5: Set by match 0, Reset by match 5.
	 */

	/* Edge setting ------------------------------------ */
	PWM_ChannelConfig(LPC_PWM1, 2, PWM_CHANNEL_DUAL_EDGE);
	PWM_ChannelConfig(LPC_PWM1, 4, PWM_CHANNEL_DUAL_EDGE);
	PWM_ChannelConfig(LPC_PWM1, 5, PWM_CHANNEL_SINGLE_EDGE);

	/* Match value setting ------------------------------------ */
	PWM_MatchUpdate(LPC_PWM1, 1, 41, PWM_MATCH_UPDATE_NOW);
	PWM_MatchUpdate(LPC_PWM1, 2, 78, PWM_MATCH_UPDATE_NOW);
	PWM_MatchUpdate(LPC_PWM1, 3, 53, PWM_MATCH_UPDATE_NOW);
	PWM_MatchUpdate(LPC_PWM1, 4, 27, PWM_MATCH_UPDATE_NOW);
	PWM_MatchUpdate(LPC_PWM1, 5, 65, PWM_MATCH_UPDATE_NOW);


	/* Match option setting ------------------------------------ */
	for (temp = 1; temp < 6; temp++)
	{
		/* Configure match option */
		PWMMatchCfgDat.IntOnMatch = DISABLE;
		PWMMatchCfgDat.MatchChannel = temp;
		PWMMatchCfgDat.ResetOnMatch = DISABLE;
		PWMMatchCfgDat.StopOnMatch = DISABLE;
		PWM_ConfigMatch(LPC_PWM1, &PWMMatchCfgDat);
	}

	/* Enable PWM Channel Output ------------------------------------ */
	/* Channel 2 */
	PWM_ChannelCmd(LPC_PWM1, 2, ENABLE);
	/* Channel 4 */
	PWM_ChannelCmd(LPC_PWM1, 4, ENABLE);
	/* Channel 5 */
	PWM_ChannelCmd(LPC_PWM1, 5, ENABLE);

	/* Reset and Start counter */
	PWM_ResetCounter(LPC_PWM1);
	PWM_CounterCmd(LPC_PWM1, ENABLE);

	/* Start PWM now */
	PWM_Cmd(LPC_PWM1, ENABLE);

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
