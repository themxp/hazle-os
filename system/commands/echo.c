#include "../../userlib/hazle.h"

int main(int argc, char **argv) {
    for (int i = 1; i < argc; i++) {
        if (i > 1) putchar(' ');
        char *s = argv[i];
        while (*s) putchar(*s++);
    }
    putchar('\n');
    return 0;
}
