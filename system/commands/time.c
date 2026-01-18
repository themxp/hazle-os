#include "../../userlib/hazle.h"

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    
    int ticks = getticks();
    char buf[32];
    itoa(ticks, buf, 10);
    
    char *s = buf;
    while (*s) putchar(*s++);
    puts(" ticks");
    
    return 0;
}
