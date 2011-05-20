/***********************************************************************
 * ID: dac_ex.c 2008-11-09 HieuNguyen
 *
 * Project: DAC driver example
 *
 * Description:
 *     This example is to test DAC peripheral through speaker on MCB2300 board.
 * The DAC peripheral will generate the sin tone, in which will increase
 * step by step to max and reduce to 0 and continue to increase to max again
 * in the same frequency continuously. We are able to hear the tone from the
 * speaker for many cycles.
 *
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
#include "lpc17xx_dac.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_pinsel.h"
#include "debug_frmwrk.h"

/************************** PRIVATE VARIABLES *************************/
uint8_t menu1[] =
"********************************************************************************\n\r"
"Hello NXP Semiconductors \n\r"
" DAC demo \n\r"
"\t - MCU: LPC17xx \n\r"
"\t - Core: ARM Cortex-M3 \n\r"
"\t - Communicate via: UART0 - 115200 kbps \n\r"
" Write new DAC value that increased by the time and output to speaker \n\r"
"********************************************************************************\n\r";
uint8_t menu2[] = "Starting DAC demo....... \n\r";


/************************** PRIVATE FUNCTION *************************/
void print_menu(void);

/**
 * @brief Print menu
 */
void print_menu(void)
{
	_DBG(menu1);
}

/**
 * @brief Main program body
 */
int c_entry(void)
{
	PINSEL_CFG_Type PinCfg;
	uint32_t time;
	float rate =0;

	/*
	 * Init DAC pin connect
	 * AOUT on P0.26
	 */
	PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 26;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);

	/*
	 * Initialize debug via UART
	 */
	debug_frmwrk_init();

	// print welcome screen
	print_menu();

	/* init DAC structure to default
	 * Maximum	current is 700 uA
	 * First value to AOUT is 0
	 */
	DAC_Init(LPC_DAC);

	_DBG(menu2);

	// main loop
	while (1)
	{
		for(time = 1; time < 0x3FF; time++)
		{
			DAC_UpdateValue ( LPC_DAC,(uint32_t)(time*rate));
		}

		rate += 0.001;

		if(rate >= 1)
		{
			rate = 0;
		}
	}

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

