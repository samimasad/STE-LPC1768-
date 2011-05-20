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

FILE - FTPD.h

DESCRIPTION
    Main interface for the FTP Daemon on the uIP 1.0 TCP/IP Stack.

MODIFICATION DETAILS
=============================================================================*/

#ifndef __FTP_DAEMON_H__
#define __FTP_DAEMON_H__

#define CMD_BUFFER_SIZE    64
#define DATA_BUFFER_SIZE   100



typedef enum
{
    FTP_UNUSED,
    LOGGED_OUT,
    LOGGED_IN,
    WAITING_FOR_CONNECT,
    CONNECT_FAILED,
    CONNECT_OK,
    TRANSFER_LIST,
    TRANSFER_SEND,
    TRANSFER_RECV,
    TRANSFER_DONE,
    TRANSFER_ABORT,
    QUITTING
} FTP_State;


typedef struct
{
    FTP_State state;
    struct psock cmdpsin;
    struct psock cmdpsout;
    struct pt cmdpt;
    u16_t cmd_index;
    char cmdinputbuffer[CMD_BUFFER_SIZE];
    char *params;
    struct psock dataps;
    struct uip_conn *data_conn;
    char datainputbuffer[DATA_BUFFER_SIZE];
    uip_ipaddr_t clientip;
    u16_t clientport;
    u16_t pasvflag;
    u16_t pasvport;
    u16_t count;
/*
    fat_fat_dir_entry_t Current_dir_entry;
    fat_fat_dir_entry_t *pCurrent_dir;
    fat_file_t *fat_file;
*/

    FIL File;
    FILINFO fno;
    DIR dir;
    char Current_path[_MAX_LFN];
    char Current_file[_MAX_LFN];
    long pos;
    unsigned int len;
} ftpd_state_t;


typedef struct
{
    struct psock ps;
    int resp421_flag;
    ftpd_state_t *ftpd_state_ptr;
} ftpd_appstate_t;

extern FATFS USBFs[_VOLUMES];		/* File system object for each logical drive */

extern u16_t driveAttached;

extern void ftpd_init(void);

extern void ftpd_appcall(void);

#endif /* __FTP_DAEMON_H__ */


