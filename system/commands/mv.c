#include "../../userlib/hazle.h"

int main(int argc, char **argv) {
    if (argc < 3) {
        puts("usage: mv <src> <dst>");
        return 2;
    }
    
    if (rename(argv[1], argv[2]) != 0) {
        puts("failed to move/rename");
        return 1;
    }
    
    return 0;
}
