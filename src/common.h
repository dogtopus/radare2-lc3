#ifndef __COMMON_H__
#define __COMMON_H__

#include <r_types.h>

/* register slots */
#define REG1 9
#define REG2 6
#define REG3 0

/* flags */
#define FLAG_BR_N 11
#define FLAG_BR_Z 10
#define FLAG_BR_P 9
#define FLAG_ADDAND_ISIMM 5
#define FLAG_JSR_ISJSR 11

/* mnemonics->opcode table, although only jmp was used once */
#define OP_BR 0x0
#define OP_ADD 0x1
#define OP_LD 0x2
#define OP_ST 0x3
#define OP_JSR 0x4
#define OP_AND 0x5
#define OP_LDR 0x6
#define OP_STR 0x7
#define OP_RTI 0x8
#define OP_NOT 0x9
#define OP_LDI 0xa
#define OP_STI 0xb
#define OP_JMP 0xc
#define OP_RFU 0xd
#define OP_LEA 0xe
#define OP_TRAP 0xf

typedef enum {
    RFU,     /* special format for op 0b1101 */
    ADDAND,  /* add, and */
    BR,      /* br(n,z,p) */
    JMP,     /* ret, jmp */
    JSR,     /* jsr(r) */
    LDSTLEA, /* ld(i), st(i), lea */
    LDRSTR,  /* ldr, str */
    NOT,     /* not */
    RTI,     /* rti */
    TRAP     /* trap */
} inst_format;

typedef struct {
    char *name;
    inst_format format;
    char has_suffix;
} inst_table;

#define OPS 16

static const inst_table INST_TABLE[OPS] = {
    { "br",     BR, 1 },       /* 0b0000 */
    { "add",    ADDAND, 0 },   /* 0b0001 */
    { "ld",     LDSTLEA, 0 },  /* 0b0010 */
    { "st",     LDSTLEA, 0 },  /* 0b0011 */
    { "jsr",    JSR, 1 },      /* 0b0100 */
    { "and",    ADDAND, 0 },   /* 0b0101 */
    { "ldr",    LDRSTR, 0 },   /* 0b0110 */
    { "str",    LDRSTR, 0 },   /* 0b0111 */
    { "rti",    RTI, 0 },      /* 0b1000 */
    { "not",    NOT, 0 },      /* 0b1001 */
    { "ldi",    LDSTLEA, 0 },  /* 0b1010 */
    { "sti",    LDSTLEA, 0 },  /* 0b1011 */
    { "jmp",    JMP, 0 },      /* 0b1100 */
    { NULL,     RFU, 0 },      /* 0b1101 (RFU) */
    { "lea",    LDSTLEA, 0 },  /* 0b1110 */
    { "trap",   TRAP, 0 }      /* 0b1111 */
};

/* get opcode type and how to handle it */
static inline inst_table getop(ut16 ins) {
    return INST_TABLE[(ins >> 12) & 0xf];
}

/* get opcode by looking up its name */
static inline int getopbyname(char *name) {
    int cmp_result;
    inst_table op;

    if (!strcmp(name, "ret")) {
        return OP_JMP;
    }

    for (int i=0; i<OPS; i++) {
        op = INST_TABLE[i];
        /* skip RFU */
        if (!op.name) {
            continue;
        }
        if (op.has_suffix) {
            /* only match the prefix */
            cmp_result = strncmp(name, op.name, strlen(op.name));
        } else {
            /* match everything */
            cmp_result = strcmp(name, op.name);
        }
        if (!cmp_result) {
            return i;
        }
    }
    return -1;
}

/* get register on specific bit position */
static inline int getreg(ut16 ins, const int shr) {
    return (ins >> shr) & 0x7;
}

/* get immediate value of specific size */
static inline int getimm(ut16 ins, const int size) {
    return ins & ((1 << size) - 1);
}

/* get flag on specific bit position */
static inline int getflag(ut16 ins, const int shr) {
    return (ins >> shr) & 1;
}

/* signed extension as described in lc3 documentation */
static inline int sext(int a, const int width) {
    int result = 0;
    int sign = (a >> (width - 1)) & 1;

    if (sign) {
        /* starts with all 1, clear bits for a and set them according to a */
        result = -1;
        result ^= (1 << width) - 1;
        result |= a;
    } else {
        result = a;
    }
    return result;
}

/* get immediate value and do sign extension */
static inline int getimmsext(ut16 ins, const int size) {
    return sext(getimm(ins, size), size);
}

static inline void setins(ut16 *ins, int op) {
    (*ins) |= op << 12;
}

static inline void setflag(ut16 *ins, const int shl) {
    (*ins) |= 1 << shl;
}

static inline int setimmsext(ut16 *ins, int val, const int size) {
    if (sext(val & ((1 << size) - 1), size) != val) {
        return -1;
    }
    (*ins) |= val & ((1 << size) - 1);
    return 0;
}

static inline int setimm(ut16 *ins, int val, const int size) {
    if (val < 0 || ((val & ((1 << size) - 1)) != val)) {
        return -1;
    }
    (*ins) |= val & ((1 << size) - 1);
    return 0;
}

static inline int setreg(ut16 *ins, int reg, const int shl) {
    if (reg < 0 || reg > 7) {
        return -1;
    }
    (*ins) |= reg << shl;
    return 0;
}
#endif
