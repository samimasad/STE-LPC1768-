/*
 * @file:    EMAC_17xx.c
 * @purpose: Wrapper for the CMSIS LPC17xx EMAC driver
 * @version: V0.01
 * @date:    13. May 2009
 *----------------------------------------------------------------------------
 *
 * Copyright (C) 2009 ARM Limited. All rights reserved.
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
 */

#include <stddef.h>
#include <Net_Config.h>
#include "EthDev.h"

extern U8 own_hw_adr[];                                    /* Net_Config.c    */
extern EthDev_IOB *pEth;

/*----------------------------------------------------------------------------
  Ethernet Device
  callback frame received
 *----------------------------------------------------------------------------*/
void *RxFrame (int size) {
  OS_FRAME *frame;

  frame = alloc_mem (size | 0x80000000);                   /* Flag 0x80000000 to skip sys_error()  */
                                                           /* call when out of memory.             */
  if (frame != NULL) {                                     /* 'alloc_mem()' ok */
    put_in_queue (frame);
    return (&frame->data[0]);
  }
  return (NULL);
}

///*----------------------------------------------------------------------------
//  Ethernet Device
//  callback frame copied
// *----------------------------------------------------------------------------*/
//void RxFrameReady (int size) {
//  int frameSize;
//
//  frameSize = size;                                        /* test purpose only */
//}

/*--------------------------- init_ethernet ---------------------------------*/

void init_ethernet (void) {
  int i;
  unsigned char McAddr[6];

  pEth->Mode = EthDev_MODE_AUTO;

  for (i = 0; i < 6; i++) {
    pEth->HwAddr[i] = own_hw_adr[i];
  }

  pEth->RxFrame = RxFrame;
//  pEth->RxFrameReady = RxFrameReady;
  pEth->Init ();
  
  pEth->SetMCFilter (1, McAddr);

}

/*--------------------------- send_frame ------------------------------------*/

void send_frame (OS_FRAME *frame) {
  int retCode;

  do {
    retCode = pEth->TxFrame (frame->data, frame->length);
  } while (retCode == -1);
}

/*--------------------------- int_enable_eth --------------------------------*/

void int_enable_eth (void) {
  pEth->UnLock ();
}


/*--------------------------- int_disable_eth -------------------------------*/

void int_disable_eth (void) {
  pEth->Lock ();
}


/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
