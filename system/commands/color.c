#include "../userlib/hazle.h"

int main(int argc, char **argv) {
    if (argc < 2) {
        puts("usage: color <fg> [bg]");
        return 2;
    }
    
    int fg = atoi(argv[1]);
    int bg = 0;
    if (argc >= 3) bg = atoi(argv[2]);
    
    if (fg < 0 || fg > 15 || bg < 0 || bg > 15) {
        puts("color values must be 0-15");
        return 1;
    }
    
    setcolor(fg, bg);
    return 0;
}
