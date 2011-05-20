/*****************************************************************************
 *   emactest.c:  main C entry file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2005.10.01  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include "string.h"
#include "crc32.h"
#include "lpc17xx_emac.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_nvic.h"
#include "lpc17xx_libcfg.h"


/* Configurable macros ----------------------------------------------- */
/* For the EMAC test, there are two ways to test:
	- TX_ONLY and BOUNCE_RX flags can be set one at a time, not both.
	When TX_ONLY is set to 1, it's a TX_ONLY packet from the MCB1700
	board to the LAN. Use the traffic analyzer such as ethereal, once
	the program is running, the packets can be monitored on the traffic
	analyzer.
	- When BOUNCE_RX is set to 1 (TX_ONLY needs to reset to 0), it's a
	test to test both TX and RX, use the traffic generator/analyzer,
	you can creat a packet with the destination address as that on the
	MCB1700 board, use the traffic generator to send packets, as long
	as the destination address matches, MCB1700 will reverse the source
	and destination address and send the packets back on the network.
	ENABLE_WOL flag is used to test power down and WOL functionality.
	BOUNCE_RX flag needs to be set to 1 when WOL is being tested.
*/
#define TX_ONLY				0
#define BOUNCE_RX			1
#define ENABLE_WOL			1
#define ENABLE_HASH			1


#if TX_ONLY
/* This is the MAC address of LPC1768 */
#define EMAC_ADDR12		0x0000101F
#define EMAC_ADDR34		0x0000E012
#define EMAC_ADDR56		0x00001D0C
/* A pseudo destination MAC address is defined for
 * both TX_ONLY and BOUNCE_RX test */
#define EMAC_DST_ADDR12		0x0000E386
#define EMAC_DST_ADDR34		0x00006BDA
#define EMAC_DST_ADDR56		0x00005000
#endif

#if BOUNCE_RX
/* This is the MAC address of LPC1768 */
#define EMAC_ADDR12		0x0000E386
#define EMAC_ADDR34		0x00006BDA
#define EMAC_ADDR56		0x00005000
/* A pseudo destination MAC address is defined for
 * both TX_ONLY and BOUNCE_RX test */
#define EMAC_DST_ADDR12		0x0000101F
#define EMAC_DST_ADDR34		0x0000E012
#define EMAC_DST_ADDR56		0x00001D0C
#endif

/* LED definitions */
#define PD_LED_PIN 		(1<<6)
#define TX_LED_PIN		(1<<5)
#define RX_LED_PIN		(1<<4)
#define KB_LED_PIN		(1<<3)
#define BLINK_LED_PIN	(1<<2)
#define LED2_MASK		((1<<2) | (1<<3) | (1<<4) | (1<<5) | (1<<6))
#define LED1_MASK		((1<<28) | (1<<29) | (1<<31))


/* Internal macros ----------------------------------------------- */

#define TX_PACKET_SIZE		114

#define MYMAC_1 	((EMAC_ADDR12 & 0xFF00) >> 8)
#define MYMAC_2 	((EMAC_ADDR12 & 0xFF))
#define MYMAC_3 	((EMAC_ADDR34 & 0xFF00) >> 8)
#define MYMAC_4 	((EMAC_ADDR34 & 0xFF))
#define MYMAC_5 	((EMAC_ADDR56 & 0xFF00) >> 8)
#define MYMAC_6 	((EMAC_ADDR56 & 0xFF))

/* For debugging... */
#include "debug_frmwrk.h"
#include <stdio.h>
#define DB	_DBG((uint8_t *)db_)
char db_[64];

#ifdef __IAR_SYSTEMS_ICC__
/* Global Tx Buffer data */
#pragma data_alignment=4
uint8_t gTxBuf[TX_PACKET_SIZE + 0x10];
/* Global Rx Buffer data */
#pragma data_alignment=4
uint8_t gRxBuf[TX_PACKET_SIZE + 0x10];
#else
/* Global Tx Buffer data */
uint8_t __attribute__ ((aligned (4))) gTxBuf[TX_PACKET_SIZE + 0x10];
/* Global Rx Buffer data */
uint8_t __attribute__ ((aligned (4))) gRxBuf[TX_PACKET_SIZE + 0x10];
#endif

/* EMAC address */
uint8_t EMACAddr[] = {MYMAC_6, MYMAC_5, MYMAC_4, MYMAC_3, MYMAC_2, MYMAC_1};

/* Tx, Rx Counters */
__IO uint32_t RXOverrunCount = 0;
__IO uint32_t RXErrorCount = 0;
__IO uint32_t TXUnderrunCount = 0;
__IO uint32_t TXErrorCount = 0;
__IO uint32_t RxFinishedCount = 0;
__IO uint32_t TxFinishedCount = 0;
__IO uint32_t TxDoneCount = 0;
__IO uint32_t RxDoneCount = 0;
__IO uint32_t ReceiveLength = 0;
__IO Bool PacketReceived = FALSE;

/* Tx Only variables */
#if TX_ONLY
__IO FlagStatus Pressed = RESET;
#endif

#if ENABLE_WOL
__IO uint32_t WOLCount = 0;
#endif


/******************************************************************************
** Function name:		PacketGen
**
** Descriptions:		Create a perfect packet for TX
**
** parameters:			None
** Returned value:		None
**
******************************************************************************/
void PacketGen( uint8_t *txptr )
{
  int i;
  uint32_t crcValue;
  uint32_t BodyLength = TX_PACKET_SIZE - 14;

  /* Dest address */
  *(txptr+0) = EMAC_DST_ADDR56 & 0xFF;
  *(txptr+1) = (EMAC_DST_ADDR56 >> 0x08) & 0xFF;
  *(txptr+2) = EMAC_DST_ADDR34 & 0xFF;
  *(txptr+3) = (EMAC_DST_ADDR34 >> 0x08) & 0xFF;
  *(txptr+4) = EMAC_DST_ADDR12 & 0xFF;
  *(txptr+5) = (EMAC_DST_ADDR12 >> 0x08) & 0xFF;

  /* Src address */
  *(txptr+6) = EMAC_ADDR56 & 0xFF;
  *(txptr+7) = (EMAC_ADDR56 >> 0x08) & 0xFF;
  *(txptr+8) = EMAC_ADDR34 & 0xFF;
  *(txptr+9) = (EMAC_ADDR34 >> 0x08) & 0xFF;
  *(txptr+10) = EMAC_ADDR12 & 0xFF;
  *(txptr+11) = (EMAC_ADDR12 >> 0x08) & 0xFF;

  /* Type or length, body length is TX_PACKET_SIZE - 14 bytes */
  *(txptr+12) = BodyLength & 0xFF;
  *(txptr+13) = (BodyLength >> 0x08) & 0xFF;

  /* Skip the first 14 bytes for dst, src, and type/length */
  for ( i=0; i < BodyLength; i++ )
  {
	*(txptr+i+14) = 0x55;
  }

  // Calculate CRC
  crcValue = crc32_bfr( txptr, TX_PACKET_SIZE );

  // Add 4-byte CRC
  *(txptr+TX_PACKET_SIZE) = (0xff & crcValue);
  *(txptr+TX_PACKET_SIZE+1) = 0xff & (crcValue >> 8 );
  *(txptr+TX_PACKET_SIZE+2) = 0xff & (crcValue >> 16);
  *(txptr+TX_PACKET_SIZE+3) = 0xff & (crcValue >> 24);
}

/**
 * @brief Init LEDs
 */
void LED_Init (void)
{
	PINSEL_CFG_Type PinCfg;

	uint8_t temp;

	PinCfg.Funcnum = 0;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 2;
	for (temp = 2; temp <= 6; temp++){
		PinCfg.Pinnum = temp;
		PINSEL_ConfigPin(&PinCfg);
	}

	PinCfg.Funcnum = 0;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 1;
	PinCfg.Pinnum = 28;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 29;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 31;
	PINSEL_ConfigPin(&PinCfg);


	// Set direction to output
	LPC_GPIO2->FIODIR |= LED2_MASK;
	LPC_GPIO1->FIODIR |= LED1_MASK;

	/* Turn off all LEDs */
	LPC_GPIO2->FIOCLR = LED2_MASK;
	LPC_GPIO1->FIOCLR = LED1_MASK;
}
/*****************************************************************************
** Function name:		LED_Blink
**
** Descriptions:		Based on the pattern, display accordingly.
**						This is used for WOL test only.
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void LED_Blink( uint32_t pattern )
{
	uint32_t j;

	LPC_GPIO2->FIOSET = pattern;
	for ( j = 0; j < 0x100000; j++ );
	LPC_GPIO2->FIOCLR = pattern;
	for ( j = 0; j < 0x100000; j++ );
}


#if TX_ONLY
/**
 * @brief Initialize External Interrupt 0
 */
void EINT0_Init(void)
{
	PINSEL_CFG_Type PinCfg;

	/* P2.10 as /EINT0 */
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 10;
	PinCfg.Portnum = 2;
	PINSEL_ConfigPin(&PinCfg);

	//Initialize EXT registers
	LPC_SC->EXTINT = 0x0;
	LPC_SC->EXTMODE = 0x0;
	LPC_SC->EXTPOLAR = 0x0;

	/* edge sensitive */
	LPC_SC->EXTMODE = 0xF;
	/* falling-edge sensitive */
	LPC_SC->EXTPOLAR = 0x0;
	/* External Interrupt Flag cleared*/
	LPC_SC->EXTINT = 0xF;

	NVIC_SetPriority(EINT0_IRQn, 4);
	NVIC_EnableIRQ(EINT0_IRQn);
}

/**
 * @brief External Interrupt 0 Handler
 */
void EINT0_IRQHandler(void)
{
	SC->EXTINT |= 0x1;  //clear the EINT0 flag
	LED_Blink(KB_LED_PIN);
	Pressed = SET;
}
#endif


/* Callback functions section --------------------------------------------- */

/**
 * Brief Receive Overrun Interrupt Callback function
 */
void RxOverrun_UsrCBS(void)
{
	RXOverrunCount++;
	_DBG_("Rx overrun");
}


/**
 * Brief Receive Error Interrupt Callback function
 * Note:
 * The EMAC doesn't distinguish the frame type and frame length,
 * so, e.g. when the IP(0x8000) or ARP(0x0806) packets are received,
 * it compares the frame type with the max length and gives the
 * "Range" error. In fact, this bit is not an error indication,
 * but simply a statement by the chip regarding the status of
 * the received frame
 */
void RxError_UsrCBS(void)
{
	if (EMAC_CheckReceiveDataStatus(EMAC_RINFO_RANGE_ERR) == RESET){
		RXErrorCount++;
		_DBG_("Rx error: ");
	}
}

/**
 * Brief Receive Finish Interrupt Callback function
 */
void RxFinish_UsrCBS(void)
{
	RxFinishedCount++;
	_DBG_("Rx finish");
}


/**
 * @brief Receive Data Packet Done Callback function.
 * Called by EMAC Standard Interrupt handler in EMAC driver
 *
 * Note: All packets are greater than (TX_PACKET_SIZE + 4)
 * will be ignore!
 */
void RxDone_UsrCBS(void)
{
	EMAC_PACKETBUF_Type RxDatbuf;
	uint32_t RxLen;

	/* Packet received, check if packet is valid. */
	if (EMAC_CheckReceiveIndex()){
		if (!EMAC_CheckReceiveDataStatus(EMAC_RINFO_LAST_FLAG)){
			goto rel;
		}
		// Get data size, trip out 4-bytes CRC field, note that length in (-1) style format
		RxLen = EMAC_GetReceiveDataSize() - 3;
		// Note that packet added 4-bytes CRC created by yourself
		if ((RxLen > (TX_PACKET_SIZE + 4)) || (EMAC_CheckReceiveDataStatus(EMAC_RINFO_ERR_MASK))) {
			/* Invalid frame, ignore it and free buffer */
			goto rel;
		}
		ReceiveLength = RxLen;
		// Valid Frame, just copy it
		RxDatbuf.pbDataBuf = (uint32_t *)gRxBuf;
		RxDatbuf.ulDataLen = RxLen;
		EMAC_ReadPacketBuffer(&RxDatbuf);
		PacketReceived = TRUE;

rel:
		/* Release frame from EMAC buffer */
		EMAC_UpdateRxConsumeIndex();
	}
	_DBG_("Rx done");
	RxDoneCount++;
}

/**
 * Brief Transmit Under-run Interrupt Callback function
 */
void TxUnderrun_UsrCBS(void)
{
	TXUnderrunCount++;
	_DBG_("Tx under-run");
}

/**
 * Brief Transmit Error Interrupt Callback function
 */
void TxError_UsrCBS(void)
{
	TXErrorCount++;
	_DBG_("Tx error");
}

/**
 * Brief Transmit Finish Interrupt Callback function
 */
void TxFinish_UsrCBS(void)
{
	TxFinishedCount++;
	_DBG_("Tx finish");
}

/**
 * Brief Transmit Done Interrupt Callback function
 */
void TxDone_UsrCBS(void)
{
	TxDoneCount++;
	_DBG_("Tx done");
}

/**
 * @brief Wake On Lan Interrupt Callback function
 */
#if ENABLE_WOL
void WoL_UsrCBS(void)
{
	/* Never gone here since interrupt in this
	 * functionality has been disable, even if in wake-up mode
	 */
	WOLCount++;
}
#endif

/* EMAC Interrupt Handler functions section ---------------------------------- */

/**
 * EMAC Interrupt sub-routine
 */
void ENET_IRQHandler (void)
{
	// Call standard EMAC interrupt handler
	EMAC_StandardIRQHandler();
}

/**
 * User EMAC initialization
 */
void Usr_Init_Emac(void)
{
	/* EMAC configuration type */
	EMAC_CFG_Type Emac_Config;
	/* pin configuration */
	PINSEL_CFG_Type PinCfg;
	uint32_t i;


	/*
	 * Enable P1 Ethernet Pins:
	 * P1.0 - ENET_TXD0
	 * P1.1 - ENET_TXD1
	 * P1.4 - ENET_TX_EN
	 * P1.8 - ENET_CRS
	 * P1.9 - ENET_RXD0
	 * P1.10 - ENET_RXD1
	 * P1.14 - ENET_RX_ER
	 * P1.15 - ENET_REF_CLK
	 * P1.16 - ENET_MDC
	 * P1.17 - ENET_MDIO
	 */
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 1;

	PinCfg.Pinnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 1;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 4;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 8;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 9;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 10;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 14;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 15;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 16;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 17;
	PINSEL_ConfigPin(&PinCfg);

	_DBG_("Init EMAC module");
	sprintf(db_,"MAC[1..6] addr: %X-%X-%X-%X-%X-%X \n\r", \
			 EMACAddr[0],  EMACAddr[1],  EMACAddr[2], \
			  EMACAddr[3],  EMACAddr[4],  EMACAddr[5]);
	DB;

	Emac_Config.Mode = EMAC_MODE_AUTO;
	Emac_Config.pbEMAC_Addr = EMACAddr;
	// Initialize EMAC module with given parameter
	while (EMAC_Init(&Emac_Config) == ERROR){
		// Delay for a while then continue initializing EMAC module
		_DBG_("Error during initializing EMAC, restart after a while");
		for (i = 0x100000; i; i--);
	}
	_DBG_("Setup callback functions");
	// Setup callback functions
	EMAC_SetupIntCBS(EMAC_INT_RX_OVERRUN, RxOverrun_UsrCBS);
	EMAC_SetupIntCBS(EMAC_INT_RX_ERR, RxError_UsrCBS);
	EMAC_SetupIntCBS(EMAC_INT_RX_FIN, RxFinish_UsrCBS);
	EMAC_SetupIntCBS(EMAC_INT_RX_DONE, RxDone_UsrCBS);
	EMAC_SetupIntCBS(EMAC_INT_TX_UNDERRUN, TxUnderrun_UsrCBS);
	EMAC_SetupIntCBS(EMAC_INT_TX_ERR, TxError_UsrCBS);
	EMAC_SetupIntCBS(EMAC_INT_TX_FIN, TxFinish_UsrCBS);
	EMAC_SetupIntCBS(EMAC_INT_TX_DONE, TxDone_UsrCBS);
#if ENABLE_WOL
	EMAC_SetupIntCBS(EMAC_INT_WAKEUP, WoL_UsrCBS);
#endif
	// Enable all interrupt
	EMAC_IntCmd((EMAC_INT_RX_OVERRUN | EMAC_INT_RX_ERR | EMAC_INT_RX_FIN \
			| EMAC_INT_RX_DONE | EMAC_INT_TX_UNDERRUN | EMAC_INT_TX_ERR \
			| EMAC_INT_TX_FIN | EMAC_INT_TX_DONE), ENABLE);
	NVIC_SetPriority(ENET_IRQn, 0);
	NVIC_EnableIRQ(ENET_IRQn);
	_DBG_("Initialize EMAC complete");
}

/*****************************************************************************
**   Main Function  main()
******************************************************************************/
int main (void)
{

	/* Data Packet format */
	EMAC_PACKETBUF_Type DataPacket;

	uint8_t *txptr;
	uint32_t i = 0;

#if TX_ONLY
	uint32_t j;
#endif

#if BOUNCE_RX
	uint8_t *rxptr;
#endif

#if ENABLE_HASH
	uint8_t dstAddr[] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
#endif

	/* Initialize system clock */
	SystemInit();

	//  Set Vector table offset value
#if (__RAM_MODE__==1)
	NVIC_SetVTOR(0x10000000);
#else
	NVIC_SetVTOR(0x00000000);
#endif

	NVIC_SetPriorityGrouping(4);  //sets PRIGROUP to 3:2 (XXX:YY)

	/*
	 * Init LED
	 */
	LED_Init();

	/*
	 * Initialize debug via UART
	 */
	debug_frmwrk_init();

	// Init EMAC
	Usr_Init_Emac();


#if TX_ONLY
	EINT0_Init();
	txptr = (uint8_t *)gTxBuf;
	/* pre-format the transmit packets */
	PacketGen(txptr);
#endif

#if ENABLE_HASH
  EMAC_SetHashFilter(dstAddr, ENABLE);
#endif


#if BOUNCE_RX
  /* copy just received data from RX buffer to TX buffer and send out */
  txptr = (uint8_t *)gTxBuf;
  rxptr = (uint8_t *)gRxBuf;
#endif

#if ENABLE_WOL

  _DBG_("Enter Sleep mode now...");
  /*
   * On default state, All Multicast frames, All Broadcast frames and Frame that matched
   * with station address (unicast) are accepted.
   * To make WoL is possible, enable Rx Magic Packet and RxFilter Enable WOL
   */
  EMAC_SetFilterMode((EMAC_RFC_PFILT_WOL_EN | EMAC_RFC_MAGP_WOL_EN), ENABLE);

	for (i = 0; i < 5; i++){
		LED_Blink(PD_LED_PIN);	/* Indicating system is in power down now. */
	}

	// Disable irq interrupt
	__disable_irq();

	/* Currently, support Sleep mode */
    /* enter sleep mode */
    LPC_SC->PCON = 0x0;

    /* Sleep Mode*/
    __WFI();

    // CPU will be suspend here...

	/* From power down to WOL, the PLL needs to be reconfigured,
	otherwise, the CCLK will be generated from 4Mhz IRC instead
	of main OSC 12Mhz */
	/* Initialize system clock */
	SystemInit();

	/*
	 * Initialize debug via UART
	 */
	debug_frmwrk_init();
	/*
	 * Init LED
	 */
	LED_Init();
	_DBG_("Wake up from sleep mode");

	/* Calling EMACInit() is overkill which also initializes the PHY, the
	main reason to do that is to make sure the descriptors and descriptor
	status for both TX and RX are clean and ready to use. It won't go wrong. */
	Usr_Init_Emac();
	// Re-Enable irq interrupt
	__enable_irq();

#endif										/* endif ENABLE_WOL */

#if BOUNCE_RX
while( 1 )
{
	LED_Blink(BLINK_LED_PIN);
	if ( PacketReceived == TRUE )
	{
	  PacketReceived = FALSE;
		/* Reverse Source and Destination, then copy the body */
		memcpy( (uint8_t *)txptr, (uint8_t *)(rxptr+6), 6);
		memcpy( (uint8_t *)(txptr+6), (uint8_t *)rxptr, 6);
		memcpy( (uint8_t *)(txptr+12), (uint8_t *)(rxptr+12), (ReceiveLength - 12));
		_DBG_("Send packet");
		DataPacket.pbDataBuf = (uint32_t *)txptr;
		DataPacket.ulDataLen = ReceiveLength;
		EMAC_WritePacketBuffer(&DataPacket);
		EMAC_UpdateTxProduceIndex();
	}
}
#endif										/* endif BOUNCE_RX */

#if TX_ONLY
	/* Transmit packets only */
	while ( 1 ) {
		while (Pressed == RESET){
			LED_Blink(BLINK_LED_PIN);
		}
		Pressed = RESET;
		txptr = (uint8_t *)gTxBuf;
		_DBG_("Send packet");
		LED_Blink(TX_LED_PIN);
		DataPacket.pbDataBuf = (uint32_t *)txptr;
		// Note that there're 4-byte CRC added
		DataPacket.ulDataLen = TX_PACKET_SIZE + 4;
		EMAC_WritePacketBuffer(&DataPacket);
		EMAC_UpdateTxProduceIndex();
		for ( j = 0; j < 0x200000; j++ );	/* delay */
	}
#endif										/* endif TX_ONLY */

  return 0;
}



/* With ARM and GHS toolsets, the entry point is main() - this will
   allow the linker to generate wrapper code to setup stacks, allocate
   heap area, and initialize and copy code and data segments. For GNU
   toolsets, the entry point is through __start() in the crt0_gnu.asm
   file, and that startup code will setup stacks and data */
int c_entry(void)
{
    return main();
}


#ifdef  DEBUG
/*******************************************************************************
* @brief		Reports the name of the source file and the source line number
* 				where the CHECK_PARAM error has occurred.
* @param[in]	file Pointer to the source file name
* @param[in]    line assert_param error line source number
* @return		None
*******************************************************************************/
void check_failed(uint8_t *file, uint32_t line)
{
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while(1);
}
#endif

/*****************************************************************************
**                            End Of File
*****************************************************************************/

