#include <stdio.h>


void do_a_call()
{
    puts("Hello world\n");
}


#ifdef WIN32
void __declspec(dllexport) symbol()
#else
void symbol()
#endif  // WIN32
{
    do_a_call();
}

