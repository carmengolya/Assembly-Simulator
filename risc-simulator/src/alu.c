#include "alu.h"

ALUResult alu_execute(ALUOp op, uint32_t a, uint32_t b) {
    ALUResult r = {0};
    switch (op) {
        case ALU_ADD: r.result = a + b; break;
        case ALU_SUB: r.result = a - b; break;
        case ALU_AND: r.result = a & b; break;
        case ALU_OR:  r.result = a | b; break;
        case ALU_XOR: r.result = a ^ b; break;
        case ALU_SHL: r.result = a << b; break;
        case ALU_SHR: r.result = a >> b; break;
        case ALU_CMP:
            r.result = a - b; 
            break;
        default: break;
    }
    r.flags.Z = (r.result == 0);
    r.flags.N = (r.result >> 31) & 1;
    return r;
}
