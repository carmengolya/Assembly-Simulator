#ifndef ENCODER_H
#define ENCODER_H

#include <stdint.h>
#include "assembler.h"

uint32_t encode_instruction(AssemblyProgram *program, Instruction *instr);

#endif // ENCODER_H
