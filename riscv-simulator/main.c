#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "assembler.h"
#include "encoder.h"
#include "memory.h"

int main(void) 
{
    char *filename = "/home/carmen/Assembly-Simulator/riscv-simulator/tests/add_sub.asm";
    AssemblyProgram program;
    if(read_asm_file(filename, &program) < 0)
    {
        printf("[FAILED] read_asm_file function failed.\n");
        return 1;
    }
    print_program(&program);

    Memory m = memory_init(400);
    uint32_t *enc = (uint32_t *)malloc(sizeof(uint32_t) * program.instruction_count);
    if(!enc)
    {
        perror("[ERROR] memory allocation for encoded array failed.");
        return 1;
    }

    printf("\n=== Encoding instructions ===\n");
    for (int i = 0; i < program.instruction_count; ++i)
    {
        Instruction *instr = &program.instructions[i];

        printf("[%02d] %s ", i, instr->opcode);
        for (int j = 0; j < instr->operand_count; ++j)
        {
            printf("%s", instr->operands[j]);
            if (j + 1 < instr->operand_count) printf(", ");
        }

        if (instr->operand_count < 3)
        {
            printf(" -> [ERROR] not enough operands (need rd, rs1, rs2)\n");
            continue;
        }

        enc[i] = encode_instruction(instr);
        printf(" -> encoded: 0x%08X\n", enc[i]);
    }

    load_program_into_memory(&m, enc, program.instruction_count, 0);
    memory_dump_words(&m, 0, program.instruction_count * 4);
    memory_free(&m);
    return 0;
}
