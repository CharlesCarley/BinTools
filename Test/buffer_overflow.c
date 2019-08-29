#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// should fill the space for buf1, and buf2, then set value to 42
const char *overflow = "0000000000000000000000000000\x2A";

int main(int arcg, char **argv)
{
    int  val = 5;
    char buf1[8];
    char buf2[8];
    //  -------
    // | buf2  | 8
    //  -------
    // | buf1  | 16
    //  -------
    // | value | 24
    //  -------
    strcpy(buf1, "one");
    strcpy(buf2, "two");
    strcpy(buf2, overflow);

    printf("The value in buf1 = %s\n", buf1);
    printf("The value in buf2 = %s\n", buf2);
    printf("The value of val  = %i\n", val);
    return 0;
}
