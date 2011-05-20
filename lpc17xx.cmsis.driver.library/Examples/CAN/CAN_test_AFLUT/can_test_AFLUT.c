/**
 * @file	: can_test_AFLUT.c
 * @purpose	: This example used to test Acceptance Filter mode
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

/************************** PRIVATE VARIABLES *************************/
uint8_t menu[]=
	"********************************************************************************\n\r"
	"Hello NXP Semiconductors \n\r"
	"CAN demo \n\r"
	"\t - MCU: LPC17xx \n\r"
	"\t - Core: ARM CORTEX-M3 \n\r"
	"\t - Communicate via: UART0 - 115200 bps \n\r"
	"Use 2 CAN peripherals: CAN1&CAN2 to transfer data\n\r"
	"This example tests full Acceptance Filter operation \n\r"
	"It supports FullCAN and Group ID Frame Format \n\r"
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

#define MAX_ENTRY			512
#define CAN_TX_MSG_CNT		10
#define CAN_RX_MSG_CNT		5

CAN_MSG_Type AFTxMsg[CAN_TX_MSG_CNT], AFRxMsg[CAN_RX_MSG_CNT]; //messages for test Acceptance Filter mode
uint32_t CANRxCount = 0, CANTxCount = 0;
uint32_t CANErrCount = 0;

AF_SectionDef AFTable;
FullCAN_Entry FullCAN_Table[6];
SFF_Entry SFF_Table[6];
SFF_GPR_Entry SFF_GPR_Table[6];
EFF_Entry EFF_Table[6];
EFF_GPR_Entry EFF_GPR_Table[6];

/************************** PRIVATE FUNCTIONS *************************/
void CAN_IRQHandler(void);
void CAN_Callback0();
void CAN_Callback1();
void CAN_Callback2();

void CAN_PinCfg(void);
void CAN_InitMessage(void);
void CAN_SetupAFTable(void);
void CAN_InitAFMessage(void);
void PrintMessage(CAN_MSG_Type* msg);
void print_menu(void);

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
 * @brief		CAN_IRQ Handler
 * @param[in]	none
 * @return 		none
 **********************************************************************/
void CAN_IRQHandler(void)
{
	CAN_IntHandler(LPC_CAN2);
}

/*********************************************************************//**
 * @brief		FullCAN Recevie Interrupt call-back function
 * @param[in]	none
 * @return 		none
 **********************************************************************/
void CAN_Callback0()
{
	//check whether has received FullCAN Object or not
	if (LPC_CANAF->FCANIC0 != 0 || LPC_CANAF->FCANIC1 != 0) {
		//read received FullCAN Object in Object Section
		FCAN_ReadObj(LPC_CANAF, &AFRxMsg[CANRxCount]);
		CANRxCount++;
	}
}

/*********************************************************************//**
 * @brief		Recevie Message Interrupt call-back function
 * @param[in]	none
 * @return 		none
 **********************************************************************/
void CAN_Callback1()
{
	CAN_ReceiveMsg(LPC_CAN2, &AFRxMsg[CANRxCount]);
	CANRxCount++;

}

/*********************************************************************//**
 * @brief		Data overrun interrupt call-back function
 * @param[in]	none
 * @return 		none
 **********************************************************************/
void CAN_Callback2()
{
	_DBG_("Data overrun has occurred!");
}

/*********************************************************************//**
 * @brief		Init AF-Look Up Table Sections entry value
 * 				We setup entries for 5 sections:
 * 				- 6 entries for FullCAN Frame Format Section
 * 				- 6 entries for Explicit Standard ID Frame Format Section
 * 				- 6 entries for Group of Standard ID Frame Format Section
 * 				- 6 entries for Explicit Extended ID Frame Format Section
 * 				- 6 entries for Group of Extended ID Frame Format Section
 * @param[in]	none
 * @return 		none
 **********************************************************************/
void CAN_SetupAFTable(void) {
	FullCAN_Table[0].controller = CAN2_CTRL;
	FullCAN_Table[0].disable = MSG_ENABLE;
	FullCAN_Table[0].id_11 = 0x01;
	FullCAN_Table[1].controller = CAN2_CTRL;
	FullCAN_Table[1].disable = MSG_ENABLE;
	FullCAN_Table[1].id_11 = 0x02;
	FullCAN_Table[2].controller = CAN2_CTRL;
	FullCAN_Table[2].disable = MSG_ENABLE;
	FullCAN_Table[2].id_11 = 0x03;
	FullCAN_Table[3].controller = CAN2_CTRL;
	FullCAN_Table[3].disable = MSG_ENABLE;
	FullCAN_Table[3].id_11 = 0x06;
	FullCAN_Table[4].controller = CAN2_CTRL;
	FullCAN_Table[4].disable = MSG_ENABLE;
	FullCAN_Table[4].id_11 = 0x0C;
	FullCAN_Table[5].controller = CAN2_CTRL;
	FullCAN_Table[5].disable = MSG_ENABLE;
	FullCAN_Table[5].id_11 = 0x0D;

	SFF_Table[0].controller = CAN2_CTRL;
	SFF_Table[0].disable = MSG_ENABLE;
	SFF_Table[0].id_11 = 0x08;
	SFF_Table[1].controller = CAN2_CTRL;
	SFF_Table[1].disable = MSG_ENABLE;
	SFF_Table[1].id_11 = 0x09;
	SFF_Table[2].controller = CAN2_CTRL;
	SFF_Table[2].disable = MSG_ENABLE;
	SFF_Table[2].id_11 = 0x0A;
	SFF_Table[3].controller = CAN2_CTRL;
	SFF_Table[3].disable = MSG_ENABLE;
	SFF_Table[3].id_11 = 0x0B;
	SFF_Table[4].controller = CAN2_CTRL;
	SFF_Table[4].disable = MSG_ENABLE;
	SFF_Table[4].id_11 = 0x0E;
	SFF_Table[5].controller = CAN2_CTRL;
	SFF_Table[5].disable = MSG_ENABLE;
	SFF_Table[5].id_11 = 0x0F;

	SFF_GPR_Table[0].controller1 = SFF_GPR_Table[0].controller2 = CAN2_CTRL;
	SFF_GPR_Table[0].disable1 = SFF_GPR_Table[0].disable2 = MSG_ENABLE;
	SFF_GPR_Table[0].lowerID = 0x10;
	SFF_GPR_Table[0].upperID = 0x20;
	SFF_GPR_Table[1].controller1 = SFF_GPR_Table[1].controller2 = CAN2_CTRL;
	SFF_GPR_Table[1].disable1 = SFF_GPR_Table[1].disable2 = MSG_ENABLE;
	SFF_GPR_Table[1].lowerID = 0x20;
	SFF_GPR_Table[1].upperID = 0x25;
	SFF_GPR_Table[2].controller1 = SFF_GPR_Table[2].controller2 = CAN2_CTRL;
	SFF_GPR_Table[2].disable1 = SFF_GPR_Table[2].disable2 = MSG_ENABLE;
	SFF_GPR_Table[2].lowerID = 0x30;
	SFF_GPR_Table[2].upperID = 0x40;
	SFF_GPR_Table[3].controller1 = SFF_GPR_Table[3].controller2 = CAN2_CTRL;
	SFF_GPR_Table[3].disable1 = SFF_GPR_Table[3].disable2 = MSG_ENABLE;
	SFF_GPR_Table[3].lowerID = 0x40;
	SFF_GPR_Table[3].upperID = 0x50;
	SFF_GPR_Table[4].controller1 = SFF_GPR_Table[4].controller2 = CAN2_CTRL;
	SFF_GPR_Table[4].disable1 = SFF_GPR_Table[4].disable2 = MSG_ENABLE;
	SFF_GPR_Table[4].lowerID = 0x50;
	SFF_GPR_Table[4].upperID = 0x60;
	SFF_GPR_Table[5].controller1 = SFF_GPR_Table[5].controller2 = CAN2_CTRL;
	SFF_GPR_Table[5].disable1 = SFF_GPR_Table[5].disable2 = MSG_ENABLE;
	SFF_GPR_Table[5].lowerID = 0x60;
	SFF_GPR_Table[5].upperID = 0x70;

	EFF_Table[0].controller = CAN2_CTRL;
	EFF_Table[0].ID_29 = (1 << 11);
	EFF_Table[1].controller = CAN2_CTRL;
	EFF_Table[1].ID_29 = (2 << 11);
	EFF_Table[2].controller = CAN2_CTRL;
	EFF_Table[2].ID_29 = (3 << 11);
	EFF_Table[3].controller = CAN2_CTRL;
	EFF_Table[3].ID_29 = (4 << 11);
	EFF_Table[4].controller = CAN2_CTRL;
	EFF_Table[4].ID_29 = (0x0e << 11);
	EFF_Table[5].controller = CAN2_CTRL;
	EFF_Table[5].ID_29 = (0x0f << 11);

	EFF_GPR_Table[0].controller1 = EFF_GPR_Table[0].controller2 = CAN2_CTRL;
	EFF_GPR_Table[0].lowerEID = (5 << 11);
	EFF_GPR_Table[0].upperEID = (6 << 11);
	EFF_GPR_Table[1].controller1 = EFF_GPR_Table[1].controller2 = CAN2_CTRL;
	EFF_GPR_Table[1].lowerEID = (7 << 11);
	EFF_GPR_Table[1].upperEID = (8 << 11);
	EFF_GPR_Table[2].controller1 = EFF_GPR_Table[2].controller2 = CAN2_CTRL;
	EFF_GPR_Table[2].lowerEID = (9 << 11);
	EFF_GPR_Table[2].upperEID = (0x0a << 11);
	EFF_GPR_Table[3].controller1 = EFF_GPR_Table[3].controller2 = CAN2_CTRL;
	EFF_GPR_Table[3].lowerEID = (0x0b << 11);
	EFF_GPR_Table[3].upperEID = (0x0c << 11);
	EFF_GPR_Table[4].controller1 = EFF_GPR_Table[4].controller2 = CAN2_CTRL;
	EFF_GPR_Table[4].lowerEID = (0x11 << 11);
	EFF_GPR_Table[4].upperEID = (0x12 << 11);
	EFF_GPR_Table[5].controller1 = EFF_GPR_Table[5].controller2 = CAN2_CTRL;
	EFF_GPR_Table[5].lowerEID = (0x13 << 11);
	EFF_GPR_Table[5].upperEID = (0x14 << 11);

	AFTable.FullCAN_Sec = &FullCAN_Table[0];
	AFTable.FC_NumEntry = 6;
	AFTable.SFF_Sec = &SFF_Table[0];
	AFTable.SFF_NumEntry = 6;
	AFTable.SFF_GPR_Sec = &SFF_GPR_Table[0];
	AFTable.SFF_GPR_NumEntry = 6;
	AFTable.EFF_Sec = &EFF_Table[0];
	AFTable.EFF_NumEntry = 6;
	AFTable.EFF_GPR_Sec = &EFF_GPR_Table[0];
	AFTable.EFF_GPR_NumEntry = 6;
}

/*********************************************************************//**
 * @brief		Init Transmit Message
 * 				We use 10 message to test Acceptance Filter operation, include:
 * 				- 5 messages that ID exit in 5 AF Sections -> they will be receive
 * 				- 5 messages that ID not exit in 5 AF Sections -> they will be ignored
 * @param[in]	none
 * @return 		none
 **********************************************************************/
void CAN_InitAFMessage(void) {
	/* 1st Message with 11-bit ID which exit in AF Look-up Table in FullCAN Section */
	AFTxMsg[0].id = 0x01;
	AFTxMsg[0].len = 0x08;
	AFTxMsg[0].type = DATA_FRAME;
	AFTxMsg[0].format = STD_ID_FORMAT;
	AFTxMsg[0].dataA[0] = AFTxMsg[0].dataA[1] = AFTxMsg[0].dataA[2]= AFTxMsg[0].dataA[3]= 0x78;
	AFTxMsg[0].dataB[0] = AFTxMsg[0].dataB[1] = AFTxMsg[0].dataB[2]= AFTxMsg[0].dataB[3]= 0x21;

	/* 2nd Message with 11-bit ID which not exit in AF Look-up Table */
	AFTxMsg[1].id = 0x04;
	AFTxMsg[1].len = 0x08;
	AFTxMsg[1].type = DATA_FRAME;
	AFTxMsg[1].format = STD_ID_FORMAT;
	AFTxMsg[1].dataA[0] = AFTxMsg[1].dataA[1] = AFTxMsg[1].dataA[2]= AFTxMsg[1].dataA[3]= 0x23;
	AFTxMsg[1].dataB[0] = AFTxMsg[1].dataB[1] = AFTxMsg[1].dataB[2]= AFTxMsg[1].dataB[3]= 0x45;

	/* 3th Message with 11-bit ID which exit in AF Look-up Table in SFF Section*/
	AFTxMsg[2].id = 0x08;
	AFTxMsg[2].len = 0x08;
	AFTxMsg[2].type = DATA_FRAME;
	AFTxMsg[2].format = STD_ID_FORMAT;
	AFTxMsg[2].dataA[0] = AFTxMsg[2].dataA[1] = AFTxMsg[2].dataA[2]= AFTxMsg[2].dataA[3]= 0x15;
	AFTxMsg[2].dataB[0] = AFTxMsg[2].dataB[1] = AFTxMsg[2].dataB[2]= AFTxMsg[2].dataB[3]= 0x36;

	/* 4th Message with 11-bit ID which not exit in AF Look-up Table */
	AFTxMsg[3].id = 0x05;
	AFTxMsg[3].len = 0x08;
	AFTxMsg[3].type = DATA_FRAME;
	AFTxMsg[3].format = STD_ID_FORMAT;
	AFTxMsg[3].dataA[0] = AFTxMsg[3].dataA[1] = AFTxMsg[3].dataA[2]= AFTxMsg[3].dataA[3]= 0x78;
	AFTxMsg[3].dataB[0] = AFTxMsg[3].dataB[1] = AFTxMsg[3].dataB[2]= AFTxMsg[3].dataB[3]= 0x21;

	/* 5th Message with 11-bit ID which exit in AF Look-up Table in Group SFF Section*/
	AFTxMsg[4].id = 0x15;
	AFTxMsg[4].len = 0x08;
	AFTxMsg[4].type = DATA_FRAME;
	AFTxMsg[4].format = STD_ID_FORMAT;
	AFTxMsg[4].dataA[0] = AFTxMsg[4].dataA[1] = AFTxMsg[4].dataA[2]= AFTxMsg[4].dataA[3]= 0x65;
	AFTxMsg[4].dataB[0] = AFTxMsg[4].dataB[1] = AFTxMsg[4].dataB[2]= AFTxMsg[4].dataB[3]= 0x37;

	/* 6th Message with 11-bit ID which not exit in AF Look-up Table */
	AFTxMsg[5].id = 0x26;
	AFTxMsg[5].len = 0x08;
	AFTxMsg[5].type = DATA_FRAME;
	AFTxMsg[5].format = STD_ID_FORMAT;
	AFTxMsg[5].dataA[0] = AFTxMsg[5].dataA[1] = AFTxMsg[5].dataA[2]= AFTxMsg[5].dataA[3]= 0x76;
	AFTxMsg[5].dataB[0] = AFTxMsg[5].dataB[1] = AFTxMsg[5].dataB[2]= AFTxMsg[5].dataB[3]= 0x32;

	/* 7th Message with 11-bit ID which exit in AF Look-up Table in EFF Section */
	AFTxMsg[6].id = (3 << 11); //0x00001800
	AFTxMsg[6].len = 0x08;
	AFTxMsg[6].type = DATA_FRAME;
	AFTxMsg[6].format = EXT_ID_FORMAT;
	AFTxMsg[6].dataA[0] = AFTxMsg[6].dataA[1] = AFTxMsg[6].dataA[2]= AFTxMsg[6].dataA[3]= 0x45;
	AFTxMsg[6].dataB[0] = AFTxMsg[6].dataB[1] = AFTxMsg[6].dataB[2]= AFTxMsg[6].dataB[3]= 0x87;

	/* 8th Message with 11-bit ID which not exit in AF Look-up Table */
	AFTxMsg[7].id = (3 << 11) + 0x05; //0x00001801
	AFTxMsg[7].len = 0x08;
	AFTxMsg[7].type = DATA_FRAME;
	AFTxMsg[7].format = EXT_ID_FORMAT;
	AFTxMsg[7].dataA[0] = AFTxMsg[7].dataA[1] = AFTxMsg[7].dataA[2]= AFTxMsg[7].dataA[3]= 0x78;
	AFTxMsg[7].dataB[0] = AFTxMsg[7].dataB[1] = AFTxMsg[7].dataB[2]= AFTxMsg[7].dataB[3]= 0x21;

	/* 9th Message with 11-bit ID which exit in AF Look-up Table in Group of EFF Section*/
	AFTxMsg[8].id = (9 << 11) + 0x01; //0x00004801
	AFTxMsg[8].len = 0x08;
	AFTxMsg[8].type = DATA_FRAME;
	AFTxMsg[8].format = EXT_ID_FORMAT;
	AFTxMsg[8].dataA[0] = AFTxMsg[8].dataA[1] = AFTxMsg[8].dataA[2]= AFTxMsg[8].dataA[3]= 0x52;
	AFTxMsg[8].dataB[0] = AFTxMsg[8].dataB[1] = AFTxMsg[8].dataB[2]= AFTxMsg[8].dataB[3]= 0x06;

	/* 10th Message with 11-bit ID which not exit in AF Look-up Table */
	AFTxMsg[9].id = (0x0A << 11) + 0x01; //0x00005001
	AFTxMsg[9].len = 0x08;
	AFTxMsg[9].type = DATA_FRAME;
	AFTxMsg[9].format = EXT_ID_FORMAT;
	AFTxMsg[9].dataA[0] = AFTxMsg[9].dataA[1] = AFTxMsg[9].dataA[2]= AFTxMsg[9].dataA[3]= 0x85;
	AFTxMsg[9].dataB[0] = AFTxMsg[9].dataB[1] = AFTxMsg[9].dataB[2]= AFTxMsg[9].dataB[3]= 0x27;
}
/*********************************************************************//**
 * @brief		print menu
 * @param[in]	none
 * @return 		none
 **********************************************************************/
void print_menu()
{
	_DBG_(menu);
}
/*********************************************************************//**
							 THE MAIN CAN BODY
 **********************************************************************/
int c_entry(void) { /* Main Program */
	uint32_t i;
	uint32_t cnt;
	CAN_ERROR error;
	CAN_PinCFG_Type CAN1PinStruct, CAN2PinStruct;

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

	/* Pin Configuration */
	CAN1PinStruct.RD = CAN_RD1_P0_0;
	CAN1PinStruct.TD = CAN_TD1_P0_1;
	CAN2PinStruct.RD = CAN_RD2_P2_7;
	CAN2PinStruct.TD = CAN_TD2_P2_8;

	PINSEL_ConfigPin((PINSEL_CFG_Type *) (&can_rd1_pin[CAN1PinStruct.RD]));
	PINSEL_ConfigPin((PINSEL_CFG_Type *) (&can_td1_pin[CAN1PinStruct.TD]));
	PINSEL_ConfigPin((PINSEL_CFG_Type *) (&can_rd2_pin[CAN2PinStruct.RD]));
	PINSEL_ConfigPin((PINSEL_CFG_Type *) (&can_td2_pin[CAN2PinStruct.TD]));

	//Initialize CAN1 & CAN2
	CAN_Init(LPC_CAN1, 125000);
	CAN_Init(LPC_CAN2, 125000);

	//Enable Receive Interrupt
	CAN_IRQCmd(LPC_CAN2, CANINT_FCE, ENABLE);
	CAN_IRQCmd(LPC_CAN2, CANINT_RIE, ENABLE);
	CAN_IRQCmd(LPC_CAN2, CANINT_DOIE, ENABLE);

	CAN_SetupCBS(CANINT_FCE,  CAN_Callback0);
	CAN_SetupCBS(CANINT_RIE,  CAN_Callback1);
	CAN_SetupCBS(CANINT_DOIE, CAN_Callback2);

	//Enable CAN Interrupt
	NVIC_EnableIRQ(CAN_IRQn);

	/* Test Acceptance Filter with a full AF Look-up Table. It has 5 sections:
	 * 		- FullCAN Identifier Frame Format
	 * 		- Explicit Standard Frame Format Identifier Section
	 * 		- Group of Standard Frame Format Identifier Section
	 * 		- Explicit Extended Frame Format Identifier Section
	 * 		- Group of Extended Frame Format Identifier Section
	 * We use 10 Message to check Acceptance filter operation of CAN, they has:
	 * 		- 5 message has ID exit in 5 sections
	 * 		- 5 message has ID not exit in AF Lookup Table
	 * Note that: FullCAN Object must be read from FullCAN Object Section next to AFLUT
	 */
	/*-------------------------Init Message & AF Look-up Table------------------------*/

	_DBG_("Test Acceptance Filter function...");
	_DBG_("Press '1' to initialize message and AF Loop-up Table...");_DBG_("");
	while(_DG !='1');
	CAN_InitAFMessage(); /* initialize Transmit Message */
	_DBG_("Init message finished!!!");
	CAN_SetupAFTable(); /* initialize AF Look-up Table sections*/
	error = CAN_SetupAFLUT(LPC_CANAF,&AFTable); /* install AF Look-up Table */
	if (error != CAN_OK) {
		_DBG_("Setup AF: ERROR...");
		while (1); // AF Table has error
	}
	else _DBG_("Setup AF: SUCCESSFUL!!!");_DBG_("");


	/*-------------------------Send messages------------------------*/
	_DBG_("Press '2' to start CAN transferring operation...");_DBG_("");
	while(_DG !='2');
	for (i = 0; i < CAN_TX_MSG_CNT; i++) {
		CAN_SendMsg(LPC_CAN1, &AFTxMsg[i]);
		PrintMessage(&AFTxMsg[i]);_DBG_("");
		for(cnt=0;cnt<10000;cnt++); //transmit delay
		CANTxCount++;
	}
	_DBG_("Sending finished !!!");


	/*-------------------------Receive messages------------------------*/
	_DBG_("Press '3' to display received messages...");_DBG_("");
	while(_DG !='3');
	for (i = 0; i < CAN_RX_MSG_CNT; i++) {
		PrintMessage(&AFRxMsg[i]);_DBG_("");
	}
	_DBG_("Demo terminal !!!");

	CAN_DeInit(LPC_CAN1);
	CAN_DeInit(LPC_CAN2);
	while (1);
	return 0;
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
