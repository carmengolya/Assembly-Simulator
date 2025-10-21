#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

// instructions.h - Definitions for RISC simulator instructions
// This file defines the instruction set architecture (ISA) for the RISC simulator,
// including opcodes, instruction formats, and related structures.
// The way an instruction is partitioned is: 32 bits: [ opcode (8 bits) | rd (4 bits) | rn (4 bits) | rm/imm (16 bits) ]

#include <stdint.h>
#include <stdbool.h>

typedef enum 
{
    OP_NOP = 0,   // no operation
    OP_MOV,       // move register or immediate
    OP_ADD,       // addition
    OP_SUB,       // subtraction
    OP_AND,       // bitwise AND
    OP_OR,        // bitwise OR
    OP_XOR,       // bitwise XOR
    OP_NOT,       // bitwise NOT
    OP_CMP,       // compare (sets flags)
    OP_LDA,       // load from memory
    OP_STR,       // store to memory
    OP_LDR,       // load register indirect
    OP_STR,       // store register indirect
    OP_B,         // unconditional branch
    OP_BEQ,       // branch if equal
    OP_BNE,       // branch if not equal
    OP_BGT,       // branch if greater than
    OP_BLT,       // branch if less than
    OP_HLT        // stop execution
} Opcode;

typedef enum 
{
    TYPE_NONE = 0,
    TYPE_R,  // register type
    TYPE_I,  // immediate type
    TYPE_M,  // memory type
    TYPE_B   // branch type
} InstructionType;


typedef struct 
{
    Opcode opcode;
    InstructionType type;

    uint8_t rd;   // destination register
    uint8_t rn;   // first operand
    uint8_t rm;   // second operand (if R-type)
    int32_t imm;  // immediate value (if I/M/B-type)

    uint32_t raw; // raw 32-bit instruction
} Instruction;


Instruction decode_instruction(uint32_t raw); // Decode a raw 32-bit instruction into an Instruction struct
const char* opcode_to_string(Opcode op); // Convert opcode to string representation
void execute_instruction(CPU *cpu, Memory *mem, Instruction instr); // Execute a given instruction on the CPU and Memory

#endif // INSTRUCTIONS_H