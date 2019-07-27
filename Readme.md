# BinTools
This project is an experimental repository for examining executable files and formats from the perspective of code. 

- [Executable and Linker Format Specification](http://man7.org/linux/man-pages/man5/elf.5.html)
- [Linker and Libraries Guide](https://docs.oracle.com/cd/E23824_01/html/819-0690/chapter6-93046.html#scrolltoc)
- [Portable Executable Specification](https://docs.microsoft.com/en-us/windows/desktop/Debug/pe-format)


### Goals
- To create an abstract interface to load both ELF, and PE. ([skBinaryFile](https://github.com/CharlesCarley/BinTools/blob/master/Source/skBinaryFile.h))
- Use [capstone](http://www.capstone-engine.org/) to disassemble the code extracted from the binary.
- Convert/extract more relevant information.
- Make it completely cross platform and dependency free.
- Keep the BinaryFile library simple and separate from any tools/tests written with it.




#### bindump Usage


##### Command line usage
```
bindump <options> <path to file>

  Options:
      -m [0-255]  Mark specific code.

      -a          Display ASCII table.
      -b          Convert output to binary.
      -d          Display disassembly in code sections.
      -h          Display this help message.

      -o [1-4]    Interactive menu option.
                  - 1. Print a hex dump of the files contents.
                  - 2. Print section headers and the hex dump of each.
                  - 3. List all loaded section names
                  - 4. List all loaded symbols

      -i          Run in interactive mode.
```



##### Interactive mode
```
 Please Select From The Following Menu:

 Print Options:
   1. Print hex dump
   2. Print section headers and the hex dump of each
   3. Print section names
   4. Display specific section name
      .bss, .init, .text, etc
   5. Print symbol names
Print Options:
   A. Display ASCII
   B. Display Binary
   D. Display Disassembly
   H. Display Hex
   M. Mark specific code
 File Options:
   F. Load file
                                            Q-Exit.
```
