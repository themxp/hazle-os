#include "commands.h"
#include "display.h"

void cmd_echo(int argc, char** argv) {
    for (int i = 1; i < argc; i++) {
        display_write(argv[i]);
        if (i < argc - 1) {
            display_putchar(' ');
        }
    }
    display_putchar('\n');
}
