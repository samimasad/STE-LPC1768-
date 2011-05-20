/**
 * @file 	eint_powerdown_test.c
 * @brief 	An example using external interrupt on INT0 as wake up source in
 * 			each power mode:
 * 			- Sleep.
 * 			- Deep Sleep.
 * 			- Power Down.
 * @version	: 1.0
 * @date	: 05. Sep. 2009
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


#include "LPC17xx.h"                    // LPC17xx definitions
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_clkpwr.h"
#include "lpc17xx_nvic.h"
#include "lpc17xx_libcfg.h"


/* Select one power mode at the same time as following */
#define SLEEP_MODE
//#define DEEPSLEEP_MODE
//#define POWERDOWN_MODE


/* LED pin in byte style on P1 */
#define POLL_LED	(1<<4)		// P1.28
#define INT0_LED	(1<<5)		// P1.29
#define INT3_LED	(1<<7)		// P1.31
#define LED2_MASK	((1<<2) | (1<<3) | (1<<4) | (1<<5) | (1<<6))
#define LED1_MASK	(POLL_LED | (INT0_LED) | (INT3_LED))

/**
 * @brief Delay function
 */
void delay (void) {
  unsigned int i;

  for (i = 0; i < 0x100000; i++) {
  }
}


/**
 * @brief External Interrupt 0 Handler
 */
void EINT0_IRQHandler(void)
{
	      int i;
		  LPC_SC->EXTINT |= 0x1;  //clear the EINT0 flag
		  for (i= 0; i<10; i++)
		  {
		    FIO_ByteSetValue(1, 3, INT0_LED);
			delay();
			FIO_ByteClearValue(1, 3, INT0_LED);
		    delay();
          }
}


void InitLED(void)
{
	//p1.31	, P1.29	and P1.28 are outputs
	FIO_ByteSetDir(1, 3, LED1_MASK, 1);
	FIO_ByteSetDir(2, 0, LED2_MASK, 1);
	// Turn off all LEDs
	FIO_ByteClearValue(1, 3, LED1_MASK);
	FIO_ByteClearValue(2, 0, LED2_MASK);
}


// Main Program
int main (void)
{
	PINSEL_CFG_Type PinCfg;
	uint32_t cnt;

	//  Set Vector table offset value
#if (__RAM_MODE__==1)
	NVIC_SetVTOR(0x10000000);
#else
	NVIC_SetVTOR(0x00000000);
#endif

	InitLED();

	//Initialize EXT registers
	LPC_SC->EXTINT = 0x0;
	LPC_SC->EXTMODE = 0x0;
	LPC_SC->EXTPOLAR = 0x0;

	/* edge sensitive */
	LPC_SC->EXTMODE = 0xF;
	/* falling-edge sensitive */
	LPC_SC->EXTPOLAR = 0x0;
	/* External Interrupt Flag cleared*/
	LPC_SC->EXTINT = 0xF;

	/* P2.10 as /EINT0 */
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 10;
	PinCfg.Portnum = 2;
	PINSEL_ConfigPin(&PinCfg);

	NVIC_SetPriorityGrouping(4);
	NVIC_SetPriority(EINT0_IRQn, 0);

	NVIC_EnableIRQ(EINT0_IRQn);

	for (cnt = 0; cnt < 5; cnt++){
		FIO_ByteSetValue(1, 3, POLL_LED);
		delay();
		FIO_ByteClearValue(1, 3, POLL_LED);
		delay();
	}

	// Enter target power down mode
#ifdef SLEEP_MODE
	CLKPWR_Sleep();
#endif

#ifdef DEEPSLEEP_MODE
	CLKPWR_DeepSleep();
#endif

#ifdef POWERDOWN_MODE
	CLKPWR_PowerDown();
#endif

	// MCU will be here after waking up
	while (1)
	{
		FIO_ByteSetValue(1, 3, POLL_LED);
		delay();
		FIO_ByteClearValue(1, 3, POLL_LED);
		delay();
	}
}



/* With ARM and GHS toolsets, the entry point is main() - this will
   allow the linker to generate wrapper code to setup stacks, allocate
   heap area, and initialize and copy code and data segments. For GNU
   toolsets, the entry point is through __start() in the crt0_gnu.asm
   file, and that startup code will setup stacks and data */
int c_entry(void)
{
    return main();
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
