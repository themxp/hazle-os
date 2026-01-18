#include "../userlib/hazle.h"

int main(int argc, char **argv) {
    if (argc < 3) {
        puts("usage: cp <src> <dst>");
        return 2;
    }
    
    if (copy(argv[1], argv[2]) != 0) {
        puts("failed to copy");
        return 1;
    }
    
    return 0;
}
