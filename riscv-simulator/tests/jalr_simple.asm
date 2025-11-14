.data
out: .word 0

.text
    auipc x5, 0
    addi  x5, x5, 20

    jalr x1, 0(x5)

    addi x2, x0, 1
    sw   x2, 4(x0)

target:
    addi x2, x0, 3
    sw   x2, 4(x0)