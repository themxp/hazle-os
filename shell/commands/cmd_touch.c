#include "commands.h"
#include "display.h"
#include "fat32.h"
#include "string.h"

void cmd_touch(int argc, char** argv) {
    if (argc < 2) {
        display_set_color(DISPLAY_COLOR_YELLOW, DISPLAY_COLOR_BLACK);
        display_writeln("Usage: touch <filename>");
        display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
        return;
    }
    
    if (!fat32_is_mounted()) {
        if (!fat32_mount()) {
            display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
            display_writeln("Error: No filesystem mounted");
            display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
            return;
        }
    }
    
    if (!fat32_create_file(argv[1])) {
        display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
        display_write("Error: Cannot create file: ");
        display_writeln(argv[1]);
        display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
    }
}
