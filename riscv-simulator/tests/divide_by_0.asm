# This test performs division and handles division-by-zero gracefully.

.data
    a:      .word 17         # Dividend (a)
    b:      .word 0          # Divisor (b = 0)
    result: .word 0          # Result placeholder

.text
    main:
        lw x10, 0(x0)            # Load a
        lw x11, 4(x0)            # Load b
        beq x11, x0, error       # Jump to error handler if b == 0.
        div x12, x10, x11        # x12 = a / b
        sw x12, 8(x0)            # Store the result in memory.
        jal x0, end

    error:
        li x12, -1               # Store -1 in result to indicate error.
        sw x12, 8(x0)

    end:                         # Program ends here.