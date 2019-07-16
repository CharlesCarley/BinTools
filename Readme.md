# BinTools
This project is an experimental repository for examining executable file formats. 

- [Executable and Linker Format Specification](http://man7.org/linux/man-pages/man5/elf.5.html)
- [Portable Executable Specification](https://docs.microsoft.com/en-us/windows/desktop/Debug/pe-format)


### Goals
- To create an abstract interface to load both ELF, and PE. ([skBinaryFile](https://github.com/CharlesCarley/BinTools/blob/master/Source/skBinaryFile.h))
- Use capstone to disassemble the code extracted from the binary.



#### TODO
- [ ] Handle 32 and 64 bit elf types.
- [ ] Convert more relevant infomation.


#### Usage


##### Command line usage
```
bindump [options] -f [path to file]
  -b  -- convert output to binary.
  -a  -- display ASCII listings.
  -h  -- display this help message.
  -i  -- interactive mode.
  -c  -- separate output into colors in order to see it better.
```



##### Interactive mode
```
  Print options:
    1. Print hex dump.
    2. Print section headers and the hex dump of each.
    3. Print section names.
    4. Display specific section name.
        .bss, .init, .text, etc.
  Print flags:
    A. Display ascii.
    B. Display Binary.
    D. Display Disassembly.
    H. Display Hex.
  File options:
    F. Load file.
                                Q-Exit.
```
