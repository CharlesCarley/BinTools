# BinTools
This project is an experimental repository for examining executable file formats. 

- [Executable and Linker Format Specification](http://man7.org/linux/man-pages/man5/elf.5.html)
- [Portable Executable Specification](https://docs.microsoft.com/en-us/windows/desktop/Debug/pe-format)


### Goals
- To create an abstract interface to load both ELF, and PE. ([skBinaryFile](https://github.com/CharlesCarley/BinTools/blob/master/Source/skBinaryFile.h))
- Use capstone to disassemble the code extracted from the binary.



### TODO
- [ ] Abstract PE file.
- [ ] Add a default unknown file type.
- [ ] Handle 32 and 64 bit elf types.
- [ ] Convert more relevant infomation.

