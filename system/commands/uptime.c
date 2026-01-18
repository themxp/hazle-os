#include "../../userlib/hazle.h"

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    
    int uptime = getuptime();
    int hours = uptime / 3600;
    int mins = (uptime % 3600) / 60;
    int secs = uptime % 60;
    
    char buf[32];
    
    itoa(hours, buf, 10);
    char *s = buf; while (*s) putchar(*s++);
    putchar('h'); putchar(' ');
    
    itoa(mins, buf, 10);
    s = buf; while (*s) putchar(*s++);
    putchar('m'); putchar(' ');
    
    itoa(secs, buf, 10);
    s = buf; while (*s) putchar(*s++);
    puts("s");
    
    return 0;
}
