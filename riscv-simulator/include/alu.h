#include <stdint.h>

#ifndef ALU_H
#define ALU_H

typedef enum
{
    ALU_ADD,
    ALU_SUB,
    ALU_XOR,
    ALU_OR,
    ALU_AND,
    ALU_SLL, // logical left
    ALU_SRL, // logical right
    ALU_SRA  // arithmetic right
} ALUOp;

int32_t alu_execute(ALUOp op, int32_t operand1, int32_t operand2);

#endif // ALU_H
