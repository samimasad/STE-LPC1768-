/**
 * @file	: can_test_two_kit.c
 * @purpose	: A simple CAN driver example to test CAN operation in two kit
 * @version	: 1.0
 * @date	: 1. June. 2009
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
#include "lpc17xx_can.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_nvic.h"
#include "lpc17xx_pinsel.h"
#include "debug_frmwrk.h"

#define CAN_TRANSMIT				0
#define CAN_RECEIVE					!CAN_TRANSMIT
#define TX_BUFFER_SIZE				4
#define RX_BUFFER_SIZE				2

/************************** PRIVATE VARIABLES *************************/
uint8_t menu[]=
	"********************************************************************************\n\r"
	"Hello NXP Semiconductors \n\r"
	"CAN demo \n\r"
	"\t - MCU: LPC17xx \n\r"
	"\t - Core: ARM CORTEX-M3 \n\r"
	"\t - Communicate via: UART0 - 115200 bps \n\r"
	"Use 2 CAN peripherals on 2 boards to transfer data \n\r"
	"********************************************************************************\n\r";
/** CAN function pin selection group 0*/
#define CAN_PINSEL_RD1_P0_0		{0, 0, 1, \
								PINSEL_PINMODE_PULLUP, \
								PINSEL_PINMODE_NORMAL}

#define CAN_PINSEL_TD1_P0_1		{0, 1, 1, \
								PINSEL_PINMODE_PULLUP, \
								PINSEL_PINMODE_NORMAL}

#define CAN_PINSEL_RD2_P0_4		{0, 4, 2, \
								PINSEL_PINMODE_PULLUP, \
								PINSEL_PINMODE_NORMAL}

#define CAN_PINSEL_TD2_P0_5		{0, 5, 2, \
								PINSEL_PINMODE_PULLUP, \
								PINSEL_PINMODE_NORMAL}

/** CAN function pin selection group 1*/
#define CAN_PINSEL_RD1_P0_21	{0, 21, 3, \
								PINSEL_PINMODE_PULLUP, \
								PINSEL_PINMODE_NORMAL}

#define CAN_PINSEL_TD1_P0_22	{0, 22, 3, \
								PINSEL_PINMODE_PULLUP, \
								PINSEL_PINMODE_NORMAL}

#define CAN_PINSEL_RD2_P2_7		{2, 7, 1, \
								PINSEL_PINMODE_PULLUP, \
								PINSEL_PINMODE_NORMAL}

#define CAN_PINSEL_TD2_P2_8		{2, 8, 1, \
								PINSEL_PINMODE_PULLUP, \
								PINSEL_PINMODE_NORMAL}

/* Max number of pin on each pin function */
#define CAN_MAX_RD1_PIN		(2)
#define CAN_MAX_TD1_PIN		(2)

#define CAN_MAX_RD2_PIN		(2)
#define CAN_MAX_TD2_PIN		(2)
/*Pin configuration*/
const PINSEL_CFG_Type can_rd1_pin[CAN_MAX_RD1_PIN] = {
		CAN_PINSEL_RD1_P0_0, CAN_PINSEL_RD1_P0_21 };
const PINSEL_CFG_Type can_td1_pin[CAN_MAX_TD1_PIN] = {
		CAN_PINSEL_TD1_P0_1, CAN_PINSEL_TD1_P0_22 };
const PINSEL_CFG_Type can_rd2_pin[CAN_MAX_RD2_PIN] = {
		CAN_PINSEL_RD2_P0_4, CAN_PINSEL_RD2_P2_7 };
const PINSEL_CFG_Type can_td2_pin[CAN_MAX_TD2_PIN] = {
		CAN_PINSEL_TD2_P0_5, CAN_PINSEL_TD2_P2_8 };

/** CAN variable definition **/
CAN_MSG_Type TXMsg, RXMsg; // messages for test Bypass mode
CAN_MSG_Type AFTxMsg[TX_BUFFER_SIZE], AFRxMsg[RX_BUFFER_SIZE]; //messages for test Acceptance Filter mode
uint32_t CANRxCount = 0, CANTxCount = 0;
uint32_t CANErrCount = 0;

/************************** PRIVATE FUNCTIONS *************************/
void CAN_IRQHandler(void);
void CAN_Callback0(void);
void CAN_Callback1(void);

void CAN_InitMessage(void);
Bool CAN_VerifyMessage(CAN_MSG_Type Msg1, CAN_MSG_Type Msg2);
CAN_ERROR CAN_SetupAFTable(void);
void CAN_InitAFMessage(void);
void PrintMessage(CAN_MSG_Type* msg);

/*********************************************************************//**
 * @brief		CAN_IRQ Handler, control receive message operation
 * param[in]	none
 * @return 		none
 **********************************************************************/
void CAN_IRQHandler()
{
	CAN_IntHandler(LPC_CAN1);
}
/*********************************************************************//**
 * @brief		CAN Callback 0 - handles recieve interrupt
 * param[in]	none
 * @return 		none
 **********************************************************************/
void CAN_Callback0()
{
	//test Acceptance Filter mode
	CAN_ReceiveMsg(LPC_CAN1, &AFRxMsg[CANRxCount]);
	PrintMessage(&AFRxMsg[CANRxCount]);_DBG_("");
	CANRxCount++;
}

/*********************************************************************//**
 * @brief		CAN Callback 1 - handles data overrun interrupt
 * param[in]	none
 * @return 		none
 **********************************************************************/
void CAN_Callback1()
{
	_DBG_("Data overrun has occurred!");
}
/*********************************************************************//**
 * @brief		Print Message via COM1
 * param[in]	msg: point to CAN_MSG_Type object that will be printed
 * @return 		none
 **********************************************************************/
void PrintMessage(CAN_MSG_Type* CAN_Msg)
{
	uint32_t data;
	_DBG("Message ID:     ");
	_DBH32(CAN_Msg->id);_DBG_("");
	_DBG("Message length: ");
	_DBH32(CAN_Msg->len);_DBG_(" BYTES");
	_DBG("Message type:   ");
	if(CAN_Msg->type==DATA_FRAME)
	{
		_DBG_("DATA FRAME ");
	}
	else
		_DBG_("REMOTE FRAME ");
	_DBG("Message format: ");
	if(CAN_Msg->format==STD_ID_FORMAT)
	{
		_DBG_("STANDARD ID FRAME FORMAT");
	}
	else
		_DBG_("EXTENDED ID FRAME FORMAT");
	_DBG("Message dataA:  ");
	data = (CAN_Msg->dataA[0])|(CAN_Msg->dataA[1])|(CAN_Msg->dataA[2])|(CAN_Msg->dataA[3]);
	_DBH32(data);_DBG_("");
	data = (CAN_Msg->dataB[0])|(CAN_Msg->dataB[1])|(CAN_Msg->dataB[2])|(CAN_Msg->dataB[3]);
	_DBG("Message dataB:  ");
	_DBH32(data);_DBG_("");
	_DBG_("");
}

/*********************************************************************//**
 * @brief		Setup Acceptance Filter Table
 * @param[in]	none
 * @return 		none
 * Note: 		not use Group Standard Frame, just use for Explicit Standard and Extended Frame
 **********************************************************************/

CAN_ERROR CAN_SetupAFTable(void) {
	uint32_t i = 0;
	uint8_t result;
	/* Set up Explicit Standard Frame Format Identifier Section
	 * In this simple test, it has 16 entries ID
	 */
	for (i = 0; i < 16; i++) {
		result = CAN_LoadExplicitEntry(LPC_CAN1, i + 1, STD_ID_FORMAT);
	}
	/* Set up Explicit Extended Frame Format Identifier Section
	 * In this simple test, it has 16 entries ID
	 */
	for (i = 0; i < 16; i++) {
		result = CAN_LoadExplicitEntry(LPC_CAN1, i << 11, EXT_ID_FORMAT);
	}
	return result;
}

/*********************************************************************//**
 * @brief		Initialize transmit and receive message for testing AF
 * 				operation
 * @param[in]	none
 * @return 		none
 **********************************************************************/
void CAN_InitAFMessage(void) {
	/* 1st Message with 11-bit ID which exit in AF Look-up Table */
	AFTxMsg[0].id = 0x08;
	AFTxMsg[0].len = 0x08;
	AFTxMsg[0].type = DATA_FRAME;
	AFTxMsg[0].format = STD_ID_FORMAT;
	AFTxMsg[0].dataA[0] = AFTxMsg[0].dataA[1] = AFTxMsg[0].dataA[2]= AFTxMsg[0].dataA[3]= 0x78;
	AFTxMsg[0].dataB[0] = AFTxMsg[0].dataB[1] = AFTxMsg[0].dataB[2]= AFTxMsg[0].dataB[3]= 0x21;

	/* 2nd Message with 11-bit ID which not exit in AF Look-up Table */
	AFTxMsg[1].id = 0xA0;
	AFTxMsg[1].len = 0x08;
	AFTxMsg[1].type = DATA_FRAME;
	AFTxMsg[1].format = STD_ID_FORMAT;
	AFTxMsg[1].dataA[0] = AFTxMsg[1].dataA[1] = AFTxMsg[1].dataA[2]= AFTxMsg[1].dataA[3]= 0x23;
	AFTxMsg[1].dataB[0] = AFTxMsg[1].dataB[1] = AFTxMsg[1].dataB[2]= AFTxMsg[1].dataB[3]= 0x45;

	/* 3th Message with 29-bit ID which exit in AF Look-up Table */
	AFTxMsg[2].id = (0x08 << 11);
	AFTxMsg[2].len = 0x08;
	AFTxMsg[2].type = DATA_FRAME;
	AFTxMsg[2].format = EXT_ID_FORMAT;
	AFTxMsg[2].dataA[0] = AFTxMsg[2].dataA[1] = AFTxMsg[2].dataA[2]= AFTxMsg[2].dataA[3]= 0x15;
	AFTxMsg[2].dataB[0] = AFTxMsg[2].dataB[1] = AFTxMsg[2].dataB[2]= AFTxMsg[2].dataB[3]= 0x36;

	/* 3th Message with 29-bit ID which exit in AF Look-up Table */
	AFTxMsg[3].id = (0xA0 << 11);
	AFTxMsg[3].len = 0x08;
	AFTxMsg[3].type = DATA_FRAME;
	AFTxMsg[3].format = EXT_ID_FORMAT;
	AFTxMsg[3].dataA[0] = AFTxMsg[3].dataA[1] = AFTxMsg[3].dataA[2]= AFTxMsg[3].dataA[3]= 0x78;
	AFTxMsg[3].dataB[0] = AFTxMsg[3].dataB[1] = AFTxMsg[3].dataB[2]= AFTxMsg[3].dataB[3]= 0x21;

	AFRxMsg[0].id = AFRxMsg[1].id = 0x00;
	AFRxMsg[0].len = AFRxMsg[1].len = 0x00;
	AFRxMsg[0].type = AFRxMsg[1].type = 0x00;
	AFRxMsg[0].format = AFRxMsg[1].format = 0x00;
	AFRxMsg[0].dataA[0] = AFRxMsg[1].dataA[0] = 0x00;
	AFRxMsg[0].dataA[1] = AFRxMsg[1].dataA[1] = 0x00;
	AFRxMsg[0].dataA[2] = AFRxMsg[1].dataA[2] = 0x00;
	AFRxMsg[0].dataA[3] = AFRxMsg[1].dataA[3] = 0x00;
	AFRxMsg[0].dataB[0] = AFRxMsg[1].dataB[0] = 0x00;
	AFRxMsg[0].dataB[1] = AFRxMsg[1].dataB[1] = 0x00;
	AFRxMsg[0].dataB[2] = AFRxMsg[1].dataB[2] = 0x00;
	AFRxMsg[0].dataB[3] = AFRxMsg[1].dataB[3] = 0x00;
}
/*********************************************************************//**
 * @brief		print menu
 * 				operation
 * @param[in]	none
 * @return 		none
 **********************************************************************/
void print_menu(void)
{
	_DBG_(menu);
}

/*********************************************************************//**
							 THE MAIN CAN BODY
 **********************************************************************/
int c_entry(void) { /* Main Program */
	uint32_t result,i;
	CAN_PinCFG_Type CAN1PinStruct;

	NVIC_DeInit();
	NVIC_SCBDeInit();

	/* Configure the NVIC Preemption Priority Bits:
	 * two (2) bits of preemption priority, six (6) bits of sub-priority.
	 * Since the Number of Bits used for Priority Levels is five (5), so the
	 * actual bit number of sub-priority is three (3)
	 */
	NVIC_SetPriorityGrouping(0x06);

	//  Set Vector table offset value
#if (__RAM_MODE__==1)
	NVIC_SetVTOR(0x10000000);
#else
	NVIC_SetVTOR(0x00000000);
#endif

	debug_frmwrk_init();
	print_menu();

	CAN1PinStruct.RD = CAN_RD1_P0_0;
	CAN1PinStruct.TD = CAN_TD1_P0_1;

	PINSEL_ConfigPin((PINSEL_CFG_Type *) (&can_rd1_pin[CAN1PinStruct.RD]));
	PINSEL_ConfigPin((PINSEL_CFG_Type *) (&can_td1_pin[CAN1PinStruct.TD]));

	//Initialize CAN1
	CAN_Init(LPC_CAN1, 125000);
#if CAN_RECEIVE
	//Enable Interrupt
	CAN_IRQCmd(LPC_CAN1, CANINT_RIE, ENABLE);
	CAN_IRQCmd(LPC_CAN1, CANINT_DOIE, ENABLE);

	CAN_SetupCBS(CANINT_RIE, CAN_Callback0);
	CAN_SetupCBS(CANINT_DOIE, CAN_Callback1);

	//Enable CAN Interrupt
	NVIC_EnableIRQ(CAN_IRQn);
	CAN_SetAFMode(LPC_CANAF, CAN_Normal);

	/* Test Acceptance Filter with a simple AF Look-up Table. It just has two sections:
	 * 		- Explicit Standard Frame Format Identifier Section
	 * 		- Explicit Extended Frame Format Identifier Section
	 * We use 4 Message:
	 * 		- 1 Message with 11-bit ID which exit in AF Look-up Table
	 *  	- 1 Message with 11-bit ID which not exit in AF Look-up Table
	 *  	- 1 Message with 29-bit ID which exit in AF Look-up Table
	 *  	- 1 Message with 29-bit ID which not exit in AF Look-up Table
	 *  to check Acceptance filter operation of CAN
	 */
	_DBG_("Test Acceptance Filter function...");
	CAN_SetAFMode(LPC_CANAF, CAN_Normal);
	result = CAN_SetupAFTable(); /* setup AF Look-up Table */
	if (result != CAN_OK) {
		_DBG_("Setup AF: ERROR...");
		while(1);
	}
	else _DBG_("Setup AF: SUCCESSFUL...");_DBG_("");
	_DBG_("Recieving message...");
#else
	CAN_InitAFMessage(); /* initialize Transmit Message */
	for (i = 0; i < TX_BUFFER_SIZE; i++)
	{
		CAN_SendMsg(CAN1, &AFTxMsg[i]);
		PrintMessage(&AFTxMsg[i]);_DBG_("");
		for(result=0;result<10000;result++);
		CANTxCount++;
	}
	_DBG_("Sending finished !!!");
#endif
	while (1);
}

/* With ARM and GHS toolsets, the entry point is main() - this will
 allow the linker to generate wrapper code to setup stacks, allocate
 heap area, and initialize and copy code and data segments. For GNU
 toolsets, the entry point is through __start() in the crt0_gnu.asm
 file, and that startup code will setup stacks and data */
int main(void) {
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
void check_failed(uint8_t *file, uint32_t line) {
	/* User can add his own implementation to report the file name and line number,
	 ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

	/* Infinite loop */
	while (1)
		;
}
#endif
