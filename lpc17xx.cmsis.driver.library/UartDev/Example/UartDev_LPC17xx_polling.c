/*
 * @file:    UartDev_LPC17xx.c
 * @purpose: UART Device Functions for LPC17xx
 * @version: V1.10 First Release
 * @date:    1. April. 2009
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


#include "LPC17xx.h"
#include "UartDev.h"
#include "UartDev_LPC17xx.h"
#include "lpc17xx_pinsel.h"

#if UART_POLLING_MODE


/*----------------------------------------------------------------------------
  UART Device "LPC17xx"
  defines to configure the UART device
 *----------------------------------------------------------------------------*/
#define  _UART_		0	/* used UART (0..3) */


/* If UART0 is used ---------------------- */
#if (_UART_==0)
/** UART0 pin re-configuration, should be:
 * - 0 (default): 	TXD0 is on P0.2
 * 					RXD0 is on P0.3
 */
#define _UART_RECONFIG_PIN	0
#endif

/* If UART1 is used ---------------------- */
#if (_UART_==1)
/** UART1 pin re-configuration, should be:
 * - 0 (default): 	TXD1 is on P0.15
 * 					RXD1 is on P0.16
 *					CTS1 is on P0.17
 *					DCD1 is on P0.18
 *					DSR1 is on P0.19
 *					DTR1 is on P0.20
 *					RI1 is on P0.21
 *					RTS1 is on P0.22
 *
 * - 1: 			TXD1 is on P2.0
 * 					RXD1 is on P2.1
 *					CTS1 is on P2.2
 *					DCD1 is on P2.3
 *					DSR1 is on P2.4
 *					DTR1 is on P2.5
 *					RI1 is on P2.6
 *					RTS1 is on P2.7
 */
#define _UART_RECONFIG_PIN	0
#endif


/* If UART2 is used ---------------------- */
#if (_UART_==2)
/** UART2 pin re-configuration, should be:
 * - 0 (default): 	TXD2 is on P0.10
 * 					RXD2 is on P0.11
 *
 * - 1: 			TXD2 is on P2.8
 * 					RXD2 is on P2.9
 */
#define _UART_RECONFIG_PIN	0
#endif



/* If UART3 is used ---------------------- */
#if (_UART_==3)
/** UART3 pin re-configuration, should be:
 * - 0 (default): 	TXD3 is on P0.0
 * 					RXD3 is on P0.1
 *
 * - 1: 			TXD3 is on P0.25
 * 					RXD3 is on P0.26
 *
 * - 2: 			TXD3 is on P4.28
 * 					RXD3 is on P4.29
 */
#define _UART_RECONFIG_PIN	0
#endif

/*----------------------------------------------------------------------------
  UART Device "LPC17xx"
  defines to configure the buffer handling
  Note: The length of the buffers must be a power of 2.
        The defined size is used for transmit and receive buffer
 *----------------------------------------------------------------------------*/
#define _BUF_SIZE_   128	     /* Must be a power of 2 (2,4,8,16,32,64,128,...) */



/*----------------------------------------------------------------------------
  UART Device "LPC17xx"
  function prototypes
 *----------------------------------------------------------------------------*/
static int  UartDev_Init    (void);
static int  UartDev_UnInit  (void);
static int  UartDev_BufTx   (void *pData, int* pSize, unsigned int flags);
static int  UartDev_BufRx   (void *pData, int* pSize, unsigned int flags);
static int  UartDev_BufFlush(void);

static void ConfigureUart (UartDev_CFG* pCfg);
static void ConfigurePins  (void);




/*----------------------------------------------------------------------------
  UART Device "LPC17xx" IO Block
 *----------------------------------------------------------------------------*/

#define UART_DEV     uart0


UartDev_IOB UART_DEV = {
                     {115200, UartDev_DATABITS_8, UartDev_STOPBITS_1, UartDev_PARITY_NONE},

                     UartDev_Init,
                     UartDev_UnInit,
                     UartDev_BufTx,
                     UartDev_BufRx,
                     UartDev_BufFlush
                       };


#define _STATE_UNKNOWN_          0
#define _STATE_INITIALIZED_      1

/*----------------------------------------------------------------------------
  Serial Device "LPC17xx"
  local typedefs
 *----------------------------------------------------------------------------*/
typedef struct {
            int    UsartNr;                           /* UART number 1..n */
            int    State;                             /* Status */
  UART_TypeDef  *pUart;                             /* pointer to UART struct */
       uint32_t    UsartPeriph;                       /* UART Paripheral Clock */
      IRQn_Type    UsartIRQn;                         /* UART Interrupt number */
} UART_INFO;


/*----------------------------------------------------------------------------
  Serial Device "LPC17xx"
  local variables
 *----------------------------------------------------------------------------*/
static UART_INFO UARTInfo = {
#if   (_UART_ == 0)
  1,                                                  /* UART#0 */
  _STATE_UNKNOWN_,                                    /* unintialized */
  UART0,
  0,
  UART1_IRQn

#elif   (_UART_ == 1)
  1,                                                  /* UART#1 */
  _STATE_UNKNOWN_,                                    /* unintialized */
  UART1,
  0,
  UART1_IRQn

#elif (_UART_ == 2)
  2,                                                  /* UART#2 */
  _STATE_UNKNOWN_,                                    /* unintialized */
  UART2,
  0,
  UART2_IRQn

#elif (_UART_ == 3)
  3,                                                  /* UART#3 */
  _STATE_UNKNOWN_,                                    /* unintialized */
  UART3,
  0,
  UART3_IRQn

#else
  #error UART not defined!
#endif

};



/*----------------------------------------------------------------------------
  Serial Device "LPC17xx"
  initialize
 *----------------------------------------------------------------------------*/
static int UartDev_Init (void) {
  int i;

  if (UARTInfo.State != _STATE_UNKNOWN_) {
    return (-1);                                      /* Device initialized */
  }

  ConfigureUart (&UART_DEV.Cfg);

  ConfigurePins ();                                   /* Re-configure the UART pins */

  UARTInfo.State = _STATE_INITIALIZED_;

  UART_TxCmd (UARTInfo.pUart, ENABLE); 					/* TX, RX enable */

  for (i = 0; i < 800; i++);                            /* eleminate schmierzeichen */

  return(0);                                       /* success */
}


/*----------------------------------------------------------------------------
  Serial Device "LPC17xx"
  uninitialize
 *----------------------------------------------------------------------------*/
static int UartDev_UnInit (void) {

   UART_DeInit(UARTInfo.pUart);

   UARTInfo.State = _STATE_UNKNOWN_;

   return(0);                                          /* Success */
}


/*----------------------------------------------------------------------------
  Serial Device "LPC17xx"
  Send buffer
 *----------------------------------------------------------------------------*/
static int UartDev_BufTx (void *pData, int* pSize, unsigned int flags) {
  char *pChar;
   int   bytesToWrite;

  if (UARTInfo.State != _STATE_INITIALIZED_) {
    return (-1);                                      /* Device not initialized */
  }

  if ((pData == 0 ) && (*pSize > 0)) {
    return (-1);                                      /* Parameter not valid */
  }

  pChar = (char *)pData;
  bytesToWrite = *pSize;

  if (flags == UartDev_FLAG_BLOCKING) {               /* Blocking transmission */

    while (bytesToWrite-- > 0) {
		UART_Send(UARTInfo.pUart,(uint8_t *)pChar++, 1, BLOCKING);	/* transmit character */
    }
  } else {                                            /* nonBlocking transmission */
      if (bytesToWrite-- > 0) {
    	  if (UART_Send(UARTInfo.pUart, (uint8_t *)pChar++, 1, NONE_BLOCKING)){	/* transmit character */
    		  *pSize = 1;
    	  } else {
    		  *pSize = 0;
    	  }
      } else {
    	  *pSize = 0;
      }
  }

  return(0);                                          /* success */
}



/*----------------------------------------------------------------------------
  Serial Device "LPC17xx"
  Read Buffer
 *----------------------------------------------------------------------------*/
static int UartDev_BufRx (void *pData, int* pSize, unsigned int flags) {
  char *pChar;
  int    bytesToRead;
  uint8_t tmpc;

  if (UARTInfo.State != _STATE_INITIALIZED_) {
    return (-1);                                      /* Device not initialized */
  }

  if ((pData == 0 ) && (*pSize > 0)) {
    return (-1);                                      /* Parameter not valid */
  }

  pChar = (char *)pData;
  bytesToRead = *pSize;

  if (flags == UartDev_FLAG_BLOCKING) {               /* Blocking transmission */
    while (bytesToRead-- > 0) {
		if (UART_Receive(UARTInfo.pUart, &tmpc, 1, BLOCKING)){
			*pChar++ = tmpc;      /* receive character */
		}
    }
  } else {                                            /* nonBlocking transmission */
    if (UART_Receive(UARTInfo.pUart, &tmpc, 1, NONE_BLOCKING)) {
    	*pChar++ = tmpc;      /* receive character */
    	*pSize = 1;
    } else {
      *pSize = 0;
    }
  }

  return(0);                                          /* Success */
}


/*----------------------------------------------------------------------------
  Serial Device "LPC17xx"
  Read Buffer
 *----------------------------------------------------------------------------*/
static int UartDev_BufFlush(void) {

  if (UARTInfo.State != _STATE_INITIALIZED_) {
    return (-1);                                      /* Device not initialized */
  }

  return(0);                                          /* Success */
}



/*----------------------------------------------------------------------------
  UART Device "LPC17xx"
  Configure UART
 *----------------------------------------------------------------------------*/
static void ConfigureUart (UartDev_CFG *pCfg) {

	UART_CFG_Type hwCfg;

	/* Baudrate --------------------------- */
	hwCfg.Baud_rate = pCfg->BaudRate;

	/* Databit --------------------------- */
	switch (pCfg->DataBits)
	{
	case UartDev_DATABITS_5:
		hwCfg.Databits = UART_DATABIT_5;
		break;

	case UartDev_DATABITS_6:
		hwCfg.Databits = UART_DATABIT_6;
		break;

	case UartDev_DATABITS_7:
		hwCfg.Databits = UART_DATABIT_7;
		break;

	case UartDev_DATABITS_8:
		hwCfg.Databits = UART_DATABIT_8;
		break;
	}

	/* Parity --------------------------- */
	switch (pCfg->Parity)
	{
	case UartDev_PARITY_NONE:
		hwCfg.Parity = UART_PARITY_NONE;
		break;

	case UartDev_PARITY_ODD:
		hwCfg.Parity = UART_PARITY_ODD;
		break;

	case UartDev_PARITY_EVEN:
		hwCfg.Parity = UART_PARITY_EVEN;
		break;
	}

	/* Stopbit --------------------------- */
	switch (pCfg->StopBits)
	{
	case UartDev_STOPBITS_1:
		hwCfg.Stopbits = UART_STOPBIT_1;
		break;

	case UartDev_STOPBITS_2:
		hwCfg.Stopbits = UART_STOPBIT_2;
		break;

	}

	/* Configure these parameter above to UART peripheral ------------------- */
	UART_Init(UARTInfo.pUart, &hwCfg);

}



/*----------------------------------------------------------------------------
  UART Device "LPC17xx"
  Configure UART Pins
 *----------------------------------------------------------------------------*/
static void ConfigurePins (void) {
	PINSEL_CFG_Type PinCfg;

#if   (_UART_ == 0)
#if (_UART_RECONFIG_PIN == 0) 			/* Default configured pin */
	/*
	 * Initialize UART0 pin connect
	 */
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 2;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 3;
	PINSEL_ConfigPin(&PinCfg);
#endif

#elif  (_UART_ == 1)
#if (_UART_RECONFIG_PIN == 0) 			/* Default configured pin */
	/*
	 * Initialize UART0 pin connect
	 */
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 0;
	for (idx = 15; idx <= 22; idx++){
		PinCfg.Pinnum = idx;
		PINSEL_ConfigPin(&PinCfg);
	}
#elif (_UART_RECONFIG_PIN == 1)
	/*
	 * Initialize UART0 pin connect
	 */
	PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 2;
	for (idx = 0; idx <= 7; idx++){
		PinCfg.Pinnum = idx;
		PINSEL_ConfigPin(&PinCfg);
	}
#endif


#elif (_UART_ == 2)
#if (_UART_RECONFIG_PIN == 0) 			/* Default configured pin */
	/*
	 * Initialize UART0 pin connect
	 */
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 10;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 11;
	PINSEL_ConfigPin(&PinCfg);
#elif (_UART_RECONFIG_PIN == 1)
	/*
	 * Initialize UART0 pin connect
	 */
	PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 8;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 9;
	PINSEL_ConfigPin(&PinCfg);
#endif


#elif (_UART_ == 3)
#if (_UART_RECONFIG_PIN == 0) 			/* Default configured pin */
	/*
	 * Initialize UART0 pin connect
	 */
	PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 0;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 1;
	PINSEL_ConfigPin(&PinCfg);
#elif (_UART_RECONFIG_PIN == 1)
	/*
	 * Initialize UART0 pin connect
	 */
	PinCfg.Funcnum = 3;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 25;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 26;
	PINSEL_ConfigPin(&PinCfg);
#elif (_UART_RECONFIG_PIN == 2)
	/*
	 * Initialize UART0 pin connect
	 */
	PinCfg.Funcnum = 3;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 28;
	PinCfg.Portnum = 4;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 29;
	PINSEL_ConfigPin(&PinCfg);
#endif


#else
  #error UART not defined!
#endif

}

#endif
