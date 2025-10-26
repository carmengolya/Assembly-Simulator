#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdint.h>

typedef enum 
{
    OP_ADD = 0x33,
    OP_SUB = 0x33 
} Opcode;

typedef struct 
{
    uint32_t funct7 : 7;
    uint32_t rs2    : 5;
    uint32_t rs1    : 5;
    uint32_t funct3 : 3;
    uint32_t rd     : 5;
    uint32_t opcode : 7;
} RTypeEncoding;

typedef union 
{
    RTypeEncoding rtype;
    uint32_t value;
} EncodedInstruction;

#endif // INSTRUCTION_H
