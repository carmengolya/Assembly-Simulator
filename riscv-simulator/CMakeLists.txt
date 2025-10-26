cmake_minimum_required(VERSION 3.10)
project(RISCV_Simulator C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)

# Source files
set(SRC_FILES
    src/alu.c
    src/assembler.c
    src/cpu.c
    src/decoder.c
    src/encoder.c
    src/memory.c
    main.c
)

# Executable
add_executable(riscv_simulator ${SRC_FILES})
