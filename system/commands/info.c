#include "../../userlib/hazle.h"

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    
    puts("Hazle OS v1.0.0");
    puts("32-bit x86 operating system");
    puts("");
    
    unsigned int total, used;
    getmem(&total, &used);
    
    char buf[32];
    itoa(total / (1024 * 1024), buf, 10);
    
    puts("memory:");
    putchar(' '); putchar(' ');
    char *s = buf; while (*s) putchar(*s++);
    puts(" MB total");
    
    itoa(used / (1024 * 1024), buf, 10);
    putchar(' '); putchar(' ');
    s = buf; while (*s) putchar(*s++);
    puts(" MB used");
    
    int uptime = getuptime();
    int hours = uptime / 3600;
    int mins = (uptime % 3600) / 60;
    int secs = uptime % 60;
    
    puts("");
    puts("uptime:");
    putchar(' '); putchar(' ');
    itoa(hours, buf, 10); s = buf; while (*s) putchar(*s++);
    putchar('h'); putchar(' ');
    itoa(mins, buf, 10); s = buf; while (*s) putchar(*s++);
    putchar('m'); putchar(' ');
    itoa(secs, buf, 10); s = buf; while (*s) putchar(*s++);
    puts("s");
    
    return 0;
}
