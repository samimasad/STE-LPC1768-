/*
 * @file:    SpiDev.h
 * @purpose: SPI Device Definitions
 * @version: V1.10 First Release
 * @date:    24. Feb. 2009
 *----------------------------------------------------------------------------
 *
 * Copyright (C) 2009 ARM Limited. All rights reserved.
 *
 * ARM Limited (ARM) is supplying this software for use with Cortex-Mx
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

#ifndef _SPIDEV_H_
#define _SPIDEV_H_

/*----------------------------------------------------------------------------
  SPI Serial Device
  Defines
 *---------------------------------------------------------------------------*/
#define SpiDev_CLOCK_POLARITY_IDLELOW      0
#define SpiDev_CLOCK_POLARITY_IDLEHIGH     1

#define SpiDev_CLOCK_PHASE_FIRSTEDGE       0
#define SpiDev_CLOCK_PHASE_SECONDEDGE      1

#define SpiDev_SSO_DISABLED                0
#define SpiDev_SSO_ENABLED                 1


/*----------------------------------------------------------------------------
  UART Device
  Configuration Structure
 *----------------------------------------------------------------------------*/
typedef struct {
   uint32_t Baudrate:25;                                      /*!< Baudrate */
   uint32_t Polarity:1;                                       /*!< clock polarity */
   uint32_t Phase:1;                                          /*!< clock Phase */
   uint32_t SlaveSelect:1;                                    /*!< Slave Select Output */
   uint32_t Reserved:4;
} SpiDev_CFG;


/*----------------------------------------------------------------------------
  UART Device
  IO Block Structure
 *----------------------------------------------------------------------------*/
typedef struct {

   /* changed by the user application before call to Init. */
   SpiDev_CFG   Cfg;
   /* Initialized by SPI driver. */
   int (*Init)       (void);                                            /*!< Initialize */
   int (*UnInit)     (void);                                            /*!< unInitialize */
   int (*BufTxRx)    (void *pDataTx, void *pDataRx, unsigned int Size); /*!< Transmit buffer*/
   int (*SetBaudrate)(unsigned int Baudrate);                           /*!< set different Baudrate */
} SpiDev_IOB;

#endif
