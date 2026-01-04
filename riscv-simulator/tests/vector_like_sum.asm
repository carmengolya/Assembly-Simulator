# This program sums a sequence of words located at absolute memory
# addresses 0, 4, 8, 12, and 16, and stores the result at address 24.

.data
    val0: .word 3           # First word in memory.
    val1: .word 5           # Second word in memory.
    val2: .word 12          # Third word in memory.
    val3: .word 7           # Fourth word in memory.
    val4: .word 9           # Fifth word in memory.
    result: .word 0         # Location intended to hold the sum.

.text
    main:
        li x11, 20              # Loop limit value.
        li x13, 0               # Memory address pointer.
        li x10, 0               # Accumulator initialized to 0.

    loop:
        lw x12, 0(x13)          # Load word from address in x13.
        add x10, x10, x12       # Add loaded value to accumulator.
        addi x13, x13, 4        # Advance address by one word (4 bytes).
        bne x13, x11, loop      # Repeat until x13 reaches loop limit.

    end:
        sw x10, 24(x0)          # Store sum to absolute address 24.