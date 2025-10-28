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
        
        default:
        {
            printf("[ERROR] alu_execute: unknown operation %d\n", op);
            return 0;
        }
    }
}