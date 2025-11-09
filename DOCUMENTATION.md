# RISC-V Assembly Simulator - Project Documentation

## Table of Contents

1. [Project Overview](#project-overview)
2. [Architecture](#architecture)
3. [Core Modules](#core-modules)
   - [Instruction Module](#instruction-module)
   - [CPU Module](#cpu-module)
   - [ALU Module](#alu-module)
   - [Memory Module](#memory-module)
   - [Assembler Module](#assembler-module)
   - [Encoder Module](#encoder-module)
   - [Decoder Module](#decoder-module)
4. [Main Entry Point](#main-entry-point)
5. [Build System](#build-system)
   - [CMakeLists.txt](#cmakeliststxt)
   - [makefile](#makefile)
6. [Testing](#testing)

---

## Project Overview

This project is a **RISC-V Assembly Simulator** written in C. It parses RISC-V assembly files, encodes them into 32-bit machine instructions, loads them into a simulated memory, and executes them on a simulated CPU. The simulator supports multiple RISC-V instruction formats including R-Type, I-Type, S-Type, U-Type, and B-Type instructions.

**Key Features:**
- Full assembly parsing with label resolution
- Support for RISC-V RV32I base instruction set
- Simulated 32-register CPU with program counter
- Memory management with bounds checking
- Instruction encoding and decoding
- Branch/jump support with label resolution
- Data section support for initialized data

---

## Architecture

The simulator follows a classic fetch-decode-execute cycle:

```
Assembly File (.asm) → Parser → Symbol Table → Encoder → Memory → CPU (Fetch-Decode-Execute)
```

**Flow:**
1. **Parse**: Read assembly file and build instruction/data structures
2. **Build Symbol Table**: Resolve labels and their addresses
3. **Encode**: Convert assembly instructions to 32-bit machine code
4. **Load**: Place encoded instructions and data into simulated memory
5. **Execute**: CPU fetches, decodes, and executes instructions sequentially

---

## Core Modules

### Instruction Module

**Files:** `include/instruction.h`

**Purpose:** Defines RISC-V instruction formats and provides inline functions for encoding/decoding instruction fields.

**Key Components:**

- **Instruction Formats Supported:**
  - **R-Type**: Register-register operations (ADD, SUB, AND, OR, XOR, shifts)
  - **I-Type**: Immediate operations and loads (ADDI, LW, JALR)
  - **S-Type**: Store operations (SW, SH, SB)
  - **U-Type**: Upper immediate operations (LUI, AUIPC)
  - **B-Type**: Branch operations (BEQ, BNE, BLT, BGE)

- **Inline Helper Functions:**
  - `rtype_get_*()`: Extract fields from R-Type instructions (opcode, rd, rs1, rs2, funct3, funct7)
  - `itype_get_*()`: Extract fields from I-Type instructions with sign extension for immediates
  - `stype_get_*()`: Extract fields from S-Type instructions with split immediate reassembly
  - `utype_get_*()`: Extract fields from U-Type instructions
  - `btype_get_*()`: Extract fields from B-Type instructions with complex immediate reconstruction

- **Data Structures:**
  - `EncodedInstruction`: Union type that can represent any instruction format
  - Each type has a 32-bit value field containing the encoded instruction

- **Bit Layout Compliance:** All functions follow official RISC-V ISA specification bit layouts

---

### CPU Module

**Files:** `include/cpu.h`, `src/cpu.c`

**Purpose:** Implements the CPU emulation including fetch-decode-execute cycle.

**Key Components:**

- **CPU Structure (cpu.h):**
  - 32 general-purpose registers (`regs[32]`), where `x0` (zero) is hardwired to 0
  - Program counter (`pc`) for tracking current instruction address
  - Pointers to memory and assembly program for runtime access
  - Execution statistics (instructions executed, halted flag, error flag)

- **Main Functions:**
  - `cpu_init()`: Initialize CPU with zeroed registers and reset state
  - `cpu_init_with_program()`: Initialize CPU with memory and program references
  - `cpu_set_reg()` / `cpu_get_reg()`: Register access with x0 protection

- **Execution Pipeline (cpu.c):**
  - `cpu_fetch()`: Fetch instruction from memory at current PC
  - `cpu_decode()`: Decode instruction format and extract fields
  - `cpu_execute()`: Execute the instruction and update CPU state
    - R-Type: ALU operations (ADD, SUB, AND, OR, XOR, shifts)
    - I-Type: Immediate ALU ops and loads (ADDI, LW, JALR)
    - S-Type: Store operations (SW)
    - U-Type: Upper immediate (LUI, AUIPC)
    - B-Type: Conditional branches (BEQ, BNE, BLT, BGE, BLTU, BGEU)
    - J-Type: Jumps (JAL)
  - `cpu_writeback()`: Write results back to destination register

- **Control Flow:**
  - `cpu_step()`: Execute one instruction (fetch → decode → execute)
  - `cpu_run()`: Loop that executes until program halts or error occurs
  - PC auto-increment by 4 (word-aligned) or branch target update

- **Special Instructions:**
  - Pseudo-halt: `addi x0, x0, 0` (NOP that sets halt flag)
  - Link register: Saves return address (PC+4) for JAL/JALR

- **Debugging:**
  - `cpu_print_registers()`: Shows all 32 registers in hex and decimal
  - `cpu_print_state()`: Shows PC, execution count, and halt/error flags

---

### ALU Module

**Files:** `include/alu.h`, `src/alu.c`

**Purpose:** Implements the Arithmetic Logic Unit operations.

**Key Components:**

- **ALU Operations (alu.h):**
  - `ALU_ADD`: Addition (signed/unsigned)
  - `ALU_SUB`: Subtraction (signed/unsigned)
  - `ALU_XOR`: Bitwise exclusive OR
  - `ALU_OR`: Bitwise OR
  - `ALU_AND`: Bitwise AND
  - `ALU_SLL`: Logical shift left
  - `ALU_SRL`: Logical shift right (zero-fill)
  - `ALU_SRA`: Arithmetic shift right (sign-extend)

- **Implementation (alu.c):**
  - `alu_execute()`: Switch statement that performs the requested operation
  - All operations work on 32-bit signed integers (`int32_t`)

- **Operations Details:**
  - **ADD/SUB**: Standard integer arithmetic (wraps on overflow per RISC-V spec)
  - **XOR/OR/AND**: Bitwise logical operations
  - **SLL**: Logical left shift (fills with zeros)
  - **SRL**: Logical right shift (fills with zeros)
  - **SRA**: Arithmetic right shift (sign-extends)

- **Shift Operations:**
  - Mask shift amount with 0x1F (use only lower 5 bits)
  - Ensures shift amount is 0-31 per RISC-V specification
  - SLL/SRL cast to unsigned for proper behavior

- **Design Philosophy:**
  - Pure function with no state or side effects
  - Easy to test and verify
  - Reusable for both R-Type and I-Type instructions

---

### Memory Module

**Files:** `include/memory.h`, `src/memory.c`

**Purpose:** Implements the simulated memory system with byte-addressable storage.

**Key Components:**

- **Memory Structure (memory.h):**
  - Dynamic byte array (`uint8_t *data`)
  - Size tracking for bounds checking
  - Little-endian byte ordering

- **Memory Management:**
  - `memory_init()`: Allocate and zero-initialize memory of specified size
  - `memory_free()`: Deallocate memory and reset structure

- **Memory Access (memory.c):**
  - `memory_read32()`: Read 32-bit word from memory (little-endian), assembles 4 bytes
  - `memory_write32()`: Write 32-bit word to memory (little-endian), splits into 4 bytes
  - Bounds checking on all accesses to prevent buffer overruns

- **Program Loading:**
  - `load_program_into_memory()`: Load encoded instructions into memory at base address
  - `load_data_into_memory()`: Load data section values into memory after instructions
  - Data section typically placed after instruction section

- **Memory Model:**
  - Byte-addressable (smallest unit is 1 byte)
  - Little-endian byte ordering (LSB at lowest address)
  - Dynamic allocation based on program needs

- **Debugging:**
  - `memory_dump_words()`: Display memory contents as 32-bit words in hex format
  - Shows address and value for each word
  - Automatically limits output to available memory

- **Safety:**
  - All functions validate pointers and bounds
  - Error messages printed for out-of-bounds access
  - Prevents buffer overruns and segmentation faults

---

### Assembler Module

**Files:** `include/assembler.h`, `src/assembler.c`

**Purpose:** Parses RISC-V assembly files and builds internal representation.

**Key Components:**

- **Data Structures (assembler.h):**
  - `Instruction`: Represents a parsed assembly instruction with label, opcode, operands, line number, and address
  - `DataEntry`: Represents a data directive with label, value, and address
  - `Symbol`: Represents a label with its resolved memory address
  - `AssemblyProgram`: Container for all instructions, data entries, and symbols

- **Constants:**
  - Maximum sizes for labels (50), opcodes (10), operands (3 per instruction, 20 chars each)
  - Program limits: 1024 instructions, 1024 data entries, 1024 symbols
  - Maximum line size: 1024 characters

- **Main Functions:**
  - `read_asm_file()`: Parse assembly file and populate AssemblyProgram structure
  - `print_program()`: Display parsed program for debugging
  - `find_symbol()`: Look up address of a label/symbol

- **Parsing Process (assembler.c):**
  - Reads assembly file line by line
  - Strips comments (// and # style, also /* */ block comments)
  - Removes whitespace
  - Identifies labels, opcodes, and operands
  - Builds symbol table with label addresses

- **Helper Functions:**
  - `eliminate_comment()`: Removes line comments (// and #)
  - `eliminate_block_comments()`: Removes /* ... */ block comments
  - `parse_operands()`: Splits operand string by commas and trims whitespace
  - `build_symbol_table()`: First pass to collect all labels and assign addresses

- **Data Section:**
  - Supports `.word` directive for initialized data
  - Parses label, value, and assigns offset address
  - Data can be referenced by labels in instructions

- **Two-Pass Design:**
  - **Pass 1**: Collect labels and build symbol table
  - **Pass 2**: Parse instructions with known label addresses

- **Error Handling:**
  - Returns negative values on parse errors
  - Includes line numbers in error messages
  - Validates file exists and can be opened

---

### Encoder Module

**Files:** `include/encoder.h`, `src/encoder.c`

**Purpose:** Converts parsed assembly instructions into 32-bit RISC-V machine code.

**Key Components:**

- **Main Function (encoder.h):**
  - `encode_instruction()`: Takes an `Instruction` and `AssemblyProgram` (for symbol lookup) and returns 32-bit encoded value

- **Builder Helper Functions (encoder.c):**
  - `build_rtype()`: Constructs R-Type instruction from fields (funct7, rs2, rs1, funct3, rd, opcode)
  - `build_itype()`: Constructs I-Type instruction with 12-bit immediate
  - `build_stype()`: Constructs S-Type instruction with split immediate (imm[11:5] and imm[4:0])
  - `build_utype()`: Constructs U-Type instruction with 20-bit immediate
  - `build_btype()`: Constructs B-Type instruction with complex immediate encoding

- **Encoding Process:**
  - Identifies instruction mnemonic (ADD, LW, BEQ, etc.)
  - Maps to appropriate encoding function
  - Parses registers using `reg_index()` from decoder
  - Parses immediates as decimal, hex, or label references

- **Label Resolution:**
  - `parse_branch_imm_or_label()`: Resolves labels to PC-relative offsets
  - Computes offset = target_address - current_address
  - Validates offset fits in instruction's immediate field
  - Ensures branch offsets are even (word-aligned)

- **Validation Helpers:**
  - `fits_imm12()`: Checks if value fits in 12-bit signed immediate (-2048 to 2047)
  - `fits_imm20()`: Checks if value fits in 20-bit signed immediate
  - `fits_branch_offset()`: Validates branch offset range and alignment

- **Supported Instructions:**
  - **R-Type**: ADD, SUB, AND, OR, XOR, SLL, SRL, SRA
  - **I-Type**: ADDI, XORI, ORI, ANDI, SLLI, SRLI, SRAI, LW, JALR
  - **S-Type**: SW
  - **U-Type**: LUI, AUIPC
  - **B-Type**: BEQ, BNE, BLT, BGE, BLTU, BGEU
  - **J-Type**: JAL

- **Pseudo-Instructions:**
  - `LI` (load immediate): Expanded to ADDI or LUI+ADDI depending on value
  - `MV` (move): Encoded as ADDI with 0 immediate
  - `NOP`: Encoded as ADDI x0, x0, 0

- **Error Handling:**
  - Returns 0 on encoding failure
  - Detailed error messages with line numbers
  - Validates all register indices and immediate ranges

---

### Decoder Module

**Files:** `include/decoder.h`, `src/decoder.c`

**Purpose:** Provides utility functions for parsing assembly operands and register names.

**Key Components:**

- **Main Functions (decoder.h):**
  - `reg_index()`: Converts register name to index (0-31)
  - `parse_immediate()`: Parses immediate values from strings
  - `parse_memory_operand()`: Parses "offset(register)" format for load/store instructions

- **Register Parsing (decoder.c):**
  - Supports numeric format (x0-x31)
  - Supports ABI names (zero, ra, sp, gp, tp, t0-t6, s0-s11, a0-a7, fp)
  - Case-insensitive matching
  - Validates register numbers are in range 0-31

- **Register Aliases:**
  - Complete RISC-V ABI name mapping:
    - x0 = zero (hardwired to 0)
    - x1 = ra (return address)
    - x2 = sp (stack pointer)
    - x3 = gp (global pointer)
    - x4 = tp (thread pointer)
    - x5-x7 = t0-t2 (temporaries)
    - x8-x9 = s0-s1, fp (saved registers, frame pointer)
    - x10-x17 = a0-a7 (function arguments/return values)
    - x18-x27 = s2-s11 (saved registers)
    - x28-x31 = t3-t6 (temporaries)

- **Immediate Parsing:**
  - Converts string to 32-bit signed integer
  - Supports multiple formats:
    - Decimal: 42, -10
    - Hexadecimal: 0x2A, 0xFF
    - Octal: 077
  - Optional '#' prefix for ARM-style syntax compatibility

- **Memory Operand Parsing:**
  - Parses "offset(register)" format used for load/store instructions
  - Examples: `0(sp)`, `4(a0)`, `-8(s0)`
  - Extracts offset value and base register index
  - Validates parentheses and register name

- **Helper Functions:**
  - `trim_inplace()`: Removes leading/trailing whitespace
  - `to_lower_inplace()`: Converts string to lowercase for case-insensitive comparison

- **Error Handling:**
  - Returns -1 for invalid register names
  - Returns -1 for malformed memory operands
  - Prints descriptive error messages

---

## Main Entry Point

### main.c

**Purpose:** Entry point for the simulator - orchestrates the complete simulation pipeline.

**Key Components:**

- **Execution Pipeline (8 Steps):**
  1. **Parse Assembly File**: Read .asm file and build program structure
  2. **Initialize Memory**: Allocate simulated memory (400 bytes default)
  3. **Encode Instructions**: Convert all assembly instructions to machine code
  4. **Load Program**: Write encoded instructions to memory starting at address 0x0
  5. **Load Data Section**: Write data values to memory after instructions
  6. **Initialize CPU**: Set up CPU with zeroed registers and memory reference
  7. **Execute Program**: Run CPU until halt or error
  8. **Print Results**: Display final CPU state and execution statistics

- **Command Line:**
  - Takes one argument: path to assembly file
  - Example: `./riscv_simulator tests/add_sub.asm`

- **Error Handling:**
  - Validates each step and aborts on failure
  - Returns non-zero exit code on errors

- **Debugging Output:**
  - Detailed logging for each execution step
  - Memory dumps before and after execution
  - Register state at start and end
  - Instruction count and halt status

- **Memory Management:**
  - Properly frees all allocated resources
  - Memory cleanup on both success and error paths

---

## Build System

### CMakeLists.txt

**Purpose:** CMake build configuration for the RISC-V simulator.

**Key Points:**
- **Project Configuration:**
  - Project name: RISCV_Simulator
  - Language: C
  - Minimum CMake version: 3.10
  - C standard: C99 (required)

- **Source Organization:**
  - Include directory: `include/` (contains all .h headers)
  - Source files: All .c files in `src/` plus `main.c`
  
- **Build Target:**
  - Executable name: `riscv_simulator`
  - Includes all source and header files
  - Output in build directory

- **Usage:**
  ```bash
  mkdir build && cd build
  cmake ..
  make
  ```

- **Build Types:**
  - Debug: `-DCMAKE_BUILD_TYPE=Debug` (with debugging symbols)
  - Release: `-DCMAKE_BUILD_TYPE=Release` (optimized, default)

---

### makefile

**Purpose:** High-level makefile that wraps CMake and provides convenient targets for building and testing.

**Key Points:**
- **Primary Targets:**
  - `make` or `make all`: Build the simulator (default target)
  - `make sim`: Build the simulator executable
  - `make test`: Run all test files and display summary
  - `make run TEST=<file.asm>`: Run a single test file
  - `make clean`: Clean build artifacts (keep CMake cache)
  - `make distclean`: Remove entire build directory
  - `make rebuild`: Full clean and rebuild

- **Configuration:**
  - Build directory: `build/`
  - Test directory: `tests/`
  - Results directory: `tests/results/`
  - Target executable: `build/riscv_simulator`

- **Testing Features:**
  - Auto-discovers all .asm files in tests/
  - Runs each test and captures output to .log file
  - Generates pass/fail summary
  - Exit code 0 if all tests pass, 1 if any fail

- **Test Output:**
  - Each test produces `tests/results/<testname>_out.log`
  - Logs include full simulator output for debugging
  - Results directory created automatically if missing

- **Build Variables:**
  - `BUILD_TYPE`: Release (default) or Debug
  - `CMAKE_ARGS`: Additional CMake arguments
  - `TEST`: Specify test file for `make run`

- **Example Usage:**
  ```bash
  make                          # Build simulator
  make test                     # Run all tests
  make run TEST=add_sub.asm     # Run specific test
  make BUILD_TYPE=Debug sim     # Debug build
  make rebuild                  # Clean and rebuild
  ```

- **Phony Targets:**
  - All targets properly marked as .PHONY to avoid file conflicts
  - Ensures targets run even if files with same names exist

---

## Testing

**Test Directory:** `riscv-simulator/tests/`

**Test Files:**
1. **add_sub.asm**: Tests basic ADD and SUB instructions with register operations
2. **btype_bne_taken.asm**: Tests BNE (branch not equal) instruction when branch is taken
3. **btype_no_labels.asm**: Tests branch instructions using immediate offsets instead of labels
4. **btype_sign_neg.asm**: Tests signed comparison branches with negative values
5. **btype_sign_pos.asm**: Tests signed comparison branches with positive values
6. **btype_sign_zero.asm**: Tests signed comparison branches with zero values
7. **lui_auipc.asm**: Tests LUI (load upper immediate) and AUIPC (add upper immediate to PC)
8. **shifts_bitwise.asm**: Tests shift operations (SLL, SRL, SRA) and bitwise operations (AND, OR, XOR)

**Running Tests:**
```bash
cd riscv-simulator
make test                    # Run all tests with summary
make run TEST=add_sub.asm    # Run specific test
```

**Test Results:**
- All tests currently pass (8/8)
- Test logs saved to `tests/results/` directory
- Each log contains full execution trace including:
  - Parsed instructions
  - Encoded machine code
  - Memory dumps
  - Register states
  - Execution statistics

**Test Coverage:**
- ✓ Arithmetic operations (ADD, SUB, ADDI)
- ✓ Logical operations (AND, OR, XOR)
- ✓ Shift operations (SLL, SRL, SRA)
- ✓ Memory operations (LW, SW)
- ✓ Branch operations (BEQ, BNE, BLT, BGE)
- ✓ Upper immediate (LUI, AUIPC)
- ✓ Label resolution
- ✓ Pseudo-instructions (LI, MV)

---

## Project Structure Summary

```
Assembly-Simulator/
├── README.md                          # Project README
├── DOCUMENTATION.md                   # This file - comprehensive project documentation
├── .gitignore                         # Git ignore rules for build artifacts
└── riscv-simulator/
    ├── CMakeLists.txt                 # CMake build configuration
    ├── makefile                       # High-level build and test automation
    ├── main.c                         # Simulator entry point and execution pipeline
    ├── include/                       # Header files (module interfaces)
    │   ├── instruction.h              # Instruction module - RISC-V formats
    │   ├── cpu.h                      # CPU module - interface
    │   ├── alu.h                      # ALU module - interface
    │   ├── memory.h                   # Memory module - interface
    │   ├── assembler.h                # Assembler module - interface
    │   ├── encoder.h                  # Encoder module - interface
    │   └── decoder.h                  # Decoder module - interface
    ├── src/                           # Source files (module implementations)
    │   ├── cpu.c                      # CPU module - implementation
    │   ├── alu.c                      # ALU module - implementation
    │   ├── memory.c                   # Memory module - implementation
    │   ├── assembler.c                # Assembler module - implementation
    │   ├── encoder.c                  # Encoder module - implementation
    │   └── decoder.c                  # Decoder module - implementation
    └── tests/                         # Test assembly programs
        ├── add_sub.asm
        ├── btype_bne_taken.asm
        ├── btype_no_labels.asm
        ├── btype_sign_neg.asm
        ├── btype_sign_pos.asm
        ├── btype_sign_zero.asm
        ├── lui_auipc.asm
        ├── shifts_bitwise.asm
        └── results/                   # Test output logs (generated)
```

**Module Organization:**
Each module consists of a header file (`.h`) defining the interface and a source file (`.c`) containing the implementation and helper functions. The header files are in `include/` and source files are in `src/`.

---

**Documentation Version:** 1.0  
**Last Updated:** 2025-11-09  
**Project:** RISC-V Assembly Simulator  
**Language:** C (C99 Standard)  
**Build System:** CMake 3.10+ with Make wrapper
