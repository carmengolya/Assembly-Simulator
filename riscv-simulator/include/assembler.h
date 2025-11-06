#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#define MAX_LABEL_SIZE 50
#define MAX_OPCODE_SIZE 10
#define MAX_OPERANDS 3
#define MAX_OPERAND_SIZE 20
#define MAX_INSTRUCTIONS 1024
#define MAX_DATA 1024
    
#define MAX_LINE_SIZE 1024

#include <stdint.h>

typedef struct 
{
    char label[MAX_LABEL_SIZE];
    char opcode[MAX_OPCODE_SIZE];
    char operands[MAX_OPERANDS][MAX_OPERAND_SIZE];
    int operand_count;
    int line_number;
} Instruction;

typedef struct
{
    char label[MAX_LABEL_SIZE];
    uint32_t value;
    uint32_t address;
} DataEntry;

typedef struct 
{
    Instruction instructions[MAX_INSTRUCTIONS];
    int instruction_count;

    DataEntry data[MAX_DATA];
    int data_count;
} AssemblyProgram;

int read_asm_file(char *filename, AssemblyProgram *program);
void print_program(AssemblyProgram *program);

#endif // ASSEMBLER_H
