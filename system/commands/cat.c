#include "../userlib/hazle.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        puts("usage: cat <file>");
        return 2;
    }
    
    int fd = open(argv[1]);
    if (fd < 0) {
        puts("file not found");
        return 1;
    }
    
    char buf[256];
    int n;
    while ((n = read(fd, buf, 256)) > 0) {
        for (int i = 0; i < n; i++) {
            putchar(buf[i]);
        }
    }
    
    close(fd);
    putchar('\n');
    return 0;
}
