# radare2-lc3

A set of LC-3 plugin for Radare2

(Development stalled. Currently assembler and disassembler should somewhat work. Anal and ESIL were unfinished. Also the 16-bit byte used by LC-3 makes anal support very difficult to be done cleanly without a companion io plugin. https://github.com/MaskRay/r2cLEMENCy might be a good start if you are looking for writing one.)

## Build

CMake and recent version of radare2 is needed in order to build this project.

### Build instructions
1. Create a build directory (This step is not needed but highly recommended)

```
mkdir build && cd build
```

2. Run CMake to generate the make file (if you have a build directory elsewhere,
change the path accordingly)

```
cmake ..
```

3. Run `make`

## Install

Just copy the generated .so file under \<build dir\>/src/ to the plugin
directory of radare2

## License & Copyright Notice

GPLv3 for all the code. LC-3 ISA documentation located in
`doc/PattPatelAppA.pdf` belongs to its original author.
