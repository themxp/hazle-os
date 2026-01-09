#include "commands.h"
#include "display.h"
#include "fat32.h"
#include "string.h"

void cmd_cd(int argc, char** argv) {
    if (!fat32_is_mounted()) {
        if (!fat32_mount()) {
            display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
            display_writeln("Error: No filesystem mounted");
            display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
            return;
        }
    }
    
    if (argc < 2) {
        fat32_change_dir("/");
        return;
    }
    
    if (!fat32_change_dir(argv[1])) {
        display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
        display_write("Error: Cannot change to directory: ");
        display_writeln(argv[1]);
        display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
    }
}

void cmd_pwd(int argc, char** argv) {
    (void)argc;
    (void)argv;
    
    if (!fat32_is_mounted()) {
        if (!fat32_mount()) {
            display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
            display_writeln("Error: No filesystem mounted");
            display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
            return;
        }
    }
    
    display_writeln(fat32_get_current_path());
}
