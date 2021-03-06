# BinTools
This project is an experimental repository for examining executable files and formats from the perspective of code. 

- [Executable and Linker Format Specification](http://man7.org/linux/man-pages/man5/elf.5.html)
- [Linker and Libraries Guide](https://docs.oracle.com/cd/E23824_01/html/819-0690/chapter6-93046.html#scrolltoc)
- [Portable Executable Specification](https://docs.microsoft.com/en-us/windows/desktop/Debug/pe-format)


### Goals
- Create an interface to load both ELF and PE files. ([skBinaryFile](https://github.com/CharlesCarley/BinTools/blob/master/Source/skBinaryFile.h))
- Use [capstone](http://www.capstone-engine.org/) to disassemble the code extracted from the binary.
- Convert/extract more relevant information.
- Make the file library completely cross-platform and dependency-free.
- Keep the BinaryFile library simple and separate from any tools/tests written with it.


#### Test tools

- [bindump](https://github.com/CharlesCarley/BinTools/blob/master/Tools/bindump)
- [hexdump](https://github.com/CharlesCarley/BinTools/blob/master/Tools/hexdump)


## Building

#### External modules 
BinTools depends on the external Utils repository. After the BinTools repository is checked out, the Utils submodule must also be checked out with the following command.
```
git submodule update -–init --checkout
```  
#### Building with CMake 

BinTools uses CMake to generate its MakeFiles. From the root of the BinTools repository issue the following commands. 
```
mkdir build
cd build
cmake ..
make
```
