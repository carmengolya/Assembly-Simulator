#ifndef CPU_H
#define CPU_H

// cpu.h - Defines the CPU state and functions for a simple RISC simulator
// The CPU has 8 general-purpose registers (r0–r7), a program counter (pc),
// a link register (lr), a stack pointer (sp), an instruction register (ir),
// and a set of status flags (N, Z, C, V).
// Its memory is represented by a Memory struct defined in memory.h.
// Its role is to fetch, decode, and execute instructions in a loop.

#include <stdint.h>
#include <stdbool.h>
#include "memory.h"

#define NUM_REGS 8

typedef struct 
{
    bool N;  // negative
    bool Z;  // zero
    bool C;  // carry
    bool V;  // overflow
} CPUFlags;

typedef struct 
{
    uint32_t regs[NUM_REGS];  // r0–r7
    uint32_t pc;              // Program Counter
    uint32_t lr;              // Link Register
    uint32_t sp;              // Stack Pointer
    uint32_t ir;              // Instruction Register
    CPUFlags flags;           // CPU status flags
    Memory mem;               // memoria sistemului
} CPUState;

void cpu_init(CPUState *cpu); // initializes registers and memory
void cpu_run(CPUState *cpu, uint32_t max_cycles); // main loop for executing instructions in fetch-decode-execute cycle

#endif // CPU_H