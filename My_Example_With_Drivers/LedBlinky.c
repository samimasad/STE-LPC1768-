/******************************************************************************/
/* BLINKY.C: LED Flasher                                                      */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2008 Keil Software. All rights reserved.                     */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#include "lpc17xx_gpio.h"
//#include "lpc17xx_libcfg.h"
#include "lpc17xx_nvic.h"

//#define LED_NUM     4                   /* Number of user LEDs                */
#define LED_NUM     3                   /* Number of user LEDs , do not use first LED , used by USB               */

//const unsigned long led_mask[] = { 1L<<18,1L<<20,1<<21, 1<<23 };
const unsigned long led_mask[] = {1L<<20,1<<21, 1<<23 };			/* Number of user LEDs , do not use first LED , used by USB   */
volatile unsigned long SysTickCnt;      /* SysTick Counter                    */

void SysTick_Handler (void) {           /* SysTick Interrupt Handler (1ms)    */
  SysTickCnt++;
}

void Delay (unsigned long tick) {       /* Delay Function                     */
  unsigned long systickcnt;

  systickcnt = SysTickCnt;
  while ((SysTickCnt - systickcnt) < tick);
}

int entry_led (void) {                       /* Main Program                       */
  int num = -1;
  int dir =  1;
/*
	//  Set Vector table offset value
#if (__RAM_MODE__==1)
	NVIC_SetVTOR(0x10000000);
#else
	NVIC_SetVTOR(0x00000000);
#endif
*/


  SysTick_Config(SystemCoreClock/1000 - 1); /* Generate interrupt each 1 ms   */


  GPIO_SetDir(1, 0xB40000, 1);           /* LEDs on PORT1 defined as Output    */
  //GPIO_SetDir(2, 0x0000007C, 1);           /* LEDs on PORT2 defined as Output    */

  GPIO_ClearValue(1, 0xB40000);
  //GPIO_ClearValue(1, 0xB0000000);
  //GPIO_ClearValue(2, 0x0000007C);

  for (;;) {                            /* Loop forever                       */
    /* Calculate 'num': 0,1,...,LED_NUM-1,LED_NUM-1,...,1,0,0,...             */
    num += dir;
    if (num == LED_NUM) { dir = -1; num =  LED_NUM-1; }
    else if   (num < 0) { dir =  1; num =  0;         }
/*
    GPIO_ClearValue(1, 0xFFFFFFFF);
    Delay(500);
    GPIO_SetValue(1, 0xFFFFFFFF);
    Delay(500);
*/



    GPIO_SetValue(1, led_mask[num]);
    vTaskDelay(500);
    //Delay(500);                         /* Delay 500ms                        */
    GPIO_ClearValue(1, led_mask[num]);
    //Delay(500);                         /* Delay 500ms                        */
    vTaskDelay(500);

    //vTaskDelete(NULL);


  }

}


/* With ARM and GHS toolsets, the entry point is main() - this will
   allow the linker to generate wrapper code to setup stacks, allocate
   heap area, and initialize and copy code and data segments. For GNU
   toolsets, the entry point is through __start() in the crt0_gnu.asm
   file, and that startup code will setup stacks and data */
void vLEDTask( void *pvParameters )
{
	entry_led();
    return ;
}


