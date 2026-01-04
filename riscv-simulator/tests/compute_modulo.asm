# Program to compute the modulo (n % m).

.data
    n:      .word 17      # The dividend (n).
    m:      .word 5       # The divisor (m).
    result: .word 0       # The resulting remainder (n % m) will be stored here.

.text
    main:
        # Load inputs
        lw x11, 0(x0)         # Load the dividend 'n' into x11.
        lw x12, 4(x0)         # Load the divisor 'm' into x12.

        # Perform the modulo operation
        div x13, x11, x12     # x13 = n / m (integer division, floor the result).
        mul x14, x13, x12     # x14 = (n / m) * m (quotient * divisor).
        sub x15, x11, x14     # x15 = n - (n / m * m) (calculate the remainder).

        # Store the result
        sw x15, 8(x0)        # Store the remainder (n % m) in 'result'.