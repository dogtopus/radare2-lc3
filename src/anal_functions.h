#ifndef __ANAL_FUNCTIONS_H__
#define __ANAL_FUNCTIONS_H__

//#include <r_asm.h>
#include <r_anal.h>
#include <r_lib.h>
#include "common.h"

int do_archinfo(RAnal *ctx, int key);
char *do_get_reg_profile(RAnal *ctx);
int do_analyze(RAnal *ctx, RAnalOp *result, ut64 addr, const ut8 *input, int len);
#endif
