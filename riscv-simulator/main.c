#include <stdio.h>
#include <stdint.h>

#include "assembler.h"
#include "encoder.h"

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

        uint32_t enc = encode_instruction(instr);
        printf(" -> encoded: 0x%08X\n", enc);
    }
    return 0;
}
