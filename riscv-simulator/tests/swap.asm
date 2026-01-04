# This program swaps two values in memory at fixed offsets (0 and 4).

.data
    value1: .word 42       # Value at offset 0.
    value2: .word 99       # Value at offset 4.

.text
    main:
        lw x10, 0(x0)          # Load value at the first offset into x10.
        lw x11, 4(x0)          # Load value at the second offset into x11.

        sw x11, 0(x0)          # Write the second value to the first offset.
        sw x10, 4(x0)          # Write the first value to the second offset.