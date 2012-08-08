/***************************************************************

Date: August 28th, 2007.
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

FILE - FTPD.c

DESCRIPTION
    Main functionality for the FTP Daemon on the uIP 1.0 TCP/IP Stack.

MODIFICATION DETAILS
=============================================================================*/

#include <stdio.h>
#include <string.h>


//#include "uip.h"
//#include "psock.h"

#include "ff.h"
#include "uip.h"
#include "psock.h"
#include "FTPD.h"
#include "FTP_Strings.h"
#include "Connections.h"
#include "lpc_types.h"
#include "usbhost_lpc17xx.h"
#include "ftp_server.h"


#include "FTP_Strings.h"


#define FTPD_CONTROL_PORT       21
#define FTPC_DATA_PORT          21

#define NUM_FTP_DAEMONS         2

#define PASV_PORT_OFFSET        1000

ftpd_state_t ftpd_state[NUM_FTP_DAEMONS];

CHAR driveAttached;


/*---------------------------------------------------------------------------*/
/*
 * Simple protothread to send a 421 (service unavailable)
 * when either the USB drive isn't attached or there aren't
 * any free dtpd_state entries.
 */
static PT_THREAD(send_421_response( ftpd_appstate_t *s ))
{
    PSOCK_BEGIN(&s->ps);

    PSOCK_SEND_STR(&s->ps, Resp421);
    PSOCK_CLOSE(&s->ps);

    PSOCK_END(&s->ps);
}


/*---------------------------------------------------------------------------*/
/*
 * This function is called whenever an uIP event occurs (e.g. when
 * a new connection is established, new data arrives, sent data is
 * acknowledged, data needs to be retransmitted, etc.), from the 
 * uip_tcp_appcall() callback.
 */
void ftpd_appcall( void )
{


    u16_t i;

    //PRINT_Log("\r\nftpd_appcall\r\n");

    /*
     * First we must check that it is the port we are listening on as
     * this function will be called for all events on all connections.
     */
    if (uip_conn->lport == HTONS(FTPD_CONTROL_PORT))
    {

    	ftpd_appstate_t *s = (ftpd_appstate_t *)uip_conn->appdata;
        /*
         * If a new connection we need to check for a few possibilities.
         */
        if (uip_connected())
        {
        	PRINT_Log("\r\nNew command connection from IP Address %d.%d.%d.%d:%d",
                   uip_ipaddr1(uip_conn->ripaddr), uip_ipaddr2(uip_conn->ripaddr),
                   uip_ipaddr3(uip_conn->ripaddr), uip_ipaddr4(uip_conn->ripaddr),
                   ntohs(uip_conn->rport));


            if (!driveAttached)
            {
            	PRINT_Log("\r\nNo drive attached!");
                // Initialise the ProtoSocket for the 421 message
                PSOCK_INIT(&s->ps, NULL, 0);
                // Set the flag to cause it to be sent
                s->resp421_flag = TRUE;
            }
            else
            {
                // find a new FTP server structure
                for (i = 0; i < NUM_FTP_DAEMONS; i++)
                {
                    if (FTP_UNUSED == ftpd_state[i].state)
                    {
                        break;
                    }
                }
                
                // Check if we found a free ftpd_state
                if (NUM_FTP_DAEMONS == i)
                {
                	PRINT_Log("\r\nNo daemons available!");
                    // Initialise the ProtoSocket for the 421 message
                    PSOCK_INIT(&s->ps, NULL, 0);
                    // Set the flag to cause it to be sent
                    s->resp421_flag = TRUE;
                }
                else
                {
                	PRINT_Log("\r\nFound unused deamon %d", i);
                    s->ftpd_state_ptr = &ftpd_state[i];
                    // Initialise the FTP Daemon state
                    ftpd_state[i].state = LOGGED_OUT;
                    ftpd_state[i].data_conn = NULL;
                    //ftpd_state[i].clientip = uip_conn->ripaddr;
                    memcpy(ftpd_state[i].clientip,uip_conn->ripaddr,sizeof(uip_ipaddr_t));
                    ftpd_state[i].clientport = FTPC_DATA_PORT;
                    // Clear the command index down
                    ftpd_state[i].cmd_index = CMD_INDEX_IDLE;
                    // Reset the directory pointer
                    //ftpd_state[i].pCurrent_dir = NULL;
                    // Clear the current path
                    strcpy(ftpd_state[i].Current_path, "/");
                    // Setup the PASV information
                    ftpd_state[i].pasvflag = FALSE;
                    
                    // Initialise the ProtoSockets and ProtoThread used by the command parser
                    PSOCK_INIT(&ftpd_state[i].cmdpsin, ftpd_state[i].cmdinputbuffer, CMD_BUFFER_SIZE);
                    PT_INIT(&ftpd_state[i].cmdpt);
                    // Clear the 421 response flag
                    s->resp421_flag = FALSE;
                }
            }
        }
        else if (uip_closed() || uip_aborted() || uip_timedout())
        {
        	PRINT_Log("\r\nCommand connection closing");
            if (s->ftpd_state_ptr)
            {
            	PRINT_Log(" for daemon %d", s->ftpd_state_ptr->pasvport - PASV_PORT_OFFSET);
                // Free up the FTP daemon
                s->ftpd_state_ptr->state = FTP_UNUSED;
                s->ftpd_state_ptr = NULL;
            }
        }
      
        if (s->resp421_flag)
        {
            send_421_response(s);
        }
        else if (s->ftpd_state_ptr)
        {
            handle_cmd_input(s->ftpd_state_ptr);
            handle_cmd_output(s->ftpd_state_ptr);
        }
    }
    else
    {
    	//PRINT_Log("\r\nSearch though all the FTPD Structures and see if this one of our data connections.");
        /* 
         * Search though all the FTPD Structures and see if this one of our data connections.
         */
        u16_t i;
        for (i = 0; i < NUM_FTP_DAEMONS; i++)
        {
            if (ftpd_state[i].data_conn == uip_conn)
            {
                if (uip_connected())
                {
                	PRINT_Log("\r\nNew active data connection for daemon %d to IP Address %d.%d.%d.%d:%d", i,
                           uip_ipaddr1(uip_conn->ripaddr), uip_ipaddr2(uip_conn->ripaddr),
                           uip_ipaddr3(uip_conn->ripaddr), uip_ipaddr4(uip_conn->ripaddr),
                           ntohs(uip_conn->rport));
                    PSOCK_INIT(&ftpd_state[i].dataps, ftpd_state[i].datainputbuffer, DATA_BUFFER_SIZE);
                    // Move the state on as required
                    if (WAITING_FOR_CONNECT == ftpd_state[i].state)
                    {
                        ftpd_state[i].state = CONNECT_OK;
                    }
                }
                else if (uip_closed() || uip_aborted() || uip_timedout())
                {
                	PRINT_Log("\r\nData connection closing for daemon %d", i);
                    if (uip_closed() && (TRANSFER_RECV == ftpd_state[i].state))
                    {
                        // There may be data in this last packet
                        handle_data_connection(&ftpd_state[i]);
                        // Closing of the connection in a RECV state indicates end of file
#if 1
                        CloseFile(&ftpd_state[i]);

#endif
                        //close the file in here

                        PRINT_Log("\r\nFinished file receive for daemon %d", i);
                        ftpd_state[i].state = TRANSFER_DONE;
                    }
                    else if((TRANSFER_LIST == ftpd_state[i].state) || (TRANSFER_SEND == ftpd_state[i].state) || (TRANSFER_RECV == ftpd_state[i].state))
                    {
                    	PRINT_Log("\r\nAborting data transfer for daemon %d", i);
                        ftpd_state[i].state = TRANSFER_ABORT;
                    }
                    ftpd_state[i].data_conn = NULL;
                }
                
                if (ftpd_state[i].data_conn)
                {
                    handle_data_connection(&ftpd_state[i]);
                }
                // Leave the loop early
                break;
            }
            else
            {
                // Check for a new connection in Passive mode
                if(ftpd_state[i].pasvflag && uip_connected() && (ntohs(uip_conn->lport) == ftpd_state[i].pasvport))
                {
                	PRINT_Log("\r\nNew passive data connection for daemon %d from IP Address %d.%d.%d.%d:%d", i,
                           uip_ipaddr1(uip_conn->ripaddr), uip_ipaddr2(uip_conn->ripaddr),
                           uip_ipaddr3(uip_conn->ripaddr), uip_ipaddr4(uip_conn->ripaddr),
                           ntohs(uip_conn->rport));
                    ftpd_state[i].data_conn = uip_conn;
                    PSOCK_INIT(&ftpd_state[i].dataps, ftpd_state[i].datainputbuffer, DATA_BUFFER_SIZE);
                    // Move the state on as required
                    if (WAITING_FOR_CONNECT == ftpd_state[i].state)
                    {
                        ftpd_state[i].state = CONNECT_OK;
                    }
                    handle_data_connection(&ftpd_state[i]);
                    // Leave the loop early
                    break;
                }
            }
        }
    }
}


/*---------------------------------------------------------------------------*/
/*
 * The initialization function. We must explicitly call this function
 * from the system initialization code, after uip_init() is called.
 */
void ftpd_init(void)
{
    u16_t i;

    // Initialise the ftp daemons
    for (i = 0; i < NUM_FTP_DAEMONS; i++)
    {
        ftpd_state[i].state = FTP_UNUSED;
        ftpd_state[i].pasvport = PASV_PORT_OFFSET + i;
    }

    /* We start to listen for connections on the FTP control port. */
    uip_listen(HTONS(FTPD_CONTROL_PORT));
}


