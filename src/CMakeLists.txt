cmake_minimum_required(VERSION 3.0.2)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

## Basic project info

project(r2-lc3)

find_program(XXD xxd)
if (NOT XXD)
    message(FATAL_ERROR "xxd is required")
endif(NOT XXD)

# xxd doesn't allow user-defined variable name (sigh)
add_custom_command(
    OUTPUT gen_regprofile.h
    COMMAND cp ${CMAKE_CURRENT_SOURCE_DIR}/regprofile.txt ${CMAKE_CURRENT_BINARY_DIR}/regprofile &&
            printf '\\0' >> ${CMAKE_CURRENT_BINARY_DIR}/regprofile &&
            pushd ${CMAKE_CURRENT_BINARY_DIR} &&
            ${XXD} -i regprofile gen_regprofile.h &&
            rm regprofile &&
            popd
    MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/regprofile.txt
)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

# Libraries
# See https://cmake.org/cmake/help/v3.0/command/add_library.html
add_library(asm_lc3 SHARED
    disassembler.c
    assembler.c
    asm_plugin.c
)

add_library(anal_lc3 SHARED
    gen_regprofile.h
    analyzer.c
    anal_plugin.c
)


set_target_properties(asm_lc3 PROPERTIES PREFIX "")
set_target_properties(anal_lc3 PROPERTIES PREFIX "")

## pkg-config support

# Load pkg-config module
find_package(PkgConfig REQUIRED)

# Search for a specific pkg-config library
pkg_search_module(LIBR_ASM REQUIRED r_asm)
pkg_search_module(LIBR_ANAL REQUIRED r_anal)

# Building parameters for targets
target_link_libraries(asm_lc3 ${LIBR_ASM_LIBRARIES})
target_include_directories(asm_lc3 PUBLIC ${LIBR_ASM_INCLUDE_DIRS})
target_compile_options(asm_lc3 PUBLIC -Wall -Wextra ${LIBR_ASM_CFLAGS_OTHER})

target_link_libraries(anal_lc3 ${LIBR_ANAL_LIBRARIES})
target_include_directories(anal_lc3 PUBLIC ${LIBR_ANAL_INCLUDE_DIRS})
target_compile_options(anal_lc3 PUBLIC -Wall -Wextra ${LIBR_ANAL_CFLAGS_OTHER})
