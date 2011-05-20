/**
 * @file	: Spi.c
 * @purpose	: An example of using CMSIS SpiDev
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

//#include <stdio.h>
//#include <string.h>
#include "LPC17xx.h"
#include "SpiDev.h"
#include "lpc17xx_nvic.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"

//#define __AT25_EXAMPLE__ 		1
#define __SC16IS750_EXAMPLE__ 	1


#if __AT25_EXAMPLE__
#include "spi_AT25.h"
#endif

/************************** PRIVATE MACROS *************************/
// PORT number that /CS pin assigned on
#define CS_PORT_NUM		0
// PIN number that  /CS pin assigned on
#define CS_PIN_NUM		19


#if __AT25_EXAMPLE__
char bufTx[256];
char bufRx[256];

char menu0[] = "Testing AT25 SPI Flash on LPC17xx ...\n\r";
char menu1[] = "Page Offset 0x10: ...";
#endif

#if __SC16IS750_EXAMPLE__

// Define macro used in command when using SPI with SC16IS740
#define SC16IS740_WR_CMD(x)		((uint8_t) (x << 3))
#define SC16IS740_RD_CMD(x)		((uint8_t) ((x << 3) | 0x80))

// Define register address of SC16IS740
#define SC16IS740_IODIR_REG		0x0A
#define SC16IS740_IOSTATE_REG	0x0B
#define SC16IS740_IOCON_REG		0x0E

uint8_t iocon_cfg[2] = {SC16IS740_WR_CMD(SC16IS740_IOCON_REG), 0x00};
uint8_t iodir_cfg[2] = {SC16IS740_WR_CMD(SC16IS740_IODIR_REG), 0xFF};
uint8_t iostate_on[2] = {SC16IS740_WR_CMD(SC16IS740_IOSTATE_REG), 0x00};
uint8_t iostate_off[2] = {SC16IS740_WR_CMD(SC16IS740_IOSTATE_REG), 0xFF};
uint8_t spireadbuf[2];

#endif

LPC_GPIO_TypeDef *GPIO_Cs;

int  retCode;

/*----------------------------------------------------------------------------
  Interface to SpiDev
 *----------------------------------------------------------------------------*/
extern SpiDev_IOB  spi;
       SpiDev_IOB *pSpi;


/************************** PRIVATE FUNCTIONS *************************/
void SPI_SlaveSelectInit (void) ;
void SPI_SlaveSelect (unsigned char ss);
void bufCpy (uint8_t *dest, uint8_t *src, uint32_t len);


/*********************************************************************//**
* @brief 		Initialize CS pin as GPIO function to drive /CS pin
* 				due to definition of CS_PORT_NUM and CS_PORT_NUM
* @param		None
* @return		None
***********************************************************************/
void SPI_SlaveSelectInit (void)
{
	GPIO_SetDir(CS_PORT_NUM, (1<<CS_PIN_NUM), 1);
	GPIO_SetValue(CS_PORT_NUM, (1<<CS_PIN_NUM));
}


/*********************************************************************//**
* @brief 		Drive /CS pin to high level to disable slave device via /CS
* 				pin state
* @param[in]	ss State of SPI Chip Select ouput pin, can be:
* 				- 0: SlaveSelect low
* 				- 1: SlaveSelect high
* @return		None
***********************************************************************/
void SPI_SlaveSelect (unsigned char ss)
{
	if (ss){
		GPIO_SetValue(CS_PORT_NUM, (1<<CS_PIN_NUM));
	}else{
		GPIO_ClearValue(CS_PORT_NUM, (1<<CS_PIN_NUM));
	}
}

/*********************************************************************//**
* @brief 		Buffer copy routine
* @param		None
* @return		None
***********************************************************************/
void bufCpy (uint8_t *dest, uint8_t *src, uint32_t len)
{
	uint8_t *pdest = dest;
	uint8_t *psrc = src;

	while (len--)
	{
		*pdest++ = *psrc++;
	}
}

/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {

	uint32_t delay;

	SystemInit ();                                     /* initialize the clocks */


	/* Expansion statement ----------------------------------------------------- */
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
	/* End of Expansion statement ------------------------------------------------ */


	pSpi = &spi;            /* Select 'spi0' as active communication interface */
	/* Note:
	 * Use own generated SS signal
	 * on the port number defined by CS_PORT_NUM symbol
	 * and the pin number defined by CS_PIN_NUM symbol
	 */
	pSpi->Cfg.SlaveSelect = SpiDev_SSO_DISABLED;
	SPI_SlaveSelectInit();
	pSpi->Init();

#if __AT25_EXAMPLE__
	bufCpy ((uint8_t *)bufTx, (uint8_t *)menu0, sizeof(menu0));
	SPI_MemWrite(0x400, 36,  (unsigned char*)bufTx);
	bufCpy ((uint8_t *)bufTx, (uint8_t *)menu1, sizeof(menu1));
	SPI_MemWrite(0x450, 21,  (unsigned char*)bufTx);
	SPI_MemRead (0x400, 128, (unsigned char*)bufRx);
#endif

#if __SC16IS750_EXAMPLE__
	/* First, send some command to reset SC16IS740 chip via SPI bus interface
	 * note driver /CS pin to low state before transferring by CS_Enable() function
	 */
	SPI_SlaveSelect(0);
	pSpi->BufTxRx ((void *)&iocon_cfg, (void *)&spireadbuf, sizeof (iocon_cfg));
	SPI_SlaveSelect(1);
	for (delay = 0; delay < 1000000; delay++);
	SPI_SlaveSelect(0);
	pSpi->BufTxRx ((void *)&iodir_cfg, (void *)&spireadbuf, sizeof (iodir_cfg));
	SPI_SlaveSelect(1);

	// main loop
	while (1)
	{
		for (delay = 0; delay < 1000000; delay++);
		SPI_SlaveSelect(0);
		pSpi->BufTxRx ((void *)&iostate_on, (void *)&spireadbuf, sizeof (iostate_on));
		SPI_SlaveSelect(1);
		for (delay = 0; delay < 1000000; delay++);
		SPI_SlaveSelect(0);
		pSpi->BufTxRx ((void *)&iostate_off, (void *)&spireadbuf, sizeof (iostate_off));
		SPI_SlaveSelect(1);
	}
#endif

	while (1) ;
}



int c_entry(void)
{
	return main();
}
