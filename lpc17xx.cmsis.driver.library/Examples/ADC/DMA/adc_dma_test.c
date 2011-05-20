/**
 * @file	: adc_interrupt_test.c
 * @purpose	: A simple adc driver example
 * @version	: 1.0
 * @date	: 3. April. 2009
 * @author	: HieuNguyen
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
#include "lpc17xx_uart.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_nvic.h"
#include "lpc17xx_gpdma.h"
#include "debug_frmwrk.h"

/************************** PRIVATE TYPES *************************/
/** DMA size of transfer */
#define DMA_SIZE		1


/************************** PRIVATE VARIABLES *************************/
uint8_t menu1[] =
"********************************************************************************\n\r"
"Hello NXP Semiconductors \n\r"
" ADC demo \n\r"
"\t - MCU: LPC17xx \n\r"
"\t - Core: ARM CORTEX-M3 \n\r"
"\t - Communicate via: UART0 - 115200bps \n\r"
" DMA testing : ADC peripheral to memory\n\r"
" Use ADC with 12-bit resolution at freq = 1MHz\n\r"
" Value ADC0 is displayed by UART, this value is taken from destination memory value of DMA function \n\r"
" Turn the potentiometer to see how ADC value changes\n\r"
"********************************************************************************\n\r";
//timer init
TIM_TIMERCFG_Type TIM_ConfigStruct;
TIM_MATCHCFG_Type TIM_MatchConfigStruct ;

// Terminal Counter flag for Channel 0
__IO uint32_t Channel0_TC;

// Error Counter flag for Channel 0
__IO uint32_t Channel0_Err;

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
 * @brief Main program body
 */
int c_entry(void)
{
	PINSEL_CFG_Type PinCfg;
	GPDMA_Channel_CFG_Type GPDMACfg;
	uint32_t adc_value, tmp;

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

	/*
	 * Init LPC_ADC pin connect
	 * AD0.2 on P0.25
	 */
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 25;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);

	/* Configuration for ADC :
	 * 	Frequency at 1Mhz
	 *  ADC channel 2, generate interrupt to make a request for DMA source
	 */
	ADC_Init(LPC_ADC, 1000000);
	ADC_IntConfig(LPC_ADC,ADC_ADINTEN2,SET);
	ADC_ChannelCmd(LPC_ADC,ADC_CHANNEL_2,SET);

	/* Initialize GPDMA controller */
	GPDMA_Init();

	// Setup GPDMA channel --------------------------------
	// channel 0
	GPDMACfg.ChannelNum = 0;
	// Source memory - unused
	GPDMACfg.SrcMemAddr = 0;
	// Destination memory
	GPDMACfg.DstMemAddr = (uint32_t) &adc_value;
	// Transfer size
	GPDMACfg.TransferSize = DMA_SIZE;
	// Transfer width - unused
	GPDMACfg.TransferWidth = 0;
	// Transfer type
	GPDMACfg.TransferType = GPDMA_TRANSFERTYPE_P2M;
	// Source connection
	GPDMACfg.SrcConn = GPDMA_CONN_ADC;
	// Destination connection - unused
	GPDMACfg.DstConn = 0;
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

	while (1) {

		// Enable GPDMA channel 0
		GPDMA_ChannelCmd(0, ENABLE);

		ADC_StartCmd(LPC_ADC,ADC_START_NOW);
		/* Wait for GPDMA processing complete */;
		while ((Channel0_TC == 0) );

		// Disable GPDMA channel 0
		GPDMA_ChannelCmd(0, DISABLE);

		//Display the result of conversion on the UART0
		_DBG("ADC value on channel 2: ");
		_DBD32(ADC_DR_RESULT(adc_value));
		_DBG_("");

		// Wait for a while
		for(tmp = 0; tmp < 1000000; tmp++);

		// Re-setup channel
		GPDMA_Setup(&GPDMACfg, GPDMA_Callback);

		/* Reset terminal counter */
		Channel0_TC = 0;
		/* Reset Error counter */
		Channel0_Err = 0;
	}
	ADC_DeInit(LPC_ADC);
	return 1;
}

/* Support required entry point for other toolchain */
int main (void)
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

