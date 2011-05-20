/*
 * @file:    UartDev.h
 * @purpose: UART Device Definitions
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

#ifndef _UARTDEV_H_
#define _UARTDEV_H_

/*
 * Should not choose these option at the same time
 */
//#define UART_POLLING_MODE 1
#define UART_INTERRUPT_MODE 1


/*----------------------------------------------------------------------------
  Serial Device
  Defines
 *---------------------------------------------------------------------------*/
#define UartDev_DATABITS_5                 5
#define UartDev_DATABITS_6                 6
#define UartDev_DATABITS_7                 7
#define UartDev_DATABITS_8                 8


#define UartDev_STOPBITS_1                 1
#define UartDev_STOPBITS_2                 2


#define UartDev_PARITY_NONE                0
#define UartDev_PARITY_ODD                 1
#define UartDev_PARITY_EVEN                2


#define UartDev_FLAG_BLOCKING              0
#define UartDev_FLAG_NONBLOCKING           1


/*----------------------------------------------------------------------------
  UART Device
  Configuration Structure
 *----------------------------------------------------------------------------*/
typedef struct {
   uint32_t BaudRate:20;                                            /*!< Baudrate */
   uint32_t DataBits:4;                                             /*!< Data bits */
   uint32_t StopBits:3;                                             /*!< Stop bits */
   uint32_t Parity:2;                                               /*!< Parity */
   uint32_t Reserved:3;
} UartDev_CFG;


/*----------------------------------------------------------------------------
  UART Device
  IO Block Structure
 *----------------------------------------------------------------------------*/
typedef struct {

   /* changed by the user application before call to Init. */
   UartDev_CFG   Cfg;
   /* Initialized by UART driver. */
   int (*Init)    (void);                                        /*!< Initialize */
   int (*UnInit)  (void);                                        /*!< unInitialize */
   int (*BufTx)   (void *pData, int* pSize, unsigned int flags); /*!< Transmit */
   int (*BufRx)   (void *pData, int* pSize, unsigned int flags); /*!< Receive */
   int (*BufFlush)(void);                                        /*!< Flush buffer */
} UartDev_IOB;

#endif
