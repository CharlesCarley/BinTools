# hexdump
Is a utility to print a colorized hexdump of the supplied input file to the terminal. 

#### Command line usage

```
hexdump <options> <path to file> 

  Options:
      -h          Display this help message.
      -m          Mark a specific hexadecimal sequence. 
                      Input is in base 16.

                      1 byte sequence [0, FF]
                      2 byte sequence [0, FFFF]
                      4 byte sequence [0, FFFFFFFF]
    
      -ar         Specify a start address and a range.
                      Input is in base 10 [0, file-length].

                      [offset, offset + range]
                      start = pointer + offset
                      end   = pointer + (offset + range)

      -xc         Remove color output.
      -xa         Remove the ASCII table in the hex dump output.
```
