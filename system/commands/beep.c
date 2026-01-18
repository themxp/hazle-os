#include "../../userlib/hazle.h"

int main(int argc, char **argv) {
    int freq = 1000;
    int duration = 200;
    
    if (argc >= 2) freq = atoi(argv[1]);
    if (argc >= 3) duration = atoi(argv[2]);
    
    beep(freq, duration);
    return 0;
}
