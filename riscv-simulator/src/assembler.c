#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "assembler.h"

static void eliminate_block_comments(char *buffer)
{
    if(!buffer)
        return;

    char *blockStart = strstr(buffer, "/*");
    while(blockStart)
    {
        char *blockEnd = strstr(blockStart, "*/");
        if(blockEnd)
        {
            memmove(blockStart, blockEnd + 2, strlen(blockEnd + 2) + 1);
            blockStart = strstr(blockStart, "/*");
        }
        else
        {
            *blockStart = '\0';
            break;
        }
    }
}

static void eliminate_comment(char *line)
{
    if(!line)
        return;

    char *lineStart = strstr(line, "//");
    if(lineStart)
    {
        *lineStart = '\0';
    }

    lineStart = strchr(line, '#');
    if(lineStart)
    {
        *lineStart = '\0';
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

int find_symbol(AssemblyProgram *program, const char *name, uint32_t *addr_out)
{
    if(!program || !name) return -1;
    for(int i = 0; i < program->symbol_count; ++i)
    {
        if(strcmp(program->symbols[i].name, name) == 0)
        {
            if(addr_out) *addr_out = program->symbols[i].address;
            return 0;
        }
    }
    return -1;
}

static void build_symbol_table(AssemblyProgram *program)
{
    program->symbol_count = 0;
    for(int i = 0; i < program->instruction_count; ++i)
    {
        Instruction *instr = &program->instructions[i];
        if(instr->label[0] != '\0')
        {
            if(program->symbol_count >= MAX_SYMBOLS)
            {
                printf("[WARN] symbol capacity exceeded\n");
                break;
            }
            Symbol s = {0};
            strncpy(s.name, instr->label, MAX_LABEL_SIZE - 1);
            s.address = instr->address;
            program->symbols[program->symbol_count++] = s;
        }
    }
}

int read_asm_file(char *filename, AssemblyProgram *program)
{
    FILE *f = NULL;
    if((f = fopen(filename, "r")) == NULL)
    {
        printf("[ERROR] opening assembly test file.\n");
        return -1;
    }

    fseek(f, 0, SEEK_END);
    long fileSize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *buffer = (char *)malloc(fileSize + 1);
    if(!buffer)
    {
        printf("[ERROR] memory allocation failed for file buffer.\n");
        fclose(f);
        return -1;
    }

    size_t readSize = fread(buffer, 1, fileSize, f);
    buffer[readSize] = '\0';

    if(fclose(f) != 0)
    {
        printf("[ERROR] closing assembly test file.\n");
        return -1;
    }

    eliminate_block_comments(buffer);

    program->instruction_count = 0;
    program->data_count = 0;

    char *line_ptr = buffer;
    char line[MAX_LINE_SIZE];
    int line_number = 0;

    int in_data = 0;
    int in_text = 0;

    while(line_ptr && *line_ptr != '\0')
    {
        // step 0: extracting one line
        char *newline = strchr(line_ptr, '\n');
        size_t line_len = newline ? (newline - line_ptr) : strlen(line_ptr);
         if(line_len >= MAX_LINE_SIZE)
            line_len = MAX_LINE_SIZE - 1;

        strncpy(line, line_ptr, line_len);
        line[line_len] = '\0';

        // step 1: trimming the line
        line_number++;
        eliminate_comment(line);
        eliminate_whitespaces(line);
        if(strlen(line) == 0)
        {
            line_ptr = newline ? newline + 1 : NULL;   
            continue;
        }

        // step 1.1: detecting the data and the text section
        if(strcmp(line, ".data") == 0)
        {
            in_data = 1;
            in_text = 0;
            line_ptr = newline ? newline + 1 : NULL;
            continue;
        }
        else if(strcmp(line, ".text") == 0)
        {
            in_text = 1;
            in_data = 0;
            line_ptr = newline ? newline + 1 : NULL;
            continue;
        }

        // optional/alternative: if we don't have .data + .text sections, treat everything as instructions
        if(!in_data && !in_text) 
        {
            in_text = 1; 
        }
        // step 1.2: processing the two blocks
        // step 1.2a: processing the .data block
        if(in_data)
        {
            char *colon = strchr(line, ':');
            char *data_ptr = line;
            char label[MAX_LABEL_SIZE] = {0};

            if(colon)
            {
                *colon = '\0';
                strncpy(label, line, MAX_LABEL_SIZE - 1);
                data_ptr = colon + 1;
                eliminate_whitespaces(data_ptr);
            }

            if(strncmp(data_ptr, ".word", 5) == 0)
            {
                data_ptr += 5;
                eliminate_whitespaces(data_ptr);

                char *token = strtok(data_ptr, ",");
                int first = 1;
                while(token)
                {
                    eliminate_whitespaces(token);
                    DataEntry entry = {0};
                    entry.address = program->data_count * 4;
                    entry.value = atoi(token);
                    if(first)
                    {
                        strncpy(entry.label, label, MAX_LABEL_SIZE - 1);
                        first = 0;
                    }
                    program->data[program->data_count++] = entry;
                    token = strtok(NULL, ",");
                }
            }

            line_ptr = newline ? newline + 1 : NULL;
            continue;
        }
        // step 1.2b: processing the .text block
        if(in_text)
        {
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
            {
                line_ptr = newline ? newline + 1 : NULL;
                continue;
            }
            
            strncpy(instr.opcode, token, MAX_OPCODE_SIZE - 1);
            for(int i = 0; instr.opcode[i]; i++)
                instr.opcode[i] = tolower(instr.opcode[i]);

            // step 3.3: [... [operands]]
            char *rest = strtok(NULL, "");
            if(rest)
            {
                eliminate_whitespaces(rest);
                if(strlen(rest) > 0)
                {
                    instr.operand_count = parse_operands(rest, instr.operands);
                }
            }

            program->instructions[program->instruction_count++] = instr;
        }

        line_ptr = newline ? newline + 1 : NULL;
    }

    for(int i = 0; i < program->instruction_count; ++i)
    {
        program->instructions[i].address = (uint32_t)(i * 4);
    }
    build_symbol_table(program);

    free(buffer);
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

    for(int i = 0; i < program->data_count; i++)
    {
        DataEntry *d = &program->data[i];
        printf("DATA[%02d] %s = %d @ address %d\n", i, d->label, d->value, d->address);
    }
}