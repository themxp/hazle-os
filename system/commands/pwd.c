#include "../../userlib/hazle.h"

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;
    char buf[256];
    getcwd(buf, 256);
    puts(buf);
    return 0;
}
