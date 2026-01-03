# Program to compute the sum of the first n natural numbers (1 to n).

.data
    n:      .word 5       # The input 'n' needed for the computation of the sum.
    result: .word 0       # The output will be stored here.

.text
    main:
        lw x11, 0(x0)     # Load n from memory.
        li x10, 0         # Initialize the sum to 0.
        li x12, 1         # Initialize counter to 1.

    loop:
        add x10, x10, x12 # Add the counter to the sum.
        beq x12, x11, end # If counter == n, exit loop.
        addi x12, x12, 1  # Increment the counter.
        jal x0, loop      # Jump back to the loop.

    end:
        sw x10, 4(x0)     # Store the result to 'result'