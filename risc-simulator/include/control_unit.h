#ifndef CONTROL_UNIT_H
#define CONTROL_UNIT_H

// control_unit.h - Defines the control unit for a simple RISC simulator
// The control unit is responsible for decoding instructions and generating control signals.
// It interprets the opcode and function fields of the instruction to determine
// the type of instruction and the necessary control signals for execution.
// It works closely with the ALU and CPU to facilitate instruction execution.

#include <stdint.h>
#include <stdbool.h>
#include "alu.h"

typedef enum 
{
    INST_TYPE_R,
    INST_TYPE_I,
    INST_TYPE_MEM,
    INST_TYPE_BRANCH,
    INST_TYPE_SPECIAL
} InstrType;

typedef struct 
{
    InstrType type;
    ALUOp alu_op;
    bool mem_read;
    bool mem_write;
    bool reg_write;
    bool branch;
    bool jump;
} ControlSignals;

ControlSignals decode_instruction(uint32_t instruction); // Decodes the instruction and generates control signals

#endif // CONTROL_UNIT_H