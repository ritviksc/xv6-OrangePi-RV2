OUTPUT_ARCH("riscv")
ENTRY(_entry)

SECTIONS
{
  /*
   * ensure that entry.S / _entry is at 0x11000000
   */
  . = 0x11000000;

  .text : {
    . = ALIGN(16);
    *(.text._entry)
    *(.text .text.*)
    PROVIDE(etext = .);
  }

  .rodata : {
    . = ALIGN(16);
    *(.srodata .srodata.*) /* do not need to distinguish this from .rodata */
    . = ALIGN(16);
    *(.rodata .rodata.*)
  }

  .data : {
    . = ALIGN(16);
    *(.sdata .sdata.*) /* do not need to distinguish this from .data */
    . = ALIGN(16);
    *(.data .data.*)
  }

  .bss : {
    . = ALIGN(16);
    bss_start = .;
    *(.sbss .sbss.*) /* do not need to distinguish this from .bss */
    . = ALIGN(16);
    *(.bss .bss.*)
    bss_end = .;
  }

  PROVIDE(end = .);
}
