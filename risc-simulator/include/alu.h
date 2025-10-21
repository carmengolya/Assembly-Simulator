#ifndef ALU_H
#define ALU_H

// alu.h - Header file for ALU operations in RISC simulator
// Defines ALU operations and result structures 
// along with function prototypes for executing ALU operations.

#include <stdint.h>
#include <stdbool.h>
#include "cpu.h"

typedef enum 
{
    ALU_ADD,
    ALU_SUB,
    ALU_AND,
    ALU_OR,
    ALU_XOR,
    ALU_SHL,
    ALU_SHR,
    ALU_CMP,
    ALU_NOP
} ALUOp;

typedef struct 
{
    uint32_t result;
    CPUFlags flags;
} ALUResult;

ALUResult alu_execute(ALUOp op, uint32_t a, uint32_t b); // Execute ALU operation

#endif // ALU_H