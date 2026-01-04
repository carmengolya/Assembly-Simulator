# This program calculates the number of set bits in a number.

.data
    number: .word 29         # Number (29 = 0b11101)
    result: .word 0          # Placeholder for the count.

.text
    main:
        lw x10, 0(x0)            # Load the number into x10.
        li x11, 0                # Initialize the count to 0.
        li x12, 32               # Total number of bits in a word.
        li x13, 1                # Constant 1 for bit masking.
        li x14, 2                # Constant 2 for division by 2.

    loop:
        and x15, x10, x13        # Extract the least significant bit (x15 = x10 & 1).
        add x11, x11, x15        # Add the bit value to the count (count += x15).
        div x10, x10, x14        # Right-shift the number by 1 bit by dividing it bt 2.
        sub x12, x12, x13        # Decrement the bit counter (x12 -= 1).
        bne x12, x0, loop        # Repeat until all bits are processed.

    end:
        sw x11, 4(x0)            # Store the count in the result.