/******************************************************************************
 * @file:    HTTP_demo.c
 * @purpose: CMSIS Cortex-M3 Core Peripheral Access Layer Source File
 *           HTTP demo using CMSIS EthDev
 * @version: V1.0
 * @date:    13. May 2009
 *----------------------------------------------------------------------------
 *
 * Copyright (C) 2008 ARM Limited. All rights reserved.
 *
 * ARM Limited (ARM) is supplying this software for use with Cortex-M3 
 * processor based microcontrollers.  This file can be freely distributed 
 * within development tools that are supporting such ARM based processors. 
 *
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/

#include <LPC17xx.h>                    /* LPC17xx definitions               */
#include <stdio.h>
#include <RTL.h>
#include <Net_Config.h>
#include "EthDev.h"
#include "GLCD.h"
#include "LED.h"
#include "KBD.h"
#include "ADC.h"


/*---local variables----------------------------------------------------------*/
uint32_t LEDval;
BOOL     LEDrun;
BOOL     LCDupdate;
BOOL     tick;
uint32_t  dhcp_tout;
uint8_t   lcd_text[6][20+1] = {"  MCB1700 CMSIS     ",         /* Buffer for LCD text   */
                               "    HTTP Demo       ",
                               "",
                               " Waiting for Link   ",
                               "",
                               ""
                              };

/*---Interface to ETh CMSIS --------------------------------------------------*/
extern EthDev_IOB eth0;
EthDev_IOB *pEth;
EthDev_STATUS EthState;


/*---Interface to TCP Stack---------------------------------------------------*/
extern LOCALM localm[];                             /* Local Machine Settings */
#define MY_IP localm[NETIF_ETH].IpAdr
#define DHCP_TOUT   50                              /* DHCP timeout 5 seconds */


volatile uint32_t msTicks;                          /* counts 100ms timeTicks */
/*----------------------------------------------------------------------------
  SysTick_Handler
 *----------------------------------------------------------------------------*/
void SysTick_Handler(void) {
  msTicks++;                        /* increment counter necessary in Delay() */
}

/*----------------------------------------------------------------------------
  delays number of tick Systicks (happens every 100 ms)
 *----------------------------------------------------------------------------*/
__inline static void Delay (uint32_t dlyTicks) {
  uint32_t curTicks;

  curTicks = msTicks;
  while ((msTicks - curTicks) < dlyTicks);
}

/*----------------------------------------------------------------------------
  update the GLCD
 *----------------------------------------------------------------------------*/
static void upd_display () {

   GLCD_clearLn (Line3);
   GLCD_displayStringLn(Line3, lcd_text[3]);
   GLCD_clearLn (Line4);
   GLCD_displayStringLn(Line4, lcd_text[4]);
   GLCD_clearLn (Line5);
   GLCD_displayStringLn(Line5, lcd_text[5]);

   LCDupdate =__FALSE;
}


/*----------------------------------------------------------------------------
  initialize the GLCD
 *----------------------------------------------------------------------------*/
static void init_display () {

  GLCD_init();                            /* initialize GLCD */

  GLCD_clear(White);
  GLCD_setBackColor(Blue);                        /* Set the Back Color    */
  GLCD_setTextColor(White);                       /* Set the Text Color    */
  GLCD_displayStringLn(Line0, lcd_text[0]);
  GLCD_displayStringLn(Line1, lcd_text[1]);
  GLCD_setBackColor(White);                        /* Set the Back Color    */
  GLCD_setTextColor(Blue);                         /* Set the Text Color    */

  upd_display ();
}


/*----------------------------------------------------------------------------
  Timer polling
 *----------------------------------------------------------------------------*/
static void timer_poll () {
  static uint32_t msTicksLast;

  if (msTicksLast != msTicks)  {                /* Timer tick every 100 ms */
      msTicksLast = msTicks;
      timer_tick ();
      tick = __TRUE;
   }
}


/*----------------------------------------------------------------------------
  Blink the LEDs on an eval board
 *----------------------------------------------------------------------------*/
static void LED_run () {
   const uint8_t led_val[16] = { 0x48,0x88,0x84,0x44,0x42,0x22,0x21,0x11,
                                 0x12,0x0A,0x0C,0x14,0x18,0x28,0x30,0x50 };
   static uint32_t cnt;

   if (tick == __TRUE) {                        /* Every 100 ms */
      tick = __FALSE;
      if (LEDrun == __TRUE) {
         LED_out (led_val[cnt]);
         if (++cnt >= sizeof(led_val)) {
            cnt = 0;
         }
      }
      if (LCDupdate == __TRUE) {
         upd_display ();
      }
   }
}


/*----------------------------------------------------------------------------
  Get ADC value (Read Analog input)
 *----------------------------------------------------------------------------*/
uint16_t AD_in (void) {
   U32 val = 0;

   ADC_startCnv();                                  /* start A/D conversion   */
   val = ADC_getCnv();                              /* read converted value   */
   ADC_stopCnv();                                   /* stop A/D conversion    */

   return (val);
}


/*----------------------------------------------------------------------------
  Get Button Status  (Read ARM Digital Input)
 *----------------------------------------------------------------------------*/
uint8_t get_button (void) {
   uint32_t val = 0;

   val = KBD_get();                                 /* read Joystick state    */
   val = (~val & KBD_MASK);                         /* key pressed is '0'     */

   return (val);
}


/*----------------------------------------------------------------------------
  Check the Link status
 *----------------------------------------------------------------------------*/
void link_check (void) {
   EthDev_STATUS newState;

   if (tick == __FALSE) {
      return;
   }
   
   newState = pEth->LinkChk ();                             /* Every 100 ms */
   if (newState.Link == EthState.Link) {
      return;                                               /* No change. */
   }
   EthState = newState;

   if (EthState.Link == EthDev_LINK_DOWN) {
      MY_IP[0] = MY_IP[1] =  MY_IP[2] = MY_IP[3] = 0;       /* reset IP address */
      sprintf((char *)lcd_text[2],"                    ");
      sprintf((char *)lcd_text[3]," Waiting for Link   ");
      sprintf((char *)lcd_text[4],"                    ");
      sprintf((char *)lcd_text[5],"                    ");
      LCDupdate = __TRUE;
      pEth->UnInit ();
      init_TcpNet ();
   }
   else {
      sprintf((char *)lcd_text[2],"                    ");
      sprintf((char *)lcd_text[3]," Waiting for DHCP   ");
      sprintf((char *)lcd_text[4],"                    ");
      sprintf((char *)lcd_text[5],"                    ");
      LCDupdate = __TRUE;
      dhcp_tout = DHCP_TOUT;
   }
}


/*----------------------------------------------------------------------------
  Monitor DHCP IP address assignment
 *----------------------------------------------------------------------------*/
static void dhcp_check () {

   if (tick == __FALSE || dhcp_tout == 0) {
      return;
   }

   if (mem_test (&MY_IP, 0, IP_ADRLEN) == __FALSE && !(dhcp_tout & 0x80000000)) {
      /* Success, DHCP has already got the IP address. */
      dhcp_tout = 0;
      sprintf((char *)lcd_text[2]," IP address:");
      sprintf((char *)lcd_text[3]," %d.%d.%d.%d", MY_IP[0], MY_IP[1], MY_IP[2], MY_IP[3]);
      sprintf((char *)lcd_text[4],((EthState.Speed  == EthDev_SPEED_100M) ?
                                    " speed : 100M" : " speed: 10M"));
      sprintf((char *)lcd_text[5],((EthState.Duplex == EthDev_DUPLEX_FULL)?
                                    " duplex: FULL" : " duplex: HALF"));
      LCDupdate = __TRUE;
      return;
   }
   if (--dhcp_tout == 0) {
      /* A timeout, disable DHCP and use static IP address. */
      dhcp_disable ();
      sprintf((char *)lcd_text[3]," DHCP failed" );
      LCDupdate = __TRUE;
      dhcp_tout = 30 | 0x80000000;              /* set timer to 3 sec to display message */
      return;
   }
   if (dhcp_tout == 0x80000000) {
      dhcp_tout = 0;
      sprintf((char *)lcd_text[2]," IP address:");
      sprintf((char *)lcd_text[3]," %d.%d.%d.%d", MY_IP[0], MY_IP[1], MY_IP[2], MY_IP[3]);
      sprintf((char *)lcd_text[4],((EthState.Speed  == EthDev_SPEED_100M) ?
                                    " speed : 100M" : " speed: 10M"));
      sprintf((char *)lcd_text[5],((EthState.Duplex == EthDev_DUPLEX_FULL)?
                                    " duplex: FULL" : " duplex: HALF"));
      LCDupdate = __TRUE;
   }
}


/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {

  SystemInit();									 /* setup clocks */
  if (SysTick_Config(SystemFrequency / 10)) {    /* Setup SysTick Timer for 100 msec interrupts  */
    while (1);                                   /* Capture error */
  }
  

  init_display ();                               /* initialize the display */
  LED_init ();                                   /* initialize LEDs */
  KBD_init ();                                   /* initialize Push Button */
  ADC_init ();                                   /* initialize ADC */
  
  pEth = &eth0;                                  /* Select 'eth0' as active communication interface. */
  EthState.Link = EthDev_LINK_DOWN;
  init_TcpNet ();

  dhcp_tout = DHCP_TOUT;
  LEDrun    = __TRUE;

  while (1) {
    LED_run ();
    switch (EthState.Link) {
      case 0:                                    /* link down */
        timer_poll ();
        link_check ();
        break;
  
      case 1:                                    /* link up */
        timer_poll ();
        link_check ();
        main_TcpNet ();
        dhcp_check ();
        break;
     }
  }

}
