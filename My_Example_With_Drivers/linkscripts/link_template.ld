/* INSERT HEADER */

/* INSERT INCLUDES */

ENTRY(${start_symbol})

SECTIONS
{
	/* INSERT TEXT_SECTION */

	/* MAIN TEXT SECTION */	
	.text : ALIGN(4)
	{
		FILL(0xff)
		KEEP(*(.isr_vector))
		/* INSERT SECTION_TABLE */

		*(.after_vectors*)
		
		/* INSERT CRP */
		*(.text*)
		/* INSERT RODATA */
		
		/* INSERT CPP_INFO */
	} > ${CODE}

	/* INSERT EXDATA */
	
	_etext = .;
		
	/* INSERT DATA_SECTION */
	
	/* MAIN DATA SECTION */

	.uninit_RESERVED : ALIGN(4)
	{
		KEEP(*(.bss.$RESERVED*))
	} > ${DATA}

	.data : ALIGN(4)
	{
		FILL(0xff)
		_data = .;
		*(vtable)
		*(.data*)
		. = ALIGN(4);
		_edata = .;
	} > ${DATA} AT>${CODE}

	/* INSERT BSS_SECTION */

	/* MAIN BSS SECTION */
	.bss : ALIGN(4)
	{
		_bss = .;
		*(.bss*)
		*(COMMON)
		. = ALIGN(4);
		_ebss = .;
		PROVIDE(end = .);
	} > ${DATA}
	
	PROVIDE(${heap_symbol} = .);
	PROVIDE(_vStackTop = __top_${DATA} - ${STACK_OFFSET});
}

