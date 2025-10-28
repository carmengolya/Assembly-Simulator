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

        uint8_t funct7, funct3, opcode;
        const char *op_name;

        funct3 = 0x00; 
        opcode = 0x33;

        if(strcmp(instr->opcode, "add") == 0)
        {
            funct7 = 0X00;
            op_name = "ADD";
        }
        else
        {
            funct7 = 0x20;
            op_name = "SUB";
        }

        uint32_t encoded = build_rtype(funct7, rs2, rs1, funct3, rd, opcode);
        printf("[ENCODE] %s x%d, x%d, x%d -> 0x%08X\n",
               op_name, rd, rs1, rs2, encoded);
        return encoded;
    }

    printf("[WARN] unknown opcode: %s\n", instr->opcode);
    return 0;
}