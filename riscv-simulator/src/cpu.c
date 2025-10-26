#include "cpu.h"

void cpu_set_reg(CPU *cpu, int index, int32_t value)
{
    if(index == 0)
    {
        perror("[ERROR] tried modifying zero register.");
        return; // if we are trying to access and modify the zero register, it will remain unchanged
    }

    cpu->regs[index] = value;
}

void cpu_init(CPU *cpu)
{
    for(int i = 0; i < REG_NUMBER; i++)
    {
        cpu_set_reg(cpu, i, 0);
    }
    cpu->pc = 0;
}

void print_registers(CPU *cpu)
{
    printf("pc[] 0x%08X\n", cpu->pc);
    for(int i = 0; i < REG_NUMBER; i++)
    {
        printf("regs[x%02d] 0x%08X\n", i, cpu->regs[i]);
    }
}
