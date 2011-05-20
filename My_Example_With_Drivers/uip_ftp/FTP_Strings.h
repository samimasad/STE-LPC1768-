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

FILE - FTP_Strings.h

DESCRIPTION
    Interface to the strings used by the FTP Daemon.

MODIFICATION DETAILS
=============================================================================*/

#define Resp125 "125 Data connection already open\r\n"
#define Resp150 "150 Opening data connection\r\n"
#define Resp200 "200 Command OK\r\n"
#define Resp214A "214-The following commands are recognised\r\n"
#define Resp214B "214 Done\r\n"
#define Resp220 "220 LPC1768 FTP Server\r\n"
#define Resp221 "221 Closing control connection\r\n"
#define Resp226 "226 Closing data connection\r\n"
#define Resp230 "230 User logged in\r\n"
#define Resp227 "227 Entering Passive Mode (%d,%d,%d,%d,%d,%d)\r\n"
#define Resp250 "250 Command complete\r\n"
#define Resp257 "257 \"%s\"\r\n"
#define Resp421 "421 Service Unavailable\r\n"
#define Resp425 "425 Cannot open data connection\r\n"
#define Resp426 "426 Transfer aborted\r\n"
#define Resp451 "451 Aborted. Local error\r\n"
#define Resp500 "500 Syntax error, command unrecognised\r\n"
#define Resp501 "501 Syntax error in parameters or arguments\r\n"
#define Resp502 "502 Command not implemented\r\n"
#define Resp504 "504 Command not implemented for that parameter\r\n"
#define Resp550 "550 Action not taken\r\n"
