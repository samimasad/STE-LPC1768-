/**
 * @file	: uart_autobaud_test.c
 * @purpose	: An example of UART using auto baud rate and interrupt mode
 * 				to test the UART driver
 * @version	: 1.0
 * @date	: 18. Mar. 2009
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

#include "lpc17xx_uart.h"		/* Central include file */
#include "lpc17xx_libcfg.h"
#include "lpc17xx_nvic.h"
#include "lpc17xx_pinsel.h"


#define UART_PORT 0

#if (UART_PORT == 0)
#define TEST_UART LPC_UART0
#elif (UART_PORT == 1)
#define TEST_UART LPC_UART1
#endif


/************************** PRIVATE MACROS *************************/
/************************** PRIVATE TYPES *************************/
/************************** PRIVATE VARIABLES *************************/
uint8_t syncmenu[] = "AutoBaudrate Status: Synchronous! \n\r";
uint8_t menu1[] = "Hello NXP Semiconductors \n\r";
uint8_t menu2[] =
"UART Auto Baudrate demo\n\r\t "
"MCU LPC17xx - ARM Cortex-M3 \n\r\t "
"UART0 - Auto Baud rate mode used \n\r";
uint8_t menu3[] = "UART demo terminated!\n";

/* Synchronous Flag */
__IO FlagStatus Synchronous;

/************************** PRIVATE FUNCTIONS *************************/
void print_menu(void);

#if (UART_PORT == 0)
void UART0_IRQHandler(void);
#elif (UART_PORT == 1)
void UART1_IRQHandler(void);
#endif


/*********************************************************************//**
 * @brief	Print Welcome Screen Menu subroutine
 * @param	None
 * @return	None
 **********************************************************************/
void print_menu(void)
{
	UART_Send(TEST_UART, menu1, sizeof(menu1), BLOCKING);
	UART_Send(TEST_UART, menu2, sizeof(menu2), BLOCKING);
}

#if (UART_PORT == 0)
/*********************************************************************//**
 * @brief	UART0 interrupt handler sub-routine reference, just to call the
 * 				standard interrupt handler in uart driver
 * @param	None
 * @return	None
 **********************************************************************/
void UART0_IRQHandler(void)
{
	// Call Standard UART 0 interrupt handler
	UART0_StdIntHandler();
}
#endif


#if (UART_PORT == 1)
/*********************************************************************//**
 * @brief	UART1 interrupt handler sub-routine reference, just to call the
 * 				standard interrupt handler in uart driver
 * @param	None
 * @return	None
 **********************************************************************/
void UART1_IRQHandler(void)
{
	// Call Standard UART 0 interrupt handler
	UART1_StdIntHandler();
}
#endif


/*********************************************************************//**
 * @brief		Auto baudrate callback function
 * @param[in]	bABType	Auto baudrate interrupt type
 * @return		None
 **********************************************************************/
void UART_ABIntCallback(uint32_t bABIntType)
{
	if (Synchronous == RESET)
	{
		/* Interrupt caused by End of auto-baud */
		if (bABIntType & UART_AUTOBAUD_INTSTAT_ABEO){
			// Disable AB interrupt
			UART_IntConfig(TEST_UART, UART_INTCFG_ABEO, DISABLE);
			// Set Sync flag
			Synchronous = SET;
		}

		/* Auto-Baudrate Time-Out interrupt (not implemented) */
		if (bABIntType & UART_AUTOBAUD_INTSTAT_ABTO) {
			/* Just clear this bit - Add your code here */
			TEST_UART->ACR |= (1<<9);
		}
	}
}

/*********************************************************************//**
 * @brief		UART Line Status Error callback
 * @param[in]	bLSErrType	UART Line Status Error Type
 * @return		None
 **********************************************************************/
void UART_IntErr(uint8_t bLSErrType)
{
	uint8_t test;
	// Loop forever
	while (1){
		// For testing purpose
		test = bLSErrType;
	}
}


/************************** MAIN SUB-ROUTINE *************************/

/*********************************************************************//**
 * @brief	Main UART testing example sub-routine
 * 			First, the user must type 'A' or 'a' character to start
 * 			Auto baud rate mode.
 * 			Once Auto baud rate mode completed, print welcome screen,
 * 			then press any key to have it read in from the terminal
 * 			and returned back to the terminal.
 * 			- Press ESC to exit
 * 			- Press 'r' to print welcome screen menu again
 **********************************************************************/
int c_entry(void)
{
	// UART Configuration structure variable
	UART_CFG_Type UARTConfigStruct;
	// UART FIFO configuration Struct variable
	UART_FIFO_CFG_Type UARTFIFOConfigStruct;
	// Pin configuration for UART0
	PINSEL_CFG_Type PinCfg;
	// Auto baudrate configuration structure
	UART_AB_CFG_Type ABConfig;

	uint32_t idx, len;
	__IO FlagStatus exitflag;
	uint8_t buffer[10];

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

#if (UART_PORT == 0)
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

#if (UART_PORT == 1)
	/*
	 * Initialize UART1 pin connect
	 */
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Pinnum = 15;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 16;
	PINSEL_ConfigPin(&PinCfg);
#endif

	/* Initialize UART Configuration parameter structure to default state:
	 * Baudrate = 9600bps
	 * 8 data bit
	 * 1 Stop bit
	 * None parity
	 */
	UART_ConfigStructInit(&UARTConfigStruct);

	/* Initialize UART0 peripheral with given to corresponding parameter
	 * in this case, don't care the baudrate value UART initialized
	 * since this will be determine when running auto baudrate
	 */
	UART_Init(TEST_UART, &UARTConfigStruct);

	/* Initialize FIFOConfigStruct to default state:
	 * 				- FIFO_DMAMode = DISABLE
	 * 				- FIFO_Level = UART_FIFO_TRGLEV0
	 * 				- FIFO_ResetRxBuf = ENABLE
	 * 				- FIFO_ResetTxBuf = ENABLE
	 * 				- FIFO_State = ENABLE
	 */
	UART_FIFOConfigStructInit(&UARTFIFOConfigStruct);

	// Initialize FIFO for UART0 peripheral
	UART_FIFOConfig(TEST_UART, &UARTFIFOConfigStruct);


	// Enable UART Transmit
	UART_TxCmd(TEST_UART, ENABLE);


	// Setup callback ---------------
	// Line Status Error callback
	UART_SetupCbs(TEST_UART, 3, (void *)UART_IntErr);
	// UART Auto baudrate callback
	UART_SetupCbs(TEST_UART, 2, (void *)UART_ABIntCallback);

    /* Enable UART End of Auto baudrate interrupt */
	UART_IntConfig(TEST_UART, UART_INTCFG_ABEO, ENABLE);
	/* Enable UART Auto baudrate timeout interrupt */
	UART_IntConfig(TEST_UART, UART_INTCFG_ABTO, ENABLE);

#if (UART_PORT == 0)
    /* preemption = 1, sub-priority = 1 */
    NVIC_SetPriority(UART0_IRQn, ((0x01<<3)|0x01));
	/* Enable Interrupt for UART0 channel */
    NVIC_EnableIRQ(UART0_IRQn);
#endif

#if (UART_PORT == 1)
    /* preemption = 1, sub-priority = 1 */
    NVIC_SetPriority(UART1_IRQn, ((0x01<<3)|0x01));
	/* Enable Interrupt for UART0 channel */
    NVIC_EnableIRQ(UART1_IRQn);
#endif


/* ---------------------- Auto baud rate section ----------------------- */
	// Reset Synchronous flag for auto-baudrate mode
	Synchronous = RESET;

	// Configure Auto baud rate mode
    ABConfig.ABMode = UART_AUTOBAUD_MODE0;
    ABConfig.AutoRestart = ENABLE;

    // Start auto baudrate mode
    UART_ABCmd(TEST_UART, &ABConfig, ENABLE);

    /* Loop until auto baudrate mode complete */
    while (Synchronous == RESET);


    // Print status of auto baudrate
    UART_Send(TEST_UART, syncmenu, sizeof(syncmenu), BLOCKING);
/* ---------------------- End of Auto baud rate section ----------------------- */

	// print welcome screen
	print_menu();

	// reset exit flag
	exitflag = RESET;

    /* Read some data from the buffer */
    while (exitflag == RESET)
    {
       len = 0;
        while (len == 0)
        {
            len = UART_Receive(TEST_UART, buffer, sizeof(buffer), NONE_BLOCKING);
        }

        /* Got some data */
        idx = 0;
        while (idx < len)
        {
            if (buffer[idx] == 27)
            {
                /* ESC key, set exit flag */
            	UART_Send(TEST_UART, menu3, sizeof(menu3), BLOCKING);
                exitflag = SET;
            }
            else if (buffer[idx] == 'r')
            {
                print_menu();
            }
            else
            {
                /* Echo it back */
            	UART_Send(TEST_UART, &buffer[idx], 1, BLOCKING);
            }
            idx++;
        }
    }

    // wait for current transmission complete - THR must be empty
    while (UART_CheckBusy(TEST_UART) == SET);

    // DeInitialize UART0 peripheral
    UART_DeInit(TEST_UART);

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
