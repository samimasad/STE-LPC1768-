/**
 * @file	: gpdma_m2m_test.c
 * @purpose	: This example will transfer 2 block of data from memory boundary
 * 			to the other memory boundary on RAM using GPDMA module with interrupt.
 * @version	: 1.0
 * @date	: 15. April. 2009
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
#include "lpc17xx_gpdma.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_nvic.h"
#include "lpc17xx_pinsel.h"
#include "debug_frmwrk.h"



/************************** PRIVATE MACROS *************************/
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


/************************** PRIVATE VARIABLES *************************/
uint8_t menu1[] =
"********************************************************************************\n\r"
"Hello NXP Semiconductors \n\r"
"GPDMA demo \n\r"
"\t - MCU: LPC17xx \n\r"
"\t - Core: ARM Cortex-M3 \n\r"
"\t - Communicate via: UART0 - 115200bps \n\r"
" This example will transfer 2 block of data from memory boundary \n\r"
" to the other memory boundary on RAM using GPDMA module with interrupt \n\r"
"********************************************************************************\n\r";
uint8_t menu2[] = "Demo terminated! \n\r";
uint8_t err_menu[] = "Buffer Check fail!";
uint8_t compl_menu[] = "Buffer Check success!";

// Terminal Counter flag for Channel 0
__IO uint32_t Channel0_TC;

// Error Counter flag for Channel 0
__IO uint32_t Channel0_Err;


/************************** PRIVATE FUNCTIONS *************************/
void print_menu(void);
void Buffer_Init(void);
void Error_Loop(void);
void DMA_IRQHandler (void);
void GPDMA_Callback(uint32_t DMA_Status);

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

/*********************************************************************//**
 * @brief		Print Welcome menu
 * @param[in]	none
 * @return 		None
 **********************************************************************/
void print_menu(void)
{
	_DBG(menu1);
}

/*********************************************************************//**
 * @brief		Initialize buffer
 * @param[in]	none
 * @return 		None
 **********************************************************************/
void Buffer_Init(void)
{
	uint8_t i;
	uint32_t *src_addr = (uint32_t *)DMA_SRC;
	uint32_t *dest_addr = (uint32_t *)DMA_DST;

	 for ( i = 0; i < DMA_SIZE/4; i++ )
	{
		*src_addr++ = i;
		*dest_addr++ = 0;
	}
}


/*********************************************************************//**
 * @brief		Verify buffer
 * @param[in]	none
 * @return 		None
 **********************************************************************/
void Buffer_Verify(void)
{
	uint8_t i;
	uint32_t *src_addr = (uint32_t *)DMA_SRC;
	uint32_t *dest_addr = (uint32_t *)DMA_DST;

	for ( i = 0; i < DMA_SIZE/4; i++ )
	{
		if ( *src_addr++ != *dest_addr++ )
		{
			/* Call Error Loop */
			Error_Loop();
		}
	}
}


/*********************************************************************//**
 * @brief		Error Loop (called by Buffer_Verify() if any error)
 * @param[in]	none
 * @return 		None
 **********************************************************************/
void Error_Loop(void)
{
	_DBG(err_menu);

	/* Loop forever */
	while (1);
}


/*********************************************************************//**
 * @brief	Main GPDMA program body
 **********************************************************************/
int c_entry(void)
{
	GPDMA_Channel_CFG_Type GPDMACfg;

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
	/* Initialize buffer */
	_DBG_("Initialize Buffer...");
	Buffer_Init();

    /* Disable GPDMA interrupt */
    NVIC_DisableIRQ(DMA_IRQn);
    /* preemption = 1, sub-priority = 1 */
    NVIC_SetPriority(DMA_IRQn, ((0x01<<3)|0x01));

    /* Initialize GPDMA controller */
	GPDMA_Init();

	// Setup GPDMA channel --------------------------------
	// channel 0
	GPDMACfg.ChannelNum = 0;
	// Source memory
	GPDMACfg.SrcMemAddr = DMA_SRC;
	// Destination memory
	GPDMACfg.DstMemAddr = DMA_DST;
	// Transfer size
	GPDMACfg.TransferSize = DMA_SIZE;
	// Transfer width
	GPDMACfg.TransferWidth = GPDMA_WIDTH_WORD;
	// Transfer type
	GPDMACfg.TransferType = GPDMA_TRANSFERTYPE_M2M;
	// Source connection - unused
	GPDMACfg.SrcConn = 0;
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

	_DBG_("Start transfer...");

	// Enable GPDMA channel 0
	GPDMA_ChannelCmd(0, ENABLE);

	/* Enable GPDMA interrupt */
	NVIC_EnableIRQ(DMA_IRQn);

	/* Wait for GPDMA processing complete */
	while ((Channel0_TC == 0) && (Channel0_Err == 0));

	/* Verify buffer */
	Buffer_Verify();

	_DBG(compl_menu);

    /* Loop forever */
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
