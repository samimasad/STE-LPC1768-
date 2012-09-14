#include "uart.h"
#include <stdint.h>
#include "usbhost_lpc17xx.h" //just for accessing the debug UART

void vZigbeeTask( void *pvParameters )
{
	uint8_t output[100] ;
	uint8_t length = 0 ;

	PRINT_Log("vZigbeeTask Starting");
	//quick check to see if we have communication or not
	UART_Init_Zigbee(9600);
	
	//lets try to read the version number of the Zigbee

	//wait for one second
	vTaskDelay(1000);
	PRINT_Log("vZigbeeTask Sending +++\n");
	//UART_Printf_Zigbee("+++");
	UART_PrintChar_Zigbee('+');
	vTaskDelay(50);
	UART_PrintChar_Zigbee('+');
	vTaskDelay(50);
	UART_PrintChar_Zigbee('+');

	vTaskDelay(2000);
	length = UARTReceive_Zigbee(LPC_UART3, output, sizeof(output));
	PRINT_Log("Zigbee Message with length %d \n",length);
	if (length == 0)
	{
		UART_PrintChar_Zigbee('+');
		vTaskDelay(50);
		UART_PrintChar_Zigbee('+');
		vTaskDelay(50);
		UART_PrintChar_Zigbee('+');
		//check the ok
		length = UARTReceive_Zigbee(LPC_UART3, output, sizeof(output));

	}

	if(length != 0)
	{
		output[length] = '\0';
		PRINT_Log("Zigbee Message with length %d and content %s\n",length,output);
	}
	PRINT_Log("vZigbeeTask Sending ATVR command to check version number\n");
	//read response
	UART_Printf_Zigbee("ATVL\n");
	length = 0;
	vTaskDelay(100);
	length = UARTReceive_Zigbee(LPC_UART3, output, sizeof(output));

	output[length] = '\0';

	PRINT_Log("Zigbee Message with length %d and content %s\n",length,output);
	//
	while(1)
	{
	    vTaskDelay(100);

	}

    return ;
}
