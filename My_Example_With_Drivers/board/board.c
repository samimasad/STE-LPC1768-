/*****************************************************************************
 *
 *   Copyright(C) 2011, Embedded Artists AB
 *   All rights reserved.
 *
 ******************************************************************************
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * Embedded Artists AB assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. Embedded Artists AB
 * reserves the right to make changes in the software without
 * notification. Embedded Artists AB also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 *****************************************************************************/


/******************************************************************************
 * Includes
 *****************************************************************************/

#include "lpc17xx.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_ssp.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_uart.h"
#include "board.h"

/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/


#define CONSOLE_DEV      ((LPC_UART_TypeDef *)LPC_UART1)

/*
 * UART receive buffer helper macros
 */

#define RX_BUF_SIZE (512)

/******************************************************************************
 * External global variables
 *****************************************************************************/

/******************************************************************************
 * Local variables
 *****************************************************************************/



/*
 * UART receive buffer
 */

static uint8_t rxqIn = 0;
static uint8_t rxqOut = 0;
static uint8_t rxq[RX_BUF_SIZE];

/******************************************************************************
 * Local Functions
 *****************************************************************************/

static void rxq_put(uint8_t data)
{
  // full
  if (rxqOut == (rxqIn + 1) % RX_BUF_SIZE) {
    return;
  }
  rxq[rxqIn] = data;
  rxqIn = (rxqIn + 1) % RX_BUF_SIZE;
}

static uint8_t rxq_get(void)
{
  uint8_t d = 0;
  // empty
  if (rxqIn == rxqOut) {
    return 0;
  }

  d = rxq[rxqOut];
  rxqOut = (rxqOut + 1) % RX_BUF_SIZE;

  return d;
}

static uint8_t rxq_isEmpty(void)
{
  return (rxqIn == rxqOut);
}

static void console_uartRecvCb(void)
{
  uint8_t data = 0;
  uint32_t len = 0;

  while(1) {
      len = UART_Receive(CONSOLE_DEV, &data, 1, NONE_BLOCKING);

      if (len) {
        rxq_put(data);
      }

      // there is no more data
      else {

        break;
      }
  }
}


/******************************************************************************
 * Public Functions
 *****************************************************************************/

/******************************************************************************
 *
 * Description:
 *   Initialize the console (UART connected to USB-to-UART bridge)
 *
 *****************************************************************************/



/******************************************************************************
 *
 * Description:
 *   Initialize the UART connected to the XBee/Jennic module
 *
 *****************************************************************************/
void console_uart_init(void)
{
	PINSEL_CFG_Type PinCfg;
	UART_CFG_Type uartCfg;

	/* Initialize UART1 pin connect */
	PinCfg.Funcnum = 1;
	PinCfg.Portnum = 0;
	PinCfg.Pinnum = 15;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 16;
	PINSEL_ConfigPin(&PinCfg);

	uartCfg.Baud_rate = 115200;
	uartCfg.Databits = UART_DATABIT_8;
	uartCfg.Parity = UART_PARITY_NONE;
	uartCfg.Stopbits = UART_STOPBIT_1;

	UART_Init(CONSOLE_DEV, &uartCfg);

	UART_SetupCbs(CONSOLE_DEV, 0, &console_uartRecvCb);

	UART_TxCmd(CONSOLE_DEV, ENABLE);

	UART_IntConfig(CONSOLE_DEV, UART_INTCFG_RBR, ENABLE);
	NVIC_EnableIRQ(UART1_IRQn);
}

/******************************************************************************
 *
 * Description:
 *   Send data to the XBee/Jennic module.
 *
 * Params:
 *   [in] txbuf - buffer containing data to send
 *   [in] buflen - number of bytes to send
 *   [in] flag - indicates blocking or non-blocking transfer
 *
 * Returns:
 *   Number of bytes sent.
 *
 *****************************************************************************/
uint32_t console_uart_send(uint8_t *txbuf, uint32_t buflen,
		TRANSFER_BLOCK_Type flag)
{
	return UART_Send(CONSOLE_DEV, txbuf, buflen, flag);
}

/******************************************************************************
 *
 * Description:
 *   Send a null-terminates string to the XBee/Jennic.
 *
 * Params:
 *   [in] str - the string to send
 *
 * Returns:
 *   Number of bytes sent.
 *
 *****************************************************************************/
uint32_t console_uart_sendString(uint8_t *str)
{
	return UART_Send(CONSOLE_DEV, str, strlen(str), 1);
}

/******************************************************************************
 *
 * Description:
 *   Receive a block of data from the Xbee/Jennic module.
 *
 * Params:
 *   [in] rxbuf - pointer to receive buffer
 *   [in] buflen - length of buffer
 *
 * Returns:
 *   Number of bytes sent.
 *
 *****************************************************************************/
uint32_t console_uart_receive(uint8_t *buf, uint32_t buflen)
{

  uint32_t pos = 0;

  while(buflen > 0 && !rxq_isEmpty()) {
    buf[pos] = rxq_get();
    pos++;
    buflen--;
  }

  return pos;
}

/******************************************************************************
 *
 * Description:
 *   Check if the receive queue is empty
 *
 *****************************************************************************/
uint8_t console_uart_recvIsEmpty(void)
{
  return rxq_isEmpty();
}

/******************************************************************************
 *
 * Description:
 *   UART1 interrupt handler
 *
 *****************************************************************************/
void UART1_IRQHandler(void)
{
  UART1_StdIntHandler();
}





