#ifndef DECODER_H
#define DECODER_H

#include "alu.h"

int reg_index(const char *name);

int32_t parse_immediate(const char *str);
int parse_memory_operand(const char *operand, int32_t *out_offset, int *out_reg);

#endif // DECODER_H
