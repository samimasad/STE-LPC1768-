/**
 * @file	: master.c
 * @purpose	: This example uses I2C as master device to transfer data
 * 				from/to I2C slave device
 *				- First, the master transmit to slave a number of data bytes
				- Then, the master receive a number of data bytes from slave.
				- Finally, the master send two bytes to slave, send repeat start
				immediately and receive from slave a number of data byte.
				- Using in polling mode.
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
#include "lpc17xx_i2c.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_libcfg_default.h"
#include "lpc17xx_nvic.h"
#include "lpc17xx_pinsel.h"
#include "lcd_i2c.h"
#include "FreeRTOS.h"
#include "queue.h"
#include "sensor.h"
/************************** PRIVATE MACROS *************************/
/** Used I2C device as slave definition */
#define USEDI2CDEV_M		2
/** Own Slave address in Slave I2C device */
#define I2CDEV_S_ADDR	(0x76>>1)

/** Max buffer length */
#define BUFFER_SIZE			33

#if (USEDI2CDEV_M == 0)
#define I2CDEV_M LPC_I2C0
#elif (USEDI2CDEV_M == 2)
#define I2CDEV_M LPC_I2C2
#else
#error "Master I2C device not defined!"
#endif


/************************** PRIVATE TYPES *************************/
/************************** PRIVATE VARIABLES *************************/

/** These global variables below used in interrupt mode - Slave device ---------------------------------*/
uint8_t Master_Buf[BUFFER_SIZE];
uint8_t master_test[2];
I2C_M_SETUP_Type transferMCfg;



/************************** PRIVATE FUNCTIONS *************************/

void Error_Loop_M(uint8_t ErrorCode);




/*********************************************************************//**
 * @brief		Initialize buffer
 * @param[in]	type:
 * 				- 0: Initialize Master_Buf with increment value from 0
 * 					Fill all member in Slave_Buf with 0
 * 				- 1: Initialize Slave_Buf with increment value from 0
 * 					Fill all member in Master_Buf with 0
 *
 * @return 		None
 **********************************************************************/
void Buffer_Init(uint8_t type)
{
	uint8_t i;

	if (type)
	{
		for (i = 0; i < BUFFER_SIZE; i++) {
			Master_Buf[i] = i;
		}
	}
	else
	{
		for (i = 0; i < BUFFER_SIZE; i++) {
			Master_Buf[i] = 0;
		}
	}
}

/*********************************************************************//**
 * @brief		A subroutine that will be called if there's any error
 * 				on I2C operation (slave)
 * @param[in]	ErrorCode Error Code Input
 * @return 		None
 **********************************************************************/
void Error_Loop_S(uint8_t ErrorCode)
{
	/*
	 * Insert your code here...
	 */
	while(1);
}


/*********************************************************************//**
 * @brief	Main I2C master and slave program body
 **********************************************************************/
int lcd_entry(void)
{

	LCD_Init();

	LCD_ClearScreen();
	char buf[32];

	//LCD_PrintText("Sen300");
	extern xQueueHandle queuToLCD ;
	sensor_readings sensor_data;
	LCD_ClearScreen();
    //vTaskDelete(NULL);
    /* Loop forever */
        while(1)
        	{
        	xQueueReceive(queuToLCD,&sensor_data,portMAX_DELAY);
        	//LCD_ClearScreen();
        	 //vTaskDelay(5);
        	 //LCD_PrintText("Testing");
        	sprintf( buf, "Sen T:%.2f H:%.2f L:%.2f", sensor_data.temp,sensor_data.hum,sensor_data.lux );
        	LCD_PrintText(buf);
        	vTaskDelay(10);
        	}
    return 1;
}

int LCD_Init(void)
{

	PINSEL_CFG_Type PinCfg;

	uint32_t tempp;
	uint8_t *pdat;

	/* I2C block ------------------------------------------------------------------- */

	/*
	 * Init I2C pin connect
	 */
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	#if ((USEDI2CDEV_M == 0))
	PinCfg.Funcnum = 1;
	PinCfg.Pinnum = 27;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 28;
	PINSEL_ConfigPin(&PinCfg);
	#endif
	#if ((USEDI2CDEV_M == 2))
	PinCfg.Funcnum = 2;
	PinCfg.Pinnum = 10;
	PinCfg.Portnum = 0;
	PINSEL_ConfigPin(&PinCfg);
	PinCfg.Pinnum = 11;
	PINSEL_ConfigPin(&PinCfg);
	#endif

	// Initialize Slave I2C peripheral
	I2C_Init(I2CDEV_M, 100000);

	/* Enable Slave I2C operation */
	I2C_Cmd(I2CDEV_M, ENABLE);



	/* Initialize buffer */
	//Buffer_Init(1);
	Master_Buf[0] = 0x00 ;
	Master_Buf[1] = 0x20 ;
	Master_Buf[2] = 0x1D ;
	Master_Buf[3] = 0x0D ;


	/* Start I2C slave device first */
	transferMCfg.sl_addr7bit = I2CDEV_S_ADDR;
	transferMCfg.tx_data = Master_Buf;
	transferMCfg.tx_length = 4;
	transferMCfg.rx_data = NULL;
	transferMCfg.rx_length = 0;
	transferMCfg.retransmissions_max = 3;
	I2C_MasterTransferData(I2CDEV_M, &transferMCfg, I2C_TRANSFER_POLLING);


	master_test [0] = 0x0C;			          /* D: Display on, C: curser off, B: blink off */
	transferMCfg.sl_addr7bit = I2CDEV_S_ADDR;
	transferMCfg.tx_data = master_test;
	transferMCfg.tx_length = 1;
	transferMCfg.rx_data = NULL;
	transferMCfg.rx_length = 0;
	transferMCfg.retransmissions_max = 3;
	I2C_MasterTransferData(I2CDEV_M, &transferMCfg, I2C_TRANSFER_POLLING);


#if 0
	Master_Buf[0] = 0x40 ;
	Master_Buf[1] = ' ' + 0x80 ;
	Master_Buf[2] = ' ' + 0x80 ;
	Master_Buf[3] = ' ' + 0x80 ;
	Master_Buf[4] = ' ' + 0x80 ;
	transferMCfg.tx_data = Master_Buf;
	transferMCfg.tx_length = 5;

	I2C_MasterTransferData(I2CDEV_M, &transferMCfg, I2C_TRANSFER_POLLING);
#endif
	return 0;

}

int LCD_ClearScreen(void)
{
	const uint8_t screen_size = 32;
	uint8_t counter = 0;




	master_test [0] = 0x0C;			          /* D: Display on, C: curser off, B: blink off */
	master_test [1] = 0x01;			          /* D: Display on, C: curser off, B: blink off */
	transferMCfg.sl_addr7bit = I2CDEV_S_ADDR;
	transferMCfg.tx_data = master_test;
	transferMCfg.tx_length = 2;
	transferMCfg.rx_data = NULL;
	transferMCfg.rx_length = 0;
	transferMCfg.retransmissions_max = 3;
	I2C_MasterTransferData(I2CDEV_M, &transferMCfg, I2C_TRANSFER_POLLING);


	master_test [0] = 0x0C;			          /* D: Display on, C: curser off, B: blink off */
	master_test [1] = 0x02;			          /* D: Display on, C: curser off, B: blink off */
	transferMCfg.sl_addr7bit = I2CDEV_S_ADDR;
	transferMCfg.tx_data = master_test;
	transferMCfg.tx_length = 2;
	transferMCfg.rx_data = NULL;
	transferMCfg.rx_length = 0;
	transferMCfg.retransmissions_max = 3;
	I2C_MasterTransferData(I2CDEV_M, &transferMCfg, I2C_TRANSFER_POLLING);




	Master_Buf[0] = 0x40 ;
	//Master_Buf[0] = 0x02 ;
	for(counter=1;counter<=screen_size;counter++)
		Master_Buf[counter] = ' ' + 0x80 ;

	transferMCfg.sl_addr7bit = I2CDEV_S_ADDR;
	transferMCfg.tx_data = Master_Buf;
	transferMCfg.tx_length = screen_size+1;
	transferMCfg.rx_data = NULL;
	transferMCfg.rx_length = 0;
	transferMCfg.retransmissions_max = 3;

	I2C_MasterTransferData(I2CDEV_M, &transferMCfg, I2C_TRANSFER_POLLING);





}
int LCD_PrintText(uint8_t *txt)
{
	uint8_t txt_length;
	txt_length = strlen(txt);
	//if the text is bigger than 32 chsracter just use the first 32 and ignore the rest
	if (txt_length > 32)
		txt_length = 32 ;
	Master_Buf[0] = 0x40 ;
	uint8_t counter = 0;
	for(counter=1;counter<=txt_length;counter++)
		Master_Buf[counter] = txt[counter-1] + 0x80 ;


	master_test [0] = 0x0C;			          /* D: Display on, C: curser off, B: blink off */
	master_test [1] = 0x02;			          /* D: Display on, C: curser off, B: blink off */
	transferMCfg.sl_addr7bit = I2CDEV_S_ADDR;
	transferMCfg.tx_data = master_test;
	transferMCfg.tx_length = 2;
	transferMCfg.rx_data = NULL;
	transferMCfg.rx_length = 0;
	transferMCfg.retransmissions_max = 3;
	I2C_MasterTransferData(I2CDEV_M, &transferMCfg, I2C_TRANSFER_POLLING);


	transferMCfg.tx_data = Master_Buf;
	transferMCfg.tx_length = txt_length+1;
	transferMCfg.sl_addr7bit = I2CDEV_S_ADDR;
	transferMCfg.tx_data = Master_Buf;
	transferMCfg.retransmissions_max = 3;


	I2C_MasterTransferData(I2CDEV_M, &transferMCfg, I2C_TRANSFER_POLLING);


}

/* With ARM and GHS toolsets, the entry point is main() - this will
   allow the linker to generate wrapper code to setup stacks, allocate
   heap area, and initialize and copy code and data segments. For GNU
   toolsets, the entry point is through __start() in the crt0_gnu.asm
   file, and that startup code will setup stacks and data */
int vLCDTask( void *pvParameters )
{
	console_uart_sendString("Starting vLCDTask\n\r");
    return lcd_entry();
}




