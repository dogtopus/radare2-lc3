#include "anal_functions.h"

RAnalPlugin r_anal_plugin_lc3 = {
    .name = "lc3",
    .desc = "Little Computer 3 (LC-3)",
    .license = "GPL3",
    .esil = false,
    .arch = "lc3",
    .archinfo = do_archinfo,
    .get_reg_profile = do_get_reg_profile,
    .op = do_analyze
};

#ifndef CORELIB
RLibStruct radare_plugin = {
	.type = R_LIB_TYPE_ANAL,
	.data = &r_anal_plugin_lc3
};
#endif
