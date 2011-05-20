/**
 * @file	: Uart.c
 * @purpose	: An example of using CMSIS UartDev
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

/* Original included header files ------------------------------ */
#include "LPC17xx.h"
#include "UartDev.h"
#include "lpc17xx_nvic.h"
//#include <string.h>
//#include <stdio.h>


char bufTx[20];
char bufRx[20];

int  retCode;

/*----------------------------------------------------------------------------
  Interface to UartDev
 *----------------------------------------------------------------------------*/
extern UartDev_IOB  uart0;
       UartDev_IOB *pUart;

/*----------------------------------------------------------------------------
  SendChar
  Write character to Serial Port.
 *----------------------------------------------------------------------------*/
int SendChar (int ch)  {
  int size = 1;

  retCode =  pUart->BufTx ((void *)&ch, &size, UartDev_FLAG_BLOCKING);
  return (ch);
}


/*----------------------------------------------------------------------------
  SendString
  Write a const string to Serial Port.
 *----------------------------------------------------------------------------*/
void SendString (const char *str)
{
	while (*str)
	{
		SendChar((int)*str++);
	}
}

/*----------------------------------------------------------------------------
  GetKey
  Read character to Serial Port.
 *----------------------------------------------------------------------------*/
int GetKey (void)  {
  int ch;
  int size = 1;


  retCode =  pUart->BufRx ((void *)&ch, &size, UartDev_FLAG_BLOCKING);
  if (size == 0)
    ch = -1;

  return (ch);
}


/*----------------------------------------------------------------------------
  MAIN function
 *----------------------------------------------------------------------------*/
int main (void) {
  int i;
  int sizeRx, sizeTx;

  SystemInit ();                                  /* initialize the clocks */


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


  pUart = &uart0;          /* Select 'uart0' as active communication interface. */
  pUart->Init();

  SendString ("CMSIS Serial Device Example\r\n\r\n");

  while (1) {
    unsigned char c;

    SendString ("\r\nPress a key. ");
    c = GetKey();
    SendString ("\r\n");
    SendString ("You pressed: \r\n\r\n");
    SendChar(c);
    SendString ("\r\n");


    while (1) {
      for (i = 0; i < 10000; i++);
      sizeRx = 10;
      retCode =  pUart->BufRx ((void *)&bufRx[0], &sizeRx, UartDev_FLAG_NONBLOCKING);
      for (i = 0; i < sizeRx; i++) bufTx[i] = bufRx[i];
      sizeTx = sizeRx;
      retCode =  pUart->BufTx ((void *)&bufTx[0], &sizeTx, UartDev_FLAG_NONBLOCKING);
      if (sizeRx > 0) {
        SendString("\n\r");
      }
    }
  }
}


int c_entry(void)
{
    return main();
}
