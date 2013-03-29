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

/** \file
 *
 *  Header file for AndroidAccessoryHost.c.
 */

#ifndef _ANDROID_ACCESSORY_HOST_H_
#define _ANDROID_ACCESSORY_HOST_H_

	/* Includes: */
#if ARCH==ARCH_LPC
		#include "USB.h"

#else
		#include <avr/io.h>
		#include <avr/wdt.h>
		#include <avr/pgmspace.h>
		#include <avr/power.h>
		#include <avr/interrupt.h>
		#include <stdio.h>

		#include <LUFA/Version.h>
		#include <LUFA/Drivers/Misc/TerminalCodes.h>
		#include <LUFA/Drivers/USB/USB.h>
		#include <LUFA/Drivers/Peripheral/Serial.h>
		#include <LUFA/Drivers/Board/LEDs.h>
#endif
		#include "DeviceDescriptor.h"
		#include "ConfigDescriptor.h"
		#include "AndroidAccessoryCommands.h"

	/* Macros: */

	/* Function Prototypes: */
		void AndroidHost_Task(void);
		void Monitor_Task(void);

	/* Event Handlers: */
		void EVENT_USB_Host_DeviceAttached(const uint8_t corenum);
		void EVENT_USB_Host_DeviceUnattached(const uint8_t corenum);
		void EVENT_USB_Host_DeviceEnumerationComplete(const uint8_t corenum);
		void EVENT_USB_Host_HostError(const uint8_t corenum, const uint8_t ErrorCode);
		void EVENT_USB_Host_DeviceEnumerationFailed(const uint8_t corenum,
													const uint8_t ErrorCode,
		                                            const uint8_t SubErrorCode);

#endif

