#include "commands.h"
#include "display.h"

void cmd_help(int argc, char** argv) {
    (void)argc;
    (void)argv;
    
    extern command_t commands[];
    extern int command_count;
    
    display_set_color(DISPLAY_COLOR_LIGHT_CYAN, DISPLAY_COLOR_BLACK);
    display_writeln("Available Commands:");
    display_writeln("==================");
    
    for (int i = 0; i < command_count; i++) {
        display_set_color(DISPLAY_COLOR_LIGHT_GREEN, DISPLAY_COLOR_BLACK);
        display_write("  ");
        display_write(commands[i].name);
        display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
        display_write(" - ");
        display_writeln(commands[i].description);
    }
}
