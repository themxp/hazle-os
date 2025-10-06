#include "commands.h"
#include "display.h"

void cmd_clear(int argc, char** argv) {
    (void)argc;
    (void)argv;
    display_clear();
}
