/**************************************************************************
 *                                                                        *
 *                              ESS, Inc.                                 *
 *                                                                        *
 *    ESS, Inc. CONFIDENTIAL AND PROPRIETARY.  This source is the sole    *
 *    property of ESS, Inc.  Reproduction or utilization of this source   *
 *    in whole or in part is forbidden without the written consent of     *
 *    ESS, Inc.                                                           *
 *                                                                        *
 **************************************************************************
 *                                                                        *
 *            (c) Copyright 1997-2009 -- All Rights Reserved              *
 *                                                                        *
 **************************************************************************
 *                                                                        *
 * Name:         lpc2148-flash                                            *
 *                                                                        *
 * Filename:     lpc2148-flash.ld                                         *
 *                                                                        *
 * Description:  Linker control file for ARM7 core LPC2148 running from   *
 *               flash memory.                                            *
 *                                                                        *
 * History:      v 1.0 - Initial release                                  *
 *                                                                        *
 *               v 1.1 - Ported for use with CodeSourcery Tool Chain      *
 *                                                                        *
 **************************************************************************/

/* Reset vector for linker. */
ENTRY(_boot)

/* Stack sizes for C run-time as well as interrupts, supervisor, abort 
   and undefined conditions.  NOTE:  This is the only place you have to
   change the stack size information.  The crt.s assembler uses these values
   to set and clear the stack. */
C_STACK_SIZE   = 0x1000;
IRQ_STACK_SIZE = 0x400;
FIQ_STACK_SIZE = 0x100;
SVC_STACK_SIZE = 0x100;
ABT_STACK_SIZE = 0x100;
UND_STACK_SIZE = 0x100;

MEMORY
{
    flash (rx)        : ORIGIN = 0x00000000, LENGTH = 512K
    ram_isp_low(a)    : ORIGIN = 0x40000120, LENGTH = 223
    ram(rw)           : ORIGIN = 0x40000000, LENGTH = 0x7f00
    ram_isp_high(a)   : ORIGIN = 0x40007FE0, LENGTH = 32
    ram_usb_dma(rw)   : ORIGIN = 0x7FD00000, LENGTH = 8192
}

SECTIONS
{
    /* Interrupt vectors must start at memory 0x00000000. */
    .crt : 
    {
        . = ALIGN(4);
            *crt.o (.text)  
        . = ALIGN(4);
     } >flash

    /* Code (text) that is written to flash. */
    .text :
    {
        *(.text)                        /* all .text sections (code)  */
        *(.rodata)                      /* all .rodata sections (constants, strings, etc.)  */
        *(.rodata*)
        *(.glue_7);                     /* ARM/Thumb inter-networking glue functions  */
        *(.glue_7t);
        
        . = ALIGN(4);
        KEEP(*(.init))
        
        . = ALIGN(4);
        __preinit_array_start = .;
        KEEP (*(.preinit_array))
        __preinit_array_end = .;
        
        . = ALIGN(4);
        __init_array_start = .;
        KEEP (*(SORT(.init_array.*)))
        KEEP (*(.init_array))
        __init_array_end = .;
        
        . = ALIGN(0x4);
        KEEP (*crtbegin.o(.ctors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .ctors))
        KEEP (*(SORT(.ctors.*)))
        KEEP (*crtend.o(.ctors))
        
        . = ALIGN(4);
        KEEP(*(.fini))
        
        . = ALIGN(4);
        __fini_array_start = .;
        KEEP (*(.fini_array))
        KEEP (*(SORT(.fini_array.*)))
        __fini_array_end = .;
        
        KEEP (*crtbegin.o(.dtors))
        KEEP (*(EXCLUDE_FILE (*crtend.o) .dtors))
        KEEP (*(SORT(.dtors.*)))
        KEEP (*crtend.o(.dtors))        
    } >flash
    
    . = ALIGN(4);
  
    /* Location in flash to save initialized data segment. */
    .etext :
    {
        . = ALIGN(4);    
        _etext = .;        
    } >flash
    
    /* All initialized data that is copied to RAM in the crt.s assembler file. */
    .data : AT (_etext)
    {
        . = ALIGN(4);
    
        _data = .;
        *(.data)
        _edata = .;
    } >ram

    /* BSS (Block Started by Symbol) block is the unitilized data segment. */
    .bss : 
    {
        __bss_start = . ;
        *(.bss)
        . = ALIGN (4);
        __bss_end = .;
    } >ram

    /* Stack allocation for the application and interrupts and exceptions. */
    .stack : 
    {
        . = ALIGN(0x100);
    
        __stack_start = . ;

        . += IRQ_STACK_SIZE;
        . = ALIGN (4);
        __irq_stack_top__ = . ;

        . += FIQ_STACK_SIZE;
        . = ALIGN (4);
        __fiq_stack_top__ = . ;

        . += SVC_STACK_SIZE;
        . = ALIGN (4);
        __svc_stack_top__ = . ;

        . += ABT_STACK_SIZE;
        . = ALIGN (4);
        __abt_stack_top__ = . ;

        . += UND_STACK_SIZE;
        . = ALIGN (4);
        __und_stack_top__ = . ;

        . += C_STACK_SIZE;
        . = ALIGN (4);
        __c_stack_top__ = . ;

        __stack_end = .;
    } >ram
    
    end = .;
}

