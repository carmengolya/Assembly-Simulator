#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdint.h>

typedef enum 
{
    OP_ADD = 0x33,
    OP_SUB = 0x33 
} Opcode;

/**
 * R-Type Instruction Format (RISC-V)
 * 
 * Layout (32 biți):
 * 
 *  31      25 24    20 19  15 14 12 11    7 6      0
 * +----------+-------+-------+-----+-------+-------+
 * |  funct7  |  rs2  |  rs1  |funct3|  rd  |opcode |
 * +----------+-------+-------+-----+-------+-------+
 *  7 biți     5 biți  5 biți 3 biți  5 biți  7 biți
 **/

static inline uint8_t rtype_get_opcode(uint32_t instr)
{
    return instr & 0x7F;  // [6:0]
}

static inline uint8_t rtype_get_rd(uint32_t instr)
{
    return (instr >> 7) & 0x1F;  // [11:7]
}

static inline uint8_t rtype_get_funct3(uint32_t instr)
{
    return (instr >> 12) & 0x07;  // [14:12]
}

static inline uint8_t rtype_get_rs1(uint32_t instr)
{
    return (instr >> 15) & 0x1F;  // [19:15]
}

static inline uint8_t rtype_get_rs2(uint32_t instr)
{
    return (instr >> 20) & 0x1F;  // [24:20]
}

static inline uint8_t rtype_get_funct7(uint32_t instr)
{
    return (instr >> 25) & 0x7F;  // [31:25]
}

typedef struct 
{
    uint32_t value;
} RTypeEncoding;

typedef union 
{
    RTypeEncoding rtype;
    uint32_t value;
} EncodedInstruction;

#endif // INSTRUCTION_H
