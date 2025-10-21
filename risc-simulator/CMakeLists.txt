cmake_minimum_required(VERSION 3.10)
project(RISC_Simulator C)

# Set C standard
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Include directories
include_directories(include)

# Source files
set(SRC_FILES
    src/alu.c
    src/cpu.c
    src/control_unit.c
    src/instructions.c
    src/memory.c
    main.c
)

# Executable
add_executable(risc_simulator ${SRC_FILES})
