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

/**
 * I-Type Instruction Format (RISC-V)
 * 
 * Layout (32 biți):
 * 
 *  31            20 19    15 14 12 11   7 6      0
 *  +----------------+-------+-----+-------+-------+
 *  |   immediate    |  rs1  |funct3|  rd  |opcode |
 *  +----------------+-------+-----+-------+-------+
 *      12 biți       5 biți 3 biți  5 biți  7 biți
 **/

static inline uint8_t itype_get_opcode(uint32_t instr)
{
    return instr & 0x7F;  // [6:0]
}

static inline uint8_t itype_get_rd(uint32_t instr)
{
    return (instr >> 7) & 0x1F;  // [11:7]
}

static inline uint8_t itype_get_funct3(uint32_t instr)
{
    return (instr >> 12) & 0x07;  // [14:12]
}

static inline uint8_t itype_get_rs1(uint32_t instr)
{
    return (instr >> 15) & 0x1F;  // [19:15]
}

static inline int32_t itype_get_immediate(uint32_t instr)
{
    int32_t imm = (instr >> 20) & 0xFFF;  // [31:20] 
    
    if(imm & 0x800)  // if it's signed
        imm |= 0xFFFFF000;  // extend it with 1s
    
    return imm;
}

typedef struct 
{
    uint32_t value;
} ITypeEncoding;

 /**
 * S-Type Instruction Format (RISC-V)
 * 
 * Layout (32 biți):
 * 
 *  31       25 24   20 19   15 14 12 11    7 6        0
 *  +----------+-------+-------+------+--------+---------+
 *  |imm[11:5] |  rs2  |  rs1  |funct3|imm[4:0]|  opcode |
 *  +----------+-------+-------+------+--------+---------+
 *    7 biți    5 biți  5 biți  3 biți  5 biți   7 biți
 **/

static inline uint8_t stype_get_opcode(uint32_t instr)
{
    return instr & 0x7F;  // [6:0]
}

static inline uint8_t stype_get_funct3(uint32_t instr)
{
    return (instr >> 12) & 0x07;  // [14:12]
}

static inline uint8_t stype_get_rs1(uint32_t instr)
{
    return (instr >> 15) & 0x1F;  // [19:15]
}

static inline uint8_t stype_get_rs2(uint32_t instr)
{
    return (instr >> 20) & 0x1F;  // [24:20] 
}

static inline int32_t stype_get_immediate(uint32_t instr)
{
    int32_t imm11_5 = (instr >> 25) & 0x7F;  // [31:25] 
    int32_t imm4_0 = (instr >> 7) & 0x1F;    // [11:7] 
    
    int32_t imm = (imm11_5 << 5) | imm4_0; 
    
    if(imm & 0x800)  // if it's signed
        imm |= 0xFFFFF000; // extend it with 1s
    
    return imm;
}

typedef struct 
{
    uint32_t value;
} STypeEncoding;

/**
 * U-Type Instruction Format (RISC-V)
 * 
 * Layout (32 biți):
 * 
 *  31             12 11    7 6      0
 *  +----------------+-------+-------+
 *  |    immediate   |  rd   |opcode |
 *  +----------------+-------+-------+
 *        20 biți     5 biți   7 biți
 *
 */

static inline uint8_t utype_get_opcode(uint32_t instr)
{
    return instr & 0x7F;  // [6:0]
}

static inline uint8_t utype_get_rd(uint32_t instr)
{
    return (instr >> 7) & 0x1F;  // [11:7]
}

static inline int32_t utype_get_imm20(uint32_t instr)
{
    return (int32_t)((instr >> 12) & 0xFFFFF);
}

static inline int32_t utype_get_immediate(uint32_t instr)
{
    return (int32_t)(utype_get_imm20(instr) << 12);
}

typedef struct
{
    uint32_t value;
} UTypeEncoding;

typedef union 
{
    RTypeEncoding rtype;
    ITypeEncoding itype;
    STypeEncoding stype;
    uint32_t value;
} EncodedInstruction;

#endif // INSTRUCTION_H
