#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "assembler.h"
#include "cpu.h"
#include "encoder.h"
#include "memory.h"

int main(int argc, char **argv) 
{
    printf("=================================================================\n");
    printf("        RISC-V Assembly Simulator - Executor Test\n");
    printf("=================================================================\n\n");

    // ===== STEP 1: PARSE ASM FILE =====
    printf("[STEP 1] Parsing assembly file...\n");
    if(argc != 2)
    {
        printf("[ERROR] main: not enough arguments.\n");
        return 1;
    }

    char *filename = argv[1];
    AssemblyProgram program = {0};

    if(read_asm_file(filename, &program) < 0)
    {
        printf("[FAILED] read_asm_file function failed.\n");
        return 1;
    }
    printf("[OK] Loaded %d instructions\n", program.instruction_count);
    print_program(&program);

    // ===== STEP 2: INITIALIZE MEMORY =====
    printf("\n[STEP 2] Initializing memory...\n");
    Memory m = memory_init(400);
    printf("[OK] Memory initialized (size: %zu bytes)\n", m.size);

    // ===== STEP 3: ENCODE INSTRUCTIONS =====
    printf("\n[STEP 3] Encoding instructions...\n");
    uint32_t *enc = (uint32_t *)malloc(sizeof(uint32_t) * program.instruction_count);
    if(!enc)
    {
        printf("[ERROR] memory allocation for encoded array failed.\n");
        return 1;
    }

    int encoded_count = 0;
    for(int i = 0; i < program.instruction_count; ++i)
    {
        Instruction *instr = &program.instructions[i];

        printf("[%02d] (PC=0x%08X) ", i, instr->address);
        if(instr->label[0] != '\0')
            printf("%s: ", instr->label);
        printf("%s ", instr->opcode);

        for (int j = 0; j < instr->operand_count; ++j)
        {
            printf("%s", instr->operands[j]);
            if(j + 1 < instr->operand_count) printf(", ");
        }

        uint32_t code = encode_instruction(&program, instr);
        enc[i] = code;
        printf(" -> encoded: 0x%08X\n", enc[i]);
        if(code == 0)
        {
            printf("[ERROR] Encoding failed at instruction %d (line %d). Aborting.\n",
                i, instr->line_number);
            free(enc);
            memory_free(&m);
            return 1;
        }
        encoded_count++;
    }
    printf("[OK] Encoded %d/%d instructions\n", encoded_count, program.instruction_count);

    // ===== STEP 4: LOAD PROGRAM INTO MEMORY =====
    printf("\n[STEP 4] Loading program into memory...\n");
    load_program_into_memory(&m, enc, program.instruction_count, 0);
    printf("[OK] Program loaded at address 0x00000000\n");

    // ===== STEP 4B: LOAD DATA INTO MEMORY (AFTER INSTRUCTIONS) =====
    uint32_t data_offset = program.instruction_count * 4;  
    if(program.data_count > 0)
    {
        printf("\n[STEP 4B] Loading data section into memory...\n");
        load_data_into_memory(&m, &program, data_offset);
        printf("[OK] Data loaded starting at address 0x%08X\n", data_offset);
    }

    printf("[OK] Data loaded at address 0x%08X\n", data_offset);

    printf("\n[DEBUG] Memory dump after loading:\n");
    memory_dump_words(&m, 0, data_offset + 16);

    // ===== STEP 5: INITIALIZE CPU =====
    printf("\n[STEP 5] Initializing CPU...\n");
    CPU cpu;
    cpu_init_with_program(&cpu, &m, &program);
    printf("[OK] CPU initialized\n");

    printf("\n[DEBUG] Initial CPU state:\n");
    cpu_print_state(&cpu);

    // ===== STEP 6: EXECUTE PROGRAM =====
    printf("\n[STEP 6] Executing program...\n");
    printf("-----------------------------------------------------------------\n");
    
    int exec_result = cpu_run(&cpu);
    
    printf("-----------------------------------------------------------------\n");

    if(exec_result < 0)
    {
        printf("[FAILED] CPU execution failed!\n");
        free(enc);
        memory_free(&m);
        return 1;
    }

    printf("\n[DEBUG] Memory dump (data region) after execution:\n");
    memory_dump_words(&m, data_offset, 8);
    // ===== STEP 7: PRINT FINAL STATE =====
    printf("\n[STEP 7] Final CPU state:\n");
    printf("-----------------------------------------------------------------\n");
    cpu_print_state(&cpu);
    printf("-----------------------------------------------------------------\n");

    // ===== STEP 8: SUMMARY =====
    printf("\n[SUMMARY]\n");
    printf("  Program instructions: %d\n", program.instruction_count);
    printf("  Instructions executed: %u\n", cpu.instructions_executed);
    printf("  Final PC: 0x%08X\n", cpu.pc);
    printf("  CPU halted: %s\n", cpu.halted ? "YES" : "NO");
    printf("  CPU error: %s\n", cpu.error ? "YES" : "NO");
    printf("\n");

    // ===== CLEANUP =====
    printf("[CLEANUP] Freeing memory...\n");
    free(enc);
    memory_free(&m);
    printf("[OK] Cleanup complete\n");

    printf("\n=================================================================\n");
    printf("                    Execution Completed\n");
    printf("=================================================================\n");
    return 0;
}
