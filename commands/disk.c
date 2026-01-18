#include "../userlib/hazle.h"

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    
    puts("disk information:");
    
    unsigned int total, free_space, cluster_size;
    diskinfo(&total, &free_space, &cluster_size);
    
    char buf[32];
    
    itoa(total / (1024 * 1024), buf, 10);
    putchar(' '); putchar(' ');
    char *s = buf; while (*s) putchar(*s++);
    puts(" MB total");
    
    itoa(free_space / (1024 * 1024), buf, 10);
    putchar(' '); putchar(' ');
    s = buf; while (*s) putchar(*s++);
    puts(" MB free");
    
    itoa(cluster_size / 1024, buf, 10);
    putchar(' '); putchar(' ');
    s = buf; while (*s) putchar(*s++);
    puts(" KB cluster size");
    
    return 0;
}
