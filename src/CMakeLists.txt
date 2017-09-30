cmake_minimum_required(VERSION 3.0.2)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif(NOT CMAKE_BUILD_TYPE)

## Basic project info

# TODO fill in project name
project(r2-lc3)

# Executables
# See https://cmake.org/cmake/help/v3.0/command/add_executable.html
#add_executable(<PROJECT_EXE1>
#    exe1.c
#)

# Libraries
# See https://cmake.org/cmake/help/v3.0/command/add_library.html
add_library(asm_lc3 SHARED
    disassembler.c
    assembler.c
    plugin.c
)

set_target_properties(asm_lc3 PROPERTIES PREFIX "")
## pkg-config support

# Load pkg-config module
find_package(PkgConfig REQUIRED)

# Search for a specific pkg-config library
pkg_search_module(LIBR_ASM REQUIRED r_asm)

# Building parameters for targets
target_link_libraries(asm_lc3 ${LIBR_ASM_LIBRARIES})
target_include_directories(asm_lc3 PUBLIC ${LIBR_ASM_INCLUDE_DIRS})
target_compile_options(asm_lc3 PUBLIC -Wall -Wextra ${LIBR_ASM_CFLAGS_OTHER})
