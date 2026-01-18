#include "../../userlib/hazle.h"

static unsigned int seed = 12345;

static unsigned int rand(void) {
    seed = seed * 1103515245 + 12345;
    return (seed / 65536) % 32768;
}

int main(int argc, char **argv) {
    seed = getticks();
    
    int max = 100;
    if (argc >= 2) max = atoi(argv[1]);
    if (max <= 0) max = 100;
    
    int result = rand() % max;
    
    char buf[32];
    itoa(result, buf, 10);
    puts(buf);
    
    return 0;
}
