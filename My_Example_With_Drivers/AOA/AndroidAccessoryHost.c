/*
             LUFA Library
     Copyright (C) Dean Camera, 2011.

  dean [at] fourwalledcubicle [dot] com
           www.lufa-lib.org
*/

/*
  Copyright 2011  Dean Camera (dean [at] fourwalledcubicle [dot] com)

  Permission to use, copy, modify, distribute, and sell this
  software and its documentation for any purpose is hereby granted
  without fee, provided that the above copyright notice appear in
  all copies and that both that the copyright notice and this
  permission notice and warranty disclaimer appear in supporting
  documentation, and that the name of the author not be used in
  advertising or publicity pertaining to distribution of the
  software without specific, written prior permission.

  The author disclaim all warranties with regard to this
  software, including all implied warranties of merchantability
  and fitness.  In no event shall the author be liable for any
  special, indirect or consequential damages or any damages
  whatsoever resulting from loss of use, data or profits, whether
  in an action of contract, negligence or other tortious action,
  arising out of or in connection with the use or performance of
  this software.
*/

/*
 * Embedded Artists AB.
 *
 * This file has been modified by Embedded Artists AB for the AOA Demo
 */

/** \file
 *
 *  Main source file for the AndroidAccessoryHost demo. This file contains the main tasks
 *  of the demo and is responsible for the initial application hardware configuration.
 */

#include "AndroidAccessoryHost.h"

#include "lpc_types.h"
#include "lpc17xx_uart.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_gpio.h"




/******************************************************************************
 * Defines and typedefs
 *****************************************************************************/


/*
 * Message indexes for messages sent to the device
 */
#define CMD_TRIMPOT (0)
#define CMD_BTN_1   (1)
#define CMD_BTN_2   (2)

/*
 * Message indexes for messages sent from the device
 */
#define CMD_RGB_LED6    (10)
#define CMD_RGB_LED7    (11)
#define CMD_CONNECT     (98)
#define CMD_DISCONNECT  (99)


uint32_t getMsTicks(void);;

/******************************************************************************
 * Local variables
 *****************************************************************************/

static uint32_t lastTrimpotCheck = 0;
static uint16_t lastTrimpot = 0;
static uint8_t lastBtn1State = 0;
static uint8_t lastBtn2State = 0;
static uint8_t connected = 0;
static uint8_t doDisconnect = 0;
/*
static int rgb6Red = 0;
static int rgb6Green = 0;
static int rgb6Blue = 0;

static int rgb7Red = 0;
static int rgb7Green = 0;
static int rgb7Blue = 0;
*/

static uint8_t sbuf[250];

static uint8_t attachedCoreNum = -1;

/******************************************************************************
 * Local functions
 *****************************************************************************/

static void sendCommand(uint8_t corenum, uint8_t cmd, uint8_t* data, uint8_t len)
{
	if (USB_HostState[corenum] != HOST_STATE_Configured)
	  return;

	/* Select the data OUT pipe */
	Pipe_SelectPipe(corenum, ANDROID_DATA_OUT_PIPE);
	Pipe_Unfreeze();

	if (Pipe_IsReadWriteAllowed(corenum)) {
		Pipe_Write_8(corenum, cmd);
		Pipe_Write_Stream_LE(corenum, data, len, NULL);
		Pipe_ClearOUT(corenum);
	}

	Pipe_Freeze();

}

static void processCommand(uint8_t corenum, uint8_t cmd, uint8_t hi, uint8_t lo)
{
  uint8_t on = 0;
  uint8_t off = 0;
	switch (cmd) {
	case CMD_RGB_LED6:



    if (lo != 0) {
      on = hi;
    }
    else {
      off = hi;
    }

	  //rgb_setLeds(LED_6, on, off);

		break;

		case CMD_RGB_LED7:

	    if (lo != 0) {
	      on = hi;
	    }
	    else {
	      off = hi;
	    }

	    //rgb_setLeds(LED_7, on, off);

			break;
	case CMD_CONNECT:
		connected = 1;
		break;
	case CMD_DISCONNECT:
		doDisconnect = 1;
		break;
	}


}

void AndroidHost_Task(void)
{
	if (attachedCoreNum == -1) return;

	if (USB_HostState[attachedCoreNum] != HOST_STATE_Configured)
	  return;	

	/* Select the data IN pipe */
	Pipe_SelectPipe(attachedCoreNum, ANDROID_DATA_IN_PIPE);
	Pipe_Unfreeze();

	/* Check to see if a packet has been received */
	if (Pipe_IsINReceived(attachedCoreNum))
	{
		/* Re-freeze IN pipe after the packet has been received */
		Pipe_Freeze();


		/* Check if data is in the pipe */
		if (Pipe_IsReadWriteAllowed(attachedCoreNum))
		{
			uint8_t numBytes = Pipe_BytesInPipe(attachedCoreNum);

			if (numBytes == 3) {

				uint8_t c = Pipe_Read_8(attachedCoreNum);
				uint8_t t = Pipe_Read_8(attachedCoreNum);
				uint8_t d = Pipe_Read_8(attachedCoreNum);
				processCommand(attachedCoreNum, c, t, d);
			}

		}

		/* Clear the pipe after all data in the packet has been read, ready for the next packet */
		Pipe_ClearIN(attachedCoreNum);
	}

	/* Re-freeze IN pipe after use */
	Pipe_Freeze();
}

void Monitor_Task(void)
{
	uint8_t data[2];
	uint8_t btn = 0;
	uint8_t btn1 = 0;
	uint8_t btn2 = 0;

	if (!connected || attachedCoreNum == -1) {
		return;
	}

	if (doDisconnect) {
		doDisconnect = 0;
		sendCommand(attachedCoreNum, CMD_DISCONNECT, 0, 0);
		connected = 0;
		return;
	}


	if (getMsTicks() > lastTrimpotCheck + 50) {
		uint16_t v = trimpot_get();

		if (v != lastTrimpot) {
			lastTrimpot = v;

			data[0] = (v >> 8);
			data[1] = (v & 0xff);

			sendCommand(attachedCoreNum, CMD_TRIMPOT, data, 2);
		}

		lastTrimpotCheck = getMsTicks();
	}

	btn =  btn_get();
	//btn1 = ((btn & BTN_SW2) != 0);
	//btn2 = ((btn & BTN_SW3) != 0);

	if (lastBtn1State != btn1) {
		lastBtn1State = btn1;

		data[0] = btn1;
		sendCommand(attachedCoreNum, CMD_BTN_1, data, 1);
	}

	if (lastBtn2State != btn2) {
		lastBtn2State = btn2;

		data[0] = btn2;
		sendCommand(attachedCoreNum, CMD_BTN_2, data, 1);
	}


}

/** Event handler for the USB_DeviceAttached event. This indicates that a device has been attached to the host, and
 *  starts the library USB task to begin the enumeration and USB management process.
 */
void EVENT_USB_Host_DeviceAttached(const uint8_t corenum)
{
//	console_sendString((uint8_t*)"Device Attached.\r\n");
	sprintf((char*)sbuf, "Device Attached %d\r\n", corenum);
	console_sendString(sbuf);
	rf_uart_sendString(sbuf);
	attachedCoreNum = corenum;
}

/** Event handler for the USB_DeviceUnattached event. This indicates that a device has been removed from the host, and
 *  stops the library USB task management process.
 */
void EVENT_USB_Host_DeviceUnattached(const uint8_t corenum)
{
//	console_sendString((uint8_t*)"\r\nDevice Unattached.\r\n");
	sprintf((char*)sbuf, "\r\nDevice Unattached %d\r\n", corenum);
	console_sendString(sbuf);
	rf_uart_sendString(sbuf);
}

/** Event handler for the USB_DeviceEnumerationComplete event. This indicates that a device has been successfully
 *  enumerated by the host and is now ready to be used by the application.
 */
void EVENT_USB_Host_DeviceEnumerationComplete(const uint8_t corenum)
{
//	console_sendString((uint8_t*)"Getting Device Data.\r\n");
	sprintf((char*)sbuf, "Getting Device Data %d\r\n", corenum);
	console_sendString(sbuf);
	rf_uart_sendString(sbuf);

	/* Get and process the configuration descriptor data */
	uint8_t ErrorCode = ProcessDeviceDescriptor(corenum);
	
	bool RequiresModeSwitch = (ErrorCode == NonAccessoryModeAndroidDevice);

	/* Error out if the device is not an Android device or an error occurred */
	if ((ErrorCode != AccessoryModeAndroidDevice) && (ErrorCode != NonAccessoryModeAndroidDevice))
	{
		if (ErrorCode == ControlError)
		{
			console_sendString((uint8_t*)"Control Error (Get Device).\r\n");
			rf_uart_sendString((uint8_t*)"Control Error (Get Device).\r\n");
		}
		else
		{
			console_sendString((uint8_t*)"Invalid Device.\r\n");
			rf_uart_sendString((uint8_t*)"Control Error (Get Device).\r\n");
		}

		sprintf((char*)sbuf, " -- Error Code: %d\r\n", ErrorCode);
		console_sendString(sbuf);
		rf_uart_sendString(sbuf);
		return;
	}

	sprintf((char*)sbuf, "Android Device Detected - %sAccessory mode.\r\n", (RequiresModeSwitch ? "Non-" : ""));
	console_sendString(sbuf);
	rf_uart_sendString(sbuf);
	
	/* Check if a valid Android device was attached, but it is not current in Accessory mode */
	if (RequiresModeSwitch)
	{
		uint16_t AndroidProtocol;
	
		/* Fetch the version of the Android Accessory Protocol supported by the device */
		if ((ErrorCode = Android_GetAccessoryProtocol(corenum, &AndroidProtocol)) != HOST_SENDCONTROL_Successful)
		{

			sprintf((char*)sbuf, "Control Error (Get Protocol).\r\n"
					" -- Error Code: %d\r\n"
					,ErrorCode);
			console_sendString(sbuf);
			rf_uart_sendString(sbuf);
			return;
		}
		
    /* Validate the returned protocol version */
    if (AndroidProtocol == 0 || AndroidProtocol >  ANDROID_PROTOCOL_Accessory)
    {
      sprintf((char*)sbuf, "Unsupported AOA protocol version: %d\r\n" ,AndroidProtocol);
      console_sendString(sbuf);
      rf_uart_sendString(sbuf);
      return;
    }
	
		/* Send the device strings and start the Android Accessory Mode */

		// Bug: The first call to _SendString seem to generate
		//      bad data. At least it isn't received correctly
		//      on the Android device.
		//
		// Workaround: calling twice for Manufacturer
		Android_SendString(corenum, ANDROID_STRING_Manufacturer, "Embedded Artists AB");

		Android_SendString(corenum, ANDROID_STRING_Manufacturer, "Embedded Artists AB");
		Android_SendString(corenum, ANDROID_STRING_Model,        "AOA Board - Basic");
		Android_SendString(corenum, ANDROID_STRING_Description,  "Demo - AOA Basic");
		Android_SendString(corenum, ANDROID_STRING_Version,      "1.0");
		Android_SendString(corenum, ANDROID_STRING_URI,          "http://www.embeddedartists.com/_aoa/Demo_AOA_Basic.apk");
		Android_SendString(corenum, ANDROID_STRING_Serial,       "N/A");


		Android_StartAccessoryMode(corenum);
		return;
	}

	console_sendString((uint8_t*)"Getting Config Data.\r\n");
	rf_uart_sendString((uint8_t*)"Getting Config Data.\r\n");

	/* Get and process the configuration descriptor data */
	if ((ErrorCode = ProcessConfigurationDescriptor(corenum)) != SuccessfulConfigRead)
	{
		if (ErrorCode == ControlError)
		{
			console_sendString((uint8_t*)"Control Error (Get Configuration).\r\n");
			rf_uart_sendString((uint8_t*)"Control Error (Get Configuration).\r\n");
		}
		else
		{
			console_sendString((uint8_t*)"Invalid Device.\r\n");
			rf_uart_sendString((uint8_t*)"Invalid Device.\r\n");
		}

		sprintf((char*)sbuf, " -- Error Code: %d\r\n", ErrorCode);
		console_sendString(sbuf);
		rf_uart_sendString(sbuf);

		return;
	}

	/* Set the device configuration to the first configuration (rarely do devices use multiple configurations) */
	if ((ErrorCode = USB_Host_SetDeviceConfiguration(corenum, 1)) != HOST_SENDCONTROL_Successful)
	{

		sprintf((char*)sbuf, "Control Error (Set Configuration).\r\n"
				" -- Error Code: %d\r\n", ErrorCode);
		console_sendString(sbuf);
		rf_uart_sendString(sbuf);
		return;
	}

	console_sendString((uint8_t*)"Accessory Mode Android Enumerated.\r\n");
	rf_uart_sendString((uint8_t*)"Accessory Mode Android Enumerated.\r\n");

}

/** Event handler for the USB_HostError event. This indicates that a hardware error occurred while in host mode. */
void EVENT_USB_Host_HostError(const uint8_t corenum, const uint8_t ErrorCode)
{
	USB_Disable();

	sprintf((char*)sbuf, "Host Mode Error\r\n"
			" -- Error Code %d\r\n", ErrorCode);
	console_sendString(sbuf);
	rf_uart_sendString(sbuf);

	for(;;);
}

/** Event handler for the USB_DeviceEnumerationFailed event. This indicates that a problem occurred while
 *  enumerating an attached USB device.
 */
void EVENT_USB_Host_DeviceEnumerationFailed(const uint8_t corenum,
											const uint8_t ErrorCode,
                                            const uint8_t SubErrorCode)
{
	sprintf((char*)sbuf, "Dev Enum Error\r\n"
	        " -- Error Code %d\r\n"
	        " -- Sub Error Code %d\r\n"
	        " -- In State %d\r\n", ErrorCode, SubErrorCode, USB_HostState);
	console_sendString(sbuf);
	rf_uart_sendString(sbuf);

}

