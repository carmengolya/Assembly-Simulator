.data
val1: .word 15   // x2 = 15
val2: .word 2    // x3 = 2

.text
lw x2, 0(x0)
lw x3, 4(x0)

sll x4, x2, x3
srl x5, x2, x3
sra x6, x2, x3

and x7, x2, x3
or  x8, x2, x3
xor x9, x2, x3

sw x4, 8(x0)
sw x5, 12(x0)
sw x6, 16(x0)
sw x7, 20(x0)
sw x8, 24(x0)
sw x9, 28(x0)
