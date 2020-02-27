//—————————— ts.s file of Program C6.1 —————————————————
.text
.code 32
.global vectors_start, vectors_end
.global reset_handler, mkptable
.global get_fault_status, get_fault_addr, get_spsr
.global lock, unlock

reset_handler:
    LDR sp, =svc_stack // set SVC stack
    BL fbuf_init // initialize LCD for display
    BL copy_vector_table // copy vector table to PA 0

    //(m1): build level-1 page table using 1MB sections in C code
    BL mkptable
    // (m2): set TTB register to 0x4000
    mov r0, #0x4000
    mcr p15, 0, r0, c2, c0, 0 // set TTB register
    mcr p15, 0, r0, c8, c7, 0 // flush TLB
    
    //(m3): set domain0 01=client(check permission) 11=master(no check)
    mov r0,#1
    mcr p15, 0, r0, c3, c0, 0
    // 01 for client mode
    //(m4): enable MMU
    mrc p15, 0, r0, c1, c0, 0
    orr r0, r0, #0x00000001
    // get c1 into r0
    // set bit0 to 1
    mcr p15, 0, r0, c1, c0, 0 // write to c1
    nop // time to allow MMU to finish
    nop
    nop
    mrc p15, 0, r2, c2, c0, 0
    // read TLB base reg c2 into r2
    mov r2, r2
    // go in ABT mode to set ABT stack
    MSR cpsr, #0x97
    LDR sp, =abt_stack
    // go in UND mode to set UND stack
    MSR cpsr, #0x9B
    LDR sp, =und_stack
    // go in IRQ mode to set IRQ stack and enable IRQ interrupts
    MSR cpsr, #0x92 // write to cspr, so in IRQ mode now
    LDR sp, =irq_stack // set IRQ stack poiner
    // go back in SVC mode
    MSR cpsr, #0x13
    // SVC mode with IRQ enabled
    // call main() in SVC mode
    BL main
    B .
swi_handler: // dummy swi_handler, not used yet

// unlock/lock: mask in/out IRQ interrupts
unlock:
  MRS r4, cpsr
  BIC r4, r4, #0x80   // clear bit means UNMASK IRQ interrupt
  MSR cpsr, r4
  mov pc, lr	

lock: 
  MRS r4, cpsr
  ORR r4, r4, #0x80   // set bit means MASK off IRQ interrupt 
  MSR cpsr, r4
  mov pc, lr

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
    swi_handler_addr: .word swi_handler
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
