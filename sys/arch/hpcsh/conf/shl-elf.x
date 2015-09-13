/*	$NetBSD$	*/

OUTPUT_FORMAT("elf32-shl-nbsd")
OUTPUT_ARCH(sh)
ENTRY(start)

MEMORY
{
  ram (a) : o = 0x8c001000, l = 16M
}
SECTIONS

{
  .text :
  {
    ftext = . ;
    *(.text)
    *(.rodata)
    *(.strings)
  }
  etext = . ;
  PROVIDE (etext = .);
  . = ALIGN(8);
  .data :
  {
    fdata = . ;
    PROVIDE (fdata = .);
    *(.data)
    CONSTRUCTORS
  }
  edata = . ;
  PROVIDE (edata = .);
  . = ALIGN(8);
  .bss :
  {
    fbss = . ;
    PROVIDE (fbss = .);
    *(.bss)
    *(COMMON)
  }
  . = ALIGN(4);
  end = . ;
  PROVIDE (end = .);
}
