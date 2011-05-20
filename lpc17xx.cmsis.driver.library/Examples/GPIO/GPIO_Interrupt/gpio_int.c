/******************************************************************************/
/* BLINKY.C: LED Flasher                                                      */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2008 Keil Software. All rights reserved.                     */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#include "LPC17xx.h"                    // LPC17xx definitions
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_nvic.h"
#include "lpc17xx_libcfg.h"

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



/**
 * @brief External Interrupt 3 Handler
 */
void EINT3_IRQHandler(void)
{
	      int j;
		  if (LPC_GPIOINT->IO0IntStatF == 0x02000000)
		  {
			  LPC_GPIOINT->IO0IntClr = 0x02000000;
			  for (j= 0; j<10; j++)
			  {
				    FIO_ByteSetValue(1, 3, INT3_LED);
					delay();
					FIO_ByteClearValue(1, 3, INT3_LED);
				    delay();
			  }
          }
}

// Main Program
int main (void)
{
	PINSEL_CFG_Type PinCfg;

	//  Set Vector table offset value
#if (__RAM_MODE__==1)
	NVIC_SetVTOR(0x10000000);
#else
	NVIC_SetVTOR(0x00000000);
#endif

	//p1.31	, P1.29	and P1.28 are outputs
	FIO_ByteSetDir(1, 3, LED1_MASK, 1);
	FIO_ByteSetDir(2, 0, LED2_MASK, 1);
	// Turn off all LEDs
	FIO_ByteClearValue(1, 3, LED1_MASK);
	FIO_ByteClearValue(2, 0, LED2_MASK);


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

	// Enable GPIO interrupt  P0.25/AD0.2
	LPC_GPIOINT->IO0IntEnF	= 0x02000000;

	NVIC_SetPriorityGrouping(4);  //sets PRIGROUP to 3:2 (XXX:YY)
	NVIC_SetPriority(EINT0_IRQn, 0);   //000:00 (bit 7:3) assign eint0 to group 0, sub-priority 0 within group 0
	NVIC_SetPriority(EINT3_IRQn, 4);   //001:00 (bit 7:3) assign GPIO int to group 1, sub-priority 0 within group 1


	NVIC_EnableIRQ(EINT0_IRQn);
	NVIC_EnableIRQ(EINT3_IRQn);

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
