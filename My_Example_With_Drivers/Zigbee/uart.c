
/**************************************************************************************************************
*                                           INCLUDE HEADER FILES
**************************************************************************************************************
*/

#include "uart.h"
#include "usbhost_lpc17xx.h" //just for accessing the debug UART
#ifdef __USE_CMSIS
#include "lpc17xx_uart.h"
#include "lpc17xx_nvic.h"
#include "lpc17xx_pinsel.h"

/************************** PRIVATE MACROS *************************/
// buffer size definition
#define UART_RING_BUFSIZE 256
/* Auto RTS and Auto CTS definition:
 * - 1: Enable Auto RTS and CTS function
 * - 0: Disable this function, in this case, handle manually
 * modem functionality */
#define AUTO_RTS_CTS_USE	0

/* Buf mask */
#define __BUF_MASK (UART_RING_BUFSIZE-1)
/* Check buf is full or not */
#define __BUF_IS_FULL(head, tail) ((tail&__BUF_MASK)==((head+1)&__BUF_MASK))
/* Check buf will be full in next receiving or not */
#define __BUF_WILL_FULL(head, tail) ((tail&__BUF_MASK)==((head+2)&__BUF_MASK))
/* Check buf is empty */
#define __BUF_IS_EMPTY(head, tail) ((head&__BUF_MASK)==(tail&__BUF_MASK))
/* Reset buf */
#define __BUF_RESET(bufidx)	(bufidx=0)
#define __BUF_INCR(bufidx)	(bufidx=(bufidx+1)&__BUF_MASK)


/** @brief UART Ring buffer structure */
typedef struct
{
    __IO uint32_t tx_head;                /*!< UART Tx ring buffer head index */
    __IO uint32_t tx_tail;                /*!< UART Tx ring buffer tail index */
    __IO uint32_t rx_head;                /*!< UART Rx ring buffer head index */
    __IO uint32_t rx_tail;                /*!< UART Rx ring buffer tail index */
    __IO uint8_t  tx[UART_RING_BUFSIZE];  /*!< UART Tx data ring buffer */
    __IO uint8_t  rx[UART_RING_BUFSIZE];  /*!< UART Rx data ring buffer */
} UART_RING_BUFFER_T;


/************************** PRIVATE VARIABLES *************************/
// UART Ring buffer
UART_RING_BUFFER_T rb_zigbee;

// RTS State
__IO int32_t RTS_State_zigbee;

// Current Tx Interrupt enable state
__IO FlagStatus TxIntStat_zigbee;

/********************************************************************//**
 * @brief 		UART transmit function (ring buffer used)
 * @param[in]	None
 * @return 		None
 *********************************************************************/
void UART_IntTransmit(void)
{

    // Disable THRE interrupt
    UART_IntConfig((LPC_UART_TypeDef *)LPC_UART3, UART_INTCFG_THRE, DISABLE);

	/* Wait for FIFO buffer empty, transfer UART_TX_FIFO_SIZE bytes
	 * of data or break whenever ring buffers are empty */
	/* Wait until THR empty */
    while (UART_CheckBusy((LPC_UART_TypeDef *)LPC_UART3) == SET);

	while (!__BUF_IS_EMPTY(rb_zigbee.tx_head,rb_zigbee.tx_tail))
    {
        /* Move a piece of data into the transmit FIFO */
    	if (UART_Send((LPC_UART_TypeDef *)LPC_UART3, (uint8_t *)&rb_zigbee.tx[rb_zigbee.tx_tail], \
			1, NONE_BLOCKING)){
        /* Update transmit ring FIFO tail pointer */
        __BUF_INCR(rb_zigbee.tx_tail);
    	} else {
    		break;
    	}
    }

    /* If there is no more data to send, disable the transmit
       interrupt - else enable it or keep it enabled */
	if (__BUF_IS_EMPTY(rb_zigbee.tx_head, rb_zigbee.tx_tail)) {
    	UART_IntConfig((LPC_UART_TypeDef *)LPC_UART3, UART_INTCFG_THRE, DISABLE);
    	// Reset Tx Interrupt state
    	TxIntStat_zigbee = RESET;
    }
    else{
      	// Set Tx Interrupt state
		TxIntStat_zigbee = SET;
    	UART_IntConfig((LPC_UART_TypeDef *)LPC_UART3, UART_INTCFG_THRE, ENABLE);
    }
}


/*********************************************************************//**
 * @brief		UART Line Status Error callback
 * @param[in]	bLSErrType	UART Line Status Error Type
 * @return		None
 **********************************************************************/
void UART_IntErr(uint8_t bLSErrType)
{
    GPIO_SetValue(1, 0xB40000);
    vTaskDelay(200);
    //Delay(500);                         /* Delay 500ms                        */
    GPIO_ClearValue(1, 0xB40000);
    //Delay(500);                         /* Delay 500ms                        */
    vTaskDelay(200);

	uint8_t test;
	// Loop forever
	while (1){
		// For testing purpose
		test = bLSErrType;
	}
}

/*********************************************************************//**
 * @brief		UART transmit function for interrupt mode (using ring buffers)
 * @param[in]	UARTPort	Selected UART peripheral used to send data,
 * 				should be UART1.
 * @param[out]	txbuf Pointer to Transmit buffer
 * @param[in]	buflen Length of Transmit buffer
 * @return 		Number of bytes actually sent to the ring buffer
 **********************************************************************/
uint32_t UARTSend_Zigbee(LPC_UART_TypeDef *UARTPort, uint8_t *txbuf, uint8_t buflen)
{
    uint8_t *data = (uint8_t *) txbuf;
    uint32_t bytes = 0;


	/* Temporarily lock out UART transmit interrupts during this
	   read so the UART transmit interrupt won't cause problems
	   with the index values */
	UART_IntConfig(UARTPort, UART_INTCFG_THRE, DISABLE);

	/* Loop until transmit run buffer is full or until n_bytes
	   expires */
	while ((buflen > 0) && (!__BUF_IS_FULL(rb_zigbee.tx_head, rb_zigbee.tx_tail)))
	{

		/* Write data from buffer into ring buffer */
		rb_zigbee.tx[rb_zigbee.tx_head] = *data;
		data++;

		/* Increment head pointer */
		__BUF_INCR(rb_zigbee.tx_head);

		/* Increment data count and decrement buffer size count */
		bytes++;
		buflen--;
	}

	/*
	 * Check if current Tx interrupt enable is reset,
	 * that means the Tx interrupt must be re-enabled
	 * due to call UART_IntTransmit() function to trigger
	 * this interrupt type
	 */
	if (TxIntStat_zigbee == RESET) {
		UART_IntTransmit();
	}
	/*
	 * Otherwise, re-enables Tx Interrupt
	 */
	else {
		UART_IntConfig(UARTPort, UART_INTCFG_THRE, ENABLE);
	}

    return bytes;
}


/*********************************************************************//**
 * @brief		UART read function for interrupt mode (using ring buffers)
 * @param[in]	UARTPort	Selected UART peripheral used to send data,
 * 				should be UART1.
 * @param[out]	rxbuf Pointer to Received buffer
 * @param[in]	buflen Length of Received buffer
 * @return 		Number of bytes actually read from the ring buffer
 **********************************************************************/
uint32_t UARTReceive_Zigbee(LPC_UART_TypeDef *UARTPort, uint8_t *rxbuf, uint8_t buflen)
{
    uint8_t *data = (uint8_t *) rxbuf;
    uint32_t bytes = 0;

	/* Temporarily lock out UART receive interrupts during this
	   read so the UART receive interrupt won't cause problems
	   with the index values */
	UART_IntConfig(UARTPort, UART_INTCFG_RBR, DISABLE);

	/* Loop until receive buffer ring is empty or
		until max_bytes expires */
	while ((buflen > 0) && (!(__BUF_IS_EMPTY(rb_zigbee.rx_head, rb_zigbee.rx_tail))))
	{
		/* Read data from ring buffer into user buffer */
		*data = rb_zigbee.rx[rb_zigbee.rx_tail];
		data++;

		/* Update tail pointer */
		__BUF_INCR(rb_zigbee.rx_tail);
		/* Increment data count and decrement buffer size count */
		bytes++;
		buflen--;



	}

	/* Re-enable UART interrupts */
	UART_IntConfig(UARTPort, UART_INTCFG_RBR, ENABLE);
    return bytes;
}



/********************************************************************//**
 * @brief 		UART receive function (ring buffer used)
 * @param[in]	None
 * @return 		None
 *********************************************************************/
void UART_IntReceive(void)
{
	uint8_t tmpc;
	uint32_t rLen;


	while (1){
		// Call UART read function in UART driver
		rLen = UART_Receive((LPC_UART_TypeDef *)LPC_UART3, &tmpc, 1, NONE_BLOCKING);
		// If data received
		if (rLen){

			/* If buffer will be full and RTS is driven manually,
			 * RTS pin should be forced into INACTIVE state
			 */


			/* Check if buffer is more space
			 * If no more space, remaining character will be trimmed out
			 */
			if (!__BUF_IS_FULL(rb_zigbee.rx_head,rb_zigbee.rx_tail)){
				rb_zigbee.rx[rb_zigbee.rx_head] = tmpc;
				__BUF_INCR(rb_zigbee.rx_head);
			}
		}
		// no more data
		else {
			break;
		}
	}
}

#endif
/*
**************************************************************************************************************
*                                         INITIALIZE UART
*
* Description: This function initializes UART port, setup pin select, clock, parity, stopbits, FIFO etc
*
* Arguments  : baud_rate    UART baud rate (115200)
*
* Returns    : None
*
**************************************************************************************************************
*/

void  UART_Init_Zigbee(uint32_t baudrate)
{
#ifdef __USE_CMSIS

	PRINT_Log("UART_Init_Zigbee Starting");
	//do it using the CMS drivers
	// UART Configuration structure variable
	UART_CFG_Type UARTConfigStruct;
	// UART FIFO configuration Struct variable
	UART_FIFO_CFG_Type UARTFIFOConfigStruct;
	// Pin configuration for UART0
	PINSEL_CFG_Type PinCfg;
	uint32_t idx, len;
	__IO FlagStatus exitflag;
	uint8_t buffer[10];

	/*
	 * Initialize UART3 pin connect
	 */
	PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 0;

	for (idx = 0; idx <= 3; idx++){
		PinCfg.Pinnum = idx;
		PINSEL_ConfigPin(&PinCfg);
	}






	/* Initialize UART Configuration parameter structure to default state:
	 * Baudrate = 9600bps
	 * 8 data bit
	 * 1 Stop bit
	 * None parity
	 */
	UART_ConfigStructInit(&UARTConfigStruct);
	UARTConfigStruct.Baud_rate =  baudrate ;
	//UARTConfigStruct.Baud_rate = 115200 ;
	// Initialize UART1 peripheral with given to corresponding parameter
	UART_Init((LPC_UART_TypeDef *)LPC_UART3, &UARTConfigStruct);

	/* Initialize FIFOConfigStruct to default state:
	 * 				- FIFO_DMAMode = DISABLE
	 * 				- FIFO_Level = UART_FIFO_TRGLEV0
	 * 				- FIFO_ResetRxBuf = ENABLE
	 * 				- FIFO_ResetTxBuf = ENABLE
	 * 				- FIFO_State = ENABLE
	 */
	UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);
	UARTFIFOConfigStruct.FIFO_Level = UART_FIFO_TRGLEV0;
	// Initialize FIFO for UART1 peripheral
	UART_FIFOConfig((LPC_UART_TypeDef *)LPC_UART3, &UARTFIFOConfigStruct);
	// Enable UART Transmit
	UART_TxCmd((LPC_UART_TypeDef *)LPC_UART3, ENABLE);

	// Reset ring buf head and tail idx
	__BUF_RESET(rb_zigbee.rx_head);
	__BUF_RESET(rb_zigbee.rx_tail);
	__BUF_RESET(rb_zigbee.tx_head);
	__BUF_RESET(rb_zigbee.tx_tail);

	UART_SetupCbs((LPC_UART_TypeDef *)LPC_UART3, 0, (void *)UART_IntReceive);
	// Transmit callback
	UART_SetupCbs((LPC_UART_TypeDef *)LPC_UART3, 1, (void *)UART_IntTransmit);
	UART_SetupCbs((LPC_UART_TypeDef *)LPC_UART3, 3, (void *)UART_IntErr);

	UART_IntConfig((LPC_UART_TypeDef *)LPC_UART3, UART_INTCFG_RBR, ENABLE);
	/*
	 * Do not enable transmit interrupt here, since it is handled by
	 * UART_Send() function, just to reset Tx Interrupt state for the
	 * first time
	 */
	TxIntStat_zigbee = RESET;

    /* preemption = 1, sub-priority = 1 */
    NVIC_SetPriority(UART3_IRQn, ((0x01<<3)|0x01));

	/* Enable Interrupt for UART1 channel */
    NVIC_EnableIRQ(UART3_IRQn);



#else
    USB_INT32U  Fdiv;
	USB_INT32U  pclkdiv, pclk;

    LPC_PINCON->PINSEL0 |= 0x00000050;       /* RxD0 and TxD0 */

    LPC_UART0->LCR = 0x83;		/* 8 bits, no Parity, 1 Stop bit */
	/* Bit 6~7 is for UART0 clock divider. */
	pclkdiv = (LPC_SC->PCLKSEL0 >> 6) & 0x03;
	switch ( pclkdiv )
	{
	  case 0x00:
	  default:
		pclk = SystemFrequency/4;
		break;
	  case 0x01:
		pclk = SystemFrequency;
		break; 
	  case 0x02:
		pclk = SystemFrequency/2;
		break; 
	  case 0x03:
		pclk = SystemFrequency/8;
		break;
	}
	Fdiv = ( pclk / 16 ) / baudrate ;	/*baud rate */
	
    LPC_UART0->DLM = Fdiv / 256;
    LPC_UART0->DLL = Fdiv % 256;
    LPC_UART0->LCR = 0x03;		/* DLAB = 0 */
    LPC_UART0->FCR = 0x07;		/* Enable and reset TX and RX FIFO. */
#endif
}

/*
**************************************************************************************************************
*                                         PRINT CHARECTER
*
* Description: This function is used to print a single charecter through UART1.
*
* Arguments  : ch    charecter to be printed
*
* Returns    : None
*
**************************************************************************************************************
*/

void  UART_PrintChar_Zigbee (uint8_t ch)
{
#ifdef __USE_CMSIS
	UARTSend_Zigbee(LPC_UART3, &ch, 1);

#else
   while (!(UART0->LSR & 0x20));
   UART0->THR  = ch;
#endif

}

/*
**************************************************************************************************************
*                                         PRINT STRING
*
* Description: This function is used to print a string
*
* Arguments  : str    Pointer to the string
*
* Returns    : None
*
**************************************************************************************************************
*/

void  UART_PrintStr_Zigbee (const uint8_t * str)
{

   while ((*str) != 0) {
      if (*str == '\n') {
         UART_PrintChar_Zigbee(*str++);
         UART_PrintChar_Zigbee('\r');
      } else {
         UART_PrintChar_Zigbee(*str++);
      }    
   }
}

/*
**************************************************************************************************************
*                                        PRINT FORMATTED STRING
*
* Description: This function is used to print formatted string. This function takes variable length arguments
*
* Arguments  : variable length arguments
*
* Returns    : None
*
**************************************************************************************************************
*/

void  UART_Printf_Zigbee (const  uint8_t *format, ...)
{
    static  uint8_t  buffer[40 + 1];
            va_list     vArgs;


    va_start(vArgs, format);
    vsprintf((char *)buffer, (char const *)format, vArgs);
    va_end(vArgs);
    UART_PrintStr_Zigbee((uint8_t*) buffer);
}
