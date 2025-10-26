#ifndef DECODER_H
#define DECODER_H

#include "alu.h"

int reg_index(const char *name);
ALUOp decode_opcode(char *opcode);

#endif // DECODER_H
