        .text
        .global start
start:  ldr sp, =stack_top
        b1  sum
stop:   b   stop

sum:
        stmfd