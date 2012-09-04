// Complete at 16:26, 16:36, 18:24, 19:31

#include <stdio.h>
#include <string.h>

typedef struct
{
    int len;
    char* str;
} String;

int main(int argc, char* argv[])
{
    String prog = { strlen(argv[0]), argv[0] };

    printf("%*s\n", prog.len, prog.str);

    int integer = prog.len;
    const char* string = prog.str;

    return 0;
}
