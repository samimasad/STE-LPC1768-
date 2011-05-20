/**
 * @file	: wdt_reset_test.c
 * @purpose	: A simple wdt driver example
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
#include "lpc17xx_wdt.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_nvic.h"
#include "debug_frmwrk.h"
#include "lpc17xx_gpio.h"



#define LED_PIN 	(1<<6)
#define LED2_MASK	((1<<2) | (1<<3) | (1<<4) | (1<<5) | (1<<6))
#define LED1_MASK	((1<<28) | (1<<29) | (1<<31))

/************************** PRIVATE TYPES *************************/
//Watchodog time out in 2 seconds
#define WDT_TIMEOUT 	1000000


/************************** PRIVATE VARIABLES *************************/
uint8_t menu1[] =
"********************************************************************************\n\r"
"Hello NXP Semiconductors \n\r"
" Watch dog timer interrupt (test or debug mode) demo \n\r"
"\t - MCU: LPC17xx \n\r"
"\t - Core: Cortex M3 \n\r"
"\t - Communicate via: UART0 -  115200 bps \n\r"
" Use WDT with Internal RC OSC, interrupt mode (test only), timeout = 1 seconds \n\r"
" To generate an interrupt, after interrupt WDT interrupt is disabled immediately! \n\r"
"********************************************************************************\n\r";
uint8_t info1[] = "BEFORE WDT interrupt!\n\r";
uint8_t info2[] = "AFTER WDT interrupt\n\r";
// UART Configuration structure variable
UART_CFG_Type UARTConfigStruct;
//timer init
TIM_TIMERCFG_Type TIM_ConfigStruct;
TIM_MATCHCFG_Type TIM_MatchConfigStruct ;

__IO Bool wdt_flag = FALSE;
__IO Bool LED_toggle = FALSE;

/************************** PRIVATE FUNCTION *************************/
void print_menu(void);
void Timer_Wait(uint32_t time);


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



/*********************************************************************//**
 * @brief		Delay millisecond
 * @param[in]	time (ms)
 * @return 		None
 **********************************************************************/
void Timer_Wait(uint32_t time)
{
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
	// Set Match value, count value is timer (timer * 1000uS = timer mS )
	TIM_MatchConfigStruct.MatchValue   = time;

	// Set configuration for Tim_config and Tim_MatchConfig
	TIM_Init(LPC_TIM0, TIM_TIMER_MODE,&TIM_ConfigStruct);
	TIM_ConfigMatch(LPC_TIM0,&TIM_MatchConfigStruct);
	// To start timer 0
	TIM_Cmd(LPC_TIM0,ENABLE);
	while ( !(TIM_GetIntStatus(LPC_TIM0,0)));
	TIM_ClearIntPending(LPC_TIM0,0);

}

void WDT_IRQHandler(void)
{

	// Disable WDT interrupt
	NVIC_DisableIRQ(WDT_IRQn);

	// Set WDT flag according
	wdt_flag = TRUE;

	// Clear TimeOut flag
	WDT_ClrTimeOutFlag();

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

	/*
	 * Initialize debug via UART
	 */
	debug_frmwrk_init();

	// print welcome screen
	print_menu();

	/* Install interrupt for WDT interrupt */
	NVIC_SetPriority(WDT_IRQn, 0x10);
	// Set Watchdog use internal RC, just generate interrupt only in 1ms if Watchdog is not feed

	// Init WDT, IRC OSC, interrupt mode, timeout = 1000000 microsecond
	WDT_Init(WDT_CLKSRC_IRC, WDT_MODE_INT_ONLY);

	/* Enable the Watch dog interrupt*/
	NVIC_EnableIRQ(WDT_IRQn);

	// Start watchdog with timeout given
	WDT_Start(WDT_TIMEOUT);

	while (1){

		if (wdt_flag == FALSE){
			_DBG_(info1);
		} else {
			_DBG_(info2);
		}

		if (LED_toggle == FALSE){
			//turn on led
			FIO_ByteSetValue(2, 0, LED_PIN);
			LED_toggle = TRUE;
		} else {
			//turn off led
			FIO_ByteClearValue(2, 0, LED_PIN);
			LED_toggle = FALSE;
		}

		// Wait for 500 millisecond
		Timer_Wait(500);
	}
	return 1;
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

