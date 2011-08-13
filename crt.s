/***************************************************************************
 *                                                                         *
 *                              ESS, Inc.                                  *
 *                                                                         *
 *    ESS, Inc. CONFIDENTIAL AND PROPRIETARY.  This source is the sole     *
 *    property of ESS, Inc.  Reproduction or utilization of this source    *
 *    in whole or in part is forbidden without the written consent of      *
 *    ESS, Inc.                                                            *
 *                                                                         *
 ***************************************************************************
 *                                                                         *
 *            (c) Copyright 1997-2009 -- All Rights Reserved               *
 *                                                                         *
 ***************************************************************************
 *                                                                         *
 * Filename:     crt.s                                                     *
 *                                                                         *
 * Description:  C-runtime library setup, interrupt vectors, and           *
 *               start-up code.  This is generally the only assembly       *
 *               code we have in the system.                               *
 *                                                                         *
 * History:      v 1.0 - Initial release                                   *
 *                                                                         *
 *               v 1.1 - Ported for use with CodeSourcery Tool Chain       *
 *                                                                         *
 ***************************************************************************/

    .global main                    // int main(void)

    .global _etext                  // -> .data initial values in ROM
    .global _data                   // -> .data area in RAM
    .global _edata                  // end of .data area
    .global __bss_start             // -> .bss area in RAM
    .global __bss_end__             // end of .bss area
    .global __stack_start
    .global __stack_end             // top of stack

// Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs
    .set  MODE_USR, 0x10            // User Mode
    .set  MODE_FIQ, 0x11            // FIQ Mode
    .set  MODE_IRQ, 0x12            // IRQ Mode
    .set  MODE_SVC, 0x13            // Supervisor Mode
    .set  MODE_ABT, 0x17            // Abort Mode
    .set  MODE_UND, 0x1B            // Undefined Mode
    .set  MODE_SYS, 0x1F            // System Mode

    .equ  I_BIT, 0x80               // when I bit is set, IRQ is disabled
    .equ  F_BIT, 0x40               // when F bit is set, FIQ is disabled

    .equ STACK_FILL, 0xdeadbeef

    .text
    .code 32
    .align 2

    .global _boot
    .func   _boot
_boot:

// Runtime Interrupt Vectors
// -------------------------
Vectors:
    b     _start                    // reset - _start
    ldr   pc,_undf                  // undefined - _undf
    ldr   pc,_swi                   // SWI - _swi
    ldr   pc,_pabt                  // program abort - _pabt
    ldr   pc,_dabt                  // data abort - _dabt
    nop                             // reserved
    ldr pc, irq_addr           // IRQ - read the VIC
    ldr   pc,_fiq                   // FIQ - _fiq

_undf:  .word __undf                    // undefined
_swi:   .word __swi                     // SWI
_pabt:  .word __pabt                    // program abort
_dabt:  .word __dabt                    // data abort
_irq:   .word __irq                     // IRQ
_fiq:   .word __fiq                     // FIQ

__undf: b     .                         // undefined
__swi:  b     .                         // SWI
__pabt: b     .                         // program abort
__dabt: b     .                         // data abort
__irq:  b     .                         // IRQ
__fiq:  b     .                         // FIQ
        .size _boot, . - _boot
        .endfunc

irq_addr: .word irq_isr
irq_isr:
// Save IRQ context, including the APCS registers, and r4-6
    sub   lr, lr, #4
    stmfd sp!, {r0-r6, ip, lr}

// Save the SPSR_irq register
    mrs r4, spsr

// Read the VICVectAddr */
    ldr r5, VICVECTADDR
    ldr r6, [r5]

// Change to SYS mode and enable IRQ
    msr cpsr_c, #MODE_SYS

// Save the banked SYS mode link register
    stmfd sp!, {lr}

// Call the C-coded handler
    mov lr, pc
    mov pc, r6

// Restore SYS mode link register
    ldmfd sp!, {lr}

// Change to IRQ mode and disable IRQ
    msr cpsr_c, #MODE_IRQ|IRQ_DISABLE

// Restore the SPSR
    msr spsr, r4

// Acknowledge the VIC
    mov r0, #0
    str r0, [r5]

// Restore IRQ context and return from interrupt
    ldmfd sp!, {r0-r6, ip, pc}^

VICVECTADDR:     .word   0xFFFFF030
    .equ IRQ_DISABLE,    (1 << 7)

// Setup the operating mode & stack.
    .global _start
    .func   _start

// Generate 32-bit arm code.
    .code 32

_start:
// Set the IRQ stack pointer.
    msr     CPSR_c,#(MODE_IRQ | I_BIT | F_BIT)
    ldr     sp,=__irq_stack_top__

// Set the FIQ stack pointer.
    msr     CPSR_c,#(MODE_FIQ | I_BIT | F_BIT)
    ldr     sp,=__fiq_stack_top__

// Set the SVC stack pointer.
    msr     CPSR_c,#(MODE_SVC | I_BIT | F_BIT)
    ldr     sp,=__svc_stack_top__

// Set the ABT stack pointer.
    msr     CPSR_c,#(MODE_ABT | I_BIT | F_BIT)
    ldr     sp,=__abt_stack_top__

// Set the UND stack pointer.
    msr     CPSR_c,#(MODE_UND | I_BIT | F_BIT)
    ldr     sp,=__und_stack_top__

// Set the C stack pointer.
    msr     CPSR_c,#(MODE_SYS | I_BIT | F_BIT)
    ldr     sp,=__c_stack_top__


// Copy initialized data to its execution address in RAM
    ldr   r1,=_etext                // -> ROM data start
    ldr   r2,=_data                 // -> data start
    ldr   r3,=_edata                // -> end of data
1:  cmp   r2,r3                     // check if data to move
    ldrlo r0,[r1],#4                // copy it
    strlo r0,[r2],#4
    blo   1b                        // loop until done

// Clear .bss
    mov   r0,#0                     // get a zero
    ldr   r1,=__bss_start           // -> bss start
    ldr   r2,=__bss_end             // -> bss end
2:  cmp   r1,r2                     // check if data to clear
    strlo r0,[r1],#4                // clear 4 bytes
    blo   2b                        // loop until done

// Fill the stack with a known pattern to make it easier to detect stack over flows.
    ldr     r0,=STACK_FILL
    ldr     r1,=__stack_start
    ldr     r2,=__stack_end
3:  cmp     r1,r2
    stmltia r1!,{r0}
    blo     3b

// Call the static C++ constructors.
    LDR     r12,=__libc_init_array
    MOV     lr,pc
    BX      r12


// Jump to the C/C++ entry point.
    b       main

    .size   _start, . - _start
    .endfunc

