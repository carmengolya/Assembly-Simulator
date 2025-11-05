#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "assembler.h"
#include "memory.h"

Memory memory_init(size_t size)
{
    Memory m;
    m.data = (uint8_t *)malloc(sizeof(uint8_t) * size);
    if(m.data == NULL)
    {
        printf("[ERROR] memory allocation failed.\n");
        m.size = 0;
        return m;
    }

    memset(m.data, 0, size);
    m.size = size;
    return m;
}

void memory_free(Memory *m)
{
    if(!m)
        return;

    free(m->data);
    m->data = NULL;
    m->size = 0;
}

static int in_bounds(Memory *m, uint32_t addr, size_t len)
{
    return addr + len <= m->size;
}

uint32_t memory_read32(Memory *m, uint32_t addr)
{
    if(!in_bounds(m , addr, 4))
    {
        printf("[ERROR] trying to read from out-of-bounds memory.\n");
        return 0;
    }
    uint32_t v = m->data[addr] | (m->data[addr + 1] << 8) | (m->data[addr + 2] << 16) | (m->data[addr + 3] << 24);
    return v;
}

void memory_write32(Memory *m, uint32_t addr, uint32_t value)
{
    if(!in_bounds(m , addr, 4))
    {
        printf("[ERROR] trying to write to out-of-bounds memory.\n");
        return;
    }

    m->data[addr] = value & 0xFF;
    m->data[addr + 1] = (value >> 8) & 0xFF;
    m->data[addr + 2] = (value >> 16) & 0xFF;
    m->data[addr + 3] = (value >> 24) & 0xFF;
}

void load_program_into_memory(Memory *m, const uint32_t *program, size_t len_words, uint32_t base_addr)
{
    size_t bytes = len_words * sizeof(uint32_t);
    if(base_addr + bytes > m->size)
    {
        printf("[ERROR] trying to load more bytes than memory can support.\n");
        return;
    }

    for(size_t i = 0; i < len_words; i++)
    {
        memory_write32(m, base_addr + i * 4, program[i]);
    }
}

void load_data_into_memory(Memory *m, const AssemblyProgram *program, uint32_t data_offset)
{
    if(program == NULL || program->data_count == 0) 
    {
        return; 
    }

    for(int i = 0; i < program->data_count; i++)
    {
        uint32_t addr = data_offset + program->data[i].address;
        uint32_t value = program->data[i].value;

        if(addr + 4 > m->size) 
        {
            printf("[ERROR] Not enough memory to load data entry at address 0x%08X.\n", addr);
            continue; 
        }

        memory_write32(m, addr, value);
    }
}

void memory_dump_words(Memory *m, uint32_t addr, size_t words)
{
    if(!m)
    {
        printf("[ERROR] memory is null: cannot read words from it.\n");
        return;
    }

    size_t start = (size_t)addr;
    if(start >= m->size) 
    {
        printf("[ERROR] memory_dump_words: start address out of bounds\n");
        return;
    }

    size_t available_bytes = m->size - start;
    size_t available_words = available_bytes / 4u;
    if(words == 0 || words > available_words) 
    {
        words = available_words;
    } 

    for (size_t i = 0; i < words; ++i) 
    {
        size_t a = start + i * 4u;
        uint32_t w = (uint32_t)m->data[a + 0]
                   | ((uint32_t)m->data[a + 1] << 8)
                   | ((uint32_t)m->data[a + 2] << 16)
                   | ((uint32_t)m->data[a + 3] << 24);
        printf("%08zx: %08x\n", a, w);
    }
}
