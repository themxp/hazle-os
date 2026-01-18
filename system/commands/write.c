#include "../../userlib/hazle.h"

int main(int argc, char **argv) {
    if (argc < 3) {
        puts("usage: write <file> <text>");
        return 2;
    }
    
    int fd = open(argv[1]);
    if (fd < 0) {
        if (create(argv[1]) != 0) {
            puts("failed to create file");
            return 1;
        }
        fd = open(argv[1]);
        if (fd < 0) {
            puts("failed to open file");
            return 1;
        }
    }
    
    for (int i = 2; i < argc; i++) {
        if (i > 2) write(fd, " ", 1);
        write(fd, argv[i], strlen(argv[i]));
    }
    write(fd, "\n", 1);
    
    close(fd);
    return 0;
}
