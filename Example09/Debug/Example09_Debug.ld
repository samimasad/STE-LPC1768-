/*
 * GENERATED FILE - DO NOT EDIT
 * (C) Code Red Technologies Ltd, 2008-9    
 * Generated C linker script file for LPC1768 
 * (created from nxp_lpc13_c.ld (v3.5.6 (201011041518)) on Fri Dec 10 08:40:27 CET 2010)
*/

INCLUDE "Example09_Debug_lib.ld"
INCLUDE "Example09_Debug_mem.ld"

ENTRY(ResetISR)

SECTIONS
{
	.text :
	{
		KEEP(*(.isr_vector))
		*(.text*)
		*(.rodata*)

	} > MFlash512


	/* for exception handling/unwind - some Newlib functions (in common with C++ and STDC++) use this. */
	
	.ARM.extab : 
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)
	} > MFlash512

	__exidx_start = .;
	.ARM.exidx :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} > MFlash512
	__exidx_end = .;

	_etext = .;
		
	.data :
	{
		_data = .;
		*(vtable)
		*(.data*)
		_edata = .;
	} > RamLoc32 AT>MFlash512

	/* zero initialized data */
	.bss :
	{
		_bss = .;
		*(.bss*)
		*(COMMON)
		_ebss = .;
	} > RamLoc32
	
	/* Where we put the heap with cr_clib */
	.cr_heap :
	{
		end = .;
		_pvHeapStart = .;
	} > RamLoc32

	_vRamTop = __top_RamLoc32 ;
	_vStackTop = _vRamTop - 0;
}
