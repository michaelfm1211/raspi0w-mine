ENTRY(main)

SECTIONS
{
    . = 0x10000000;
    __prog_begin = .;

    __prog_text_start = .;
    .text : {
        *(.text)
    }
    . = ALIGN(4096);
    __prog_text_end = .;

    __prog_data_start = .;
    .data : {
        *(.data)
    }
    . = ALIGN(4096);
    __prog_data_end = .;

    __prog_bss_start = .;
    .bss : {
        *(.bss)
    }
    . = ALIGN(4096);
    __prog_bss_end = .;

    __end = .;
}
