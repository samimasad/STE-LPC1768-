/**
 * @file	: rit_simple.c
 * @purpose	: A simple RIT driver example
 * @version	: 1.0
 * @date	: 7. May . 2009
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
#include "lpc17xx_rit.h"
#include "lpc17xx_libcfg.h"
#include "lpc17xx_nvic.h"
#include "debug_frmwrk.h"

///************************** PRIVATE MACROS *************************/
#define LED_NUM     8

/************************** PRIVATE VARIABLE ***********************/
const int led_mask[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };
                   /* Number of user LEDs*/
uint8_t menu[]=
	"********************************************************************************\n\r"
	"Hello NXP Semiconductors \n\r"
	" RIT demo \n\r"
	"\t - MCU: LPC17xx \n\r"
	"\t - Core: ARM CORTEX-M3 \n\r"
	"\t - Communicate via: UART0 - 115200 bps \n\r"
	" Use RIT as a timer to generate interrupt to control 8 LEDs display\n\r"
	"********************************************************************************\n\r";

//just use for debugging
LPC_RIT_TypeDef* RITx=LPC_RIT;

/*********************************************************************//**
 * @brief	Main I2S program body
 **********************************************************************/
int c_entry (void) {                       /* Main Program  */
	RIT_CMP_VAL  value;
	int num = -1;
	int dir =  1;

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

	debug_frmwrk_init();
	 _DBG_(menu);

	value.CMPVAL = 0x00100000;
	value.COUNTVAL = 0x00000000;
	value.MASKVAL = 0x00000000;
	RIT_Init(LPC_RIT);
	RIT_TimerConfig(LPC_RIT,&value);
	RIT_TimerClearCmd(LPC_RIT,ENABLE);

	LPC_GPIO2->FIODIR = 0xFF;                 /* LEDs defined as Output */

	for (;;) {                            /* Loop forever  */
		/* Calculate 'num': 0,1,...,LED_NUM-1,LED_NUM-1,...,1,0,0,... */
		num += dir;
		if (num == LED_NUM) { dir = -1; num =  LED_NUM-1; }
		else if   (num < 0) { dir =  1; num =  0; }

		while (!RIT_GetIntStatus(LPC_RIT));
		LPC_GPIO2->FIOPIN |=  led_mask[num];
		_DBG("LED"); _DBD(num);_DBG_(": ON");
		while (!RIT_GetIntStatus(LPC_RIT));
		LPC_GPIO2->FIOPIN &= ~led_mask[num];
		_DBG("LED"); _DBD(num);_DBG_(": OFF");
	}
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
