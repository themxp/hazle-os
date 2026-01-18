#include "../../userlib/hazle.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        puts("usage: rm <path>");
        return 2;
    }
    
    if (remove(argv[1]) != 0) {
        puts("failed to remove");
        return 1;
    }
    
    return 0;
}
