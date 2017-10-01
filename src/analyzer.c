#include "anal_functions.h"
#include "gen_regprofile.h"

int do_archinfo(RAnal *ctx, int key) {
    switch (key) {
        case R_ANAL_ARCHINFO_MIN_OP_SIZE:
        case R_ANAL_ARCHINFO_MAX_OP_SIZE:
        case R_ANAL_ARCHINFO_ALIGN:
        //case R_ANAL_ARCHINFO_DATA_ALIGN:
        default:
            return 2;
            break;
    }
    return 2;
}

char *do_get_reg_profile(RAnal *ctx) {
    return strndup((const char *) regprofile, regprofile_len);
}

int do_analyze(RAnal *ctx, RAnalOp *result, ut64 addr, const ut8 *input, int len) {
    return 2;
}
