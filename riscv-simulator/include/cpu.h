#ifndef CPU_H
#define CPU_H

#include <stdint.h>

#define REG_NUMBER 32

typedef struct
{
    int32_t regs[REG_NUMBER];
    uint32_t pc;
} CPU;

void cpu_set_reg(CPU *cpu, int index, int32_t value);
void cpu_init(CPU *cpu);
void print_registers(CPU *cpu);

#endif // CPU_H
