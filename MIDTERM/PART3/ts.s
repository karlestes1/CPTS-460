.text
.code 32
.global reset_handler, vectors_start, vectors_end
.global get_fault_status, get_fault_addr

reset_handler:          // entry point

    // Versatilepb: 256 MB RAM, 2 1 MB I/O sections at 256 MB
    // clear ptable at 0x4000 (16 KB) to 0
    mov r0, #0x4000 // ptable at 0x4000 = 16 KB
    mov r1, #4096 // 4096 entries
    mov r2, #0 // fill all with 0

1:
    str r2, [r0], #4            // store r3 to [r0]; inc r0 by 4
    subs r1, r1, #1             // r1–; set condition flags
    bgt 1b                      // loop r1 = 4096 times
    //(m1): ptable[0] ID map low 1 MB VA to PA
    //      ptable[2048-2295] map VA = [2 GB,2 GB + 258 MB] to low 258 MB PA
    mov r0, #0x4000
    mov r1, r0
    add r1, r1, #(2048*4)       // entry 2048 in ptable[ ]
    mov r2, #256                // r2 = 256
    add r2, r2, #2              // r2 = 258 entries
    mov r3, #0x100000           // r3 = 1 M increments
    mov r4, #0x400              // r4 = AP = 01 (KRW, user no) AP = 11: both KU r/w
    orr r4, r4, #0x12           // r4 = 0x412 (OR 0xC12 if AP = 11)
    str r4, [r0]                // ptable[0]

// ptable[2048-2257] map to low 258 MB PA
2:
    str r4, [r1], #4 // store r4 to [r1]; inc r1 by 4
    add r4, r4, r3 // inc r4 by 1 M
    subs r2,r2, #1 // r2–
    bgt 2b // loop r2 = 258 times
    
    //(m2): set TTB register pointing at pgtable at 0x4000
    mov r0, #0x4000
    mcr p15, 0, r0, c2, c0, 0 // set TTBR with PA = 0x4000
    mcr p15, 0, r0, c8, c7, 0 // flush TLB
    
    
    //(m3): set domain0: 01 = client(check permission)11 = manager(no check)
    mov r0, #0x1 // b01 for CLIENT
    mcr p15, 0, r0, c3, c0, 0 // write to domain REG c3
    
    
    //(m4): enable MMU
    mov r0, #0x1
    mcr p15, 0, r0, c1, c0, 0 // read control REG to r0
    orr r0, r0, #0x00000001 // set bit0 of r0 to 1
    mcr p15, 0, r0, c1, c0, 0 // write to control REG c1 ==> MMU on
    nop
    nop
    nop
    mrc p15, 0, r2, c2, c0, 0 // read TLB base reg c2 into r2
    mov r2, r2 // time to allow MMU to finish
    // set SVC stack to HIGH END of svc_stack[ ]
    LDR r0, = svc_stack // r0 points svc_stack[]
    
    ADD r1, r0, #4096 // r1 - >  high end of svc_stack[]
    MOV sp, r1
    
    // set IRQ stack and enable IRQ interrupts
    MSR cpsr, #0x92 // write to cspr
    ldr sp, = irq_stack // u32 irq_stack[1024] in t.c
    add sp, sp, #4096 // ensure it's a VA from 2 GB
    
    // set ABT stack */
    MSR cpsr, #0x97
    LDR sp, = abt_stack_top
    
    // go back to SVC mode
    MSR cpsr, #0x93 // SVC mode with IRQ off
    BL copy_vector_table // copy vector table to PA 0
    MSR cpsr, #0x13 // SVC mode with IRQ on
    BL main // call main() in C
    B .

swi_handler: // dummy swi_handler, not used yet

data_handler:
    sub lr, lr, #4
    stmfd sp!, {r0-r12, lr}
    bl data_chandler // call data_chandler() in C
    ldmfd sp!, {r0-r12, pc}^

irq_handler:
    sub lr, lr, #4
    stmfd sp!, {r0-r12, lr}
    bl irq_chandler
    ldmfd sp!, {r0-r12, pc}^

vectors_start: // vector table
    LDR PC, reset_handler_addr
    LDR PC, undef_handler_addr
    LDR PC, swi_handler_addr
    LDR PC, prefetch_abort_handler_addr
    LDR PC, data_abort_handler_addr
    B .
    LDR PC, irq_handler_addr
    LDR PC, fiq_handler_addr
    reset_handler_addr: .word reset_handler
    undef_handler_addr: .word undef_handler
    svc_handler_addr: .word swi_entry
    prefetch_abort_handler_addr: .word prefetch_abort_handler
    data_abort_handler_addr: .word data_handler
    irq_handler_addr: .word irq_handler
    fiq_handler_addr: .word fiq_handler
vectors_end:

get_fault_status: // read and return MMU reg 5
    MRC p15,0,r0,c5,c0,0 // read DFSR c5
    mov pc, lr
get_fault_addr: // read and return MMU reg 6
    MRC p15,0,r0,c6,c0,0  // read DFAR
    mov pc, lr  
get_spsr: // get SPSR
    mrs r0, spsr
    mov pc, lr
    
    
.end
