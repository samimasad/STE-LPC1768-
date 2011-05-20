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

#if UART_INTERRUPT_MODE


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

#if   (_UART_ == 0)
void UART0_IRQHandler (void);
#elif   (_UART_ == 1)
void UART1_IRQHandler (void);
#elif (_UART_ == 2)
void UART2_IRQHandler (void);
#elif (_UART_ == 3)
void UART3_IRQHandler (void);
#else
  #error UART not defined!
#endif

void UartRxCallBack(void);
void UartTxCallBack(void);


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
	int    			UARTNr;			/* UART number 1..n */
    int    			State;          /* Status */
    LPC_UART_TypeDef  	*pUart;         /* pointer to UART structure */
    uint32_t    	UARTPeriph;     /* UART Peripheral Clock */
    IRQn_Type    	UARTIRQn;       /* UART Interrupt number */
} UART_INFO;


/*----------------------------------------------------------------------------
  Serial Device "LPC17xx"
  local variables
 *----------------------------------------------------------------------------*/
static UART_INFO UARTInfo = {
#if   (_UART_ == 0)
  1,                                                  /* UART#0 */
  _STATE_UNKNOWN_,                                    /* unintialized */
  LPC_UART0,
  0,
  UART0_IRQn

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

static unsigned int TxRestart;                        /* NZ if TX restart is required */


/*----------------------------------------------------------------------------
  UART Device "LPC17xx"
  Buffer handling
 *----------------------------------------------------------------------------*/
#if _BUF_SIZE_ < 2
#error _BUF_SIZE_ is too small.  It must be larger than 1.
#elif ((_BUF_SIZE_ & (_BUF_SIZE_-1)) != 0)
#error _BUF_SIZE_ must be a power of 2.
#endif

typedef struct {
   unsigned int    WrIdx;                             /* Next In Index */
   unsigned int    RdIdx;                             /* Next Out Index */
           char    Data[_BUF_SIZE_];                  /* Buffer */
} BUF_INFO;

static BUF_INFO BufRx;                                /* receive  buffer */
static BUF_INFO BufTx;                                /* transmit buffer */

unsigned int atomic_incr (unsigned int *pVar) {
#if   ((defined __CORTEX_M) && (__CORTEX_M == 0x00))
  __disable_irq();
  *pVar = *pVar + 1;
  __enable_irq();
#elif ((defined __CORTEX_M) && (__CORTEX_M == 0x03))
  unsigned int var;
           int result;

  do {
    var = __LDREXW(pVar);
    result = __STREXW((var+1), pVar);
  } while (result != 0);
#else
  *pVar = *pVar + 1;
#endif  /* (__CORTEX_M == ...) */
  return *pVar;
}

#define _BUF_MASK_              (_BUF_SIZE_ - 1ul)

#define __BUF_RESET(buf)        (buf.RdIdx = buf.WrIdx = 0)
#define __BUF_WR(buf, dataIn)   (buf.Data[_BUF_MASK_ & atomic_incr(&buf.WrIdx)] = (dataIn))
#define __BUF_RD(buf)           (buf.Data[_BUF_MASK_ & atomic_incr(&buf.RdIdx)])
#define __BUF_EMPTY(buf)        (buf.RdIdx == buf.WrIdx)
#define __BUF_FULL(buf)         (buf.RdIdx == buf.WrIdx+1)
#define __BUF_COUNT(buf)        (_BUF_MASK_ & (buf.WrIdx - buf.RdIdx))



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

  __BUF_RESET(BufTx);                                 /* reset transmit buffer */
  __BUF_RESET(BufRx);                                 /* reset receive buffer */
  TxRestart = 1;

  UARTInfo.State = _STATE_INITIALIZED_;

  UART_TxCmd (UARTInfo.pUart, ENABLE); 					/* TX, RX enable */

  for (i = 0; i < 800; i++);                            /* eleminate schmierzeichen */

  /* Setting up callback function for UART driver */
	// Receive callback
	UART_SetupCbs(UARTInfo.pUart, 0, (void *)UartRxCallBack);
	// Transmit callback
	UART_SetupCbs(UARTInfo.pUart, 1, (void *)UartTxCallBack);

  /* Enable Rx Interrrupt in UART peripheral */
  UART_IntConfig(UARTInfo.pUart, UART_INTCFG_RBR, ENABLE);

  /* preemption = 1, sub-priority = 1 */
  NVIC_SetPriority(UARTInfo.UARTIRQn, ((0x01<<3)|0x01));

  NVIC_EnableIRQ(UARTInfo.UARTIRQn);               /* enable Interrupt */

  return(0);                                       /* success */
}


/*----------------------------------------------------------------------------
  Serial Device "LPC17xx"
  uninitialize
 *----------------------------------------------------------------------------*/
static int UartDev_UnInit (void) {

   NVIC_DisableIRQ(UARTInfo.UARTIRQn);              /* disable Interrupt */

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
   int   bytesToWrite, bytesWritten;
   uint8_t tmpc;

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
      while (__BUF_FULL(BufTx));                      /* Block until space is available */
      __BUF_WR(BufTx, *pChar++);

      if (TxRestart) {                                /* If transmit interrupt is disabled, enable it */
        // TxRestart = 0;
        if (UART_GetLineStatus(UARTInfo.pUart) & UART_LINESTAT_THRE){
        	if (!__BUF_EMPTY(BufTx)) {
        		TxRestart = 0;
        		tmpc = __BUF_RD(BufTx);
        		UART_Send(UARTInfo.pUart, &tmpc, 1, BLOCKING);
        		UART_IntConfig(UARTInfo.pUart, UART_INTCFG_THRE, ENABLE);     /* enable TX interrupt */
        	}
        }
        // UART_IntConfig(UARTInfo.pUart, UART_INTCFG_THRE, ENABLE);     /* enable TX interrupt */
      }
    }
  } else {                                            /* nonBlocking transmission */
    bytesWritten = 0;
    while (bytesToWrite-- > 0) {
      if (!__BUF_FULL(BufTx)) {
        __BUF_WR(BufTx, *pChar++);                    /* just fill buffer */
        bytesWritten++;
      } else {
        bytesToWrite = 0;
      }
    }
    *pSize = bytesWritten;                            /* return bytes written */
    if (TxRestart) {                                  /* If transmit interrupt is disabled, enable it */
      // TxRestart = 0;
      if (UART_GetLineStatus(UARTInfo.pUart) & UART_LINESTAT_THRE){
         	if (!__BUF_EMPTY(BufTx)){
         		TxRestart = 0;
         		tmpc = __BUF_RD(BufTx);
         		UART_Send(UARTInfo.pUart, &tmpc, 1, BLOCKING);
         		UART_IntConfig(UARTInfo.pUart, UART_INTCFG_THRE, ENABLE);	      /* enable TX interrupt */
         	}
         }
      // UART_IntConfig(UARTInfo.pUart, UART_INTCFG_THRE, ENABLE);	      /* enable TX interrupt */
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
  int    bytesToRead, bytesRead;

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
      while (__BUF_EMPTY(BufRx));                     /* Block until data is available */
      *pChar++ = __BUF_RD(BufRx);
    }
  } else {                                            /* nonBlocking transmission */
    bytesRead = 0;
    while (bytesToRead-- > 0) {
      if (!__BUF_EMPTY(BufRx)) {
        *pChar++ = __BUF_RD(BufRx);
        bytesRead++;
      } else {
        bytesToRead = 0;
      }
    }
    *pSize = bytesRead;                               /* return the number of bytes read */
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
  __BUF_RESET(BufTx);                                 /* reset transmit buffer */
  __BUF_RESET(BufRx);                                 /* reset receive buffer */

  return(0);                                          /* Success */
}



/*----------------------------------------------------------------------------
  UART Device "LPC17xx"
  Configure UART
 *----------------------------------------------------------------------------*/
static void ConfigureUart (UartDev_CFG *pCfg) {

	UART_CFG_Type hwCfg;
	uint32_t tmp;

	/* Baudrate --------------------------- */
	hwCfg.Baud_rate = pCfg->BaudRate;

	tmp = pCfg->DataBits;

	/* Databit --------------------------- */
	switch(tmp)
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

/*-----------------------------------------------------------------------------
 * Receive Call-back dispatch for UART driver
 *-----------------------------------------------------------------------------*/
void UartRxCallBack(void)
{
	uint8_t tmpc, len;
	len = UART_Receive(UARTInfo.pUart, &tmpc, 1, NONE_BLOCKING);
	if ((!__BUF_FULL(BufRx)) && len) {
		__BUF_WR(BufRx, tmpc);
	}
}

/*-----------------------------------------------------------------------------
 * Transmit Call-back dispatch for UART driver
 *-----------------------------------------------------------------------------*/
void UartTxCallBack(void)
{
	uint8_t tmpc;

	if (!__BUF_EMPTY(BufTx)) {
		tmpc = __BUF_RD(BufTx);
		UART_Send(UARTInfo.pUart, &tmpc, 1, BLOCKING);
		TxRestart = 0;
	} else {
		/* disable TX interrupt if nothing to send */
		UART_IntConfig(UARTInfo.pUart, UART_INTCFG_THRE, DISABLE);
		TxRestart = 1;
	}
}


/*----------------------------------------------------------------------------
  UART Device "LPC17xx"
  UART Interrupt function.
 *----------------------------------------------------------------------------*/
#if   (_UART_ == 0)
void UART0_IRQHandler (void){
	// Call Standard UART0 interrupt handler
	UART0_StdIntHandler();
#elif   (_UART_ == 1)
void UART1_IRQHandler (void){
	// Call Standard UART1 interrupt handler
	UART1_StdIntHandler();
#elif (_UART_ == 2)
void UART2_IRQHandler (void){
	// Call Standard UART2 interrupt handler
	UART2_StdIntHandler();
#elif (_UART_ == 3)
void UART3_IRQHandler (void){
	// Call Standard UART3 interrupt handler
	UART3_StdIntHandler();
#else
  #error UART not defined!
#endif

}
#endif
