/**
 * @file	: i2s_dma_test.c
 * @purpose	: An example of I2S using DMA mode to test the I2S driver
 * @version	: 1.0
 * @date	: 14. May. 2009
 * @author	: NguyenCao
 *----------------------------------------------------------------------------
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * products. This software is supplied "AS IS" without any warranties.
 * NXP Semiconductors assumes no responsibility or liability for the
 * use of the software, conveys no license or title under any patent,
 * copyright, or mask work right to the product. NXP Semiconductors
 * reserves the right to make changes in the software without
 * notification. NXP Semiconductors also make no representation or
 * warranty that such application will be suitable for the specified
 * use without further testing or modification.
 **********************************************************************/

#include "lpc17xx_i2s.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_nvic.h"
#include "lpc17xx_gpdma.h"
#include "debug_frmwrk.h"
#include "lpc17xx_pinsel.h"


/************************** PRIVATE MACROS *************************/
/** Max buffer length */
#define BUFFER_SIZE			0x0A
/** DMA transfer size */
#define DMA_SIZE		0x100UL
/** DMA Source Address is AHBRAM1_BASE that used for USB RAM purpose, but
 * it is not used in this example, so this memory section can be used for general purpose
 * memory
 */
#define DMA_SRC			LPC_AHBRAM1_BASE
/** DMA Source Address is (AHBRAM1_BASE + DMA_SIZE) that used for USB RAM purpose, but
 * it is not used in this example, so this memory section can be used for general purpose
 * memory
 */
#define DMA_DST			(DMA_SRC+DMA_SIZE)
/************************** PRIVATE VARIABLE ***********************/
uint8_t menu[]=
	"********************************************************************************\n\r"
	"Hello NXP Semiconductors \n\r"
	" I2S test DMA mode demo \n\r"
	"\t - MCU: LPC17xx \n\r"
	"\t - Core: ARM CORTEX-M3 \n\r"
	"\t - Communicate via: UART0 - 115200 bps \n\r"
	" Use two I2S channels in the same board to transfer data use DMA mode\n\r"
	"********************************************************************************\n\r";
/** I2S Receive function pin selection group 0*/
#define I2S_PINSEL_SRX_CLK_P0_4		{0, 4, 1, \
									PINSEL_PINMODE_PULLUP, \
									PINSEL_PINMODE_NORMAL}

#define I2S_PINSEL_SRX_WS_P0_5		{0, 5, 1, \
									PINSEL_PINMODE_PULLUP, \
									PINSEL_PINMODE_NORMAL}

#define I2S_PINSEL_SRX_SDA_P0_6		{0, 6, 1, \
									PINSEL_PINMODE_PULLUP, \
									PINSEL_PINMODE_NORMAL}

/** I2S Receive function pin selection group 1*/
#define I2S_PINSEL_SRX_CLK_P0_23	{0, 23, 2, \
									PINSEL_PINMODE_PULLUP, \
									PINSEL_PINMODE_NORMAL}

#define I2S_PINSEL_SRX_WS_P0_24		{0, 24, 2, \
									PINSEL_PINMODE_PULLUP, \
									PINSEL_PINMODE_NORMAL}

#define I2S_PINSEL_SRX_SDA_P0_25	{0, 25, 2, \
									PINSEL_PINMODE_PULLUP, \
									PINSEL_PINMODE_NORMAL}

/** I2S RX_MCLK for two receive group */
#define I2S_PINSEL_RX_MCLK_P4_28	{4, 28, 1, \
									PINSEL_PINMODE_PULLUP, \
									PINSEL_PINMODE_NORMAL}

/** I2S Transmit function pin selection group 0*/
#define I2S_PINSEL_STX_CLK_P0_7		{0, 7, 1, \
									PINSEL_PINMODE_PULLUP, \
									PINSEL_PINMODE_NORMAL}

#define I2S_PINSEL_STX_WS_P0_8		{0, 8, 1, \
									PINSEL_PINMODE_PULLUP, \
									PINSEL_PINMODE_NORMAL}

#define I2S_PINSEL_STX_SDA_P0_9		{0, 9, 1, \
									PINSEL_PINMODE_PULLUP, \
									PINSEL_PINMODE_NORMAL}

/** I2S Transmit function pin selection group 1*/
#define I2S_PINSEL_STX_CLK_P2_11	{2, 11, 3, \
									PINSEL_PINMODE_PULLUP, \
									PINSEL_PINMODE_NORMAL}

#define I2S_PINSEL_STX_WS_P2_12		{2, 12, 3, \
									PINSEL_PINMODE_PULLUP, \
									PINSEL_PINMODE_NORMAL}

#define I2S_PINSEL_STX_SDA_P2_13	{2, 13, 3, \
									PINSEL_PINMODE_PULLUP, \
									PINSEL_PINMODE_NORMAL}

/** I2S TX_MCLK for two transmit group */
#define I2S_PINSEL_TX_MCLK_P4_29	{4, 29, 1, \
									PINSEL_PINMODE_PULLUP, \
									PINSEL_PINMODE_NORMAL}

/* Max number of pin on each pin function */
#define I2S_MAX_SRX_CLK_PIN		(2)
#define I2S_MAX_SRX_WS_PIN		(2)
#define I2S_MAX_SRX_SDA_PIN		(2)
#define I2S_MAX_RX_MCLK_PIN		(1)

#define I2S_MAX_STX_CLK_PIN		(2)
#define I2S_MAX_STX_WS_PIN		(2)
#define I2S_MAX_STX_SDA_PIN		(2)
#define I2S_MAX_TX_MCLK_PIN		(1)

/* Pin configuration data */
const PINSEL_CFG_Type i2s_srx_clk_pin[I2S_MAX_SRX_CLK_PIN] = {
		I2S_PINSEL_SRX_CLK_P0_4, I2S_PINSEL_SRX_CLK_P0_23 };
const PINSEL_CFG_Type i2s_srx_ws_pin[I2S_MAX_SRX_WS_PIN] = {
		I2S_PINSEL_SRX_WS_P0_5, I2S_PINSEL_SRX_WS_P0_24 };
const PINSEL_CFG_Type i2s_srx_sda_pin[I2S_MAX_SRX_SDA_PIN] = {
		I2S_PINSEL_SRX_SDA_P0_6, I2S_PINSEL_SRX_SDA_P0_25 };
const PINSEL_CFG_Type i2s_rx_mclk_pin[I2S_MAX_RX_MCLK_PIN] = {
		I2S_PINSEL_RX_MCLK_P4_28 };

const PINSEL_CFG_Type i2s_stx_clk_pin[I2S_MAX_STX_CLK_PIN] = {
		I2S_PINSEL_STX_CLK_P0_7, I2S_PINSEL_STX_CLK_P2_11 };
const PINSEL_CFG_Type i2s_stx_ws_pin[I2S_MAX_STX_WS_PIN] = {
		I2S_PINSEL_STX_WS_P0_8, I2S_PINSEL_STX_WS_P2_12 };
const PINSEL_CFG_Type i2s_stx_sda_pin[I2S_MAX_STX_SDA_PIN] = {
		I2S_PINSEL_STX_SDA_P0_9, I2S_PINSEL_STX_SDA_P2_13 };
const PINSEL_CFG_Type i2s_tx_mclk_pin[I2S_MAX_TX_MCLK_PIN] = {
		I2S_PINSEL_TX_MCLK_P4_29 };

// Terminal Counter flag for Channel 0
__IO uint32_t Channel0_TC;

// Error Counter flag for Channel 0
__IO uint32_t Channel0_Err;

// Terminal Counter flag for Channel 1
__IO uint32_t Channel1_TC;

// Error Counter flag for Channel 1
__IO uint32_t Channel1_Err;

volatile uint32_t *I2STXBuffer = (uint32_t*)(DMA_SRC);
volatile uint32_t *I2SRXBuffer = (uint32_t*)(DMA_DST);

/************************** PRIVATE FUNCTIONS *************************/
void DMA_IRQHandler (void);
void GPDMA_Callback0(uint32_t DMA_Status);
void GPDMA_Callback1(uint32_t DMA_Status);

void Buffer_Init(void);
Bool Buffer_Verify(void);
void print_menu(void);

/*********************************************************************//**
 * @brief		GPDMA interrupt handler sub-routine
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void DMA_IRQHandler (void)
{
	// Execute GPDMA_IntHandler() in GPDMA driver
	GPDMA_IntHandler();
}

/*********************************************************************//**
 * @brief		DMA call-back function
 * @param[in]	DMA_Status	DMA status input, could be
 * 							- GPDMA_STAT_INTTC: Terminate Counter interrupt
 * 							- GPDMA_STAT_INTERR: Error interrupt
 * @return		None
 **********************************************************************/
void GPDMA_Callback0(uint32_t DMA_Status)
{
	// Incase  of terminal counter
	if(DMA_Status & GPDMA_STAT_INTTC) {
		Channel0_TC++;
	}

	// incase of error
	if (DMA_Status & GPDMA_STAT_INTERR) {
		Channel0_Err++;
	}
}
/*********************************************************************//**
 * @brief		DMA call-back function
 * @param[in]	DMA_Status	DMA status input, could be
 * 							- GPDMA_STAT_INTTC: Terminate Counter interrupt
 * 							- GPDMA_STAT_INTERR: Error interrupt
 * @return		None
 **********************************************************************/
void GPDMA_Callback1(uint32_t DMA_Status)
{
	// Incase  of terminal counter
	if(DMA_Status & GPDMA_STAT_INTTC) {
		Channel1_TC++;
	}

	// incase of error
	if (DMA_Status & GPDMA_STAT_INTERR) {
		Channel1_Err++;
	}
}

/*********************************************************************//**
 * @brief		Initialize buffer
 * @param[in]	None
 * @return 		None
 **********************************************************************/
void Buffer_Init(void) {
	uint8_t i;

	for (i = 0; i < BUFFER_SIZE; i++) {
		I2STXBuffer[i] = ((i+1)<<16) + i + 1;
		I2SRXBuffer[i] = 0;
	}
}

/*********************************************************************//**
 * @brief		Verify buffer
 * @param[in]	none
 * @return 		None
 **********************************************************************/
Bool Buffer_Verify(void) {
	uint8_t i;
	uint32_t *pTX = (uint32_t *) &I2STXBuffer[0];
	uint32_t *pRX = (uint32_t *) &I2SRXBuffer[1];

	for (i = 0; i < BUFFER_SIZE; i++) {
		if (*pTX++ != *pRX++)  {
			/* Call Error Loop */
			return FALSE;
		}
	}
	return TRUE;
}
void print_menu(void)
{
	_DBG_(menu);
}
/************************** MAIN SUB-ROUTINE *************************/

/*********************************************************************//**
 * @brief Main I2S using GPDMA program body
 **********************************************************************/
int c_entry(void)
{
	uint32_t i;
	GPDMA_Channel_CFG_Type GPDMACfg;
	I2S_MODEConf_Type I2S_ClkConfig;
	I2S_CFG_Type I2S_ConfigStruct;
	I2S_PinCFG_Type I2S_PinStruct;
	I2S_DMAConf_Type I2S_DMAStruct;

	// DeInit NVIC and SCBNVIC
	NVIC_DeInit();
	NVIC_SCBDeInit();

	/* Configure the NVIC Preemption Priority Bits:
	 * two (2) bits of preemption priority, six (6) bits of sub-priority.
	 * Since the Number of Bits used for Priority Levels is five (5), so the
	 * actual bit number of sub-priority is three (3)
	 */
	NVIC_SetPriorityGrouping(0x05);

	//  Set Vector table offset value
#if (__RAM_MODE__==1)
	NVIC_SetVTOR(0x10000000);
#else
	NVIC_SetVTOR(0x00000000);
#endif
//	 NVIC_SetPriorityGrouping(0x06);

	Buffer_Init();
	debug_frmwrk_init();
	print_menu();
	_DBG_("Press '1' to initialize buffer...");
	while(_DG !='1');
	_DBG_("Transmit Buffer init: ...");
	for(i=0;i<BUFFER_SIZE;i++)
	{
		_DBH32(I2STXBuffer[i]);_DBG_("");
	}
	_DBG_("Receive Buffer init: ...");
	for(i=0;i<BUFFER_SIZE;i++)
	{
		_DBH32(I2SRXBuffer[i]);_DBG_("");
	}

	/* Initializes pin corresponding to I2S function */
	I2S_PinStruct.CLK_Pin=I2S_STX_CLK_P0_7;
	I2S_PinStruct.WS_Pin=I2S_STX_WS_P0_8;
	I2S_PinStruct.SDA_Pin=I2S_STX_SDA_P0_9;
	I2S_PinStruct.MCLK_Pin=I2S_TX_MCLK_P4_29;

	PINSEL_ConfigPin((PINSEL_CFG_Type *) (&i2s_stx_clk_pin[I2S_PinStruct.CLK_Pin]));
	PINSEL_ConfigPin((PINSEL_CFG_Type *) (&i2s_stx_ws_pin[I2S_PinStruct.WS_Pin]));
	PINSEL_ConfigPin((PINSEL_CFG_Type *) (&i2s_stx_sda_pin[I2S_PinStruct.SDA_Pin]));
	PINSEL_ConfigPin((PINSEL_CFG_Type *) (&i2s_tx_mclk_pin[I2S_PinStruct.MCLK_Pin]));

	// Configure pinsel for I2S_RX
	I2S_PinStruct.CLK_Pin=I2S_SRX_CLK_P0_4;
	I2S_PinStruct.WS_Pin=I2S_SRX_WS_P0_5;
	I2S_PinStruct.SDA_Pin=I2S_SRX_SDA_P0_6;
	I2S_PinStruct.MCLK_Pin=I2S_RX_MCLK_P4_28;

	PINSEL_ConfigPin((PINSEL_CFG_Type *) (&i2s_srx_clk_pin[I2S_PinStruct.CLK_Pin]));
	PINSEL_ConfigPin((PINSEL_CFG_Type *) (&i2s_srx_ws_pin[I2S_PinStruct.WS_Pin]));
	PINSEL_ConfigPin((PINSEL_CFG_Type *) (&i2s_srx_sda_pin[I2S_PinStruct.SDA_Pin]));
	PINSEL_ConfigPin((PINSEL_CFG_Type *) (&i2s_rx_mclk_pin[I2S_PinStruct.MCLK_Pin]));

	I2S_Init(LPC_I2S);

	//Setup for I2S: RX is similar with TX
	/* setup:
	 * 		- wordwidth: 16 bits
	 * 		- stereo mode
	 * 		- master mode for I2S_TX and slave for I2S_RX
	 * 		- ws_halfperiod is 31
	 * 		- not use mute mode
	 * 		- use reset and stop mode
	 * 		- select the fractional rate divider clock output as the source,
	 * 		- disable 4-pin mode
	 * 		- MCLK ouput is disable
	 * 		- Frequency = 44.1 kHz (x=8,y=51 - automatic setting)
	 * Because we use mode I2STXMODE[3:0]= 0000, I2SDAO[5]=0 and
	 * I2SRX[3:0]=0000, I2SDAI[5] = 1. So we have I2SRX_CLK = I2STX_CLK
	 * --> I2SRXBITRATE = 1 (not divide TXCLK to produce RXCLK)
	 */

	/* Audio Config*/
	I2S_ConfigStruct.wordwidth = I2S_WORDWIDTH_16;
	I2S_ConfigStruct.mono = I2S_STEREO;
	I2S_ConfigStruct.stop = I2S_STOP_ENABLE;
	I2S_ConfigStruct.reset = I2S_RESET_ENABLE;
	I2S_ConfigStruct.ws_sel = I2S_MASTER_MODE;
	I2S_ConfigStruct.mute = I2S_MUTE_DISABLE;
	I2S_Config(LPC_I2S,I2S_TX_MODE,&I2S_ConfigStruct);

	I2S_ConfigStruct.ws_sel = I2S_SLAVE_MODE;
	I2S_Config(LPC_I2S,I2S_RX_MODE,&I2S_ConfigStruct);

	/* Clock Mode Config*/
	I2S_ClkConfig.clksel = I2S_CLKSEL_0;
	I2S_ClkConfig.fpin = I2S_4PIN_DISABLE;
	I2S_ClkConfig.mcena = I2S_MCLK_DISABLE;
	I2S_ModeConfig(LPC_I2S,&I2S_ClkConfig,I2S_TX_MODE);
	I2S_ModeConfig(LPC_I2S,&I2S_ClkConfig,I2S_RX_MODE);

	/* Set up frequency and bit rate*/
	I2S_FreqConfig(LPC_I2S, 44100, I2S_TX_MODE);
	I2S_SetBitRate(LPC_I2S, 1, I2S_RX_MODE);
	_DBG_("Press '2' to initialize DMA...");
	while(_DG !='2');
	  /* GPDMA Interrupt configuration section ------------------------------------------------- */

	 /* Initialize GPDMA controller */
	 GPDMA_Init();
	 LPC_GPDMA->DMACConfig = 0x01;

	 /* Setting GPDMA interrupt */
     // Disable interrupt for DMA
     NVIC_DisableIRQ (DMA_IRQn);
     /* preemption = 1, sub-priority = 1 */
     NVIC_SetPriority(DMA_IRQn, ((0x01<<3)|0x01));

	/*
	 * Configure GPDMA channel 0 -------------------------------------------------------------
	 * Used for I2S Transmit
	 */
	// Setup GPDMA channel --------------------------------
	// channel 0
	GPDMACfg.ChannelNum = 0;
	// Source memory
	GPDMACfg.SrcMemAddr = DMA_SRC;
	// Destination memory
	GPDMACfg.DstMemAddr = 0;
	// Transfer size
	GPDMACfg.TransferSize = BUFFER_SIZE;
	// Transfer width - unused
	GPDMACfg.TransferWidth = 0;
	// Transfer type
	GPDMACfg.TransferType = GPDMA_TRANSFERTYPE_M2P;
	// Source connection
	GPDMACfg.SrcConn = 0;
	// Destination connection - unused
	GPDMACfg.DstConn = GPDMA_CONN_I2S_Channel_0;
	// Linker List Item - unused
	GPDMACfg.DMALLI = 0;
	// Setup channel with given parameter
	GPDMA_Setup(&GPDMACfg, GPDMA_Callback0);
	_DBG_("DMA Channel 0 setting finised...");
	/* Reset terminal counter */
	Channel0_TC = 0;
	/* Reset Error counter */
	Channel0_Err = 0;

	/*
	* Configure GPDMA channel 1 -------------------------------------------------------------
	* Used for UART0 Receive
	*/
	// Setup GPDMA channel --------------------------------
	// channel 1
	GPDMACfg.ChannelNum = 1;
	// Source memory - unused
	GPDMACfg.SrcMemAddr = 0;
	// Destination memory
	GPDMACfg.DstMemAddr = DMA_DST;
	// Transfer size
	GPDMACfg.TransferSize = BUFFER_SIZE+1;
	// Transfer width - unused
	GPDMACfg.TransferWidth = 0;
	// Transfer type
	GPDMACfg.TransferType = GPDMA_TRANSFERTYPE_P2M;
	// Source connection - unused
	GPDMACfg.SrcConn = GPDMA_CONN_I2S_Channel_1;
	// Destination connection
	GPDMACfg.DstConn = 0;
	// Linker List Item - unused
	GPDMACfg.DMALLI = 0;
	// Setup channel with given parameter
	GPDMA_Setup(&GPDMACfg, GPDMA_Callback1);
	_DBG_("DMA Channel 1 setting finised...");
	/* Reset terminal counter */
	Channel1_TC = 0;
	/* Reset Error counter */
	Channel1_Err = 0;

	// Enable GPDMA channel 0 & 1
	GPDMA_ChannelCmd(0, ENABLE);
	GPDMA_ChannelCmd(1, ENABLE);

	// Enable interrupt for DMA
	NVIC_EnableIRQ (DMA_IRQn);
	_DBG_("Press '3' to start I2S transfer process...");
	while(_DG !='3');
	_DBG_("I2S Start...");


//	I2S->I2SDMA2 = (0x01<<0) | (0x08<<8);	/* Channel 2 is for RX, enable RX first. */
//	I2S->I2SDMA1 = (0x01<<1) | (0x01<<16);/* Channel 1 is for TX. */

	I2S_DMAStruct.DMAIndex = I2S_DMA_2;
	I2S_DMAStruct.depth = 8;
	I2S_DMAConfig(LPC_I2S, &I2S_DMAStruct, I2S_RX_MODE);
	I2S_DMAStruct.DMAIndex = I2S_DMA_1;
	I2S_DMAStruct.depth = 1;
	I2S_DMAConfig(LPC_I2S, &I2S_DMAStruct, I2S_TX_MODE);

	I2S_Start(LPC_I2S);

	I2S_DMACmd(LPC_I2S, I2S_DMA_2, I2S_RX_MODE, ENABLE);
	I2S_DMACmd(LPC_I2S, I2S_DMA_1, I2S_TX_MODE, ENABLE);

	while ((Channel0_TC == 0)||(Channel1_TC == 0) );

	_DBG_("I2S Finish...");
	_DBG_("Receive Buffer data: ...");
	for(i=0;i<BUFFER_SIZE+1;i++)
	{
	 _DBH32(I2SRXBuffer[i]);
	 if(I2SRXBuffer[i]==0)
	 {
		 _DBG_(" ->Dummy data");
	 }
	 else _DBG_("");
	}
	I2S_DeInit(LPC_I2S);
	while(1);
	return 1;
}

/* With ARM and GHS toolsets, the entry point is main() - this will
   allow the linker to generate wrapper code to setup stacks, allocate
   heap area, and initialize and copy code and data segments. For GNU
   toolsets, the entry point is through __start() in the crt0_gnu.asm
   file, and that startup code will setup stacks and data */
int main(void)
{
    return c_entry();
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
