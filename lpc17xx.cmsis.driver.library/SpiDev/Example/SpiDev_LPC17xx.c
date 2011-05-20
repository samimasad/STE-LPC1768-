/*
 * @file:    SpiDev_LPC17xx.c
 * @purpose: SPI Device Functions for LPC17xx
 * @version: V1.10 First Release
 * @date:    3. April. 2009
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
#include "SpiDev.h"
#include "SpiDev_LPC17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_clkpwr.h"



/*----------------------------------------------------------------------------
  SPI Device "LPC17xx"
  defines to configure the SPI device
 *----------------------------------------------------------------------------*/
#define  _SPI_		0	/* used SPI (0) */


/* If SPI0 is used ---------------------- */
#if (_SPI_==0)
/** SPI0 pin re-configuration, should be:
 * - 0 (default): 	SCK is on P0.15
 * 					SSEL is on P0.16 (slave mode)
 * 					MISO is on P0.17
 * 					MOSI is on P0.18
 */
#define _SPI_RECONFIG_PIN	0

#elif
#error "SPI not defined!"

#endif


/*----------------------------------------------------------------------------
  SPI Device "LPC17xx"
  function prototypes
 *----------------------------------------------------------------------------*/
static int      SpiDev_Init    (void);
static int      SpiDev_UnInit  (void);
static int      SpiDev_BufTxRx (void *pDataTx, void *pDataRx, unsigned int Size);
static int      SpiDev_SetBaudrate(unsigned int Baudrate);

static void     SpiConfigure    (SpiDev_CFG* pCfg);
static void     SpiConfigurePins(SpiDev_CFG *pCfg);
static uint8_t  SpiWriteRead    (uint8_t Data);



/*----------------------------------------------------------------------------
  SPI Device "LPC17xx" IO Block
 *----------------------------------------------------------------------------*/
#define SPI_DEV     spi


SpiDev_IOB SPI_DEV = {
                       { //500000,
					     100000,
                         SpiDev_CLOCK_POLARITY_IDLELOW,
                         SpiDev_CLOCK_PHASE_FIRSTEDGE,
                         SpiDev_SSO_DISABLED
                       },

                       SpiDev_Init,
                       SpiDev_UnInit,
                       SpiDev_BufTxRx,
                       SpiDev_SetBaudrate
                     };


#define _STATE_UNKNOWN_          0
#define _STATE_INITIALIZED_      1

/*----------------------------------------------------------------------------
  SPI Serial Device "LPC17xx"
  local typedefs
 *----------------------------------------------------------------------------*/
typedef struct {
            int    SpiNr;                             /* Spi number 1..n */
            int    State;                             /* Status */
    LPC_SPI_TypeDef   *pSpi;                              /* pointer to USART struct */
   unsigned int    SpiPeriph;                         /* SPI Peripheral Clock */
      IRQn_Type    SpiIRQn;                           /* SPI Interrupt number */
} SPI_INFO;


/*----------------------------------------------------------------------------
  Serial Device "LPC17xx"
  local variables
 *----------------------------------------------------------------------------*/
static SPI_INFO SpiInfo = {
#if   (_SPI_ == 0)
  1,                                                  /* SPI#1 */
  _STATE_UNKNOWN_,                                    /* unintialized */
  LPC_SPI,
  0,
  SPI_IRQn
#else
  #error "SPI not defined!"
#endif

};


/*----------------------------------------------------------------------------
  SPI Serial Device "LPC17xx"
  initialize
 *----------------------------------------------------------------------------*/
static int SpiDev_Init (void) {

  if (SpiInfo.State != _STATE_UNKNOWN_) {
    return (-1);                                      /* Device initialized */
  }


  SpiConfigure (&SPI_DEV.Cfg);

  SpiConfigurePins (&SPI_DEV.Cfg);                    /* configure the SPI pins */

  SpiInfo.State = _STATE_INITIALIZED_;

  return(0);                                          /* success */
}


/*----------------------------------------------------------------------------
  SPI Serial Device "LPC17xx"
  uninitialize
 *----------------------------------------------------------------------------*/
static int SpiDev_UnInit (void) {

#if (_SPI_ == 0)
	SPI_DeInit(LPC_SPI);

#else
#error "SPI not defined!"

#endif

  SpiInfo.State = _STATE_UNKNOWN_;

  return(0);                                          /* Success */
}


/*----------------------------------------------------------------------------
  SPI Serial Device "LPC17xx"
  Send / receive buffer
 *----------------------------------------------------------------------------*/
static int SpiDev_BufTxRx (void *pDataTx, void *pDataRx, unsigned int Size) {
  unsigned char *pCharTx, *pCharRx;
            int   bytesToWrite;

  if (SpiInfo.State != _STATE_INITIALIZED_) {
    return (-1);                                      /* Device not initialized */
  }

  if ((pDataTx == 0) || (pDataRx == 0)) {
    return (-1);                                      /* Parameter not valid */
  }

  pCharTx = (uint8_t *)pDataTx;
  pCharRx = (uint8_t *)pDataRx;
  bytesToWrite = Size;

  while (bytesToWrite-- > 0) {
    *pCharRx++ = SpiWriteRead (*pCharTx++);
  }

  return(0);                                          /* success */
}


/*----------------------------------------------------------------------------
  SPI Serial Device "LPC17xx"
  Set Baudrate
 *----------------------------------------------------------------------------*/
static int SpiDev_SetBaudrate (unsigned int Baudrate) {

  if (SpiInfo.State != _STATE_INITIALIZED_) {
    return (-1);                                      /* Device not initialized */
  }

  SPI_SetClock(SpiInfo.pSpi, Baudrate);

  return(0);                                          /* Success */
}


/*----------------------------------------------------------------------------
  SPI Serial Device "LPC17xx"
  Configure SPI
 *----------------------------------------------------------------------------*/
static void SpiConfigure (SpiDev_CFG *pCfg) {

	SPI_CFG_Type hwSPICfg;

	if (pCfg->Phase == SpiDev_CLOCK_PHASE_FIRSTEDGE)
	{
		hwSPICfg.CPHA = SPI_CPHA_FIRST;
	}
	else if (pCfg->Phase == SpiDev_CLOCK_PHASE_SECONDEDGE)
	{
		hwSPICfg.CPHA = SPI_CPHA_SECOND;
	}

	if (pCfg->Polarity == SpiDev_CLOCK_POLARITY_IDLELOW)
	{
		hwSPICfg.CPOL = SPI_CPOL_HI;
	}
	else if (pCfg->Polarity == SpiDev_CLOCK_POLARITY_IDLEHIGH)
	{
		hwSPICfg.CPOL = SPI_CPOL_LO;
	}

	hwSPICfg.ClockRate = pCfg->Baudrate;

	hwSPICfg.DataOrder = SPI_DATA_MSB_FIRST;

	hwSPICfg.Databit = SPI_DATABIT_8;

	hwSPICfg.Mode = SPI_MASTER_MODE;


#if (_SPI_ == 0)
	SPI_Init(LPC_SPI, &hwSPICfg);

#else
	#error "SPI not defined!"

#endif
}


/*----------------------------------------------------------------------------
  SPI Serial Device "LPC17xx"
  Write and Read a byte on SPI interface
 *----------------------------------------------------------------------------*/
static uint8_t SpiWriteRead (uint8_t Data) {

	//dummy to clear status
	SPI_GetStatus(SpiInfo.pSpi);

	//read for empty buffer
	SPI_ReceiveData(SpiInfo.pSpi);

	// Send data...
	SPI_SendData(SpiInfo.pSpi, Data);

	// Wait for transfer complete
	while (SPI_CheckStatus(SPI_GetStatus(SpiInfo.pSpi), SPI_STAT_SPIF) == RESET);

	return ((uint8_t)SPI_ReceiveData(SpiInfo.pSpi));
}


/*----------------------------------------------------------------------------
  SPI Serial Device "LPC17xx"
  Configure SPI Pins
 *----------------------------------------------------------------------------*/
static void SpiConfigurePins (SpiDev_CFG *pCfg) {

	PINSEL_CFG_Type PinCfg;

#if   (_SPI_ == 0)
#if (_SPI_RECONFIG_PIN == 0)

	/*
		 * Initialize SPI pin connect
		 * P0.15 - SCK;
		 * P0.17 - MISO
		 * P0.18 - MOSI
		 */
		PinCfg.Funcnum = 3;
		PinCfg.OpenDrain = 0;
		PinCfg.Pinmode = 0;
		PinCfg.Portnum = 0;
		PinCfg.Pinnum = 15;
		PINSEL_ConfigPin(&PinCfg);
		PinCfg.Pinnum = 17;
		PINSEL_ConfigPin(&PinCfg);
		PinCfg.Pinnum = 18;
		PINSEL_ConfigPin(&PinCfg);

	// Disable SSO
	if (pCfg->SlaveSelect == SpiDev_SSO_DISABLED)
	{
		PinCfg.Pinnum = 16;
		PINSEL_ConfigPin(&PinCfg);
	}
	// Enable SSO
	else if (pCfg->SlaveSelect == SpiDev_SSO_ENABLED)
	{
		PinCfg.Pinnum = 16;
		PinCfg.Pinmode = 0;
		PINSEL_ConfigPin(&PinCfg);
	}

#else
	#error "SPI reconfiguration not defined!"

#endif

#else
	#error SPI not defined!
#endif
}


/*----------------------------------------------------------------------------
  SPI Serial Device "LPC17xx"
  SPI Interrupt function.
  (not static because of startup_LPC17xx.s)
 *----------------------------------------------------------------------------*/
#if   (_SPI_ == 0)
void SPI_IRQHandler (void)

#else
  #error SPI not defined!

#endif
{
	// Read SPI interrupt flag
	SPI_GetIntStatus(SpiInfo.pSpi);

}
