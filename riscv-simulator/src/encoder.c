#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "assembler.h"
#include "decoder.h"
#include "encoder.h"
#include "instruction.h"

static uint32_t build_rtype(uint32_t funct7, uint32_t rs2, uint32_t rs1, uint32_t funct3, uint32_t rd, uint32_t opcode)
{
    return ((funct7 & 0x7F) << 25) |
           ((rs2    & 0x1F) << 20) |
           ((rs1    & 0x1F) << 15) |
           ((funct3 & 0x07) << 12) |
           ((rd     & 0x1F) << 7)  |
           ( opcode & 0x7F );
}



uint32_t encode_instruction(Instruction *instr)
{
    if (!instr)
    {
        printf("[ERROR] encode_instruction: instr is NULL\n");
        return 0;
    }
    
    EncodedInstruction enc = {0};

    if(strcmp(instr->opcode, "add") == 0 || strcmp(instr->opcode, "sub") == 0)
    {
        if (instr->operand_count < 3)
        {
            printf("[ERROR] encode_instruction: not enough operands for '%s' (line %d)\n",
                   instr->opcode, instr->line_number);
            return 0;
        }

        int rd = reg_index(instr->operands[0]);
        int rs1 = reg_index(instr->operands[1]);
        int rs2 = reg_index(instr->operands[2]);

        if (rd < 0 || rs1 < 0 || rs2 < 0)
        {
            printf("[ERROR] not enough registers.\n");
            return 0;
        }

        enc.rtype.opcode = 0x33;
        enc.rtype.rd = (uint32_t)rd;
        enc.rtype.rs1 = (uint32_t)rs1;
        enc.rtype.rs2 = (uint32_t)rs2;
        enc.rtype.funct3 = 0x00;
        if(strcmp(instr->opcode, "add") == 0)
        {
            enc.rtype.funct7 = 0X00;
        }
        else
        {
            enc.rtype.funct7 = 0x20;
        }

        uint32_t expected = build_rtype(enc.rtype.funct7, enc.rtype.rs2, enc.rtype.rs1, enc.rtype.funct3, enc.rtype.rd, enc.rtype.opcode);
        if(expected != enc.value)
        {
            enc.value = expected;
        }
        return enc.value;
    }

    printf("[WARN] unknown opcode: %s\n", instr->opcode);
    return 0;
}