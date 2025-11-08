# RISC-V Assembly Simulator - Project Documentation

## Table of Contents

1. [Project Overview](#project-overview)
2. [Architecture](#architecture)
3. [Header Files](#header-files)
   - [instruction.h](#instructionh)
   - [cpu.h](#cpuh)
   - [alu.h](#aluh)
   - [memory.h](#memoryh)
   - [assembler.h](#assemblerh)
   - [encoder.h](#encoderh)
   - [decoder.h](#decoderh)
4. [Source Files](#source-files)
   - [main.c](#mainc)
   - [cpu.c](#cpuc)
   - [alu.c](#aluc)
   - [memory.c](#memoryc)
   - [assembler.c](#assemblerc)
   - [encoder.c](#encoderc)
   - [decoder.c](#decoderc)
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

## Header Files

### instruction.h

**Purpose:** Defines RISC-V instruction formats and provides inline functions for encoding/decoding instruction fields.

**Key Points:**
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

### cpu.h

**Purpose:** Defines the CPU structure and its interface for fetching, decoding, and executing instructions.

**Key Points:**
- **CPU Structure:**
  - 32 general-purpose registers (`regs[32]`), where `x0` (zero) is hardwired to 0
  - Program counter (`pc`) for tracking current instruction address
  - Pointers to memory and assembly program for runtime access
  - Execution statistics (instructions executed, halted flag, error flag)

- **Core Functions:**
  - `cpu_init()`: Initialize CPU with zeroed registers and reset state
  - `cpu_init_with_program()`: Initialize CPU with memory and program references
  - `cpu_set_reg()` / `cpu_get_reg()`: Register access with x0 protection

- **Execution Pipeline:**
  - `cpu_fetch()`: Fetch instruction from memory at current PC
  - `cpu_decode()`: Decode instruction format and extract fields
  - `cpu_execute()`: Execute the instruction and update CPU state
  - `cpu_writeback()`: Write results back to destination register

- **Control Flow:**
  - `cpu_step()`: Execute one instruction (fetch → decode → execute)
  - `cpu_run()`: Execute program until halt or error condition

- **Debugging:**
  - `cpu_print_registers()`: Display all register values
  - `cpu_print_state()`: Display comprehensive CPU state

---

### alu.h

**Purpose:** Defines the Arithmetic Logic Unit operations supported by the simulator.

**Key Points:**
- **ALU Operations:**
  - `ALU_ADD`: Addition (signed/unsigned)
  - `ALU_SUB`: Subtraction (signed/unsigned)
  - `ALU_XOR`: Bitwise exclusive OR
  - `ALU_OR`: Bitwise OR
  - `ALU_AND`: Bitwise AND
  - `ALU_SLL`: Logical shift left
  - `ALU_SRL`: Logical shift right (zero-fill)
  - `ALU_SRA`: Arithmetic shift right (sign-extend)

- **Function:**
  - `alu_execute()`: Performs the specified operation on two 32-bit operands
  - Returns 32-bit result
  - Handles shift operations with proper masking (only lower 5 bits used)

- **Design Philosophy:**
  - Pure function with no side effects
  - Supports both signed and unsigned arithmetic through casting
  - Used by CPU during execution of arithmetic/logical instructions

---

### memory.h

**Purpose:** Defines the simulated memory system with byte-addressable storage.

**Key Points:**
- **Memory Structure:**
  - Dynamic byte array (`uint8_t *data`)
  - Size tracking for bounds checking
  - Little-endian byte ordering

- **Memory Management:**
  - `memory_init()`: Allocate and zero-initialize memory of specified size
  - `memory_free()`: Deallocate memory and reset structure

- **Memory Access:**
  - `memory_read32()`: Read 32-bit word from memory (little-endian)
  - `memory_write32()`: Write 32-bit word to memory (little-endian)
  - Bounds checking on all accesses to prevent buffer overruns

- **Program Loading:**
  - `load_program_into_memory()`: Load encoded instructions into memory at base address
  - `load_data_into_memory()`: Load data section values into memory after instructions

- **Debugging:**
  - `memory_dump_words()`: Display memory contents as 32-bit words in hex format

- **Safety:** All operations include bounds checking and error reporting

---

### assembler.h

**Purpose:** Defines structures and functions for parsing RISC-V assembly files.

**Key Points:**
- **Data Structures:**
  - `Instruction`: Represents a parsed assembly instruction with label, opcode, operands, line number, and address
  - `DataEntry`: Represents a data directive with label, value, and address
  - `Symbol`: Represents a label with its resolved memory address
  - `AssemblyProgram`: Container for all instructions, data entries, and symbols

- **Constants:**
  - Maximum sizes for labels (50), opcodes (10), operands (3 per instruction, 20 chars each)
  - Program limits: 1024 instructions, 1024 data entries, 1024 symbols
  - Maximum line size: 1024 characters

- **Core Functions:**
  - `read_asm_file()`: Parse assembly file and populate AssemblyProgram structure
  - `print_program()`: Display parsed program for debugging
  - `find_symbol()`: Look up address of a label/symbol

- **Two-Pass Assembly:**
  - First pass: Parse instructions and build symbol table
  - Second pass: Resolve labels to addresses for branches/jumps

---

### encoder.h

**Purpose:** Converts parsed assembly instructions into 32-bit RISC-V machine code.

**Key Points:**
- **Main Function:**
  - `encode_instruction()`: Takes an `Instruction` and `AssemblyProgram` (for symbol lookup) and returns 32-bit encoded value
  
- **Encoding Process:**
  - Identifies instruction type (R, I, S, U, B)
  - Parses register names (x0-x31 or aliases like sp, ra, a0, etc.)
  - Parses immediate values (decimal, hex, or labels)
  - Resolves labels to PC-relative offsets for branches
  - Constructs 32-bit instruction according to RISC-V format
  
- **Validation:**
  - Checks immediate value ranges (12-bit for I-Type, 20-bit for U-Type, etc.)
  - Validates register indices
  - Ensures branch offsets are word-aligned
  
- **Error Handling:**
  - Returns 0 on encoding failure
  - Prints detailed error messages with line numbers

---

### decoder.h

**Purpose:** Provides utility functions for parsing assembly operands and register names.

**Key Points:**
- **Register Parsing:**
  - `reg_index()`: Converts register name to index (0-31)
  - Supports numeric format (x0-x31)
  - Supports ABI names (zero, ra, sp, gp, tp, t0-t6, s0-s11, a0-a7)
  - Case-insensitive matching
  
- **Immediate Parsing:**
  - `parse_immediate()`: Parses decimal, hexadecimal (0x), or octal (0) immediate values
  - Supports optional '#' prefix
  - Returns signed 32-bit integer
  
- **Memory Operand Parsing:**
  - `parse_memory_operand()`: Parses format like `offset(register)` for load/store instructions
  - Extracts offset value and base register index
  - Example: `4(sp)` → offset=4, reg=2 (sp is x2)
  
- **Error Handling:**
  - Returns -1 for invalid inputs
  - Provides descriptive error messages

---

## Source Files

### main.c

**Purpose:** Entry point for the simulator - orchestrates the complete simulation pipeline.

**Key Points:**
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

### cpu.c

**Purpose:** Implements the CPU emulation including fetch-decode-execute cycle.

**Key Points:**
- **Initialization:**
  - `cpu_init()`: Clears all registers (x0 hardwired to 0), resets PC, and clears flags
  - `cpu_init_with_program()`: Associates CPU with memory and program for execution

- **Register Management:**
  - `cpu_set_reg()`: Writes to register with x0 protection (writes to x0 are ignored)
  - `cpu_get_reg()`: Reads register value with bounds checking

- **Fetch-Decode-Execute:**
  - `cpu_fetch()`: Reads 32-bit instruction from memory at PC address
  - `cpu_decode()`: Extracts opcode and determines instruction format
  - `cpu_execute()`: Performs instruction operation based on type
    - R-Type: ALU operations (ADD, SUB, AND, OR, XOR, shifts)
    - I-Type: Immediate ALU ops and loads (ADDI, LW, JALR)
    - S-Type: Store operations (SW)
    - U-Type: Upper immediate (LUI, AUIPC)
    - B-Type: Conditional branches (BEQ, BNE, BLT, BGE, BLTU, BGEU)
    - J-Type: Jumps (JAL)

- **Control Flow:**
  - `cpu_step()`: Executes one instruction cycle (fetch → decode → execute)
  - `cpu_run()`: Loop that calls cpu_step() until program halts or error occurs
  - PC auto-increment by 4 (word-aligned) or branch target update

- **Special Instructions:**
  - Pseudo-halt: `addi x0, x0, 0` (NOP that sets halt flag)
  - Link register: Saves return address (PC+4) for JAL/JALR

- **Debugging:**
  - `cpu_print_registers()`: Shows all 32 registers in hex and decimal
  - `cpu_print_state()`: Shows PC, execution count, and halt/error flags

---

### alu.c

**Purpose:** Implements the Arithmetic Logic Unit operations.

**Key Points:**
- **Implementation:**
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

- **Error Handling:**
  - Returns 0 for unknown operations
  - Prints error message for debugging

- **Design:**
  - Pure function with no state
  - Easy to test and verify
  - Reusable for both R-Type and I-Type instructions

---

### memory.c

**Purpose:** Implements the simulated memory system.

**Key Points:**
- **Memory Model:**
  - Byte-addressable (smallest unit is 1 byte)
  - Little-endian byte ordering (LSB at lowest address)
  - Dynamic allocation based on program needs

- **Initialization:**
  - `memory_init()`: Allocates and zeros memory array
  - Returns Memory structure with pointer and size
  - Handles allocation failures gracefully

- **Access Functions:**
  - `memory_read32()`: Assembles 4 bytes into 32-bit word (little-endian)
  - `memory_write32()`: Splits 32-bit word into 4 bytes (little-endian)
  - Both functions check bounds before access

- **Loading Functions:**
  - `load_program_into_memory()`: Loads array of instructions starting at base address
  - `load_data_into_memory()`: Loads data section entries at their specified offsets
  - Data section typically placed after instruction section

- **Debugging:**
  - `memory_dump_words()`: Prints memory contents in hex format
  - Shows address and value for each 32-bit word
  - Automatically limits output to available memory

- **Safety:**
  - All functions validate pointers and bounds
  - Error messages printed for out-of-bounds access
  - Prevents buffer overruns and segmentation faults

---

### assembler.c

**Purpose:** Parses RISC-V assembly files and builds internal representation.

**Key Points:**
- **Parsing Process:**
  - Reads assembly file line by line
  - Strips comments (// and # style)
  - Removes whitespace
  - Identifies labels, opcodes, and operands
  - Builds symbol table with label addresses

- **Comment Handling:**
  - `eliminate_comment()`: Removes line comments (// and #)
  - `eliminate_block_comments()`: Removes /* ... */ block comments
  - Allows both C-style and assembly-style comments

- **Operand Parsing:**
  - `parse_operands()`: Splits operand string by commas
  - Trims whitespace from each operand
  - Stores up to 3 operands per instruction

- **Symbol Table:**
  - `build_symbol_table()`: First pass to collect all labels
  - Assigns addresses based on instruction position (word-aligned)
  - `find_symbol()`: Looks up label address for branch/jump resolution

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

- **Output:**
  - `print_program()`: Debug function to display parsed program
  - Shows all instructions with labels, opcodes, and operands

---

### encoder.c

**Purpose:** Converts parsed assembly instructions into 32-bit RISC-V machine code.

**Key Points:**
- **Builder Functions:**
  - `build_rtype()`: Constructs R-Type instruction from fields (funct7, rs2, rs1, funct3, rd, opcode)
  - `build_itype()`: Constructs I-Type instruction with 12-bit immediate
  - `build_stype()`: Constructs S-Type instruction with split immediate (imm[11:5] and imm[4:0])
  - `build_utype()`: Constructs U-Type instruction with 20-bit immediate
  - `build_btype()`: Constructs B-Type instruction with complex immediate encoding

- **Instruction Encoding:**
  - Identifies instruction mnemonic (ADD, LW, BEQ, etc.)
  - Maps to appropriate encoding function
  - Parses registers using `reg_index()` from decoder
  - Parses immediates as decimal, hex, or label references

- **Label Resolution:**
  - `parse_branch_imm_or_label()`: Resolves labels to PC-relative offsets
  - Computes offset = target_address - current_address
  - Validates offset fits in instruction's immediate field
  - Ensures branch offsets are even (word-aligned)

- **Validation:**
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

### decoder.c

**Purpose:** Utility functions for parsing assembly syntax elements.

**Key Points:**
- **Register Name Resolution:**
  - `reg_index()`: Maps register name to index (0-31)
  - Supports two naming conventions:
    - **Numeric**: x0, x1, ..., x31
    - **ABI Names**: zero, ra, sp, gp, tp, t0-t6, s0-s11, a0-a7, fp
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
  - `parse_immediate()`: Converts string to 32-bit signed integer
  - Supports multiple formats:
    - Decimal: 42, -10
    - Hexadecimal: 0x2A, 0xFF
    - Octal: 077
  - Optional '#' prefix for ARM-style syntax compatibility

- **Memory Operand Parsing:**
  - `parse_memory_operand()`: Parses "offset(register)" format
  - Used for load/store instructions (LW, SW, etc.)
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
    ├── include/                       # Header files directory
    │   ├── instruction.h              # RISC-V instruction format definitions
    │   ├── cpu.h                      # CPU structure and interface
    │   ├── alu.h                      # ALU operations definitions
    │   ├── memory.h                   # Memory system interface
    │   ├── assembler.h                # Assembly parser structures
    │   ├── encoder.h                  # Instruction encoder interface
    │   └── decoder.h                  # Operand decoder utilities
    ├── src/                           # Source files directory
    │   ├── cpu.c                      # CPU implementation (fetch-decode-execute)
    │   ├── alu.c                      # ALU operations implementation
    │   ├── memory.c                   # Memory system implementation
    │   ├── assembler.c                # Assembly file parser
    │   ├── encoder.c                  # Instruction encoder implementation
    │   └── decoder.c                  # Operand decoder implementation
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

---

**Documentation Version:** 1.0  
**Last Updated:** 2025-11-08  
**Project:** RISC-V Assembly Simulator  
**Language:** C (C99 Standard)  
**Build System:** CMake 3.10+ with Make wrapper
