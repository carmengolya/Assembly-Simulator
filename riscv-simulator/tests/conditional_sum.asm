# Program to compute:
# - The sum of all natural numbers from 1 to n, if n is odd.
# - The value n itself, if n is even.

.data
    n:      .word 5          # The input ('n') used for computation (replace 5 with your value).
    result: .word 0          # Output will be stored here.

.text
main:
    lw x11, 0(x0)           # Load the value of 'n' into x11.
    li x10, 0               # Initialize the sum to 0 in x10.
    li x12, 1               # Initialize the counter to 1 in x12.

    # Check whether 'n' is even
    li x14, 1               # Load the value 1 into x14.
    and x13, x11, x14       # x13 = n & 1 (extract the least significant bit).
    beq x13, x0, is_even    # If x13 == 0, n is even. Jump to is_even.

    # Compute the sum of numbers from 1 to n (if n is odd)
odd_case:
    add x10, x10, x12       # Add the current counter to the sum: sum += counter.
    beq x12, x11, store     # If counter == n, exit the loop.
    addi x12, x12, 1        # Increment the counter: counter += 1.
    jal x0, odd_case        # Jump back to the start of the loop.

is_even:
    # Return 'n' as the result (if n is even)
    add x10, x11, x0        # x10 = n (copy n into the result register x10).

store:
    # Store the result in memory
    sw x10, 4(x0)          # Write the result into the 'result' memory location.