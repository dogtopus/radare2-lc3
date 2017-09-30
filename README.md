# radare2-lc3

A set of LC-3 plugin for Radare2

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
