/* Entry Point */
ENTRY(SEGGER_FL_Prepare)

_estack = 0x20020000;    /* end of 128K RAM */
STACK_SIZE = 0x400; /* required amount of stack */

/* Specify the memory areas */
MEMORY
{
  RAM (xrw)       : ORIGIN = 0x20000000, LENGTH = 128K
}

/* Define output sections */
SECTIONS
{
    /* Flash os information */
    PrgCode :
    {
        . = ALIGN(4);
        KEEP(*(PrgCode PrgCode.*));
        . = ALIGN(4);
    } > RAM

    /* Vector table. Has the initial stack pointer and the initial 
       structure for the arm interrupts */
    .vectors :
    {
        . = ALIGN(128);
        /* vector table */
        KEEP(*(.vectors .vectors.*));
        . = ALIGN(4);
    } > RAM

    /* Text segment, stores all user code */
    .text :
    {
        . = ALIGN(4);
        /* text segment */
        *(.text .text.* .gnu.linkonce.t.*);
        /* glue arm to thumb code, glue thumb to arm code*/
        *(.glue_7t .glue_7);
        /* c++ exceptions */
        *(.eh_frame .eh_frame_hdr)
        /* exception unwinding information */
        
        . = ALIGN(4);
        *(.ARM.extab* .gnu.linkonce.armextab.*);
        *(.ARM.exidx*)

        . = ALIGN(4);
        KEEP(*(.init));
        . = ALIGN(4);
        KEEP(*(.fini));

        . = ALIGN(4);
        _etext = .;        /* define a global symbols at end of code */
    } > RAM

    /* Read only data */
    .rodata : 
    {
        . = ALIGN(4);

        /* Constands, strings, etc */
        *(.rodata .rodata.* .gnu.linkonce.r.*);

        . = ALIGN(4);
    } > RAM

    /* Support C constructors, and C destructors in both user code
       and the C library. This also provides support for C++ code. */    
    .preinit_array :
    {
        . = ALIGN(4);
        PROVIDE(__preinit_array_start = .);

        KEEP(*(.preinit_array))

        PROVIDE(__preinit_array_end = .);
    } > RAM

    .init_array :
    {
        . = ALIGN(4);
        PROVIDE(__init_array_start = .);

        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))

        PROVIDE(__init_array_end = .);
    } > RAM

    .fini_array :
    {
        . = ALIGN(4);
        PROVIDE(__fini_array_start = .);

        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        
        PROVIDE(__fini_array_end = .);
    } > RAM

    PrgData :
    {
        . = ALIGN(4);
        KEEP(*(PrgData PrgData.*))
        . = ALIGN(4);
    } > RAM

    _sidata = LOADADDR(.data);
    /* Data that needs to be initialized to a value different than 0 */
    .data :
    {
        . = ALIGN(4);
        _sdata = .;        /* create a global symbol at data start */
        PROVIDE(__data_init_start = LOADADDR(.data));
        PROVIDE(__data_start = .);

        . = ALIGN(4);
        *(.data .data.* .gnu.linkonce.d.*)
        . = ALIGN(4);
        
        PROVIDE(__data_end = .);
        PROVIDE(__data_init_end = LOADADDR(.data));
        _edata = .;        /* define a global symbol at data end */
    } > RAM

    /* Data that needs to be initialized to 0 */
    .bss (NOLOAD) :
    {
        . = ALIGN(4);
        _sbss = .;         /* define a global symbol at bss start */
        __bss_start__ = _sbss;

        *(.bss .bss.* .gnu.linkonce.b.*)
        *(COMMON);
        . = ALIGN(4);
        _ebss = .;         /* define a global symbol at bss end */
        __bss_end__ = _ebss;
    } > RAM

    /* Flash device information */
    DevDscr :
    {
        . = ALIGN(4);
        KEEP(*(DevDscr DevDscr.*));
        . = ALIGN(4);
    } > RAM

    /* Remove information from the standard libraries */
    /DISCARD/ :
    {
        libc.a ( * )
        libm.a ( * )
        libgcc.a ( * )
        *(.note.GNU-stack)
        *(.comment)
        *(.debug*)
        *(.stack*)
        *(.heap*)
        *(.vectors*)
    }

    .ARM.attributes 0 : { KEEP(*(.ARM.attributes)) }    
}