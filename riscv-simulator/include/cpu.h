#ifndef CPU_H
#define CPU_H

#include <stdint.h>

#include "assembler.h"
#include "instruction.h"
#include "memory.h"

#define REG_NUMBER 32

typedef struct
{
    int32_t regs[REG_NUMBER];
    uint32_t pc;

    Memory *memory;              
    AssemblyProgram *program;       
    
    uint32_t instructions_executed; 
    int halted;                     
    int error; 
} CPU;

void cpu_init(CPU *cpu);
void cpu_init_with_program(CPU *cpu, Memory *memory, AssemblyProgram *program);

void cpu_set_reg(CPU *cpu, int index, int32_t value);
int32_t cpu_get_reg(CPU *cpu, int index);

// fetch -> decode -> execute
EncodedInstruction cpu_fetch(CPU *cpu);
int cpu_decode(CPU *cpu, EncodedInstruction enc); 
int cpu_execute(CPU *cpu, EncodedInstruction enc);

void cpu_writeback(CPU *cpu, int rd, int32_t value);

int cpu_step(CPU *cpu);
int cpu_run(CPU *cpu);

void cpu_print_registers(CPU *cpu);
void cpu_print_state(CPU *cpu);

#endif // CPU_H
