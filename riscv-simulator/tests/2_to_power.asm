# This program computes 2^n using the SLL instruction.

.data
    n:      .word 10         # Exponent
    result: .word 0          # Output placeholder

.text
    main:
        lw x11, 0(x0)            # Load the value of n.
        li x10, 1                # Start with 2^0 = 1.

        sll x10, x10, x11        # Left shift 1 by n bits to compute 2^n.

        sw x10, 4(x0)            # Store the result at offset 4.