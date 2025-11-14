#ifndef INSTRUCTION_H
#define INSTRUCTION_H

#include <stdint.h>

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
 * Layout (32 biti):
 * 
 *  31       25 24   20 19   15 14 12 11    7 6        0
 *  +----------+-------+-------+------+--------+---------+
 *  |imm[11:5] |  rs2  |  rs1  |funct3|imm[4:0]|  opcode |
 *  +----------+-------+-------+------+--------+---------+
 *    7 biti    5 biti  5 biti  3 biti  5 biti   7 biti
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
    
    if(imm & 0x800)
        imm |= 0xFFFFF000;
    
    return imm;
}

typedef struct 
{
    uint32_t value;
} STypeEncoding;

/**
 * U-Type Instruction Format (RISC-V)
 * 
 * Layout (32 biti):
 * 
 *  31             12 11    7 6      0
 *  +----------------+-------+-------+
 *  |    immediate   |  rd   |opcode |
 *  +----------------+-------+-------+
 *        20 biti     5 biti   7 biti
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

/**
 * B-Type Instruction Format (RISC-V)
 * 
 * Layout (32 biti):
 * 
 *  31       30          25 24    20 19    15 14    12 11       8 7      6       0
 *  +----------+-----------+--------+--------+--------+----------+-------+-------+
 *  | imm[12]  | imm[10:5] |  rs2   |  rs1   | funct3 | imm[4:1] |imm[11]|opcode |
 *  +----------+-----------+--------+--------+--------+----------+-------+-------+
 *     1 bit      6 biti     5 biti   5 biti   3 biti    4 biti    1 bit   7 biti
 *
 */

static inline uint8_t btype_get_opcode(uint32_t instr)
{
    return instr & 0x7F;  // [6:0]
}

static inline uint8_t btype_get_funct3(uint32_t instr)
{
    return (uint8_t)((instr >> 12) & 0x07); // [14:12]
}

static inline uint8_t btype_get_rs1(uint32_t instr)
{
    return (uint8_t)((instr >> 15) & 0x1F); // [19:15]
}

static inline uint8_t btype_get_rs2(uint32_t instr)
{
    return (uint8_t)((instr >> 20) & 0x1F); // [24:20]
}

static inline int32_t btype_get_imm(uint32_t instr)
{
    int32_t imm = 0;

    int32_t imm12   = (int32_t)((instr >> 31) & 0x1);
    int32_t imm11   = (int32_t)((instr >> 7)  & 0x1);
    int32_t imm10_5 = (int32_t)((instr >> 25) & 0x3F);
    int32_t imm4_1  = (int32_t)((instr >> 8)  & 0x0F);

    imm |= (imm12   << 12);
    imm |= (imm11   << 11);
    imm |= (imm10_5 << 5);
    imm |= (imm4_1  << 1);

    if (imm & (1 << 12))
    {
        imm |= ~((1 << 13) - 1);
    }

    return imm;
}

typedef struct
{
    uint32_t value;
} BTypeEncoding;

/*
* J-Type Instruction Format (RISC-V)
*
* Layout (32 biți):
*
* 31       30 29       22 21     20 19       12 11 7 6       0
* +----------+-----------+---------+-----------+----+--------+
* | imm[20]  | imm[10:1] | imm[11] | imm[19:12]| rd | opcode |
* +----------+-----------+---------+-----------+----+--------+
*    1 bit      10 biti     1 bit      8 biti  5 biti  7 biti
*
*/

static inline uint8_t jtype_get_opcode(uint32_t instr)
{
    return instr & 0x7F;  // [6:0]
}

static inline int32_t jtype_get_immediate(uint32_t instr)
{
    int32_t imm20     = (instr >> 31) & 0x1;
    int32_t imm10_1   = (instr >> 21) & 0x3FF;
    int32_t imm11     = (instr >> 20) & 0x1;
    int32_t imm19_12  = (instr >> 12) & 0xFF;

    int32_t imm = (imm20 << 20) | (imm19_12 << 12) | (imm11 << 11) | (imm10_1 << 1);

    if (imm & (1 << 20))
        imm |= 0xFFF00000;

    return imm;
}

typedef struct
{
    uint32_t value;
} JTypeEncoding;

typedef union 
{
    RTypeEncoding rtype;
    ITypeEncoding itype;
    STypeEncoding stype;
    BTypeEncoding btype;
    JTypeEncoding jtype;
    uint32_t value;
} EncodedInstruction;

#endif // INSTRUCTION_H
