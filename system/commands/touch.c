#include "../userlib/hazle.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        puts("usage: touch <file>");
        return 2;
    }
    
    if (create(argv[1]) != 0) {
        puts("failed to create file");
        return 1;
    }
    
    return 0;
}
