/***************************************************************

 Date: June 14th, 2007.
 Copyright (c) 2007 Cyan Technology Limited. All Rights Reserved.

 Cyan Technology Limited, hereby grants a non-exclusive license
 under Cyan Technology Limited's copyright to copy, modify and
 distribute this software for any purpose and without fee,
 provided that the above copyright notice and the following
 paragraphs appear on all copies.

 Cyan Technology Limited makes no representation that this
 source code is correct or is an accurate representation of any
 standard.

 In no event shall Cyan Technology Limited have any liability
 for any direct, indirect, or speculative damages, (including,
 without limiting the forgoing, consequential, incidental and
 special damages) including, but not limited to infringement,
 loss of use, business interruptions, and loss of profits,
 respective of whether Cyan Technology Limited has advance
 notice of the possibility of any such damages.

 Cyan Technology Limited specifically disclaims any warranties
 including, but not limited to, the implied warranties of
 merchantability, fitness for a particular purpose, and non-
 infringement. The software provided hereunder is on an "as is"
 basis and Cyan Technology Limited has no obligation to provide
 maintenance, support, updates, enhancements, or modifications.

 ****************************************************************/

/*=============================================================================
 Cyan Technology Limited

 FILE - main.c

 DESCRIPTION
 Main functionality and program loop for the uIP stack.

 MODIFICATION DETAILS
 =============================================================================*/

// Library files

#include <stdio.h>

// uIP include files


/* Demo includes. */
#include "EthDev_LPC17xx.h"
#include "EthDev.h"
#include "lpc_types.h"
#include "usbhost_lpc17xx.h"

/* uip includes. */
#include "uip.h"
#include "uip_arp.h"
#include "psock.h"

#include "ff.h"
/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

/* uip includes. */

#include "timer.h"
#include "clock-arch.h"
#include "ftp_server.h"


// Other Project support files
#include "FTPD.h"

#define UIP_CONF_FIXEDADDR	1


#define uipINIT_WAIT_ftp    ( 100 / portTICK_RATE_MS )
/* Shortcut to the header within the Rx buffer. */
#define xHeader_ftp ((struct uip_eth_hdr *) &uip_buf[ 0 ])
/* The semaphore used by the ISR to wake the uIP task. */
extern xSemaphoreHandle xEMACSemaphore ;
// Declaration of public functions.
// Private constants and types.
// Declaration of static functions.
// Global variables.
// Module global variables.

// Per connection TCP application data
// Only one set of data per connection is required
// so keep them all in a union and cast to the type
// required by that connection.
typedef union {
	ftpd_appstate_t ftpd_appdata;
} tcp_appdata_t;

//tcp_appdata_t tcp_appdata[UIP_CONNS];
tcp_appdata_t tcp_appdata[UIP_CONNS];

// Per connection UDP application data
#if UIP_UDP
/* No per connection UDP application data required.
 typedef union
 {
 // None for this application
 } udp_appdata_t;

 udp_appdata_t udp_appdata[UIP_UDP_CONNS];
 */
#endif

// Descriptors and buffers required by the EMAC peripheral
// These must be in IRAM and allow one extra word as they
// need to be 32 bit aligned when used.


// Definition of API functions.
void appdata_init(void);


static void prvSetMACAddress( void )
{
struct uip_eth_addr xAddr;

	/* Configure the MAC address in the uIP stack. */
	xAddr.addr[ 0 ] = configMAC_ADDR0;
	xAddr.addr[ 1 ] = configMAC_ADDR1;
	xAddr.addr[ 2 ] = configMAC_ADDR2;
	xAddr.addr[ 3 ] = configMAC_ADDR3;
	xAddr.addr[ 4 ] = configMAC_ADDR4;
	xAddr.addr[ 5 ] = configMAC_ADDR5;
	uip_setethaddr( xAddr );
}
// ---------------------------------------------------------------------------------------------------------------------
/*!
 * \brief Main entry function to httpd
 * 
 * \return 0
 */
// -----------------------------------------------------------------------------------------------------------------------
void ftp_main(void *pvParameters)
{
	portBASE_TYPE i;
	uip_ipaddr_t xIPAddr;
	struct timer periodic_timer, arp_timer;
	extern void ( vEMAC_ISR_Wrapper )( void );
	uint8_t buffer[100]; //for debug

	sprintf(buffer,"Init starting\n\r");
	UART_Send((LPC_UART_TypeDef *)LPC_UART1, buffer, strlen(buffer),BLOCKING);

	vTaskDelay(1000);
	( void ) pvParameters;
	/* Initialise the uIP stack. */
	timer_set( &periodic_timer, configTICK_RATE_HZ / 2 );
	timer_set( &arp_timer, configTICK_RATE_HZ * 10 );
	uip_init();
	uip_ipaddr( xIPAddr, configIP_ADDR0, configIP_ADDR1, configIP_ADDR2, configIP_ADDR3 );
	uip_sethostaddr( xIPAddr );
	uip_ipaddr( xIPAddr, configNET_MASK0, configNET_MASK1, configNET_MASK2, configNET_MASK3 );
	uip_setnetmask( xIPAddr );
	// Initialise the FTP Daemon
	appdata_init();
	ftpd_init();
	sprintf(buffer,"FTP Init succesful\n\r");
	UART_Send((LPC_UART_TypeDef *)LPC_UART1, buffer, strlen(buffer),BLOCKING);
	/* Create the semaphore used to wake the uIP task. */
	vSemaphoreCreateBinary( xEMACSemaphore);

	/* Initialise the MAC. */
	while( lEMACInit() != pdPASS )
	{
		vTaskDelay( uipINIT_WAIT_ftp );
	}
	sprintf(buffer,"Initialise the MAC\n\r");
	UART_Send((LPC_UART_TypeDef *)LPC_UART1, buffer, strlen(buffer),BLOCKING);

	portENTER_CRITICAL();
	{
		LPC_EMAC->IntEnable = ( INT_RX_DONE | INT_TX_DONE );

		/* Set the interrupt priority to the max permissible to cause some
		 interrupt nesting. */
		NVIC_SetPriority( ENET_IRQn, configEMAC_INTERRUPT_PRIORITY );

		/* Enable the interrupt. */
		NVIC_EnableIRQ( ENET_IRQn );
		prvSetMACAddress();
	}
	portEXIT_CRITICAL();

	sprintf(buffer,"getting into infinite loop of the ftp server loop\n\r");
	UART_Send((LPC_UART_TypeDef *)LPC_UART1, buffer, strlen(buffer),BLOCKING);
	for(;; )
	{
		/* Is there received data ready to be processed? */
		uip_len = ulGetEMACRxData();

		if( ( uip_len > 0 ) && ( uip_buf != NULL ) )
		{
			/* Standard uIP loop taken from the uIP manual. */
			if( xHeader_ftp->type == htons( UIP_ETHTYPE_IP ) )
			{
				//PRINT_Log("Got IP with type UIP_ETHTYPE_IP");
				uip_arp_ipin();
				uip_input();

				/* If the above function invocation resulted in data that
				 should be sent out on the network, the global variable
				 uip_len is set to a value > 0. */
				if( uip_len > 0 )
				{
					uip_arp_out();
					vSendEMACTxData( uip_len );
				}
			}
			else if( xHeader_ftp->type == htons( UIP_ETHTYPE_ARP ) )
			{
				//PRINT_Log("Got IP with type UIP_ETHTYPE_ARP");
				uip_arp_arpin();

				/* If the above function invocation resulted in data that
				 should be sent out on the network, the global variable
				 uip_len is set to a value > 0. */
				if( uip_len > 0 )
				{
					vSendEMACTxData( uip_len );
				}
			}
		}
		else
		{
			if( timer_expired( &periodic_timer ) && ( uip_buf != NULL ) )
			{
				timer_reset( &periodic_timer );
				for( i = 0; i < UIP_CONNS; i++ )
				{
					uip_periodic( i );

					/* If the above function invocation resulted in data that
					 should be sent out on the network, the global variable
					 uip_len is set to a value > 0. */
					if( uip_len > 0 )
					{
						uip_arp_out();
						vSendEMACTxData( uip_len );
					}
				}

				/* Call the ARP timer function every 10 seconds. */
				if( timer_expired( &arp_timer ) )
				{
					timer_reset( &arp_timer );
					uip_arp_timer();
				}
			}
			else
			{
				/* We did not receive a packet, and there was no periodic
				 processing to perform.  Block for a fixed period.  If a packet
				 is received during this period we will be woken by the ISR
				 giving us the Semaphore. */
				xSemaphoreTake( xEMACSemaphore, configTICK_RATE_HZ / 2 );
			}
		}
	}

}

/*
 * Initialise all the per connection application data for TCP and UDP
 */
void appdata_init(void) {
	unsigned int i;

	for (i = 0; i < UIP_CONNS; i++)
	{
		uip_conns[i].appdata = (void *)&(tcp_appdata[i]);

	}
    f_mount(USB,&USBFs[USB]);
    //change driver to USB
    f_chdrive(USB);



#if UIP_UDP
	/* No per connection UDP application data required.
	 for (i = 0; i < UIP_UDP_CONNS; i++)
	 {
	 uip_udp_conns[i].appdata = (void *)&(udp_appdata[i]);
	 }
	 */
#endif
}

#if UIP_LOGGING == 1
void uip_log(char *msg)
{
	//printf ("\n\r%s\n\r", msg);
	UART_Printf("\n\r%s\n\r", msg);
}
#endif

/*
 * uIP Application Callbacks
 */

// TCP Application Callback
void uip_tcp_appcall(void) {
	ftpd_appcall();
}


// UDP Application Callback
#if UIP_UDP
void uip_udp_appcall(void)
{
#if !UIP_CONF_FIXEDADDR
	dhcpc_appcall();
#endif // UIP_CONF_FIXEDADDR
}
#endif



