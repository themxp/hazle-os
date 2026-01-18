#include "../userlib/hazle.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        puts("usage: mkdir <name>");
        return 2;
    }
    
    if (mkdir(argv[1]) != 0) {
        puts("failed to create directory");
        return 1;
    }
    
    return 0;
}
