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

static uint32_t build_itype(uint32_t imm, uint32_t rs1, uint32_t funct3, uint32_t rd, uint32_t opcode)
{
    return (((imm & 0xFFF) << 20) |
            ((rs1     & 0x1F) << 15) |
            ((funct3  & 0x07) << 12) |
            ((rd      & 0x1F) << 7)  |
            ( opcode  & 0x7F ));
}

static uint32_t build_stype(uint32_t imm, uint32_t rs2, uint32_t rs1, uint32_t funct3, uint32_t opcode)
{
    uint32_t imm_hi = (imm >> 5) & 0x7F;     // [11:5]
    uint32_t imm_lo = imm & 0x1F;             // [4:0]

    return ((imm_hi & 0x7F) << 25) |
           ((rs2     & 0x1F) << 20) |
           ((rs1     & 0x1F) << 15) |
           ((funct3  & 0x07) << 12) |
           ((imm_lo  & 0x1F) << 7)  |
           ( opcode  & 0x7F );
}

static uint32_t build_utype(uint32_t imm, uint32_t rd, uint32_t opcode)
{
    return ((imm & 0xFFFFF)  << 12)  |
           ((rd    & 0x1F)   <<  7)  |
           ( opcode & 0x7F );
}

static int fits_imm12(int32_t imm) 
{
    return (imm >= -2048 && imm <= 2047);
}

static int fits_imm20(int32_t imm)
{
    return (imm >= -524288 && imm <= 524287);
}

uint32_t encode_instruction(Instruction *instr)
{
    if(!instr)
    {
        printf("[ERROR] encode_instruction: instr is NULL\n");
        return 0;
    }
    
    EncodedInstruction enc = {0};

    if(strcmp(instr->opcode, "add") == 0 || strcmp(instr->opcode, "sub") == 0)
    {
        if(instr->operand_count < 3)
        {
            printf("[ERROR] encode_instruction: not enough operands for '%s' (line %d)\n",
                   instr->opcode, instr->line_number);
            return 0;
        }

        int rd = reg_index(instr->operands[0]);
        int rs1 = reg_index(instr->operands[1]);
        int rs2 = reg_index(instr->operands[2]);

        if(rd < 0 || rs1 < 0 || rs2 < 0)
        {
            printf("[ERROR] encode_instruction: invalid registers for '%s' (line %d)\n",
               instr->opcode, instr->line_number);
            printf("  rd=%d, rs1=%d, rs2=%d\n", rd, rs1, rs2);
            printf("  operands: '%s', '%s', '%s'\n", 
                instr->operands[0], instr->operands[1], instr->operands[2]);
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
    else if(strcmp(instr->opcode, "sll") == 0 || strcmp(instr->opcode, "srl") == 0 || strcmp(instr->opcode, "sra") == 0 ||
        strcmp(instr->opcode, "and") == 0 || strcmp(instr->opcode, "or") == 0 || strcmp(instr->opcode, "xor") == 0)
    {
        if(instr->operand_count < 3)
        {
            printf("[ERROR] encode_instruction: not enough operands for '%s' (line %d)\n",
                   instr->opcode, instr->line_number);
            return 0;
        }

        int rd = reg_index(instr->operands[0]);
        int rs1 = reg_index(instr->operands[1]);
        int rs2 = reg_index(instr->operands[2]);

        if(rd < 0 || rs1 < 0 || rs2 < 0)
        {
            printf("[ERROR] encode_instruction: invalid registers for '%s' (line %d)\n",
               instr->opcode, instr->line_number);
            printf("  rd=%d, rs1=%d, rs2=%d\n", rd, rs1, rs2);
            printf("  operands: '%s', '%s', '%s'\n", 
                instr->operands[0], instr->operands[1], instr->operands[2]);
            return 0;
        }

        uint8_t funct3, funct7 = 0x00;
        uint8_t opcode = 0x33;

        if(strcmp(instr->opcode, "sll") == 0)
        {     
            funct3 = 0x1;
        }
        else if(strcmp(instr->opcode, "srl") == 0)
        {
            funct3 = 0x5;
        }
        else if(strcmp(instr->opcode, "sra") == 0) 
        {
            funct3 = 0x5;
            funct7 = 0x20;
        }
        else if(strcmp(instr->opcode, "and") == 0) 
        {
            funct3 = 0x7;
        }
        else if(strcmp(instr->opcode, "or")  == 0)
        {
            funct3 = 0x6;
        }
        else if(strcmp(instr->opcode, "xor") == 0)
        {
            funct3 = 0x4;
        }
        else
        {
            funct3 = 0x0;
        }

        uint32_t encoded = build_rtype(funct7, rs2, rs1, funct3, rd, opcode);
        printf("[ENCODE] %s x%d, x%d, x%d -> 0x%08X\n",
                instr->opcode, rd, rs1, rs2, encoded);
        return encoded;
    }
    else if(strcmp(instr->opcode, "addi") == 0)
    {
        if(instr->operand_count < 3)
        {
            printf("[ERROR] encode_instruction: not enough operands for 'addi' (line %d)\n",
                   instr->line_number);
            return 0;
        }

        int rd = reg_index(instr->operands[0]);
        int rs1 = reg_index(instr->operands[1]);
        int32_t imm = parse_immediate(instr->operands[2]);

        if(rd < 0 || rs1 < 0)
        {
            printf("[ERROR] encode_instruction: invalid registers for 'addi' (line %d)\n",
                   instr->line_number);
            return 0;
        }
        if(!fits_imm12(imm))
        {
            printf("[ERROR] encode_instruction: immediate out of 12-bit range for 'addi' (line %d, imm=%d)\n",
                   instr->line_number, imm);
            return 0;
        }

        uint8_t opcode = 0x13;  
        uint8_t funct3 = 0x0;   

        uint32_t encoded = build_itype((uint32_t)imm, (uint32_t)rs1, funct3, (uint32_t)rd, opcode);
        enc.value = encoded;
        printf("[ENCODE] ADDI x%d, x%d, %d -> 0x%08X\n", 
            rd, rs1, imm, encoded);
        return enc.value;
    }
    else if(strcmp(instr->opcode, "li") == 0)
    {
        if(instr->operand_count < 2)
        {
            printf("[ERROR] encode_instruction: not enough operands for 'li' (line %d)\n",
                   instr->line_number);
            return 0;
        }

        int rd = reg_index(instr->operands[0]);
        int32_t imm = parse_immediate(instr->operands[1]);

        if(rd < 0)
        {
            printf("[ERROR] encode_instruction: invalid destination register for 'li' (line %d)\n",
                   instr->line_number);
            return 0;
        }
        if(!fits_imm12(imm))
        {
            printf("[ERROR] encode_instruction: 'li' immediate out of 12-bit range (line %d, imm=%d)\n",
                   instr->line_number, imm);
            return 0;
        }

        int rs1 = 0;     
        uint8_t opcode = 0x13;
        uint8_t funct3 = 0x0;

        uint32_t encoded = build_itype((uint32_t)imm, (uint32_t)rs1, funct3, (uint32_t)rd, opcode);
        enc.value = encoded;
        printf("[ENCODE] LI x%d, %d -> (ADDI x%d, x0, %d) -> 0x%08X\n",
            rd, imm, rd, imm, encoded);
        return enc.value;
    }
    else if(strcmp(instr->opcode, "lui") == 0)
    {
        if(instr->operand_count < 2)
        {
            printf("[ERROR] encode_instruction: not enough operands for 'lui' (line %d)\n",
                   instr->line_number);
            return 0;
        }

        int rd = reg_index(instr->operands[0]);
        int32_t imm = parse_immediate(instr->operands[1]);

        if(rd < 0)
        {
            printf("[ERROR] encode_instruction: invalid destination register for 'lui' (line %d)\n",
                   instr->line_number);
            return 0;
        }
        if(!fits_imm20(imm))
        {
            printf("[ERROR] encode_instruction: immediate out of 20-bit range for 'lui' (line %d, imm=%d)\n",
                   instr->line_number, imm);
            return 0;
        }

        uint32_t encoded = build_utype((uint32_t)imm, (uint32_t)rd, 0x37);
        printf("[ENCODE] LUI x%d, 0x%05X -> 0x%08X\n",
               rd, (unsigned)((uint32_t)imm & 0xFFFFF), encoded);
        return encoded;
    }
    else if(strcmp(instr->opcode, "auipc") == 0)
    {
        if(instr->operand_count < 2)
        {
            printf("[ERROR] encode_instruction: not enough operands for 'auipc' (line %d)\n",
                   instr->line_number);
            return 0;
        }

        int rd = reg_index(instr->operands[0]);
        int32_t imm = parse_immediate(instr->operands[1]);

        if (rd < 0)
        {
            printf("[ERROR] encode_instruction: invalid destination register for 'auipc' (line %d)\n",
                   instr->line_number);
            return 0;
        }
        if (!fits_imm20(imm))
        {
            printf("[ERROR] encode_instruction: immediate out of 20-bit range for 'auipc' (line %d, imm=%d)\n",
                   instr->line_number, imm);
            return 0;
        }

        uint32_t encoded = build_utype((uint32_t)imm, (uint32_t)rd, 0x17);
        printf("[ENCODE] AUIPC x%d, 0x%05X -> 0x%08X\n",
               rd, (unsigned)((uint32_t)imm & 0xFFFFF), encoded);
        return encoded;
    }
    else if(strcmp(instr->opcode, "lw") == 0)
    {
        if(instr->operand_count < 2)
        {
            printf("[ERROR] encode_instruction: not enough operands for 'lw' (line %d)\n",
                   instr->line_number);
            return 0;
        }

        int rd = reg_index(instr->operands[0]);
        if(rd < 0)
        {
            printf("[ERROR] Invalid destination register for lw: %s (line %d)\n",
                   instr->operands[0], instr->line_number);
            return 0;
        }

        int32_t offset;
        int rs1;
        if(parse_memory_operand(instr->operands[1], &offset, &rs1) < 0)
        {
            printf("[ERROR] Failed to parse memory operand for lw (line %d)\n", instr->line_number);
            return 0;
        }

        uint8_t funct3 = 0x2;  
        uint8_t opcode = 0x03; 
        
        uint32_t encoded = build_itype(offset & 0xFFF, rs1, funct3, rd, opcode);
        printf("[ENCODE] LW x%d, %d(x%d) -> 0x%08X\n",
               rd, offset, rs1, encoded);
        return encoded;
    }
    else if(strcmp(instr->opcode, "sw") == 0)
    {
        if(instr->operand_count < 2)
        {
            printf("[ERROR] encode_instruction: not enough operands for 'sw' (line %d)\n",
                   instr->line_number);
            return 0;
        }

        int rs2 = reg_index(instr->operands[0]);
        if(rs2 < 0)
        {
            printf("[ERROR] Invalid source register for sw: %s (line %d)\n",
                   instr->operands[0], instr->line_number);
            return 0;
        }

        int32_t offset;
        int rs1;
        if(parse_memory_operand(instr->operands[1], &offset, &rs1) < 0)
        {
            printf("[ERROR] Failed to parse memory operand for sw (line %d)\n", instr->line_number);
            return 0;
        }

        uint8_t funct3 = 0x2;  
        uint8_t opcode = 0x23; 

        uint32_t encoded = build_stype(offset & 0xFFF, rs2, rs1, funct3, opcode);
        printf("[ENCODE] SW x%d, %d(x%d) -> 0x%08X\n",
               rs2, offset, rs1, encoded);
        return encoded;
    }

    printf("[WARN] unknown opcode: %s\n", instr->opcode);
    return 0;
}