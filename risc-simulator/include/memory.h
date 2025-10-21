#ifndef MEMORY_H
#define MEMORY_H

// memory.h - Definitions for RISC simulator memory
// This file defines the memory structure and functions for reading and writing
// data in the RISC simulator.
// The memory is byte-addressable and has a fixed size.
// Memory size is defined as 65536 bytes (64 KB).
// All memory accesses are aligned to word boundaries (4 bytes).

#include <stdint.h>
#include <stdbool.h>

#define MEM_SIZE 65536 

typedef struct 
{
    uint8_t data[MEM_SIZE];
} Memory;

void mem_init(Memory *m); // Initialize memory to zero
uint32_t mem_read_word(Memory *m, uint32_t addr); // Read a 32-bit word from memory
void mem_write_word(Memory *m, uint32_t addr, uint32_t val); // Write a 32-bit word to memory

#endif // MEMORY_H