# This test is computing the n-th Fibonacci term.

.data
    n:      .word 8             # The 8th Fibonacci term should be calculated
    fib:    .word 0             # The computed result.

.text
    main:
        lw x11, 0(x0)           # Load n into x11.
        li x10, 0               # F(0) = 0 (F(n-2) in x10)
        li x12, 1               # F(1) = 1 (F(n-1) in x12)
        li x13, 0               # Counter = 0 (start from F(0))

        # Handle base case: n == 0.
        beq x11, x13, store     # If n == 0, store F(0) and exit.

        # Handle base case: n == 1.
        addi x13, x13, 1        # Increment counter to 1.
        beq x11, x13, store     # If n == 1, store F(1) and exit.

    loop:
        # Compute the next Fibonacci number: F(n) = F(n-1) + F(n-2).
        add x14, x10, x12       # x14 = F(n) = F(n-1) + F(n-2)
        add x10, x12, x0        # Update x10 to F(n-2) = F(n-1)
        add x12, x14, x0        # Update x12 to F(n-1) = F(n)

        # Increment the counter and check if we’ve reached the target index.
        addi x13, x13, 1        # Increment counter (x13 += 1)
        bge x13, x11, store     # If counter >= n, exit the loop and store the result.
        jal x0, loop            # Otherwise, repeat the loop to compute the next Fibonacci number.

    store:
        sw x12, 4(x0)           # Store F(n) in the 'fib' memory location