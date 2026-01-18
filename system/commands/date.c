#include "../../userlib/hazle.h"
#include "../../userlib/syscall.h"

typedef struct {
    unsigned char second;
    unsigned char minute;
    unsigned char hour;
    unsigned char day;
    unsigned char month;
    unsigned short year;
} rtc_time_t;

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    
    rtc_time_t time;
    syscall1(SYS_GETTIME, (int)&time);
    
    char buf[32];
    
    itoa(time.year, buf, 10);
    char *s = buf; while (*s) putchar(*s++);
    putchar('-');
    
    if (time.month < 10) putchar('0');
    itoa(time.month, buf, 10);
    s = buf; while (*s) putchar(*s++);
    putchar('-');
    
    if (time.day < 10) putchar('0');
    itoa(time.day, buf, 10);
    s = buf; while (*s) putchar(*s++);
    putchar(' ');
    
    if (time.hour < 10) putchar('0');
    itoa(time.hour, buf, 10);
    s = buf; while (*s) putchar(*s++);
    putchar(':');
    
    if (time.minute < 10) putchar('0');
    itoa(time.minute, buf, 10);
    s = buf; while (*s) putchar(*s++);
    putchar(':');
    
    if (time.second < 10) putchar('0');
    itoa(time.second, buf, 10);
    s = buf; while (*s) putchar(*s++);
    putchar('\n');
    
    return 0;
}
