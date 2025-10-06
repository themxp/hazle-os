#include "commands.h"
#include "display.h"
#include "string.h"

void cmd_color(int argc, char** argv) {
    if (argc != 3) {
        display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
        display_writeln("Usage: color <fg> <bg>");
        display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
        display_writeln("Colors: 0-15 (0=black, 7=light grey, 15=white)");
        return;
    }
    
    int fg = atoi(argv[1]);
    int bg = atoi(argv[2]);
    
    if (fg < 0 || fg > 15 || bg < 0 || bg > 15) {
        display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
        display_writeln("Error: Color values must be between 0 and 15");
        return;
    }
    
    display_set_color((display_color_t)fg, (display_color_t)bg);
    display_writeln("Color scheme changed!");
}
