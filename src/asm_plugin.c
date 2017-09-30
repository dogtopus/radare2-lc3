#include "asm_functions.h"

// Plugin description
RAsmPlugin r_asm_plugin_lc3 = {
    .name = "lc3",
    .desc = "Little Computer 3 (LC-3)",
    .arch = "lc3",
    .bits = 16,
    .license = "GPL3",
    //.init = NULL,
    //.fini = NULL,
    //.modify = NULL,
    .assemble = &do_assemble,
    .disassemble = &do_disassemble
};

// Plugin header, if it is a dynamic loaded plugin
#ifndef CORELIB
struct r_lib_struct_t radare_plugin = {
	.type = R_LIB_TYPE_ASM,
	.data = &r_asm_plugin_lc3
};
#endif
