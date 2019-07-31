# bindump
Bin dump is a tool that prints structured information about the  ELF and PE executable file formats.

#### bindump Usage


##### Command line usage
```
bindump <options> <path to file>

  Options:
      -m [0-255]  Mark specific code.

      -a          Remove the ASCII table in the hex dump output.
      -b          Display a binary table in the hex dump output.
      -d          Display disassembly in code sections.
      -h          Display this help message.

      -o [1-5]    Interactive menu option.
                  - 1. Print a hex dump of the files contents.
                  - 2. Print section headers and the dump of each.
                  - 3. List all loaded section names.
                  - 4. List all loaded symbols.
                  - 5. Display only headers.

      -i          Run in interactive mode.
```



##### Interactive mode
```
Please Select From The Following Menu:

  Print Options:
    1. Print a hex dump of the files contents.
    2. Print section headers and the hex dump of each.
    3. List all loaded section names.
    4. List all loaded symbols.
    5. Display only headers.
    6. Display specific section name.
         .bss, .init, .text, etc

  Display Options:
    A. Display ASCII
    B. Display Binary
    D. Display Disassembly
    H. Display Hex
    M. Mark specific code.

  File Options:
    F. Path to file
    
                                        Q-Exit.
```
