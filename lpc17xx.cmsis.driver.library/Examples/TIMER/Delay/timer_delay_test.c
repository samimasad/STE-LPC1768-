/**
 * @file	: timer_delay_test.c
 * @purpose	: A simple timer driver example uses timer 0 in polling mode
 * @version	: 1.0
 * @date	: 3. April. 2009
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
#include "lpc17xx_timer.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_nvic.h"
#include "debug_frmwrk.h"


#define LED0_PIN 	(1<<6)
#define LED1_PIN 	(1<<5)
#define LED2_MASK	((1<<2) | (1<<3) | (1<<4) | (1<<5) | (1<<6))
#define LED1_MASK	((1<<28) | (1<<29) | (1<<31))


/************************** PRIVATE TYPES *************************/

/************************** PRIVATE VARIABLES *************************/
uint8_t menu1[] =
"********************************************************************************\n\r"
"Hello NXP Semiconductors \n\r"
"Timer delay demo \n\r"
"\t - MCU: LPC17xx \n\r"
"\t - Core: ARM Cortex-M3 \n\r"
"\t - Communicate via: UART0 - 115200 bps \n\r"
" Use timer 0 in polling mode \n\r"
" Wait 1 second to turn ON/OFF LED in sequence \n\r"
"********************************************************************************\n\r";
uint8_t	info1[] ="Tick ON \n\r";
uint8_t info2[] ="Tick OFF \n\r";


//timer init
TIM_TIMERCFG_Type TIM_ConfigStruct;
TIM_MATCHCFG_Type TIM_MatchConfigStruct;
/************************** PRIVATE FUNCTION *************************/
void print_menu(void);


// Print Menu 1
void print_menu(void)
{
	_DBG(menu1);
}


void LED_Init (void)
{
	PINSEL_CFG_Type PinCfg;

	uint8_t temp;

	PinCfg.Funcnum = 0;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 2;
	for (temp = 2; temp <= 6; temp++){
		PinCfg.Pinnum = temp;
		PINSEL_ConfigPin(&PinCfg);
	}

	PinCfg.Funcnum = 0;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 1;
	PinCfg.Pinnum = 28;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 29;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 31;
	PINSEL_ConfigPin(&PinCfg);


	// Set direction to output
	LPC_GPIO2->FIODIR |= LED2_MASK;
	LPC_GPIO1->FIODIR |= LED1_MASK;

	/* Turn off all LEDs */
	LPC_GPIO2->FIOCLR = LED2_MASK;
	LPC_GPIO1->FIOCLR = LED1_MASK;
}

int c_entry(void)
{
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
	// Init LED port
	LED_Init();

	/* Init debug */
	debug_frmwrk_init();

	// print welcome screen
	print_menu();

	// Initialize timer 0, prescale count time of 100uS
	TIM_ConfigStruct.PrescaleOption = TIM_PRESCALE_USVAL;
	TIM_ConfigStruct.PrescaleValue	= 100;
	// use channel 0, MR0
	TIM_MatchConfigStruct.MatchChannel = 0;
	// Disable interrupt when MR0 matches the value in TC register
	TIM_MatchConfigStruct.IntOnMatch   = TRUE;
	//Enable reset on MR0: TIMER will reset if MR0 matches it
	TIM_MatchConfigStruct.ResetOnMatch = TRUE;
	//Stop on MR0 if MR0 matches it
	TIM_MatchConfigStruct.StopOnMatch  = FALSE;
	//Toggle MR0.0 pin if MR0 matches it
	TIM_MatchConfigStruct.ExtMatchOutputType =TIM_EXTMATCH_TOGGLE;
	// Set Match value, count value of 10000 (10000 * 100uS = 1S --> 1Hz)
	TIM_MatchConfigStruct.MatchValue   = 10000;

	// Set configuration for Tim_config and Tim_MatchConfig
	TIM_Init(LPC_TIM0, TIM_TIMER_MODE,&TIM_ConfigStruct);
	TIM_ConfigMatch(LPC_TIM0,&TIM_MatchConfigStruct);
	TIM_Cmd(LPC_TIM0,ENABLE);
	while (1)
	{

		// Wait for 1000 millisecond
		while ( !(TIM_GetIntStatus(LPC_TIM0,0)));

		{
		TIM_ClearIntPending(LPC_TIM0,0);
		//turn on led
		LPC_GPIO2->FIOSET = LED0_PIN;
		UART_Send(LPC_UART0, info1, sizeof(info1), BLOCKING);
		}

		// Wait for 1000 millisecond
		while ( !(TIM_GetIntStatus(LPC_TIM0,0)));
		{
		TIM_ClearIntPending(LPC_TIM0,0);
		//turn off led
		LPC_GPIO2->FIOCLR = LED0_PIN;
		UART_Send(LPC_UART0, info2, sizeof(info2), BLOCKING);
		}
	}
	TIM_DeInit(LPC_TIM0);
	return (1);
}

/* Support required entry point for other toolchain */
int main (void)
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

