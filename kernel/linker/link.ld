OUTPUT_FORMAT(binary)
ENTRY(_start)

SECTIONS
{
    /* The UEFI loader jumps to 0x100000 */
    . = 0x100000;

    .text : {
        *(.text.prologue) /* Ensure the assembly entry is FIRST */
        *(.text)
    }

    .data : { *(.data) }
    .bss  : { *(.bss)  }

    /DISCARD/ : {
        *(.note*);
        *(.comment*);
    }
}
