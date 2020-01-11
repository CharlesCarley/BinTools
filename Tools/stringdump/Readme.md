# stringdump
A utility to print a list of ASCII characters in a file. 

The test range starts at 32, and stops at 127

#### Command line usage

```
stringdump <options> <path to file> 

  Options:
      -h          Display this help message.
      -ar         Specify a start address and a range.
                      Input is in base 10 [0, file-length].

                      [offset, offset + range]
                      start = pointer + offset
                      end   = pointer + (offset + range)
```
