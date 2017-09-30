#ifndef __FUNCTIONS_H__
#define __FUNCTIONS_H__

#include <r_asm.h>
#include <r_lib.h>
#include "common.h"

int do_disassemble(RAsm *ctx, RAsmOp *result, const ut8 *input, int len);
int do_assemble(RAsm *ctx, RAsmOp *result, const char *input);

#endif
