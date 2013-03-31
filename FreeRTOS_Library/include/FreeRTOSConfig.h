/*
    FreeRTOS V6.0.0 - Copyright (C) 2009 Real Time Engineers Ltd.

    This file is part of the FreeRTOS distribution.

    FreeRTOS is free software; you can redistribute it and/or modify it under
    the terms of the GNU General Public License (version 2) as published by the
    Free Software Foundation AND MODIFIED BY the FreeRTOS exception.
    ***NOTE*** The exception to the GPL is included to allow you to distribute
    a combined work that includes FreeRTOS without being obliged to provide the
    source code for proprietary components outside of the FreeRTOS kernel.
    FreeRTOS is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
    more details. You should have received a copy of the GNU General Public
    License and the FreeRTOS license exception along with FreeRTOS; if not it
    can be viewed here: http://www.freertos.org/a00114.html and also obtained
    by writing to Richard Barry, contact details for whom are available on the
    FreeRTOS WEB site.

    1 tab == 4 spaces!

    http://www.FreeRTOS.org - Documentation, latest information, license and
    contact details.

    http://www.SafeRTOS.com - A version that is certified for use in safety
    critical systems.

    http://www.OpenRTOS.com - Commercial support, development, porting,
    licensing and training services.
*/


/******************************************************************************
	See http://www.freertos.org/a00110.html for an explanation of the
	definitions contained in this file.
******************************************************************************/

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "LPC17xx.h"

/*-----------------------------------------------------------
 * Application specific definitions.
 *
 * These definitions should be adjusted for your particular hardware and
 * application requirements.
 *
 * THESE PARAMETERS ARE DESCRIBED WITHIN THE 'CONFIGURATION' SECTION OF THE
 * FreeRTOS API DOCUMENTATION AVAILABLE ON THE FreeRTOS.org WEB SITE.
 *----------------------------------------------------------*/

#define configUSE_PREEMPTION		1
#define configUSE_IDLE_HOOK			1
#define configMAX_PRIORITIES		( ( unsigned portBASE_TYPE ) 5 )
#define configUSE_TICK_HOOK			1
#define configCPU_CLOCK_HZ			( 100000000UL )
#define configTICK_RATE_HZ			( ( portTickType ) 1000 )
#define configMINIMAL_STACK_SIZE	( ( unsigned short ) 100 )
#define configTOTAL_HEAP_SIZE		( ( size_t ) ( 15 * 1024 ) )
#define configMAX_TASK_NAME_LEN		( 12 )
#define configUSE_TRACE_FACILITY	0
#define configUSE_16_BIT_TICKS		0
#define configIDLE_SHOULD_YIELD		0
#define configUSE_CO_ROUTINES 		0
#define configUSE_MUTEXES			1

#define configMAX_CO_ROUTINE_PRIORITIES ( 2 )

#define configUSE_COUNTING_SEMAPHORES 	1
#define configUSE_ALTERNATIVE_API 		0
#define configCHECK_FOR_STACK_OVERFLOW	2
#define configUSE_RECURSIVE_MUTEXES		0
#define configQUEUE_REGISTRY_SIZE		0
#define configGENERATE_RUN_TIME_STATS	0
#define configUSE_MALLOC_FAILED_HOOK	1

/* Set the following definitions to 1 to include the API function, or zero
to exclude the API function. */

#define INCLUDE_vTaskPrioritySet			1
#define INCLUDE_uxTaskPriorityGet			1
#define INCLUDE_vTaskDelete					1
#define INCLUDE_vTaskCleanUpResources		0
#define INCLUDE_vTaskSuspend				1
#define INCLUDE_vTaskDelayUntil				1
#define INCLUDE_vTaskDelay					1
#define INCLUDE_uxTaskGetStackHighWaterMark	0
/* MAC address configuration. */
#define configMAC_ADDR0	0x00
#define configMAC_ADDR1	0x12
#define configMAC_ADDR2	0x13
#define configMAC_ADDR3	0x10
#define configMAC_ADDR4	0x15
#define configMAC_ADDR5	0x11

/* IP address configuration. */
#define configIP_ADDR0		192
#define configIP_ADDR1		168
#define configIP_ADDR2		0
#define configIP_ADDR3		120

/* System Control Block (SCB) includes:
   Flash Accelerator Module, Clocking and Power Control, External Interrupts,
   Reset, System Control and Status
*/
#define SCB_BASE_ADDR   0x400FC000

#define PCONP_PCTIM0    0x00000002
#define PCONP_PCTIM1    0x00000004
#define PCONP_PCUART0   0x00000008
#define PCONP_PCUART1   0x00000010
#define PCONP_PCPWM1    0x00000040
#define PCONP_PCI2C0    0x00000080
#define PCONP_PCSPI     0x00000100
#define PCONP_PCRTC     0x00000200
#define PCONP_PCSSP1    0x00000400
#define PCONP_PCAD      0x00001000
#define PCONP_PCCAN1    0x00002000
#define PCONP_PCCAN2    0x00004000
#define PCONP_PCGPIO    0x00008000
#define PCONP_PCRIT     0x00010000
#define PCONP_PCMCPWM   0x00020000
#define PCONP_PCQEI     0x00040000
#define PCONP_PCI2C1    0x00080000
#define PCONP_PCSSP0    0x00200000
#define PCONP_PCTIM2    0x00400000
#define PCONP_PCTIM3    0x00800000
#define PCONP_PCUART2   0x01000000
#define PCONP_PCUART3   0x02000000
#define PCONP_PCI2C2    0x04000000
#define PCONP_PCI2S     0x08000000
#define PCONP_PCGPDMA   0x20000000
#define PCONP_PCENET    0x40000000
#define PCONP_PCUSB     0x80000000

#define PLLCON_PLLE     0x00000001
#define PLLCON_PLLC     0x00000002
#define PLLCON_MASK     0x00000003

#define PLLCFG_MUL1     0x00000000
#define PLLCFG_MUL2     0x00000001
#define PLLCFG_MUL3     0x00000002
#define PLLCFG_MUL4     0x00000003
#define PLLCFG_MUL5     0x00000004
#define PLLCFG_MUL6     0x00000005
#define PLLCFG_MUL7     0x00000006
#define PLLCFG_MUL8     0x00000007
#define PLLCFG_MUL9     0x00000008
#define PLLCFG_MUL10    0x00000009
#define PLLCFG_MUL11    0x0000000A
#define PLLCFG_MUL12    0x0000000B
#define PLLCFG_MUL13    0x0000000C
#define PLLCFG_MUL14    0x0000000D
#define PLLCFG_MUL15    0x0000000E
#define PLLCFG_MUL16    0x0000000F
#define PLLCFG_MUL17    0x00000010
#define PLLCFG_MUL18    0x00000011
#define PLLCFG_MUL19    0x00000012
#define PLLCFG_MUL20    0x00000013
#define PLLCFG_MUL21    0x00000014
#define PLLCFG_MUL22    0x00000015
#define PLLCFG_MUL23    0x00000016
#define PLLCFG_MUL24    0x00000017
#define PLLCFG_MUL25    0x00000018
#define PLLCFG_MUL26    0x00000019
#define PLLCFG_MUL27    0x0000001A
#define PLLCFG_MUL28    0x0000001B
#define PLLCFG_MUL29    0x0000001C
#define PLLCFG_MUL30    0x0000001D
#define PLLCFG_MUL31    0x0000001E
#define PLLCFG_MUL32    0x0000001F
#define PLLCFG_MUL33    0x00000020
#define PLLCFG_MUL34    0x00000021
#define PLLCFG_MUL35    0x00000022
#define PLLCFG_MUL36    0x00000023

#define PLLCFG_DIV1     0x00000000
#define PLLCFG_DIV2     0x00010000
#define PLLCFG_DIV3     0x00020000
#define PLLCFG_DIV4     0x00030000
#define PLLCFG_DIV5     0x00040000
#define PLLCFG_DIV6     0x00050000
#define PLLCFG_DIV7     0x00060000
#define PLLCFG_DIV8     0x00070000
#define PLLCFG_DIV9     0x00080000
#define PLLCFG_DIV10    0x00090000
#define PLLCFG_MASK		0x00FF7FFF

#define PLLSTAT_MSEL_MASK	0x00007FFF
#define PLLSTAT_NSEL_MASK	0x00FF0000

#define PLLSTAT_PLLE	(1 << 24)
#define PLLSTAT_PLLC	(1 << 25)
#define PLLSTAT_PLOCK	(1 << 26)

#define PLLFEED_FEED1   0x000000AA
#define PLLFEED_FEED2   0x00000055

#define NVIC_IRQ_WDT         0u         // IRQ0,  exception number 16
#define NVIC_IRQ_TIMER0      1u         // IRQ1,  exception number 17
#define NVIC_IRQ_TIMER1      2u         // IRQ2,  exception number 18
#define NVIC_IRQ_TIMER2      3u         // IRQ3,  exception number 19
#define NVIC_IRQ_TIMER3      4u         // IRQ4,  exception number 20
#define NVIC_IRQ_UART0       5u         // IRQ5,  exception number 21
#define NVIC_IRQ_UART1       6u         // IRQ6,  exception number 22
#define NVIC_IRQ_UART2       7u         // IRQ7,  exception number 23
#define NVIC_IRQ_UART3       8u         // IRQ8,  exception number 24
#define NVIC_IRQ_PWM1        9u         // IRQ9,  exception number 25
#define NVIC_IRQ_I2C0        10u        // IRQ10, exception number 26
#define NVIC_IRQ_I2C1        11u        // IRQ11, exception number 27
#define NVIC_IRQ_I2C2        12u        // IRQ12, exception number 28
#define NVIC_IRQ_SPI         13u        // IRQ13, exception number 29
#define NVIC_IRQ_SSP0        14u        // IRQ14, exception number 30
#define NVIC_IRQ_SSP1        15u        // IRQ15, exception number 31
#define NVIC_IRQ_PLL0        16u        // IRQ16, exception number 32
#define NVIC_IRQ_RTC         17u        // IRQ17, exception number 33
#define NVIC_IRQ_EINT0       18u        // IRQ18, exception number 34
#define NVIC_IRQ_EINT1       19u        // IRQ19, exception number 35
#define NVIC_IRQ_EINT2       20u        // IRQ20, exception number 36
#define NVIC_IRQ_EINT3       21u        // IRQ21, exception number 37
#define NVIC_IRQ_ADC         22u        // IRQ22, exception number 38
#define NVIC_IRQ_BOD         23u        // IRQ23, exception number 39
#define NVIC_IRQ_USB         24u        // IRQ24, exception number 40
#define NVIC_IRQ_CAN         25u        // IRQ25, exception number 41
#define NVIC_IRQ_GPDMA       26u        // IRQ26, exception number 42
#define NVIC_IRQ_I2S         27u        // IRQ27, exception number 43
#define NVIC_IRQ_ETHERNET    28u        // IRQ28, exception number 44
#define NVIC_IRQ_RIT         29u        // IRQ29, exception number 45
#define NVIC_IRQ_MCPWM       30u        // IRQ30, exception number 46
#define NVIC_IRQ_QE          31u        // IRQ31, exception number 47
#define NVIC_IRQ_PLL1        32u        // IRQ32, exception number 48
#define NVIC_IRQ_USB_ACT     33u        // IRQ33, exception number 49
#define NVIC_IRQ_CAN_ACT     34u        // IRQ34, exception number 50


/* Priorities passed to NVIC_SetPriority() do not require shifting as the
function does the shifting itself.  Note these priorities need to be equal to
or lower than configMAX_SYSCALL_INTERRUPT_PRIORITY - therefore the numeric
value needs to be equal to or greater than 5 (on the Cortex M3 the lower the
numeric value the higher the interrupt priority). */
#define configEMAC_INTERRUPT_PRIORITY		5
#define configUSB_INTERRUPT_PRIORITY		6

/* Netmask configuration. */
#define configNET_MASK0		255
#define configNET_MASK1		255
#define configNET_MASK2		255
#define configNET_MASK3		0
/* Use the system definition, if there is one */
#ifdef __NVIC_PRIO_BITS
	#define configPRIO_BITS       __NVIC_PRIO_BITS
#else
	#define configPRIO_BITS       5        /* 32 priority levels */
#endif

/* The lowest priority. */
#define configKERNEL_INTERRUPT_PRIORITY 	( 31 << (8 - configPRIO_BITS) )
/* Priority 5, or 160 as only the top three bits are implemented. */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY 	( 5 << (8 - configPRIO_BITS) )



#endif /* FREERTOS_CONFIG_H */

