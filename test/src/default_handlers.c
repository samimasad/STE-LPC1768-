/*****************************************************************************
 *   +--+
 *   | ++----+
 *   +-++    |
 *     |     |
 *   +-+--+  |
 *   | +--+--+
 *   +----+    Copyright (c) 2010 Code Red Technologies Ltd.
 *
 * LPC29xx default exception handler code for use with Red Suite
 *
 * Version : 101026
 *
 * Software License Agreement
 *
 * The software is owned by Code Red Technologies and/or its suppliers, and is
 * protected under applicable copyright laws.  All rights are reserved.  Any
 * use in violation of the foregoing restrictions may subject the user to criminal
 * sanctions under applicable laws, as well as to civil liability for the breach
 * of the terms and conditions of this license.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * USE OF THIS SOFTWARE FOR COMMERCIAL DEVELOPMENT AND/OR EDUCATION IS SUBJECT
 * TO A CURRENT END USER LICENSE AGREEMENT (COMMERCIAL OR EDUCATIONAL) WITH
 * CODE RED TECHNOLOGIES LTD.
 *
 ******************************************************************************/

/*************************************************************************
 * The functions in this file provide default implementations of the
 * handlers for IRQ, FIQ and SWI exceptions.
 *
 * The names of these handlers matches the names used in the vector table
 * which can found in cr_vectors_lp29xx.s
 *
 * In most embedded applications these default handlers will be replaced
 * by real handlers. If this is the case in your application, then you
 * will need to remove the appropriate default handler from your build.
 * This can be done simply by undefining the symbols USE_DEFAULT_IRQ,
 * USE_DEFAULT_FIQ or USE_DEFAULT_SWI.
 *
 * Note that if the name of your handler for a particular exception is
 * different from the name used for the default handler, then you will
 * need to change the name used in the vector table code in the file
 * cr_vectors_lp29xx.s.
 *
 * Default handlers for prefetch abort, data abort and undefined
 * instruction exceptions, which you are less likely to need to change,
 * can be found in cr_vectors_lp29xx.s
 *************************************************************************/

// ***************************************************
// Undef the following symbols if the corresponding
// default handler is not required.
// ***************************************************
#define USE_DEFAULT_IRQ 1
#define USE_DEFAULT_FIQ 1
#define USE_DEFAULT_SWI 1

// *******************
// Default IRQ handler
// *******************
#ifdef USE_DEFAULT_IRQ
void __attribute__((interrupt("IRQ"))) IRQ_Exception (void)
{
	while (1);
}
#endif

// *******************
// Default FIQ handler
// *******************
#ifdef USE_DEFAULT_FIQ
void __attribute__((interrupt("FIQ"))) FIQ_Exception (void)
{
	while (1);
}
#endif

// *******************
// Default SWI handler
// *******************
#ifdef USE_DEFAULT_SWI
void __attribute__((interrupt("SWI"))) SWI_Handler (void)
{
	while (1);
}
#endif
