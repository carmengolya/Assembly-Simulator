.data
out: .word 0

.text
    # PC=0: jal x1, target  (rd=x1)
    jal x1, skip
    addi x2, x0, 1
    sw   x2, 4(x0)

skip:
    addi x2, x0, 2
    sw   x2, 4(x0)