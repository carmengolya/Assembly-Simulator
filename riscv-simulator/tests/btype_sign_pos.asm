.data
in:   .word 7
out:  .word 0

.text
    lw   x5, 0(x0)

    blt  x5, x0, 20
    beq  x5, x0, 16

    addi x2, x0, 1
    sw   x2, 4(x0)
    beq  x0, x0, 12

    addi x2, x0, 0
    sw   x2, 4(x0)