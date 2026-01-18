#include "../userlib/hazle.h"

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    
    unsigned int total, used;
    getmem(&total, &used);
    
    char buf[32];
    
    puts("memory usage:");
    
    itoa(total / (1024 * 1024), buf, 10);
    putchar(' '); putchar(' ');
    char *s = buf; while (*s) putchar(*s++);
    puts(" MB total");
    
    itoa(used / (1024 * 1024), buf, 10);
    putchar(' '); putchar(' ');
    s = buf; while (*s) putchar(*s++);
    puts(" MB used");
    
    itoa((total - used) / (1024 * 1024), buf, 10);
    putchar(' '); putchar(' ');
    s = buf; while (*s) putchar(*s++);
    puts(" MB free");
    
    return 0;
}
