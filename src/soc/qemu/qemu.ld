MEMORY {
       rom (rx) : ORIGIN = 0, LENGTH = 1M
       ram (rwx) : ORIGIN = 0x20000000, LENGTH = 1M
}

ENTRY(_start)

SECTIONS {
         .text : {
               . = 0; KEEP(*(.isr_vectors));
               *(.text .text.*)
               *(.rodata .rodata.*)
         } > rom

         . = ALIGN(4);
         _etext = .;

         .bss (NOLOAD) : {
              _sbss = .;
              *(.bss .bss*)
              _ebss = .;
         } > ram

         _sidata = LOADADDR(.data);

        .data : AT(_etext) {
               _sdata = .;
               *(.data .data*)
               . = ALIGN(4);
               _edata = .;
         } > ram

         _end = .;
}
