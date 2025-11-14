.data
out:    .word 0

.text
    li   x2, 2
    li   x3, 6

    mul  x4, x2, x3

    li   x6, 3

    div  x5, x4, x6

    sw   x5, 0(x0)