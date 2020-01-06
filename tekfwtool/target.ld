SECTIONS
{
	. = 0x05010000;
	.text : { *(.text) }
	_bss_begin = .;
	.bss  : { *(.bss) }
	_bss_end = .;
	.data : { *(.data) }
}
