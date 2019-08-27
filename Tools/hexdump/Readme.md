# hexdump
Is a utility to print a colorized hexdump of the supplied input file to the terminal. 

#### Command line usage

```
hexdump <options> <path to file> 

  Options:
      -h          Display this help message.
      -m          Mark a specific hexadecimal sequence.

                  1 byte sequence [0, 255]
                  2 byte sequence [0, 65535]
                  4 byte sequence [0, 4294967295]

      -xc         Remove color output.
      -xa         Remove the ASCII table in the hex dump output.
```
