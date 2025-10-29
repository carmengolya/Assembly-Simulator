#ifndef MEMORY_H
#define MEMORY_H

#include <stdint.h>
#include <stddef.h>

typedef struct 
{
    uint8_t *data;
    size_t size;
} Memory;

Memory memory_init(size_t size);
void memory_free(Memory *m);

uint32_t memory_read32(Memory *m, uint32_t addr);

void memory_write32(Memory *m, uint32_t addr, uint32_t value);

void load_program_into_memory(Memory *m, const uint32_t *program, size_t len_words, uint32_t base_addr);

void memory_dump_words(Memory *m, uint32_t addr, size_t words);

#endif // MEMORY_H
