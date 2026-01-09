#include "commands.h"
#include "display.h"
#include "fat32.h"
#include "string.h"

void cmd_cat(int argc, char** argv) {
    if (argc < 2) {
        display_set_color(DISPLAY_COLOR_YELLOW, DISPLAY_COLOR_BLACK);
        display_writeln("Usage: cat <filename>");
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
    
    int fd = fat32_open(argv[1]);
    if (fd < 0) {
        display_set_color(DISPLAY_COLOR_LIGHT_RED, DISPLAY_COLOR_BLACK);
        display_write("Error: Cannot open file: ");
        display_writeln(argv[1]);
        display_set_color(DISPLAY_COLOR_LIGHT_GREY, DISPLAY_COLOR_BLACK);
        return;
    }
    
    char buffer[513];
    int bytes_read;
    
    while ((bytes_read = fat32_read(fd, buffer, 512)) > 0) {
        buffer[bytes_read] = '\0';
        display_write(buffer);
    }
    
    display_writeln("");
    fat32_close(fd);
}
