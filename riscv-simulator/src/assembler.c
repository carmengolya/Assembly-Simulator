#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "assembler.h"

static void eliminate_comment(char *line)
{
    char *commPtr = strstr(line, "//");
    if(commPtr)
    {
        *commPtr = '\0';
    }
}

static void eliminate_whitespaces(char *line)
{
    if(!line)
        return;
    
    char *wsPtr = line;
    while(isspace(*wsPtr))
    {
        wsPtr++;
    }

    if(wsPtr != line)
    {
        memmove(line, wsPtr, strlen(wsPtr) + 1);
    }

    int len = strlen(line);
    while(len > 0 && isspace(line[len - 1]))
    {
        line[--len] = '\0';
    }
}

static int parse_operands(char *line, char dest_operands[MAX_OPERANDS][MAX_OPERAND_SIZE])
{
    int count = 0;
    char *token = strtok(line, ",");
    while(token && count < MAX_OPERANDS)
    {
        eliminate_whitespaces(token);
        strncpy(dest_operands[count++], token, MAX_OPERAND_SIZE - 1);
        dest_operands[count - 1][MAX_OPERAND_SIZE - 1] = '\0';
        token = strtok(NULL, ",");
    }
    return count;
}

int read_asm_file(char *filename, AssemblyProgram *program)
{
    FILE *f = NULL;
    if((f = fopen(filename, "r")) == NULL)
    {
        perror("[ERROR] opening assembly test file.");
        return -1;
    }

    program->instruction_count = 0;

    char line[MAX_LINE_SIZE];
    int line_number = 0;

    while(fgets(line, sizeof(line), f))
    {
        // step 1: trimming the line
        line_number++;
        eliminate_comment(line);
        eliminate_whitespaces(line);
        if(strlen(line) == 0)
            continue;

        // step 2: if it isn't an empty line, we create an instruction
        Instruction instr = {0};
        instr.line_number = line_number;

        // step 3: parse the command through this syntax -> [ label: ] [opcode [operands]] 
        // step 3.1: [label]
        char *colon = strchr(line, ':');
        if(colon)
        {
            *colon = '\0';
            strncpy(instr.label, line, MAX_LABEL_SIZE - 1);
            memmove(line, colon + 1, strlen(colon + 1) + 1);
            eliminate_whitespaces(line);
        }

        // step 3.2: [opcode]
        char *token = strtok(line, " \t");
        if(!token)
            continue;
        
        strncpy(instr.opcode, token, MAX_OPCODE_SIZE - 1);

        // step 3.3: [... [operands]]
        char *rest = strtok(NULL, "");
        eliminate_whitespaces(rest);
        if(rest)
        {
            instr.operand_count = parse_operands(rest, instr.operands);
        }

        program->instructions[program->instruction_count++] = instr;
    }

    if(fclose(f) != 0)
    {
        perror("[ERROR] closing assembly test file.");
        return -1;
    }

    return 0;
}

void print_program(AssemblyProgram *program)
{
    for(int i = 0; i < program->instruction_count; i++)
    {
        Instruction *current = &program->instructions[i];
        printf("[%02d] ", i);
        if(strlen(current->label))
            printf("%s : ", current->label);
        printf("%s ", current->opcode);
        for(int i = 0; i < current->operand_count; i++)
        {
            printf("%s", current->operands[i]);
            if(i != current->operand_count - 1)
                printf(", ");
        }
        printf("\n");
    }
}