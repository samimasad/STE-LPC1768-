/*
 * AOA_task.c
 *
 *  Created on: Mar 29, 2013
 *      Author: Sami
 */
#include "lpc17xx_pinsel.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_ssp.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_can.h"
#include <string.h>
#include <stdio.h>
#include "USB.h"
#include "AndroidAccessoryHost.h"

void vAOATask(void *pvParameters) {




	console_uart_sendString((uint8_t*) "vAOATask Init\r\n");
	while (1) {
		Monitor_Task();
		AndroidHost_Task();
		USB_USBTask();
		//vTaskDelay(5);


	}
}
