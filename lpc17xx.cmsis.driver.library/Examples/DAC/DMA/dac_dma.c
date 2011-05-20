/***********************************************************************
 * ID: dac_ex.c 2008-11-09 HieuNguyen
 *
 * Project: DAC driver example
 *
 * Description:
 *     This example is to test DAC peripheral through speaker on MCB2300 board.
 * The DAC peripheral will generate the sin tone, in which will increase
 * step by step to max and reduce to 0 and continue to increase to max again
 * in the same frequency continuously. We are able to hear the tone from the
 * speaker for many cycles.
 *
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
#include "lpc17xx_dac.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_nvic.h"
#include "debug_frmwrk.h"

/************************** PRIVATE TYPES *************************/
/** DMA size of transfer */
#define DMA_SIZE		1


/************************** PRIVATE VARIABLES *************************/
uint8_t menu1[] =
"********************************************************************************\n\r"
"Hello NXP Semiconductors \n\r"
" DAC demo \n\r"
"\t - MCU: LPC17xx \n\r"
"\t - Core: ARM Cortex-M3 \n\r"
"\t - Communicate via: UART0 - 115200 kbps \n\r"
"DMA testing : DAC  memory to peripheral \n\r"
" Value update for DAC is taken from one cell memory, using DMA function to transfer this value to DAC \n\r"
"********************************************************************************\n\r";
uint8_t menu2[] = "Starting DAC demo....... \n\r";

// Terminal Counter flag for Channel 0
__IO uint32_t Channel0_TC;

// Error Counter flag for Channel 0
__IO uint32_t Channel0_Err;

GPDMA_Channel_CFG_Type GPDMACfg;

/************************** PRIVATE FUNCTION *************************/
void print_menu(void);
void GPDMA_Callback(uint32_t DMA_Status);
void DMA_IRQHandler (void);


/**
 * @brief Print menu
 */
void print_menu(void)
{
	_DBG(menu1);
}

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
void GPDMA_Callback(uint32_t DMA_Status)
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


/**
 * @brief Main Program body
 */
int c_entry(void)
{
	PINSEL_CFG_Type PinCfg;
	DAC_CONVERTER_CFG_Type DAC_ConverterConfigStruct;
	uint32_t dac_value =0;

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

	/*
	 * Init DAC pin connect
	 * AOUT on P0.26
	 */
	PinCfg.Funcnum = 2;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 26;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);

	/*
	 * Initialize debug via UART
	 */
	debug_frmwrk_init();

	// print welcome screen
	print_menu();

	/* GPDMA block section -------------------------------------------- */

	/* Disable GPDMA interrupt */
	NVIC_DisableIRQ(DMA_IRQn);

	/* preemption = 1, sub-priority = 1 */
	NVIC_SetPriority(DMA_IRQn, ((0x01<<3)|0x01));

	DAC_ConverterConfigStruct.CNT_ENA =SET;
	DAC_ConverterConfigStruct.DMA_ENA = SET;
	DAC_Init(LPC_DAC);
	/* set time out for DAC*/
	DAC_SetDMATimeOut(LPC_DAC,0xFFFF);
	DAC_ConfigDAConverterControl(LPC_DAC, &DAC_ConverterConfigStruct);

	/* Initialize GPDMA controller */
	GPDMA_Init();

	// Setup GPDMA channel --------------------------------
	// channel 0
	GPDMACfg.ChannelNum = 0;
	// Source memory
	GPDMACfg.SrcMemAddr = (uint32_t)(&dac_value);
	// Destination memory - unused
	GPDMACfg.DstMemAddr = 0;
	// Transfer size
	GPDMACfg.TransferSize = DMA_SIZE;
	// Transfer width - unused
	GPDMACfg.TransferWidth = 0;
	// Transfer type
	GPDMACfg.TransferType = GPDMA_TRANSFERTYPE_M2P;
	// Source connection - unused
	GPDMACfg.SrcConn = 0;
	// Destination connection
	GPDMACfg.DstConn = GPDMA_CONN_DAC;
	// Linker List Item - unused
	GPDMACfg.DMALLI = 0;
	// Setup channel with given parameter
	GPDMA_Setup(&GPDMACfg, GPDMA_Callback);

	/* Reset terminal counter */
	Channel0_TC = 0;
	/* Reset Error counter */
	Channel0_Err = 0;

	/* Enable GPDMA interrupt */
	NVIC_EnableIRQ(DMA_IRQn);

	/* Wait for GPDMA processing complete */
	while (1) {

		// Enable GPDMA channel 0
		GPDMA_ChannelCmd(0, ENABLE);

		while ((Channel0_TC == 0) );

		// Disable GPDMA channel 0
		GPDMA_ChannelCmd(0, DISABLE);

		dac_value ++;
		if (dac_value == 0xFFFF) dac_value =0;

		/* Reset terminal counter */
		Channel0_TC = 0;

		// Re-setup channel
		GPDMA_Setup(&GPDMACfg, GPDMA_Callback);
	}

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

