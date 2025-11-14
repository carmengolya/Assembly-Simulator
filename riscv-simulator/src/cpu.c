#include <stdio.h>

#include "cpu.h"
#include "instruction.h"
#include "alu.h"

// ================================================================= //
//                              INIT                                 //
// ================================================================= //

void cpu_init(CPU *cpu)
{
    for(int i = 1; i < REG_NUMBER; i++)
    {
        cpu_set_reg(cpu, i, 0);
    }
    cpu->pc = 0;

    cpu->memory = NULL;              
    cpu->program = NULL;       
    
    cpu->instructions_executed = 0; 
    cpu->halted = 0;                     
    cpu->error = 0; 
}

void cpu_init_with_program(CPU *cpu, Memory *memory, AssemblyProgram *program)
{
    if(!cpu || !memory || !program)
    {
        printf("[ERROR] null argument for cpu, memory or program.\n");
        return;
    }

    cpu_init(cpu);
    cpu->memory = memory;
    cpu->program = program;
    cpu->pc = 0;
}

void cpu_set_reg(CPU *cpu, int index, int32_t value)
{
    if(index == 0)
    {
        printf("[ERROR] tried modifying zero register.\n");
        return; 
    }

    cpu->regs[index] = value;
}

int32_t cpu_get_reg(CPU *cpu, int index)
{
    if(!cpu)
    {
        printf("[ERROR] cpu is null, cannot get any register value.\n");
        return 0;
    }

    if(index < 0 || index >= REG_NUMBER)
    {
        printf("[ERROR] invalid register index.\n");
        return 0;
    }

    return cpu->regs[index];
}


void cpu_print_registers(CPU *cpu)
{
    if(!cpu)
    {
        printf("[ERROR] cpu_print_registers: CPU is NULL\n");
        return;
    }

    printf("=== REGISTERS ===\n");
    printf("PC: 0x%08X\n", cpu->pc);
    for(int i = 0; i < REG_NUMBER; i++)
    {
        printf("x%02d: 0x%08X (%11d)", i, cpu->regs[i], cpu->regs[i]);

        if((i + 1) % 2 == 0)
            printf("\n");
        else
            printf(" | ");
    }
}

void cpu_print_state(CPU *cpu)
{
    if(!cpu)
    {
        printf("[ERROR] cpu_print_state: CPU is NULL\n");
        return;
    }

    printf("\n=== CPU STATE ===\n");
    printf("PC: 0x%08X\n", cpu->pc);
    printf("Instructions executed: %u\n", cpu->instructions_executed);
    printf("Halted: %s\n", cpu->halted ? "YES" : "NO");
    printf("Error: %s\n", cpu->error ? "YES" : "NO");
    printf("\n");

    cpu_print_registers(cpu);
    printf("\n");
}

// ================================================================= //
//                              FETCH                                //
// ================================================================= //

EncodedInstruction cpu_fetch(CPU *cpu)
{
    EncodedInstruction enc = {0};
    if(!cpu || !cpu->memory)
    {
        printf("[ERROR] cpu or cpu memory is null.\n");
        return enc;
    }

    enc.value = memory_read32(cpu->memory, cpu->pc);
    return enc;
}

// ================================================================= //
//                              DECODE                               //
// ================================================================= //

static int cpu_decode_rtype(CPU *cpu, EncodedInstruction enc)
{
    if(!cpu)
    {
        printf("[ERROR] cpu_decode_rtype: CPU is NULL\n");
        return -1;
    }

    uint8_t funct7 = rtype_get_funct7(enc.value);
    uint8_t rs2 = rtype_get_rs2(enc.value);
    uint8_t rs1 = rtype_get_rs1(enc.value);
    uint8_t funct3 = rtype_get_funct3(enc.value);
    uint8_t rd = rtype_get_rd(enc.value);

    printf("[DECODE] R-Type: funct7=0x%02X, rs2=%d, rs1=%d, funct3=0x%X, rd=%d\n",
           funct7, rs2, rs1, funct3, rd);

    return 0;
}

static int cpu_decode_itype(CPU *cpu, EncodedInstruction enc)
{
    if(!cpu)
    {
        printf("[ERROR] cpu_decode_itype: CPU is NULL\n");
        return -1;
    }

    uint8_t funct3 = itype_get_funct3(enc.value);
    uint8_t rs1 = itype_get_rs1(enc.value);
    uint8_t rd = itype_get_rd(enc.value);
    int32_t imm = itype_get_immediate(enc.value);

    printf("[DECODE] I-Type: funct3=0x%X, rs1=%d, rd=%d, imm=%d\n",
           funct3, rs1, rd, imm);

    return 0;
}

static int cpu_decode_stype(CPU *cpu, EncodedInstruction enc)
{
    if(!cpu)
    {
        printf("[ERROR] cpu_decode_stype: CPU is NULL\n");
        return -1;
    }

    printf("[DECODE] S-Type (placeholder)\n");

    return 0;
}

static int cpu_decode_utype(CPU *cpu, EncodedInstruction enc)
{
    if(!cpu)
    {
        printf("[ERROR] cpu_decode_utype: CPU is NULL\n");
        return -1;
    }

    uint8_t rd  = utype_get_rd(enc.value);
    uint32_t imm20 = (uint32_t)utype_get_imm20(enc.value);
    uint8_t opcode = utype_get_opcode(enc.value);
    const char *name = (opcode == 0x37) ? "LUI" : ((opcode == 0x17) ? "AUIPC" : "U-TYPE");

    printf("[DECODE] %s: rd=%d, imm20=0x%05X\n", name, rd, imm20);
    return 0;
}

static int cpu_decode_btype(CPU *cpu, EncodedInstruction enc)
{
    uint32_t funct3 = btype_get_funct3(enc.value);
    uint32_t rs1 = btype_get_rs1(enc.value);
    uint32_t rs2 = btype_get_rs2(enc.value);
    int32_t imm = btype_get_imm(enc.value);
    printf("[DECODE] B-Type: funct3=0x%X, rs1=%u, rs2=%u, imm=%d\n",
           funct3, rs1, rs2, imm);
    return 0;
}

static int cpu_decode_jtype(CPU *cpu, EncodedInstruction enc)
{
    if(!cpu)
    {
        printf("[ERROR] cpu_decode_jtype: CPU is NULL\n");
        return -1;
    }

    uint8_t rd = rtype_get_rd(enc.value);
    int32_t imm = jtype_get_immediate(enc.value);

    printf("[DECODE] J-Type: rd=%u, imm=%d\n", rd, imm);
    return 0;
}

int cpu_decode(CPU *cpu, EncodedInstruction enc)
{
    if(!cpu)
    {
        printf("[ERROR] cpu is null.\n");
        return -1;
    }

    uint8_t opcode = enc.value & 0x7F;
    printf("[DECODE DISPATCH] Opcode=0x%02X\n", opcode);
    switch(opcode)
    {
        case 0x33:
            return cpu_decode_rtype(cpu, enc);

        case 0x03:
        case 0x13:
        case 0x67:
            return cpu_decode_itype(cpu, enc);

        case 0x23:
            return cpu_decode_stype(cpu, enc);

        case 0x17:
        case 0x37:
            return cpu_decode_utype(cpu, enc);

        case 0x63:
            return cpu_decode_btype(cpu, enc);

        case 0x6F:
            return cpu_decode_jtype(cpu, enc);

        default:
            {
                printf("[ERROR] cpu_decode: unknown opcode 0x%02X at PC 0x%08X\n",
                   opcode, cpu->pc);
                cpu->error = 1;
                return -1;
            }
    }
}

// ================================================================= //
//                              EXECUTE                              //
// ================================================================= //

static int cpu_execute_rtype(CPU *cpu, EncodedInstruction enc)
{
    if(!cpu)
    {
        printf("[ERROR] cpu_execute_rtype: CPU is null.\n");
        return -1;
    }

    uint8_t funct7 = rtype_get_funct7(enc.value);
    uint8_t rs2 = rtype_get_rs2(enc.value);
    uint8_t rs1 = rtype_get_rs1(enc.value);
    uint8_t funct3 = rtype_get_funct3(enc.value);
    uint8_t rd = rtype_get_rd(enc.value);

    int32_t val_rs1 = cpu_get_reg(cpu, rs1);
    int32_t val_rs2 = cpu_get_reg(cpu, rs2);

    ALUOp operation;
    const char *op_name = "UNKNOWN";

    if(funct3 == 0x00)
    {
        if(funct7 == 0x00)
        {
            operation = ALU_ADD;
            op_name = "ADD";
        }
        else if(funct7 == 0x20)
        {
            operation = ALU_SUB;
            op_name = "SUB";
        }
        else
        {
            printf("[ERROR] unsupported rtype function funct7 0x%X at PC 0x%08X\n",
               funct7, cpu->pc);
            cpu->error = 1;
            return -1;
        }
    }
    else if(funct3 == 0x01)
    {
        operation = ALU_SLL;
        op_name = "SLL";
    }
    else if(funct3 == 0x04)
    {
        operation = ALU_XOR;
        op_name = "XOR";
    }
    else if(funct3 == 0x05)
    {
        if(funct7 == 0)
        {
            operation = ALU_SRL;
            op_name = "SRL";
        }
        else if(funct7 == 0x20)
        {
            operation = ALU_SRA;
            op_name = "SRA";
        }
        else
        {
            printf("[ERROR] unsupported rtype function funct7 0x%X at PC 0x%08X\n",
               funct7, cpu->pc);
            cpu->error = 1;
            return -1;
        }
    }
    else if(funct3 == 0x06)
    {
        operation = ALU_OR;
        op_name = "OR";
    }
    else if(funct3 == 0x07)
    {
        operation = ALU_AND;
        op_name = "AND";
    }
    else
    {
        printf("[ERROR] cpu_execute_rtype: unsupported funct3 0x%X at PC 0x%08X\n",
               funct3, cpu->pc);
        cpu->error = 1;
        return -1;
    }

    int32_t result = alu_execute(operation, val_rs1, val_rs2);
    cpu_writeback(cpu, rd, result);

    printf("[EXEC] %s x%d, x%d, x%d -> x%d = 0x%08X (rs1=0x%08X, rs2=0x%08X)\n",
           op_name, rd, rs1, rs2, rd, result, val_rs1, val_rs2);

    return 0;
}

static int cpu_execute_itype(CPU *cpu, EncodedInstruction enc)
{
    if(!cpu)
    {
        printf("[ERROR] cpu_execute_itype: CPU is null.\n");
        return -1;
    }

    uint8_t opcode = itype_get_opcode(enc.value);
    uint8_t funct3 = itype_get_funct3(enc.value);
    int32_t imm = itype_get_immediate(enc.value);
    uint8_t rs1 = itype_get_rs1(enc.value);
    uint8_t rd = itype_get_rd(enc.value);

    const char *op_name = "UNKNOWN";
    int32_t value = 0;

    if(opcode == 0x03)
    {
        if(funct3 == 0x2)  
        {
            int32_t addr_base = cpu_get_reg(cpu, rs1);
            uint32_t data_offset = cpu->program->instruction_count * 4;
            uint32_t addr = data_offset + addr_base + imm;

            op_name = "LW";
            value = memory_read32(cpu->memory, addr);
            cpu_writeback(cpu, rd, value);
            printf("[EXEC] %s x%d, %d(x%d) -> Load from 0x%08X = 0x%08X\n",
                op_name, rd, imm, rs1, addr, value);
            return 0;
        }
        else
        {
            printf("[ERROR] cpu_execute_itype: unsupported funct3 0x%X at PC 0x%08X\n",
                funct3, cpu->pc);
            cpu->error = 1;
            return -1;
        }
    }
    else if(opcode == 0x13)
    {
        if(funct3 == 0x00)  
        {
            int32_t val_rs1 = cpu_get_reg(cpu, rs1);
            int32_t result  = val_rs1 + imm;
            cpu_writeback(cpu, rd, result);

            if(rs1 == 0)
                printf("[EXEC] LI x%d, %d -> x%d = 0x%08X\n",
                   rd, imm, rd, result);
            else
                printf("[EXEC] ADDI x%d, x%d, %d -> x%d = 0x%08X (rs1=0x%08X)\n",
                   rd, rs1, imm, rd, result, val_rs1);
            return 0;
        }
        else
        {
            printf("[ERROR] cpu_execute_itype: unsupported OP-IMM funct3 0x%X at PC 0x%08X\n",
                funct3, cpu->pc);
            cpu->error = 1;
            return -1;
        }
    }
    else if(opcode == 0x67)
    {
        if(funct3 == 0x0)
        {
            uint32_t pc_before_inc = cpu->pc - 4;
            int32_t base = cpu_get_reg(cpu, rs1);
            uint32_t target = (uint32_t)((base + imm) & ~1U);

            cpu_writeback(cpu, rd, (int32_t)(pc_before_inc + 4));
            cpu->pc = target;

            printf("[EXEC] JALR x%d, x%d, imm=%d -> new PC=0x%08X (rs1=0x%08X)\n",
                rd, rs1, imm, cpu->pc, (uint32_t)base);

            return 0;
        }
        else
        {
            printf("[ERROR] cpu_execute_itype: unsupported I-type funct3=0x%X for opcode 0x67 at PC 0x%08X\n",
                   funct3, cpu->pc - 4);
            cpu->error = 1;
            return -1;
        }
    }

    printf("[ERROR] cpu_execute_itype: unsupported I-type opcode 0x%02X at PC 0x%08X\n",
           opcode, cpu->pc);
    cpu->error = 1;
    return -1;
}

static int cpu_execute_stype(CPU *cpu, EncodedInstruction enc)
{
    if(!cpu)
    {
        printf("[ERROR] cpu_execute_stype: CPU is null.\n");
        return -1;
    }

    uint8_t funct3 = stype_get_funct3(enc.value);
    uint8_t rs1 = stype_get_rs1(enc.value);
    uint8_t rs2 = stype_get_rs2(enc.value);
    int32_t imm = stype_get_immediate(enc.value);

    int32_t addr_base = cpu_get_reg(cpu, rs1);
    int32_t value = cpu_get_reg(cpu, rs2);

    uint32_t data_offset = cpu->program->instruction_count * 4;
    uint32_t addr = data_offset + addr_base + imm;

    const char *op_name = "UNKNOWN";

    if(funct3 == 0x2)  
    {
        op_name = "SW";
        printf("[EXEC] %s x%d, %d(x%d) -> Store 0x%08X to 0x%08X\n",
               op_name, rs2, imm, rs1, value, addr);
        memory_write32(cpu->memory, addr, value);
        return 0;
    }

    printf("[ERROR] cpu_execute_stype: unsupported funct3 0x%X at PC 0x%08X\n",
        funct3, cpu->pc);
    cpu->error = 1;
    return -1;
}

static int cpu_execute_utype(CPU *cpu, EncodedInstruction enc)
{
    if(!cpu)
    {
        printf("[ERROR] cpu_execute_utype: CPU is null.\n");
        return -1;
    }

    uint8_t opcode = utype_get_opcode(enc.value);
    uint8_t rd = utype_get_rd(enc.value);
    int32_t imm_aligned = utype_get_immediate(enc.value);

    if (opcode == 0x37)
    {
        cpu_writeback(cpu, rd, imm_aligned);
        printf("[EXEC] LUI x%d, 0x%05X -> x%d = 0x%08X\n",
               rd, (unsigned)utype_get_imm20(enc.value), rd, (uint32_t)imm_aligned);
        return 0;
    }
    else if (opcode == 0x17)
    {
        uint32_t pc_before = cpu->pc - 4; 
        uint32_t result = pc_before + (uint32_t)imm_aligned;
        
        cpu_writeback(cpu, rd, (int32_t)result);
        printf("[EXEC] AUIPC x%d, 0x%05X -> x%d = PC(0x%08X) + 0x%08X = 0x%08X\n",
               rd, (unsigned)utype_get_imm20(enc.value), rd, pc_before, (uint32_t)imm_aligned, result);
        return 0;
    }

    printf("[ERROR] cpu_execute_utype: unsupported U-type opcode 0x%02X at PC 0x%08X\n",
           opcode, cpu->pc);
    cpu->error = 1;
    return -1;
}

static int cpu_execute_btype(CPU *cpu, EncodedInstruction enc)
{
    uint32_t funct3 = btype_get_funct3(enc.value);
    uint32_t rs1 = btype_get_rs1(enc.value);
    uint32_t rs2 = btype_get_rs2(enc.value);
    int32_t imm = btype_get_imm(enc.value);

    int32_t v1 = cpu_get_reg(cpu, rs1);
    int32_t v2 = cpu_get_reg(cpu, rs2);
    int take = 0;
    const char *name = NULL;

    switch(funct3)
    {
        case 0x0:
            name = "BEQ";
            take = (v1 == v2);
            break;
        case 0x1:
            name = "BNE";
            take = (v1 != v2);
            break;
        case 0x4:
            name = "BLT";
            take = (v1 < v2);
            break;
        case 0x5:
            name = "BGE";
            take = (v1 >= v2);
            break;
        default:
            printf("[ERROR] cpu_execute_btype: unsupported funct3=0x%X at PC=0x%08X\n",
                   funct3, cpu->pc - 4);
            cpu->error = 1;
            return -1;
    }

    printf("[EXEC] %s x%d, x%d, imm=%d -> %s (rs1=0x%08X, rs2=0x%08X)\n",
           name, rs1, rs2, imm, take ? "TAKEN" : "NOT TAKEN",
           (uint32_t)v1, (uint32_t)v2);

    if(take)
    {
        uint32_t pc_before_inc = cpu->pc - 4;
        cpu->pc = pc_before_inc + imm;
    }
    return 0;
}

static int cpu_execute_jtype(CPU *cpu, EncodedInstruction enc)
{
    if(!cpu)
    {
        printf("[ERROR] cpu_execute_jtype: CPU is null.\n");
        return -1;
    }

    uint8_t rd = rtype_get_rd(enc.value);
    int32_t imm = jtype_get_immediate(enc.value);

    uint32_t pc_before_inc = cpu->pc - 4;

    cpu_writeback(cpu, rd, (int32_t)(pc_before_inc + 4));

    cpu->pc = pc_before_inc + imm;

    printf("[EXEC] JAL x%d, imm=%d -> new PC=0x%08X (return=0x%08X)\n",
           rd, imm, cpu->pc, (uint32_t)(pc_before_inc + 4));

    return 0;
}

int cpu_execute(CPU *cpu, EncodedInstruction enc)
{
    if(!cpu)
    {
        printf("[ERROR] cpu is null.\n");
        return -1;
    }

    uint8_t opcode = enc.value & 0x7F;
    switch(opcode)
    {
        case 0x03:
        case 0x13:
        case 0x67:
            return cpu_execute_itype(cpu, enc);

        case 0x23:  
            return cpu_execute_stype(cpu, enc);

        case 0x33:
            return cpu_execute_rtype(cpu, enc);

        case 0x17:
        case 0x37:
            return cpu_execute_utype(cpu, enc);

        case 0x63:
            return cpu_execute_btype(cpu, enc);

        case 0x6F:
            return cpu_execute_jtype(cpu, enc);

        default:
        {
            printf("[ERROR] cpu_execute: unknown opcode 0x%02X at PC 0x%08X\n",
                   opcode, cpu->pc);
            cpu->error = 1;
            return -1;
        }
    }
}

void cpu_writeback(CPU *cpu, int rd, int32_t value)
{
    if(!cpu)
    {
        printf("[ERROR] cpu is null.\n");
        return;
    }

    cpu_set_reg(cpu, rd, value);
}

int cpu_step(CPU *cpu)
{
    if(!cpu)
    {
        printf("[ERROR] cpu is null.\n");
        return -1;
    }

    if(cpu->halted)
    {
        printf("[INFO] cpu is halted.\n");
        return 0;
    }

    uint32_t program_end = cpu->program->instruction_count * 4;

    if(cpu->pc >= program_end)
    {
        printf("[INFO] cpu_step: PC (0x%08X) reached end of program (program size: %d bytes)\n",
               cpu->pc, program_end);
        cpu->halted = 1;
        return 0;
    }

    if(cpu->pc >= cpu->memory->size)
    {
        printf("[INFO] cpu_step: PC (0x%08X) reached end of program\n", cpu->pc);
        cpu->halted = 1;
        return 0;
    }

    // 1. fetch
    EncodedInstruction enc = cpu_fetch(cpu);

    printf("\n[STEP %u] PC=0x%08X, Instruction=0x%08X\n",
           cpu->instructions_executed, cpu->pc, enc.value);

    // 1.1 increment
    cpu->pc += 4; 

    // 2. decode
    if(cpu_decode(cpu, enc) < 0)
    {
        printf("[ERROR] cpu_step: decode failed\n");
        return -1;
    }

    // 3. execute
    if(cpu_execute(cpu, enc) < 0)
    {
        printf("[ERROR] cpu_step: execution failed\n");
        return -1;
    }

    cpu->instructions_executed++;

    return 0;
}

int cpu_run(CPU *cpu)
{
    if(!cpu)
    {
        printf("[ERROR] cpu_run: CPU is NULL\n");
        return -1;
    }

    printf("\n=== Starting CPU Execution ===\n");

    while (!cpu->halted && cpu->instructions_executed < 1000)
    {
        if(cpu_step(cpu) < 0)
        {
            printf("[ERROR] cpu_run: execution failed at step %u\n",
                   cpu->instructions_executed);
            return -1;
        }
    }

    if(cpu->instructions_executed >= 1000)
    {
        printf("[WARN] cpu_run: execution limit (1000 instructions) reached\n");
    }

    printf("\n=== CPU Execution Finished ===\n");
    printf("Total instructions executed: %u\n", cpu->instructions_executed);

    return 0;
}
