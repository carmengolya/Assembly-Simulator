.data
out:  .word 0

.text
    addi x5, x0, 1
    bne  x5, x0, 12
    addi x2, x0, 0
    sw   x2, 0(x0)
    addi x2, x0, 1
    sw   x2, 0(x0)