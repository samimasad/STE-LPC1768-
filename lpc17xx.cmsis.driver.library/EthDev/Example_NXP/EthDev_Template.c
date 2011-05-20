/*
 * @file:    EthDev_Template.c
 * @purpose: Ethernet Device Functions Template
 * @version: V1.10
 * @date:    24. Feb. 2009
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


#include "EthDev.h"

/*----------------------------------------------------------------------------
  Ethernet Device local functions
 *----------------------------------------------------------------------------*/
static int           EthDev_Init        (void);
static int           EthDev_UnInit      (void);
static int           EthDev_SetMCFilter (int NumHwAddr, unsigned char *pHwAddr);
static int           EthDev_TxFrame     (void *pData, int size);
static void          EthDev_Lock        (void);
static void          EthDev_UnLock      (void);
static EthDev_STATUS EthDev_LinkChk     (void);

static void *        EthDev_RxFrame     (int size);
static void          EthDev_RxFrameReady(int size);

static int           phy_Rd             (unsigned int PhyReg);
static int           phy_Wr             (unsigned int PhyReg, unsigned short Data);

/*----------------------------------------------------------------------------
  Ethernet Device IO Block
 *----------------------------------------------------------------------------*/

/* This defines the ethernet adapter index used by the application:
     eth0 - first adapter
     eth1 - second adapter
     eth2 - third adapter
     eth3 - fourth adapter.
   The user needs to change only this define, nothing else in the driver. */
#define ETHDEV     eth1

EthDev_IOB ETHDEV = { /* The following entries are initialized by the user 
                         application  before the ETH->Init is called. */
                      EthDev_MODE_AUTO,
                      /* If MAC address is 00:00:00:00:00:00, then the driver
                         shall use it's own MAC which is stored to the EEPROM.
                         After the EEPROM MAC is used, the Init function shall
                         copy this MAC to this entry to provide the application
                         the MAC address for the Ethernet Device. 
                         If this MAC is not 0, then this MAC shall be used. */
                      0,0,0,0,0,0,
                      EthDev_RxFrame,      /* Place holder, initialized by the user */
                      EthDev_RxFrameReady, /* Place holder, initialized by the user */

                      /* The following entries are initialized by the EthDev driver. */
                      EthDev_Init,
                      EthDev_UnInit,
                      EthDev_SetMCFilter,
                      EthDev_TxFrame,
                      EthDev_Lock,
                      EthDev_UnLock,
                      EthDev_LinkChk
                    };

static EthDev_STATUS Status;

/*----------------------------------------------------------------------------
  Ethernet Device place holder function for user callback 
 *----------------------------------------------------------------------------*/
static void *EthDev_RxFrame (int size) {
  return (NULL);                         /* do nothing */
}


/*----------------------------------------------------------------------------
  Ethernet Device place holder function for user callback 
 *----------------------------------------------------------------------------*/
static void EthDev_RxFrameReady (int size) {
  ;                                      /* do nothing */
}


/*----------------------------------------------------------------------------
  Ethernet Device initialize
 *----------------------------------------------------------------------------*/
static int EthDev_Init (void) {

  /* Initialize Ethernet controller here and enable interrupts. */

  Status.Link = EthDev_LINK_DOWN;
  return (0);
}


/*----------------------------------------------------------------------------
  Ethernet Device Uninitialize
 *----------------------------------------------------------------------------*/
static int EthDev_UnInit (void) {

  /* Disable Ethernet controller interrupts, Power Down PHY */

  return (0);
}


/*----------------------------------------------------------------------------
  Ethernet Device Interrupt Disable
 *----------------------------------------------------------------------------*/
static void EthDev_Lock (void) {
  /* Disable Ethernet RX interrupts. */
}


/*----------------------------------------------------------------------------
  Ethernet Device Interrupt Enable
 *----------------------------------------------------------------------------*/
static void EthDev_UnLock (void) {
  /* Enable Ethernet RX interrupts. */
}


/*----------------------------------------------------------------------------
  Set Multicast Filter Address to ethernet controller 
 *----------------------------------------------------------------------------*/
static int EthDev_SetMCFilter (int NumHwAddr, unsigned char *pHwAddr) {

  return (0);
}
/*----------------------------------------------------------------------------
  Send frame to ENET ethernet controller 
 *----------------------------------------------------------------------------*/
static int EthDev_TxFrame (void *pData, int size) {

  return (0);
}


/*----------------------------------------------------------------------------
  Ethernet Device Ethernet Controller Interrupt function.
 *----------------------------------------------------------------------------*/
void EthernetIntHandler2 (void) __irq {               /* EthDev_ISR */

  /* Process Ethernet RX interrupts. */
}


/*----------------------------------------------------------------------------
  Ethernet Device Check the Link
 *----------------------------------------------------------------------------*/
static EthDev_STATUS EthDev_LinkChk (void) {
  /* This function needs to be called by the user application 
     in intervals from 100ms to 2s. After the ETH->Init is called, this function
     checks when the link is up and update the Ethernet controller registers
     such as Enable Duplex, adjust the clock for 100MBit if required. 
     This function also returns the status of the link:
       link:   UP/DOWN,
       duplex: HALF/FULL
       speed:  10/100
   */
  return (Status);
}


/*----------------------------------------------------------------------------
  Write a data 'Data' to PHY register 'PhyReg'.  
 *----------------------------------------------------------------------------*/
static int phy_Wr (unsigned int PhyReg, unsigned short Data) {

  /* Write data to PHY, return 0 if OK, -1 if error */

  return (-1);
}


/*----------------------------------------------------------------------------
  Read a PHY register 'PhyReg'. 
 *----------------------------------------------------------------------------*/
static int phy_Rd (unsigned int PhyReg) {

  /* Read data from PHY, return -1 if error, or 16-bit value if OK. */
  return (-1);
}


