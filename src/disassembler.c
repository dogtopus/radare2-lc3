/* Little Computer 3 (LC-3) disassembler for Radare2 */

#include "functions.h"

int do_disassemble(RAsm *ctx, RAsmOp *result, const ut8 *input, int len) {
    char argsbuf[32];
    int p1, p2, p3;

    ut16 ins = (input[0] << 8) | input[1];
    inst_table op = getop(ins);
    result->size = 2;

    if (ctx->pc & 1) {
        strcpy(result->buf_asm, "unaligned");
        return -1;
    }

    if (op.format == RFU) {
        strcpy(result->buf_asm, "invalid");
        return -1;
    }

    strcpy(result->buf_asm, op.name);

    argsbuf[0] = 0;

    switch (op.format) {
        case BR:
            if (getflag(ins, FLAG_BR_N)) strcat(result->buf_asm, "n");
            if (getflag(ins, FLAG_BR_Z)) strcat(result->buf_asm, "z");
            if (getflag(ins, FLAG_BR_P)) strcat(result->buf_asm, "p");
            p1 = getimmsext(ins, 9);
            sprintf(argsbuf, "0x%llx", ctx->pc + p1);
            break;
        case ADDAND:
            p1 = getreg(ins, REG1);
            p2 = getreg(ins, REG2);
            if (getflag(ins, FLAG_ADDAND_ISIMM)) {
                p3 = getimmsext(ins, 5);
                sprintf(argsbuf, "r%d, r%d, %d", p1, p2, p3);
            } else {
                p3 = getreg(ins, REG3);
                sprintf(argsbuf, "r%d, r%d, r%d", p1, p2, p3);
            }
            break;
        case JMP:
            p1 = getreg(ins, REG2);
            if (p1 == 7) {
                /* we call it ret */
                strcpy(result->buf_asm, "ret");               
            } else {
                /* just a regular jump */
                sprintf(argsbuf, "r%d", p1);
            }
            break;
        case JSR:
            if (getflag(ins, FLAG_JSR_ISJSR)) {
                p1 = getimmsext(ins, 11);
                sprintf(argsbuf, "0x%llx", ctx->pc + p1);
            } else {
                strcat(result->buf_asm, "r");
                p1 = getreg(ins, REG2);
                sprintf(argsbuf, "r%d", p1);
            }
            break;
        case LDSTLEA:
            p1 = getreg(ins, REG1);
            p2 = getimmsext(ins, 9);
            if (!strcmp(op.name, "lea")) {
                sprintf(argsbuf, "r%d, 0x%llx", p1, ctx->pc + p2);
            } else {
                sprintf(argsbuf, "r%d, [0x%llx]", p1, ctx->pc + p2);
            }
            break;
        case LDRSTR:
            p1 = getreg(ins, REG1);
            p2 = getreg(ins, REG2);
            p3 = getimmsext(ins, 6);
            sprintf(argsbuf, "r%d, [r%d, %d]", p1, p2, p3);
            break;
        case NOT:
            p1 = getreg(ins, REG1);
            p2 = getreg(ins, REG2);
            sprintf(argsbuf, "r%d, r%d", p1, p2);
            break;
        case RTI:
            break;
        case TRAP:
            p1 = getimm(ins, 8);
            sprintf(argsbuf, "0x%x", p1);
            break;
        default:
            strcpy(result->buf_asm, "unhandled");
            return -1;
    }
    
    if (argsbuf[0]) {
        strcat(result->buf_asm, " ");
        strcat(result->buf_asm, argsbuf);
    }

    return result->size;
}
