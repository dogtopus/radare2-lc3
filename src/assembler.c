#include "asm_functions.h"

#define GTFO(...) eprintf(__VA_ARGS__); return -1

#define scanreg(num, to) \
if (!sscanf(args[num], "r%d", to)) { \
    GTFO("Arg %d must be a register\n", num+1); \
}

/* perhaps use strtoull() for this? */
#define _scanimmhex(num, to) \
if (!sscanf(args[num], "0x%llx", to)) { \
    GTFO("Arg %d must be a hex immediate value\n", num+1); \
}

#define _scanimmdec(num, to) \
if (!sscanf(args[num], "%llu", to)) { \
    GTFO("Arg %d must be a immediate value\n", num+1); \
}

#define scanimm(num, to) \
if (!strncmp(args[num], "0x", 2)) { \
    _scanimmhex(num, to); \
} else { \
    _scanimmdec(num, to); \
}

#define chksetreg(ins, reg, shl) \
if (setreg(ins, reg, shl) < 0) { \
    GTFO("Invalid register\n"); \
}

#define chksetimm(ins, val, size) \
if (setimmsext(ins, val, size) < 0) { \
    GTFO("Immediate value out of range\n"); \
}

#define chksetimmzext(ins, val, size) \
if (setimm(ins, val, size) < 0) { \
    GTFO("Immediate value out of range\n"); \
}

static inline void skip_space(const char **buf, int max_len) {
    /* also treat square brackets as spaces since they are just there for cosmetic purpose */
    for (int i=0; i<max_len; i++) {
        if ((*buf)[0] == ' ' || (*buf)[0] == '[' || (*buf)[0] == ']') {
            (*buf)++;
        } else {
            break;
        }
    }
}

int do_assemble(RAsm *ctx, RAsmOp *result, const char *input) {
    int opcode;
    inst_table op;
    int p1, p2, p3, i;
    ut64 off;
    ut16 ins = 0;
    char insname[8];
    const char *args[3], *argsbuf;

    memset(args, 0, sizeof(args));

    /* scan for instruction name, with or without space */
    if (!sscanf(input, "%7s", insname)) {
        GTFO("Empty input\n");
    }

    argsbuf = &(input[strlen(insname)]);

    if (argsbuf[0] && argsbuf[0] != ' ') {
        eprintf("Instruction name too long\n");
    }

    for (i=0; i<3; i++) {
        /* TODO calculate this properly */
        skip_space(&argsbuf, R_ASM_BUFSIZE);
        /* stop immediately if this is the end of the string or there are still spaces left */
        if (!argsbuf[0] || argsbuf[0] == ' ' || argsbuf[0] == '[' || argsbuf[0] == ']') {
            break;
        } else {
            args[i] = argsbuf;
        }

        argsbuf = strchr(argsbuf, ',');
        if (!argsbuf) {
            break;
        } else {
            argsbuf++;
        }
    }

    opcode = getopbyname(insname);
    if (opcode < 0) {
        GTFO("Unrecognized instruction\n");
    }

    setins(&ins, opcode);
    op = INST_TABLE[opcode];

    result->size = 2;

    switch (op.format) {
        case BR:
            /* starts from br */
            for (i=2; i<5; i++) {
                p1 = insname[i];
                if (p1 == 'n' && !getflag(ins, FLAG_BR_N)) {
                    setflag(&ins, FLAG_BR_N);
                } else if (p1 == 'z' && !getflag(ins, FLAG_BR_Z)) {
                    setflag(&ins, FLAG_BR_Z);
                } else if (p1 == 'p' && !getflag(ins, FLAG_BR_P)) {
                    setflag(&ins, FLAG_BR_P);
                } else if (!p1) {
                    break;
                } else {
                    GTFO("Unrecognized br flag %c\n", p1);
                }
            }
            scanimm(0, &off);
            p1 = (int) (off - ctx->pc);
            chksetimm(&ins, p1, 9);
            break;
        case ADDAND:
            scanreg(0, &p1);
            scanreg(1, &p2);
            if (setreg(&ins, p1, REG1) < 0 || setreg(&ins, p2, REG2) < 0) {
                GTFO("Invalid register\n");
            }
            if (args[2][0] == 'r') {
                scanreg(2, &p3);
                chksetreg(&ins, p3, REG3);
            } else {
                scanimm(2, &off);
                p3 = (int) off;
                setflag(&ins, FLAG_ADDAND_ISIMM);
                chksetimm(&ins, p3, 9);
            }
            break;
        case JMP:
            if (!strcmp(insname, "ret")) {
                p1 = 7;
            } else {
                scanreg(0, &p1);
            }
            chksetreg(&ins, p1, REG2);
            break;
        case JSR:
            if (!strcmp(insname, "jsrr")) {
                scanreg(0, &p1);
                chksetreg(&ins, p1, REG2);
            } else {
                scanimm(0, &off);
                p1 = (int) (off - ctx->pc);
                chksetimm(&ins, p1, 11);
            }
            break;
        case LDSTLEA:
            scanreg(0, &p1);
            scanimm(1, &off);
            p2 = (int) (off - ctx->pc);
            chksetreg(&ins, p1, REG1);
            chksetimm(&ins, p2, 9);
            break;
        case LDRSTR:
            scanreg(0, &p1);
            scanreg(1, &p2);
            scanimm(2, &off);
            p3 = (int) off;
            chksetreg(&ins, p1, REG1);
            chksetreg(&ins, p2, REG2);
            chksetimm(&ins, p3, 6);
            break;
        case NOT:
            scanreg(0, &p1);
            scanreg(1, &p2);
            chksetreg(&ins, p1, REG1);
            chksetreg(&ins, p2, REG2);
            break;
        case RTI:
            break;
        case TRAP:
            scanimm(0, &off);
            p1 = (int) off;
            chksetimmzext(&ins, p1, 8);
            break;
        default:
            GTFO("Unhandled instruction type\n");
    }
    r_write_be16(result->buf, ins);
    return result->size;
}

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
