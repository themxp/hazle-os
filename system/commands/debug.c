#include "../../userlib/hazle.h"

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    
    puts("debug information:");
    
    unsigned int total, used;
    getmem(&total, &used);
    
    char buf[32];
    
    puts("memory:");
    itoa(total, buf, 16);
    putchar(' '); putchar(' ');
    char *s = buf; while (*s) putchar(*s++);
    puts(" total bytes");
    
    itoa(used, buf, 16);
    putchar(' '); putchar(' ');
    s = buf; while (*s) putchar(*s++);
    puts(" used bytes");
    
    puts("");
    puts("ticks:");
    itoa(getticks(), buf, 10);
    putchar(' '); putchar(' ');
    s = buf; while (*s) putchar(*s++);
    putchar('\n');
    
    return 0;
}
