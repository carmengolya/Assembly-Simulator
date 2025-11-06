#include <stdio.h>
#include <stdint.h>
#include "alu.h"

int32_t alu_execute(ALUOp op, int32_t operand1, int32_t operand2)
{
    switch (op)
    {
        case ALU_ADD:
            return operand1 + operand2;

        case ALU_SUB:
            return operand1 - operand2;

        case ALU_XOR:
            return operand1 ^ operand2;
        
        case ALU_OR:
            return operand1 | operand1;

        case ALU_AND:
            return operand1 & operand2;

        case ALU_SLL:
            return (int32_t)((uint32_t)operand1 << (operand2 & 0x1F));

        case ALU_SRL:
            return (int32_t)((uint32_t)operand1 >> (operand2 & 0x1F));

        case ALU_SRA:
            return operand1 >> (operand2 & 0x1F);
        
        default:
        {
            printf("[ERROR] alu_execute: unknown operation %d\n", op);
            return 0;
        }
    }
}