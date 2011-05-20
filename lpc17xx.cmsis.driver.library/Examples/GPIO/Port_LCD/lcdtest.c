/**
 * @file	: lcdtest.c
 * @purpose	: This example used to test a 4-bit LCD via GPIO peripheral on
 * 			LPC1768
 * @version	: 1.0
 * @date	: 22. April. 2009
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
#include "lpc17xx_gpio.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_nvic.h"
#include "lpc17xx_pinsel.h"
#include "debug_frmwrk.h"

//#include "portlcd.h"
#include "GLCD.h"



/************************** PRIVATE MACROS *************************/
// Time out definition - used in blocking mode in read/write function
#define TIME_OUT	10000


/************************** PRIVATE TYPES *************************/


/************************** PRIVATE VARIABLES *************************/
uint8_t menu1[] =
"********************************************************************************\n\r"
"Hello NXP Semiconductors \n\r"
" 4-bit LCD with GPIO demo \n\r"
"\t - MCU: LPC17xx \n\r"
"\t - Core: ARM Cortex-M3 \n\r"
"\t - Communicate via: UART0 - 115200bps \n\r"
" This example used to test LCD via GPIO peripheral on LPC1768 \n\r"
"********************************************************************************\n\r";
uint8_t menu2[] = "Demo terminated! \n\r";


// UART Configuration structure variable
UART_CFG_Type UARTConfigStruct;

uint8_t lcd_text[2][16+1] = {"   NXP SEMI.    ",      /* Buffer for LCD text      */
                          "  LPC1768/CM3" };


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
 * @brief	Main 4-bit LCD porting with GPIO program body
 **********************************************************************/
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

	/* Initialize debug
	 */
	debug_frmwrk_init();

	// print welcome screen
	print_menu();

	/* LCD block section -------------------------------------------- */
	GLCD_Init();
//	LCD_cur_off();
	GLCD_Clear(White);

	/* Update LCD Module display text. */
	GLCD_DisplayString(0,0, lcd_text[0] );
	GLCD_DisplayString(1,2, lcd_text[1] );

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
