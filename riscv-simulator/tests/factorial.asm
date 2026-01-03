# This program computes the factorial of 'n', in this case n = 6 (so the result will be 720).

.data
    n:      .word 6          # Input value for which the factorial should be computed (n = 5).
    result: .word 0          # Output placeholder for the computed factorial.

.text
    main:
        # Load the input 'n' into a register.
        lw x11, 0(x0)           # Load n into x11.
        li x10, 1               # Initialize result = 1 (factorial starts from 1).
        li x12, 1               # Initialize counter = 1.

        # Handle base case: if n == 0 or n == 1
        beq x11, x0, store      # If n == 0, store result (1) and exit.
        beq x11, x12, store     # If n == 1, store result (1) and exit.

    loop:
        # Multiply result by the counter.
        mul x10, x10, x12       # result = result * counter.

        # Increment the counter and check termination condition.
        addi x12, x12, 1        # counter += 1.
        blt x12, x11, loop      # If counter < n, continue the loop.
        beq x12, x11, loop      # If counter == n, perform the last multiplication.

    store:
        # Store the computed factorial in the 'result' memory location.
        sw x10, 4(x0)          # Write the result (factorial) to 'result'.